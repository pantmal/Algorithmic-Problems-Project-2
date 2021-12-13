#ifndef DISCRETEFRECHET_H
#define DISCRETEFRECHET_H
#include <list>
#include <string>

#include "CurveElement.h"

//Output file is set to extern because it helps when we want to output stuff from many different files.
extern std::ofstream myLogFile;

//Declaring helper funcs

double min_3(double x, double y, double z);
void discreteFrechet(double **table, int i, int j, std::vector<std::tuple<unsigned int,double>> v1, std::vector<std::tuple<unsigned int,double>> v2);
double ret_DFD(int i, int j, std::vector<std::tuple<unsigned int,double>> v1, std::vector<std::tuple<unsigned int,double>> v2);
int minIndex(double x, double y, double z);
void optimalTraversal(std::vector<std::tuple <unsigned int, double,unsigned int, double > >& traversal,double **table, int m1, int m2, std::vector<std::tuple<unsigned int,double>> v1, std::vector<std::tuple<unsigned int,double>> v2);
CurveElement* MeanCurve(CurveElement*, CurveElement*);

#endif /* DISCRETEFRECHET_H */