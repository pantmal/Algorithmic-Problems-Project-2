#include <list>
#include <ostream>
#include <iostream>
#include <chrono>
#include <random>
#include <climits>
#include <algorithm>
#include <list>
#include <vector>
#include <set>

#include "LSHash.h"
#include "VectorElement.h"
#include "Helpers.h"
#include "Neighbours.h"

using namespace std;

int LSHash::hashNumber = 0;

//LSHash constructor
LSHash::LSHash(int b, int v_size, int k_arg, int w_arg)
{

  //Get the k and w
  this->k_arg = k_arg;
  this->w_arg = w_arg;

  hashTableNumber = hashNumber; //Hash number during creation
  hashNumber++;

  //Creating the LSHash table
  this->buckets = b;
  table = new list<VectorElement *>[buckets];

  //Array of v values so they may be reused easily.
  this->array_of_v = new double *[k_arg];
  for (unsigned k = 0; k < k_arg; k++)
  {
    this->array_of_v[k] = new double[v_size];
  }

  unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
  default_random_engine e(seed);
  normal_distribution<double> N(0, 1);

  //Getting values for V with normal dist
  for (unsigned k = 0; k < k_arg; k++)
  {
    double *inner_array = this->array_of_v[k];
    for (unsigned column = 0; column < v_size; column++)
    {
      double n_val = N(e);
      inner_array[column] = n_val;
    }
  }

  //Saving t values too for reuse. With uniform dist
  int open_w = w_arg - 1;
  uniform_real_distribution<> U(0.0, open_w);
  this->array_of_t = new double[k_arg];
  for (int k = 0; k < k_arg; k++)
  {
    double t = U(e);
    this->array_of_t[k] = t;
  }

  //Clustering reverse assignment stuff
  cluster_mode = false;
  current_cluster = 0;
  assigned_total = 0;
}

//Initializing the Neighbours info table for every query.
void LSHash::initNeighboursInfo(int query_rows, int N)
{

  query_rows_field = query_rows;
  this->neighboursInfoTable = new neighboursInfo *[query_rows];
  for (int i = 0; i < query_rows; i++)
  {

    this->neighboursInfoTable[i] = new neighboursInfo(N);
  }
}

//The g function which maps an elem to its bucket
int LSHash::AmplifiedHashFunction(VectorElement *key, int *r_array)
{

  //Get h values
  int *h_array = new int[k_arg];
  for (int k = 0; k < k_arg; k++)
  {

    double *array_start = key->arrayVectorElement;
    double inner_prod = inner_product(array_start, array_start + (key->size), this->array_of_v[k], 0.0);    
    h_array[k] = floor((inner_prod + array_of_t[k]) / w_arg);
  }

  //Now use the array of r vector and get the mod
  int pre_mod = 0;
  unsigned int M = UINT_MAX - 5;
  for (int i = 0; i < k_arg; i++)
  {
    unsigned int euc_mod = euclidean_mod(r_array[i] * h_array[i], M);
    pre_mod += euc_mod;
  }

  //And get the index
  unsigned int final_mod = euclidean_mod(pre_mod, M);
  unsigned int index = euclidean_mod(final_mod, buckets);

  delete[] h_array;

  //Saving the ID used for the 'query trick'
  key->QueryTrickid[hashTableNumber] = final_mod;

  return index;
}

