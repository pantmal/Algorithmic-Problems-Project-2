
#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>

#include "VectorElement.h"
#include "CurveElement.h"

using namespace std;

//Cluster class which keeps the centroid vector/curve and a list with its elements.
class Cluster
{

public:
    int id;
    int size;
    
    VectorElement* centroid;
    CurveElement* centroid_frechet;
    list<VectorElement*> cluster_elements;
    list<CurveElement*> frechet_elements;
    double silhouette_cluster;

public:

    Cluster(VectorElement*, int);
    Cluster(CurveElement*, int);

};

#endif /* CLUSTER_H */