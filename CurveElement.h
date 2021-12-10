#ifndef CURVEELEMENT_H
#define CURVEELEMENT_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <bits/stdc++.h>

class CurveElement
{
public:
    std::string id; //name/id of the element
    int size;
    std::vector<std::tuple<int,double>> arrayElementTwoD;
    std::vector<double>  arrayElementOneD;
    
    std::vector<std::tuple<int,double>> gridElementTwoD;
    std::vector<double>  gridElementOneD;

    std::vector<double>  filteredElementOneD;

    double distanceCurrQ;
    unsigned int *QueryTrickid;
    double silhouette_score;
    bool assigned;
    std::list<int> assigned_clusters;

public:
    CurveElement(int, std::string, int,std::string);
    ~CurveElement();
    void setDistance(int x)
    {
        distanceCurrQ = x;
    }
    void setDistanceRandom();
    void displayId();
    void displayVectorElementArray();
    void displayVectorElementGrid();
    void displayDistanceCurrQ();
    void getL2Distance(CurveElement *);
};



#endif /* CURVEELEMENT_H */