//Finds N nearest neighbors
void LSHash::calculateDistanceAndFindN(VectorElement *q, int *r_array, int j, int N, string algorithm) //j=no of query
{
  int index = AmplifiedHashFunction(q, r_array);

  if (table[index].size() == 0)
  {
    return;
  }

  list<VectorElement *>::iterator hitr1;
  list<VectorElement *>::iterator hitr2;


  if (algorithm == "LSH"){
    //Get the L2 distance of all elements in the bucket with the given query and sort the list based on the distance.
    for (hitr1 = table[index].begin(); hitr1 != table[index].end(); ++hitr1)
    {
      VectorElement *vobj = *hitr1;
      vobj->distanceCurrQ = 0.0;

      //ID for faster query search is disabled here but you may re-enable if you want.
      //if (q->QueryTrickid[hashTableNumber] != vobj->QueryTrickid[hashTableNumber]) continue;

      vobj->getL2Distance(q);
    }
    table[index].sort(cmp);

  }else{

    for (hitr1 = table[index].begin(); hitr1 != table[index].end(); ++hitr1)
    {
      VectorElement *vobj = *hitr1;
      vobj->currentDFD = 0.0;

      CurveElement* input_curve = vobj->original_curve;
      CurveElement* query_curve = q->original_curve;

      int d = input_curve->arrayElementTwoD.size();
      double dfd = discreteFrechet(d-1,d-1,input_curve->arrayElementTwoD,query_curve->arrayElementTwoD);     

      vobj->currentDFD = dfd;
    }
    table[index].sort(cmpDFD);

  }

  

  //Now start collecting neighbors in the neighboursInfoTable and increment the counter
  int Ni = 0;
  for (hitr2 = table[index].begin(); hitr2 != table[index].end(); ++hitr2)
  {
    if (Ni == N)
      break;

    VectorElement *vobj = *hitr2;
    
    if(algorithm == "LSH"){
      neighboursInfoTable[j]->arrayDistance[Ni] = vobj->distanceCurrQ;
    }else{
      neighboursInfoTable[j]->arrayDistance[Ni] = vobj->currentDFD;
    }
    
    neighboursInfoTable[j]->arrayId[Ni] = vobj->id;
    Ni++;
  }
}

//Similar searching for Range
void LSHash::RangeSearch(VectorElement *q, int *r_array, int j, double range) //j=no of query
{

  int index = AmplifiedHashFunction(q, r_array);

  if (table[index].size() == 0)
  {
    return;
  }

  list<VectorElement *>::iterator hitr1;
  list<VectorElement *>::iterator hitr2;

  set<int> visited;

  for (hitr1 = table[index].begin(); hitr1 != table[index].end(); ++hitr1)
  {
    VectorElement *vobj = *hitr1;
    vobj->getL2Distance(q);
  }
  table[index].sort(cmp);

  for (hitr2 = table[index].begin(); hitr2 != table[index].end(); ++hitr2)
  {

    VectorElement *vobj = *hitr2;
    if (vobj->distanceCurrQ <= range)
    {

      if (cluster_mode == true && vobj->assigned == true)
        continue;

      range_list.push_back(vobj);

      if (cluster_mode == true)
      { //Used in clustering Reverse assignment only
        vobj->assigned = true;
        vobj->assigned_clusters.push_back(current_cluster);
        assigned_total++;
      }
    }
  }
}

//Inserting item using the g func
void LSHash::insertItem(VectorElement *key, int *r_array)
{

  int index = AmplifiedHashFunction(key, r_array);
  table[index].push_back(key);
}

//Debug method
void LSHash::displayHash()
{

  for (int i = 0; i < buckets; i++)
  {
    myLogFile << table[i].size() << endl;
    myLogFile << "INDEX: " << i << endl;
    for (auto x : table[i])
    {
      myLogFile << " --> " << endl;
      myLogFile << "id" << x->id << endl;
      x->displayVectorElementArray();
      //x->displayDistanceCurrQ();
    }
  }
  myLogFile << endl;
}

//Debug method
void LSHash::displayNeighbours(int N)
{
  myLogFile << "hash table no: " << this->hashTableNumber << endl;
  for (int i = 0; i < N; i++)
  {
    myLogFile << "id is: " << this->neighboursInfoTable[0]->arrayId[i] << "  "
              << "distance is: " << this->neighboursInfoTable[0]->arrayDistance[i] << endl;
  }
}

//LSHash destructor
LSHash::~LSHash()
{

  for (unsigned k = 0; k < k_arg; k++)
  {
    delete[] this->array_of_v[k];
  }

  delete[] this->array_of_v;
  delete[] this->array_of_t;
  delete[] table;

  if (query_rows_field > 0){
    for (int i = 0; i < query_rows_field; i++)
    {

      //neighboursInfoTable[i]->arrayDistance[0] = 0;
      //neighboursInfoTable[i]->arrayId[0] = "";

      //delete[] neighboursInfoTable[i]->arrayId;
      //delete[] neighboursInfoTable[i]->arrayDistance;

      //delete neighboursInfoTable[i];
      delete neighboursInfoTable[i];
    }

    delete[] neighboursInfoTable;
    // delete neighboursInfoTable;
  }
  
}