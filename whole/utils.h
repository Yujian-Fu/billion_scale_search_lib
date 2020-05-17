
#ifndef INDEX_LAYER_UTILS_H
#define INDEX_LAYER_UTILS_H

#include <queue>
#include<limits>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <string.h>

namespace Index{
    class StopW{
        std::chrono::steady_clock::time_point time_begin;
        public:
            StopW(){
                time_begin = std::chrono::steady_clock::now();
            }

            float getElapsedTimeMicro(){
                std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
                return (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_begin).count());
            }

            void reset(){
                time_begin = std::chrono::steady_clock::now();
            }
    };
    
    template<typename T>
    void readXvec(std::ifstream &in, T *data, const size_t d, const size_t n = 1){
        uint32_t dim = d;
        for (size_t i = 0; i < n; i++){
            in.read((char *) &dim, sizeof(uint32_t));
            if (dim != d){
                std::cout << "Dimension error! " << std::endl;
                exit(1);
            }
            in.read((char *) (data + i * dim), dim * sizeof(T));
        }
    }

    template<typename T>
    void writeXvec(std::ofstream &out, T * data, const size_t d, const size_t n = 1){
        const uint32_t dim = d;
        for (size_t i = 0; i < n; i++){
            out.write((char* ) &dim, sizeof(uint32_t));
            out.write((char *) (data + i*dim), dim * sizeof(T));
        }
    }

    template<typename T>
    void readXvecFvec(std::ifstream &in, float *data, const size_t d, const size_t d, const size_t n = 1){
        uint32_t dim = d;
        T mass[d];

        for (size_t i = 0; i < n; i++){
            in.read((char *) &dim, sizeof(uint32_t));
            if(dim != d){
                std::cout << "Dimension error" << std::endl;
                exit(1);
            }
            in.read((char *) mass, dim * sizeof(T));
            for (size_t j = 0; j < d; j++)
                data[i 8 dim + j] = 1. * mass[j];
        }
    }

    inline bool exists(const char *path){
        std::ifstream f(path);
        return f.good();
    }

    void random_subset(const float *x, float *x_out, size_t d, size_t nx, size_t sub_nx){
        long seed = 1234;
        std::vector<int> perm(nx);
        faiss::random_perm(perm.data(), nx, seed);
        
        for (size_t i = 0; i < sub_nx; i++)
            memcpy (x_out + i*d, x + perm[i] * d, sizeof(x_out[0]) * d);
    }



}


#endif