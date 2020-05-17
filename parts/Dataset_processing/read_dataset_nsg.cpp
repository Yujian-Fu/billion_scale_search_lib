void read_fvecs(const char* filename, float* &data, unsigned &num_points, unsigned &dim)
{
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open())
    {
        std::cout << "file open error" << std::endl;
        exit(-1);
    }

    infile.read((char*)&dim, 4);
    infile.seekg(0, std::ios::end);
    std::ios::pos_type ss = infile.tellg();
    size_t fsize = (size_t) ss;
    num_points =  (unsigned)(fsize / (dim + 1) /4);
    data = new float[(size_t)num_points * (size_t)dim];

    infile.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num_points; i++)
    {
        infile.seekg(4, std::ios::cur);
        infile.read((char*)(data + i*dim), dim * 4);
    }

    for (size_t i= 0; i < num_points * dim; i++)
    {
        std::cout << (float)data[i];
        if (!i)
        {
            std::cout << " ";
            continue;
        }

        if (i % (dim - 1) != 0)
        {
            std::cout << " ";
        }
        else
        {
            std::cout << std::endl;
        }
    }

    infile.close();
}

int main(){
    //*****************
    // Testing read file from NSG
    //*****************
    const char *path_gt = "/home/yujian/Downloads/similarity_search_datasets/ANN_SIFT1M/sift_base.fvecs";

    float* data;
    unsigned num_point;
    unsigned dimension;
    read_fvecs(path_gt, data, num_point, dimension);
}