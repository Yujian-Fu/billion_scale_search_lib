#include<iostream>
#include<fstream>
#include<vector>

typedef uint8_t idx_t;


template<typename T>
void PrintVector(T *data, const size_t dimension, const size_t num_vector){
    for (size_t i= 0; i < 10 * dimension; i++)
    {
        std::cout << (T)data[i];
        if (!i)
        {
            std::cout << " ";
            continue;
        }

        if (i % (dimension - 1) != 0)
        {
            std::cout << " ";
        }
        else
        {
            std::cout << std::endl;
        }
    }
}


template<typename T>
void readXvec(std::ifstream & in, T *data, const size_t dimension, const size_t num_vector = 1){
    size_t dim = dimension;
    //in.read((char *) &dim, sizeof(uint32_t));
    //std::cout << "Print the dim " << dim << std::endl;
    
    std::cout << "In ReadXvec function" << std::endl;
    std::ifstream in(path, std::ios::binary);
    std::cout << "The type size is " << sizeof(T) << std::endl;
    std::cout << "The file is pointing to " << (size_t) in.tellg() << std::endl;
    for (size_t i = 0; i < 1000; i++){
        in.read((char *) &dim, sizeof(uint32_t));
        if (dim != dimension){
            std::cout << dim << " dimension error \n";
            exit(1);
        }
        in.read((char *) (data + i * dim), dim*sizeof(T));
    }
    PrintVector(data, dimension, num_vector);
    in.close();
}

/// Write fvec/ivec/bvec format vectors
template<typename T>
void writeXvec(std::ofstream &out, T *data, const size_t d, const size_t n = 1)
{
    const uint32_t dim = d;
    for (size_t i = 0; i < n; i++) {
        out.write((char *) &dim, sizeof(uint32_t));
        out.write((char *) (data + i * dim), dim * sizeof(T));
    }
}

/// Read fvec/ivec/bvec format vectors and convert them to the float array
template<typename T>
void readXvecFvec(const char* path, float *data, const size_t dimension, const size_t num_vector = 1)
{
    std::ifstream in(path, std::ios::binary);
    uint32_t dim = dimension;
    T mass[dimension];
    for (size_t i = 0; i < num_vector; i++) {
        in.read((char *) &dim, sizeof(uint32_t));
        //std::cout << dim << std::endl;
        if (dim != dimension) {
            std::cout << "file error\n";
            exit(1);
        }
        in.read((char *) mass, dim * sizeof(T));
        for (size_t j = 0; j < dimension; j++){
            data[i * dim + j] = 1. * mass[j];
            std::cout << data[i * dim + j] << " " << mass[j] << " ";
        }
    }
    //PrintVector(data, dimension, num_vector);
    in.close();
}


int main(){
    std::cout <<"Reading dataset file" << std::endl;
    const char *path = "/home/y/yujianfu/ivf-hnsw/data/SIFT1B/bigann_base.bvecs";
    std::ifstream in(path, std::ios::binary);
    size_t num_vector = 100000000;
    size_t dimension = 128;

    in.read((char *) &dimension, sizeof(uint32_t));
    std::cout << "Print the dim " << dimension << std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t) ss;    
    num_vector = (unsigned) (fsize / (dimension + 4) / sizeof(uint8_t));

    
    std::vector<idx_t> massQA(num_vector * dimension);
    
    std::cout << "Print the fsize " << fsize << std::endl;
    std::cout << "Print the num " << num_vector <<  std::endl;
    //****************
    // Testing read file from ive-hnsw
    //****************
    readXvec<idx_t>(in, massQA.data(), dimension, num_vector);


    //std::vector<float> massTrain(num_vector * dimension);
    //readXvecFvec<idx_t>(path, massTrain.data(), dimension, num_vector);
    in.close();

    
}