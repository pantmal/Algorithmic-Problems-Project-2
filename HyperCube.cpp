
#include <list>
#include <ostream>
#include <iostream>
#include <chrono>
#include <random>
#include <cmath>
#include <string>
#include <bitset>
#include <bits/stdc++.h>

#include "VectorElement.h"
#include "TableF.h"
#include "HyperCube.h"
#include "Neighbours.h"
#include "Helpers.h"

//Hypercube constructor
HyperCube::HyperCube(int k_arg, int v_size, int w_arg, int N_arg, int M_arg, int probes_arg, double range_arg)
{

  //Every 'i' var is the counter of its respective number (i.e. Ni is counter for N neighbors)
  N = N_arg;
  Ni = 0;

  M = M_arg;
  Mi = 0;

  probes = probes_arg;
  probes_counter = 0;

  range = range_arg;

  hammingCounter = 0;

  w_cube = w_arg;
  k_cube = k_arg;

  //Creating the HyperCube table
  int dims = pow(2, k_arg);
  this->buckets = dims;
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

  //And setting the TableF objects which will save the 0/1 values of all f functions.
  TableOfValuesF = new TableF *[k_arg];
  for (int i = 0; i < k_arg; i++)
  {
    TableOfValuesF[i] = new TableF();
  }

  //Clustering reverse assignment stuff
  cluster_mode = false;
  current_cluster = 0;
  assigned_total = 0;
}

//Initializing the Neighbours info table for every query.
void HyperCube::initNeighboursInfo(int query_rows)
{

  query_rows_field = query_rows;
  this->neighboursInfoTable = new neighboursInfo *[query_rows];

  for (int i = 0; i < query_rows; i++)
  {
    this->neighboursInfoTable[i] = new neighboursInfo(N);
  }
}

//Function f will check its TableF to see if the result of h is already set to 0 or 1 and will return the right bit. Otherwise it creates a new value with uniform dist.
int HyperCube::functionF(int h, int i)
{

  unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
  default_random_engine e(seed);
  uniform_int_distribution<> U(0, 1);

  int checkpos = TableOfValuesF[i]->checkItem(h);

  if (checkpos == -1)
  {
    int bit = U(e);
    TableOfValuesF[i]->insertItem(h, bit);
    return bit;
  }
  else
  {
    //cout << "found index" << endl;
    return checkpos;
  }
}

//Returns the index for the VectorElement provided.
int HyperCube::indexBuilder(VectorElement *key)
{

  string index_s = "";
  int k_arg = k_cube;
  int *h_array = new int[k_arg];
  for (int i = 0; i < k_arg; i++)
  {
    double *array_start = key->arrayVectorElement;
    double inner_prod = inner_product(array_start, array_start + (key->size), this->array_of_v[i], 0.0); //~=[-11,11]
    h_array[i] = floor((inner_prod + array_of_t[i]) / w_cube);
  }

  for (int i = 0; i < k_arg; i++)
  {
    int bit = functionF(h_array[i], i);
    string bit_s = to_string(bit);
    index_s += bit_s;
  }

  //cout << "int " << stoi(index_s,nullptr,2) << endl;

  int index = stoi(index_s, nullptr, 2);

  return index;
}

//Inserts an item to the HyperCube
void HyperCube::insertItem(VectorElement *key)
{

  int index = indexBuilder(key);
  table[index].push_back(key);
}

//Used for the query searching. It will get the index value it belongs to and will start searching.
void HyperCube::getFirstProbe(VectorElement *key, int j, string search)
{

  probesCandidates.clear();
  visited.clear();
  hammingCounter = 0;
  Ni = 0;
  Mi = 0;
  probes_counter = 0;

  int index = indexBuilder(key);

  init_index = index;

  if (search == "NN")
  {
    calculateDistanceAndFindN(key, j, init_index);
  }
  else
  {
    RangeSearch(key, j, init_index);
  }
}

