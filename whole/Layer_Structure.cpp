#include "Layer_Structure.h"

namespace Index
{
    void Layer_Structure::build_quantizer(const char *path_data, const char*path_info,
                                          const char *path_edges, size_t M, size_t efConstruction)
    {
        if (exists(path_info) && exists(path_edges))
        {
            quantizer = new hnswlib::HierarchicalNSW(path)info, path_data, path_edges);
            quantizer->efSearch = efConstruction;
        }

        quantizer = new hnslib::HierarchicalNSW(d, nc, M, 2*M, efConstruction);
        std::cout << "Constructing quantizer" << std::endl;

        std::ifstream input (path_data, std::ios::binary);

        size_t report_iterations = 10000;
        for (size_t i = 0; i < nc; i++)
        {
            float mass[d];
            readXvec<float>(input, mass, d);
            if (i % report_iterations == 0)
            {
                std::cout << i / (0.01 * nc) << " %\n";
            }
            quantizer->addPoint(mass);
        }
        quantizer->SaveInfo(path_info);
        quantizer->SaveEdges(path_edges);
    }


    }


}

