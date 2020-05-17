#ifndef LAYER_STRUCTURE_H
#define LAYER_STRUCTURE_H

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include <queue>
#include <unordered_set>
#include <stdint.h>


namespace Index
{
    struct Layer_Structure
    {
        typedef uint32_t idx_t;

        size_t d; 
        size_t nc;
        size_t code_size;              // code_size after quantization per vector in bytes
        
        hnswlib::HierarchicalNSW * quantizer;
        faiss::ProductQuantizer * pq;
        faiss::ProductQuantizer *norm_pq;
        faiss::LinearTransform * opq_matrix;

        bool do_opq;

        size_t num_probe;
        size_t max_codes;

        std::vector<std::vector<idx_t>> ids;
        std::vector<std::vector<uint8_t>> codes;
        std::vector<std::vector<uint8_t>> norm_codes;


        size_t nsubc;                  //number of sub-centroid per group 
        bool do_pruning;               // do pruning or not


        std::vector<std::vector<idx_t>> nn_centroid_idxs;
        std::vector<std::vector<idx_t>> subgroup_sizes;
        std::vector<float> alphas;

    public:
        Layer_Structure(size_t dim, size_t ncentroids, size_t bytes_per_code,
        size_t nbits_per_idx, size_t nsubcentroids);

        void add_group(size_t group_idx, size_t group_size, const float *x, const id_x * ids);

        void search(size_t k, const float * x, float * distances, long * labels);

        void write (const char *path_index);

        void train_pq(size_t, const float * x);

        void compute_inter_centroid_dists();

        void build_quantizer(const char* path_data, const char *path_info,
                            const char* path_edges, size_t M = 16, size_t efConstruction = 500);

        void assign (size_t n, const float *x, idx_t *labels, size_t k = 1);

        void compute_centroid_norms();

        void add_batch(size_t n, const float* x, const idx_t *xids, const idx_t * recomputed_idx);

        void rotate_quantizer();

    protected:
        std::vector<float> query_centroid_dists;

        std::vector<std::vector<float>> inter_centroid_dists;

    private:
        void compute_residuals(size_t n, const float *x, float * residuals,
                                const float *subcentroids, const idx_t *keys);

        void reconstruct(size_t n, float *x, const float *decode_residuals,
                        const float *subsentroids, const idx_t *keys);
        
        void compute_subcentroids_idxs(idx_t *subcentroid_idxs, const float *subcentroids,
                                        const float *points, size_t group_size);

        float compute_alpha(const float *centroid_vectors, const float *points,
                            const float *centroid, const float *centroid_vector_norm_L2sqr,
                            size_t group_size);
    };
}



#endif