//Gets next probe using hamming dist
void HyperCube::getNextProbe(VectorElement *key, int j, int index, string search)
{

  //If probesCandidates list is empty we will get new hamming dist nearest probes.
  if (probesCandidates.empty())
  {

    hammingCounter++; //Add the counter every time you update the list. First time, it will have probes of hamming dist 1, next time hamming dist 2 etc

    for (int bucket = 0; bucket < buckets; bucket++)
    {
      if (index == bucket)
        continue;

      if (!visited.empty())
      {
        bool visit_check = visited.find(bucket) != visited.end(); //Skip hamming dist nearest probes if already calculated.
        if (visit_check)
          continue;
      }

      //Getting string format of the bucket nums
      string index_s = getBinaryString(index, k_cube);
      string bucket_s = getBinaryString(bucket, k_cube);

      int dist = hammingDistance(index_s, bucket_s);

      //Add the candidate if the distance is on the right track.
      if (dist == hammingCounter)
      {
        probesCandidates.push_back(bucket);
      }
    }

    //Remember the visited probes
    for (list<int>::iterator i = probesCandidates.begin(); i != probesCandidates.end(); i++)
    {
      int item = *i;
      visited.insert(item);
    }
  }

  //Now pick a random probe from the list using uniform dist
  unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
  default_random_engine e(seed);
  uniform_int_distribution<> U(0, probesCandidates.size() - 1);
  int random_element = U(e);

  //Find the item in the list and remove it. We only check each probe once.
  list<int>::iterator it = probesCandidates.begin();
  advance(it, random_element);
  int item = *it;
  probesCandidates.erase(it);

  //Make the next search on the item(bucket) you picked.
  if (search == "NN")
  {
    calculateDistanceAndFindN(key, j, item);
  }
  else
  {
    RangeSearch(key, j, item);
  }
}

//Finds N nearest neighbors
void HyperCube::calculateDistanceAndFindN(VectorElement *q, int j, int index)
{

  if (table[index].size() == 0)
  {
    return;
  }

  list<VectorElement *>::iterator hitr1;
  list<VectorElement *>::iterator hitr2;

  //Get the L2 distance of all elements in the bucket with the given query and sort the list based on the distance.
  for (hitr1 = table[index].begin(); hitr1 != table[index].end(); ++hitr1)
  {
    VectorElement *vobj = *hitr1;
    vobj->getL2Distance(q);
  }
  table[index].sort(cmp);

  //Now start collecting neighbors in the neighboursInfoTable and increment the counters
  for (hitr2 = table[index].begin(); hitr2 != table[index].end(); hitr2++)
  {

    if (Ni == N)
    {
      break;
    }

    if (Mi == M)
    {
      break;
    }

    VectorElement *vobj = *hitr2;
    neighboursInfoTable[j]->arrayDistance[Ni] = vobj->distanceCurrQ;
    neighboursInfoTable[j]->arrayId[Ni] = vobj->id;

    Ni++;
    Mi++;
  }

  probes_counter++;

  //First check if N is reached, next check for M argument and finally for probes. Also make sure that you won't exceed bucket size.
  if (Ni < N)
  {
    if (Mi != M)
    {
      if (probes_counter != probes && probes_counter != buckets)
      {
        getNextProbe(q, j, init_index, "NN");
      }
    }
  }
}

//Similar searching for Range
void HyperCube::RangeSearch(VectorElement *q, int j, int index)
{

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

      if (Mi == M)
        break;

      if (cluster_mode == true && vobj->assigned == true)
        continue;

      range_list.push_back(vobj);

      if (cluster_mode == true)
      { //Used in clustering Reverse assignment only
        vobj->assigned = true;
        vobj->assigned_clusters.push_back(current_cluster);
        assigned_total++;
      }

      Mi++;
    }
  }

  probes_counter++;

  if (Mi != M)
  {
    if (probes_counter != probes && probes_counter != buckets)
    {
      getNextProbe(q, j, init_index, "range");
    }
  }
}

//Debug method
void HyperCube::displayCube()
{
  for (int i = 0; i < buckets; i++)
  {
    cout << "INDEX: " << i << endl;
    for (auto x : table[i])
    {
      cout << " --> " << x << endl;
      x->displayVectorElementArray();
    }
  }
}

//HyperCube destructor
HyperCube::~HyperCube()
{

  for (unsigned k = 0; k < k_cube; k++)
  {
    delete[] this->array_of_v[k];
  }

  delete[] this->array_of_v;
  delete[] this->array_of_t;

  for (int i = 0; i < k_cube; i++)
  {
    delete TableOfValuesF[i];
  }
  delete[] TableOfValuesF;

  delete[] table;

  if (N > 0){
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
  }
  
}
