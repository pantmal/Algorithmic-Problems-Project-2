#include <iostream>
#include <fstream>
#include "Helpers.h"
#include <list>
#include <vector>
#include <sstream>
#include <string>
#include <sstream>
using namespace std;

//euclidean_mod used for LSH amplified hash function.
int euclidean_mod(int a, unsigned int b){
  int r = a % b;
  return r >= 0 ? r : r + b; 
}

//Setting a number to a binary string representation. Needed for Hypercube.
string getBinaryString(int num, int bits) {
    vector<char> digits(bits);
    for (int i = 0; i < bits; ++i) {
      digits.push_back(num % 2 + '0');
      num >>= 1;  
    }
    return string(digits.rbegin(), digits.rend());
}

//Hamming distance on string vars.
int hammingDistance(string str1, string str2){
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
int binarySearch(double array[], double target, int size){
    
    if (target <= array[0]) return 0;
    
    
    if (target >= array[size - 1]) return size - 1;
 
    
    int i = 0, j = size, mid = 0;
    while (i < j) {

        mid = (i + j) / 2;
 
        if (array[mid] == target) return mid;
 
        if (target < array[mid]) {
 
            if (mid > 0 && target > array[mid - 1]) return mid;
 
            j = mid;
        }
 
        
        else {
            if (mid < size - 1 && target < array[mid + 1]) return mid+1;
        
            i = mid + 1;
        }
    }
 
    return mid;
}

//The rest are debug stuff.
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

std::ofstream myLogFile;