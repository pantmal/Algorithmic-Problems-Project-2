
#ifndef KMEANS_H
#define KMEANS_H


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Cluster.h"
#include "VectorElement.h"
#include "HyperCube.h"
#include "LSHash.h"

//KMeans clss
class KMeans{

    public:

        int clusters;
        Cluster** ClusterArray;
        string assigner;
        string updater;

        LSHash **KMeans_Hash_Array;
        int* r_array;
        int hashes;
        int k_arg;
        int w_arg;
        
        HyperCube* KMeans_Hyper;

    public:

        KMeans(string, string, int);
        void initialization(VectorElement** , int );
        void ClassicAssignment(VectorElement**, int);
        void ReverseAssignment(VectorElement**, int );
        void update_vec(int);
        double silhouette(int);
        ~KMeans();

};


#endif /* CLUSTERRUNNER_H */