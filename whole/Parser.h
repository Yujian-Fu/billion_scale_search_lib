#ifndef PARSER_H
#define PARSER_H

#include <cstring>
#include <iostream>

// *****************
// The Parser Class for
// parsing the input
// *****************

struct Parser
{
    size_t M;
    size_t efConstruction;


    size_t num_query;             // number of query
    size_t num_gt;                // number of groundtruth per query
    size_t d;                     // the dimension of query vector

    size_t num_centroid;
    size_t code_size;
    size_t nsubc;
    size_t num_train; 
    size_t num_sub_train;
    size_t num_base;

    bool do_opq;


    const char *path_index;
    const char *path_opq_matrix;
    const char *path_pq;
    const char *path_norm_pq;
    const char *path_base;
    const char *path_learn;
    const char *path_query;
    const char *path_gt;
    const char *path_centroids;
    const char *path_info;
    const char *path_edges;
    const char *path_precomputed_idxs;


};

#endif