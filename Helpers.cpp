#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <sstream>
#include <string>
#include <sstream>
#include <math.h>
#include <bits/stdc++.h>

#include "Helpers.h"
#include "CurveElement.h"

#include "fred/config.hpp"
#include "fred/curve.hpp"
#include "fred/frechet.hpp"
#include "fred/interval.hpp"
#include "fred/point.hpp"
#include "fred/simplification.hpp"
#include "fred/types.hpp"


using namespace std;

//euclidean_mod used for LSH amplified hash function.
int euclidean_mod(int a, unsigned int b)
{
    int r = a % b;
    return r >= 0 ? r : r + b;
}

//Setting a number to a binary string representation. Needed for Hypercube.
string getBinaryString(int num, int bits)
{
    vector<char> digits(bits);
    for (int i = 0; i < bits; ++i)
    {
        digits.push_back(num % 2 + '0');
        num >>= 1;
    }
    return string(digits.rbegin(), digits.rend());
}

//Hamming distance on string vars.
int hammingDistance(string str1, string str2)
{
    int i = 0, count = 0;
    while (str1[i] != '\0')
    {
        if (str1[i] != str2[i])
            count++;
        i++;
    }
    return count;
}

//Binary search to find x in KMeans initialization.
int binarySearch(double array[], double target, int size)
{

    if (target <= array[0])
        return 0;

    if (target >= array[size - 1])
        return size - 1;

    int i = 0, j = size, mid = 0;
    while (i < j)
    {

        mid = (i + j) / 2;

        if (array[mid] == target)
            return mid;

        if (target < array[mid])
        {

            if (mid > 0 && target > array[mid - 1])
                return mid;

            j = mid;
        }

        else
        {
            if (mid < size - 1 && target < array[mid + 1])
                return mid + 1;

            i = mid + 1;
        }
    }

    return mid;
}

//Output for NN search. This is the same in all algorithms. Returns the maf value.
double OutputNN(list<idDistancePair>& PairList, list<idDistancePair>& PairListBF, int NUMBER_OF_NEIGHBOURS, double last_maf){


    int currNeighbours = 0;
    list<idDistancePair>::iterator hitr1 = PairList.begin();
    list<idDistancePair>::iterator hitrbf2 = PairListBF.begin();
    for (; hitr1 != PairList.end() && hitrbf2 != PairListBF.end(); ++hitr1, ++hitrbf2)
    {
        if (currNeighbours == NUMBER_OF_NEIGHBOURS)
            break;

        myLogFile << "Approximate Nearest neighbor-" << (currNeighbours + 1) << ": " << hitr1->getId() << endl;
        myLogFile << "True Nearest neighbor-" << (currNeighbours + 1) << ": " << hitrbf2->getId() << endl;
        myLogFile << "distanceApproximate: " << hitr1->getDistance() << endl;
        myLogFile << "distanceTrue: " << hitrbf2->getDistance() << endl;
        myLogFile << endl;

        if (currNeighbours == 0){
            
            double curr_maf = -1.0;
            if (hitrbf2->getDistance() != 0){
                curr_maf = hitr1->getDistance() / hitrbf2->getDistance();
            }
            
            if (curr_maf > last_maf){
                last_maf = curr_maf;
            }     
        }
        
        currNeighbours++;
    }

    return last_maf;

}

//The rest are debug stuff (except the last one).
int testL2()
{
    unsigned long x;
    x = rand();
    x <<= 15;
    x ^= rand();
    x %= 10000001;
    return x;
}

void coutLineWithMessage(std::string m)
{
    std::cout << "------" << m << "------" << std::endl;
}

void coutLineWithMessage(int k, std::string m)
{
    std::cout << "------" << m + " " << k << "------" << std::endl;
}

void logFileLineWithMessage(std::string m)
{
    myLogFile << "------" << m << "------" << std::endl;
}

bool cmpListPair(idDistancePair &L, idDistancePair &R)
{
    return L.getDistance() < R.getDistance();
}

bool cmpListPairR(idDistancePair &L, idDistancePair &R)
{
    return L.getId() < R.getId();
}

int string_into_int(std::string &s)
{
    stringstream sso(s);
    int x = 0;
    sso >> x;
    return x;
}

int args_string_to_int(std::string &x)
{
    int num;
    x = x.erase(0, x.find("-") + 1);
    num = string_into_int(x);
    return num;
}

double args_string_to_double(std::string &x)
{
    double num;
    x = x.erase(0, x.find("-") + 1);
    num = string_into_int(x);
    return num;
}

//Continuous Frechet used in A3.
double ret_CFD(CurveElement* input, CurveElement* query, bool bf_filter){

  //Getting Points of 1st curve
  Points v1_points(1);
  
  //If bf_filter is set, get points of filtered curve.
  if (bf_filter){
    for (int i = 0; i < input->filteredElementOneD.size(); i++){
      Point p(1); 
      p.set(0,input->filteredElementOneD[i]);
      v1_points.add(p);
    }
  }else{
    for (int i = 0; i < input->arrayElementOneD.size(); i++){
      Point p(1); 
      p.set(0,input->arrayElementOneD[i]);
      v1_points.add(p);
    }
  }

  //Getting Points of 2nd curve. same logic
  Points v2_points(1);

  if (bf_filter){
    for (int i = 0; i < query->filteredElementOneD.size(); i++){
      Point p(1);
      p.set(0,query->filteredElementOneD[i]);
      v2_points.add(p);
    }
  }else{
    for (int i = 0; i < query->arrayElementOneD.size(); i++){
      Point p(1);
      p.set(0,query->arrayElementOneD[i]);
      v2_points.add(p);
    }
  }

  //Init Curve objs
  Curve* c1 = new Curve(v1_points,"input");
  Curve* c2 = new Curve(v2_points,"query");

  //Getting the distance
  Frechet::Continuous::Distance cont_dist = Frechet::Continuous::distance(*c1, *c2);

  //clearup
  delete c1;
  delete c2;

  double dist = cont_dist.value;

  //cout<< "dist is " << dist << endl;

  //Returning
  return dist;

}

std::ofstream myLogFile;
