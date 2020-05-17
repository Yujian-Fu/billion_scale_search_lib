#ifndef BILLION_LIB_H
#define BILLION_LIB_H
#include <cstdio>
#include<typeinfo>
#include<string>
#include<vector>


namespace billion_lib {

    struct Index{
        using distance_t = float;

        size_t dimension;
        size_t n_level;
        size_t ntotal;
        std::vector<std::string> layers;

        bool is_trained;

        explicit Index(size_t dimension = 0, std::vector<std::string> layers): 
                        dimension(dimension), 
                        ntotal(0),
                        is_trained(false),
                        layers(layers){};

        virtual ~Index();

        virtual void train(size_t num, const float* x);

        virtual void add (size_t num, const float* x);

        virtual void add_with_ids (size_t num, const float * x, const size_t * xids);

        virtual void search (size_t n, const float* x, size_t k,
                             float * distances, size_t * labels);

        virtual size_t remove_ids (const std::vector<size_t> sel);

        virtual void compute_residual (const float * x, 
                                       float * residual, size_t key);


    };


}

#endif
