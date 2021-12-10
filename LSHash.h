
#ifndef LSHASH_H
#define LSHASH_H

#include <list>
#include <ostream>
#include <iostream>

#include "VectorElement.h"
#include "Helpers.h"
#include "Neighbours.h"

using namespace std;

//LSHash class
class LSHash
{
public:
    static int hashNumber;
    int hashTableNumber;
    int buckets; 

    list<VectorElement *> *table;
    neighboursInfo **neighboursInfoTable;
    double **array_of_v;
    double *array_of_t;
    int query_rows_field;

    int k_arg;
    int w_arg;


    bool cluster_mode;
    int current_cluster;
    int assigned_total;
    list<VectorElement * > range_list;

    double t;

    int temp;

public:
 
    void calculateDistanceAndFindN(VectorElement *, int *r_array, int j, int N, string algorithm);
    void displayNeighbours(int);
    void RangeSearch(VectorElement*, int*, int, double);

public:
    LSHash(int V, int v_size, int, int);

    void initNeighboursInfo(int query_rows, int N);
    void DeleteNeighborsInfo();

    void insertItem(VectorElement *x, int *r_array);
    static bool cmp(VectorElement *L, VectorElement *R)
    {
        return L->distanceCurrQ < R->distanceCurrQ;
    }

    static bool cmpDFD(VectorElement *L, VectorElement *R)
    {
        return L->currentDFD < R->currentDFD;
    }

    int AmplifiedHashFunction(VectorElement *x, int *r_array);
    void displayHash();

    ~LSHash();
};

#endif /* LSHASH_H */
