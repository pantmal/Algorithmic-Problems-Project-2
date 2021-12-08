#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Cluster.h"
#include "VectorElement.h"

//Cluster constructor sets the centroid field and its id (the counter during its creation).
Cluster::Cluster(VectorElement* centroid_arg, int id_arg){


    this->centroid = centroid_arg;
    this->id = id_arg;

    silhouette_cluster = 0.0;

}