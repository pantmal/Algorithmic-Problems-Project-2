#ifndef HELPERS_H
#define HELPERS_H
#include <list>
#include "IdDistancePair.h"
#include <string>

//Output file is set to extern because it helps when we want to output stuff from many different files.
extern std::ofstream myLogFile;

//Declaring helper funcs

int euclidean_mod(int a, unsigned int b);
std::string getBinaryString(int num, int bits);
int hammingDistance(std::string str1, std::string str2);
int binarySearch(double array[], double target, int size);
void coutLineWithMessage(std::string m = "");
void coutLineWithMessage(int, std::string m = "");
void logFileLineWithMessage(std::string m = "");
bool cmpListPair(idDistancePair &, idDistancePair &);
bool cmpListPairR(idDistancePair &, idDistancePair &);
int string_into_int(std::string &s);
int args_string_to_int(std::string &x);
double args_string_to_double(std::string &x);
double min_3(double x, double y, double z);
double discreteFrechet(int i, int j, std::vector<std::tuple<int,double>> v1, std::vector<std::tuple<int,double>> v2);
int testL2();

#endif /* HELPERS_H */