#ifndef QUANTIZER_H
#define QUANTIZER_H
#include <cstdio>
#include<typeinfo>
#include<string>
#include<vector>

namespace billion_lib{
    
    struct VectorQuantizer{
        size_t niter;
        size_t nredo;

        bool verbose;
        bool int_centroids;
        bool update_index;
        bool frozen_centroids;

        size_t min_points_per_centroid;
        size_t max_points_per_centroid;

        size_t seed;

        float obj;
        double time;
        double time_search;
        double imbalance_factor;
        int nsplit;

        size_t dimension;
        size_t k;

        std::vector<float> centroids;

        VectorQuantizer(size_t dimension, size_t k);

        virtual void train(size_t n, const float * x, billion_lib::Index & index,
                           const float * x_weights = nullptr);
        
        
    };

    struct LineQuantizer{

    };

    struct ProductQuantizer{

    };

    float kmeans_cluster (size_t d, size_t n, size_t k,
                          const float *x,
                          float *centroids);
}

#endif