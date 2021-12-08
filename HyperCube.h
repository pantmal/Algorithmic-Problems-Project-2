
#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <list>
#include <ostream>
#include <iostream>
#include <bits/stdc++.h>

#include "VectorElement.h"
#include "TableF.h"
#include "Neighbours.h"

using namespace std;

//Hypercube class
class HyperCube
{
    int buckets;
    int k_cube;
    int w_cube;

    list<VectorElement *> *table;
    double **array_of_v;
    double *array_of_t;
    TableF **TableOfValuesF;

    int hammingCounter;
    list<int> probesCandidates;
    set<int> visited;

    int query_rows_field;

public:
    int N;
    int Ni;

    int M;
    int Mi;

    int probes;
    int probes_counter;

    int init_index;

    double range;

    bool cluster_mode;
    int current_cluster;
    int assigned_total;

    neighboursInfo **neighboursInfoTable;

    list<VectorElement *> range_list;

    HyperCube(int, int, int, int, int, int, double);
    void initNeighboursInfo(int);
    int functionF(int, int);
    int indexBuilder(VectorElement *x);
    void insertItem(VectorElement *x);
    void getFirstProbe(VectorElement *x, int, string);
    void getNextProbe(VectorElement *x, int, int, string);
    void calculateDistanceAndFindN(VectorElement *x, int, int);
    void RangeSearch(VectorElement *x, int, int);
    static bool cmp(VectorElement *L, VectorElement *R)
    {
        return L->distanceCurrQ < R->distanceCurrQ;
    }
    void displayCube();

    ~HyperCube();
};

#endif /* HYPERCUBE_H */
