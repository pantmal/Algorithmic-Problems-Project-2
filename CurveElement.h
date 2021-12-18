#ifndef CURVEELEMENT_H
#define CURVEELEMENT_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <bits/stdc++.h>

//CurveElement class
class CurveElement
{
public:
    std::string id; //name/id of the element
    int size;
    std::vector<std::tuple<double,double>> arrayElementTwoD;
    std::vector<double>  arrayElementOneD;
    
    std::vector<std::tuple<double,double>> gridElementTwoD;
    std::vector<double>  gridElementOneD;

    std::vector<double>  filteredElementOneD;

    double distanceCurrQ;
    unsigned int *QueryTrickid;
    double silhouette_score;
    bool assigned;
    std::list<int> assigned_clusters;

    bool mark_deletion;

public:
    CurveElement(int, std::string, int,std::string);
    CurveElement(std::vector<std::tuple <double, double, double, double > >& traversal);
    ~CurveElement();
    void setDistance(int x)
    {
        distanceCurrQ = x;
    }
    void Snapping2d(double, double, double, int );
    std::string Vectorization2d(int, int);
    void Filtering(double);
    void Snapping1d(double, double);
    void MinMax();
    std::string Vectorization1d(int, int);
    void setDistanceRandom();
    void displayId();
    void displayVectorElementArray2D(); //for clustering
    void displayVectorElementArray();
    void displayVectorElementGrid();
    void displayDistanceCurrQ();
    void getL2Distance(CurveElement *);
};



#endif /* CURVEELEMENT_H */