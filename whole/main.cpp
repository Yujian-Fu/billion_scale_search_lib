#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include <queue>
#include <unordered_set>
#include <stdint.h>


#include "Parser.h"
#include "Layer_Structure.h"
#include "utils.h"

using namespace Index;

//*************************
// This is the main program 
// for constructing your own 
// billion scale search structure
// , running the test and 
// generate the report
// ************************

int main(int argc, char**argv)
{
    // ************************
    // Parse the option and parameters
    // from the bash file
    // ************************

    Parser opt = Parser(argc, argv);

    // *************************
    // Load the ground truth for query
    // *************************
    std::cout << "Loading the groundtruth from" << opt.path_gt << std::endl;;
    std::vector<idx_t> massQA(opt.num_query * opt.num_gt);
    {
        std::ifstream gt_input(opt.path_gt, std::ios::binary);
        readXvec<idx_t>（gt_input, massQA.data(), opt.num_gt, opt.num_query);
    }

    // **************************
    // Load Queries
    // **************************
    std::cout << "Loading queries from " << opt.path_query;
    std::vector<float> massQ(opt.num_query * opt.d);
    {
        std::ifstream query_input(opt.path_query, std::ios::binary);
        readXvecFvec<uint8_t>(query_input, massQ.data(), opt.d, opt.num_query);
    }

    // **************************
    // Initialize the index
    // **************************
    Layer_Structure *index = new Layer_Structure(opt.d, opt.num_centroid, opt.code_size, 8, opt.nsubc);
    index->build_quantizer(opt.path_centroids, opt.path_info, opt.path_edges, opt.M, opt.efConstruction);
    index->do_opq = opt.do_opq;



    // ***************************
    // Train PQ
    // ***************************
    if (exists(opt.path_pq) && exists(opt.path_norm_pq))
    {
        std::cout << "Loading Residual PQ codebook from " << opt.path_pq << std::endl;
        if (index->pq) delete index->pq;
        index->pq = faiss::read_ProductQuatizer(opt.path_pq);

        if (opt.do_opq)
        {
            std::cout << "Loading Residual OPQ rotation matrix from " << opt.path
            index->opq_matrix = dynamic_cast<faiss::LinearTransform *>(faiss::read_VectorTransform(opt.path_opq_matrix));            
        }
        std::cout << "Loading Norm PQ codebook from " << opt.path_norm_pq << std::endl;
        if(index->norm_pq) delete index->norm_pq;
        index->norm_pq = faiss::read_ProductQuantizer(opt.path_norm_pq);
    }
    else
    {
        std::vector<float> trainvecs(opt.num_train * opt.d);
        {
            std::ifstream learn_input(opt.path_learn, std::ios::binary);
            readXvecFvec<uint8_t>(learn_input, trainvecs.data(), opt.d, opt.num_train);
        }

        std::vector<float>  trainvecs_rnd_subset(opt.num_sub_train * opt.d);
        random_subset(trainvecs.data(), trainvecs_rnd_subset.data(), opt.d, opt.num_train, opt.num_sub_train);

        std::cout << "Training PQ codebooks" << std::endl;
        index->train_pq(opt.num_sub_train, trainvecs_rnd_subset.data());

        if (opt.do_opq)
        {
            std::cout << "Saving Residual OPQ rotation matrix to " << opt.path_opq_matrix << std::endl;
            faiss::write_ProductQuantizer(index->pq, opt.path_pq);

            std::cout << "Saving Norm PQ codebook to " << opt.path_norm_pq << std::endl;
            faiss::write_ProductQuantizer(index->norm_pq, opt.path_norm_pq);
        }
    }

    //*************************
    // Precompute Indices
    //*************************

    if (!exists(opt.path_precomputed_idxs))
    {
        std::cout << "Precomputing indices" << std::endl;
        StopW stopw = StopW();

        std::ifstream input(opt.path_base, std::ios::binary);
        std::ofstream output(opt.path_precomputed_idxs, std::ios::binary);

        const uint32_t batch_size = 1000000;
        const size_t nbatches = opt.num_base / batch_size;

        std::vector<float> batch(batch_size * opt.d);
        std::vector<idx_t> precomputed_idx(batch_size);

        index->quantizer->efSearch = 220;
        for (size_t i = 0; i < nbatches; i++){
            if (i % 10 == 0){
                std::cout << "[" << stopw.getElapsedTimeMicro() / 1000000 << "s] "
                          << (100. *i) / nbatches << "%" << std::endl;
            }
            readXvecFvec<uint8_t>(input, batch.data(), opt.d, batch_size);
            index->assign(batch_size, batch.data(), precomputed_idx.data());

            output.write((char *) &batch_size, sizeof(uint32_t));
            output.write((char *) precomputed_idx.data(), batch_size * sizeof(idx_t));
        }
    }

    // *************************
    // Construct Hierarchachy structure
    // *************************
    if (exists(opt.path_index)){
        std::cout << "Loading index from " << opt.path_index << std::endl;
        index->read(opt.path_index);
    }
    else{
        std::cout << "Adding groups to index " << std::endl;
        StopW stopw = StopW();

        const size_t batch_size = 1000000;
        const size_t nbatches = opt.num_base / batch_size;
        size_t groups_per_iter = 250000;

        std::vector<uint8_t>batch(batch_size * opt.d);
        std::vector<idx_t> idx_batch(batch_size);

        for (size_t ngroups_added = 0; ngroups_added < opt.num_centroid; ngroups_added += groups_per_iter)
        {
            std::cout << "[" << stopw.getElapsedTimeMicro() / 1000000 << "s]"
                     << ngroups_added << " / " << opt.num_centroid << std::endl;
            
            std::vector<std::vector<uint8_t>> data(groups_per_iter);
            std::vector<std::vector<idx_t>> ids(groups_per_iter);

            std::ifstream base_input(opt.path_base, std::ios::binary);
            std::ifstream idx_input(opt.path_precomputed_idxs, std::ios::binary);

            for (size_t b = 0; b < nbatches; b++)
            {
                readXvec<uint8_t>(idx_input, batch.data(), opt.d, batch_size);
                readXvec<idx_t>(idx_input, idx_batch.data(), batch_size, 1);

                for(size_t i = 0; i < batch_size; i++)
                {
                    if (idx_batch[i] < ngroups_added || 
                    idx_batch[i] >= ngroups_added + groups_per_iter)
                    continue;

                    idx_t idx = idx_batch[i] % groups_per_iter;
                    for(size_t j = 0; j < opt.d; j++)
                    {
                        data[idx].push_back(batch[i * opt.d + j]);
                    }
                    ids[idx].push_back(b * batch_size + i);
                }
            }

            if(opt.num_centroid - ngroups_added <= groups_per_iter)
                groups_per_iter = opt.num_centroid - ngroups_added;
            
            size_t j = 0;
            #pragma omp parallel for
            for (size_t i = 0; i < groups_per_iter; i++){
                #pragma omp critical
                {
                    if (j % 10000 == 0){
                        std::cout << "[" << stopw.getElapsedTimeMicro() / 10000000 << "s]"
                                  << (100. * (ngroups_added + j)) / opt.num_centroid << "%" << std::endl;
                    }
                    j ++;
                }
                const size_t group_size = ids[i].size();
                std::vector<float> group_data(group_size * opt.d);
                for(size_t k = 0; k < group_size * opt.d; k++)
                    group_data[k] = 1. * data[i][k];
                index->add_group(ngroups_added + i, group_size, group_data.data(), ids[i].data());
            
            }
        }
        std::cout << "Computing centroid norms " << 

    }
    
}
