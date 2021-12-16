#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <sstream>
#include <string>
#include <sstream>
#include <math.h>
#include <bits/stdc++.h>

#include "CurveElement.h"

using namespace std;

double min_3(double x, double y, double z)
{
    return std::min(std::min(x, y), z);
}

double pointL2(double x1, double y1, double x2, double y2)
{
    double temp;
    double square;
    double distance = 0.0;
    
    temp =  x1 - x2;// this->arrayVectorElement[i] - q->arrayVectorElement[i];
    square = pow(temp, 2.0);
    distance += square;
    
    temp = y1 - y2;
    square = pow(temp, 2.0);
    distance += square;
    

    return sqrt(distance);
}

//void discreteFrechet(double **table, int i, int j, vector<tuple<int,double>>& v1, vector<tuple<int,double>>& v2) //how_many_columns -1 
void discreteFrechet(double **table, int i, int j, vector<tuple<unsigned int,double>>& v1, vector<tuple<unsigned int,double>>& v2) //how_many_columns -1 
{
    //double table[i][j]{-1.0};

    double x1 = get<0>(v1[0]);
    double y1 = get<1>(v1[0]);
    double x2 = get<0>(v2[0]);
    double y2 = get<1>(v2[0]);
    double c = pointL2(x1,y1,x2,y2); //abs(v1[0] - v2[0]);
    
    
    for (int l = 0; l < j; l++)
    {
       
        double x1 = get<0>(v1[0]);
        double y1 = get<1>(v1[0]);
        double x2 = get<0>(v2[l]);
        double y2 = get<1>(v2[l]);
        double temp = pointL2(x1,y1,x2,y2); //abs(v1[0] - v2[l]);
        
        if (l == 0)
        { //first row first column
            table[0][0] = c;
        }else{
            table[0][l] = max(table[0][l - 1], temp); //first row (except the first column)
        }
        
    }

    // for (int k = 0; k < j; k++)
    // {
       
    //     double x1 = get<0>(v1[k]);
    //     double y1 = get<1>(v1[k]);
    //     double x2 = get<0>(v2[0]);
    //     double y2 = get<1>(v2[0]);
    //     double temp = pointL2(x1,y1,x2,y2); //abs(v1[0] - v2[l]);
        
    //     if (k == 0)
    //     { //first row first column
    //         table[0][0] = c;
    //     }else{
    //         table[k][0] = max(table[k - 1][0], temp); //first row (except the first column)
    //     }
        
    // }

    for (int row = 1; row < i; row++) //for the rest of the table(second row every column)
    {
        for (int col = 0; col < j; col++)
        {
            if (col == 0)
            {
                double x1 = get<0>(v1[row]);
                double y1 = get<1>(v1[row]);
                double x2 = get<0>(v2[0]);
                double y2 = get<1>(v2[0]);
                double temp = pointL2(x1,y1,x2,y2); //abs(v1[row] - v2[0]);
                
                table[row][col] = max(table[row - 1][1], temp);
            } //for every row the first column
            

            double x1 = get<0>(v1[row]);
            double y1 = get<1>(v1[row]);
            double x2 = get<0>(v2[col]);
            double y2 = get<1>(v2[col]);
            double temp2 = pointL2(x1,y1,x2,y2); //abs(v1[row] - v2[col]);
            
            table[row][col] = max(min_3(table[row - 1][col], table[row - 1][col - 1], table[row][col - 1]), temp2);
        }
    }

    // for (int r1 = 0; r1 < i; r1++)
    // {
    //     for (int c1 = 0; c1 < i; c1++)
    //     {
    //         myLogFile << "table: " << table[r1][c1] << " ";
    //     }
    //     myLogFile << "row end" << endl;
    // }

    //cout << "Discrete Frechet Distance is: " << table[i - 1][j - 1] << endl;
    
    //return table;//[i-1][j-1];
}

//double ret_DFD(int i, int j, vector<tuple<int,double>>& v1, vector<tuple<int,double>>& v2){
double ret_DFD(int i, int j, vector<tuple<unsigned int,double>>& v1, vector<tuple<unsigned int,double>>& v2){


    double **table = new double*[i];
    for (int row = 0; row < i; row++) table[row] = new double[j];

    discreteFrechet(table,i,j,v1,v2);         
    double dfd = table[i-1][j-1];

    for (int row = 0; row < i; row++) delete[] table[row];
    delete[] table;

    return dfd;

}

int minIndex(double x, double y, double z)
{
    double temp;
    // cout << "read x y z: " << x << " " << y << " " << z << endl;
    temp = std::min(std::min(x, y), z);
    if (temp == x)
    {
        // cout << "smallest value: " << x << endl;
        return 0;
    }
    if (temp == y)
    {
        // cout << "smallest value: " << y << endl;
        return 1;
    }
    if (temp == z)
    {
        // cout << "smallest value: " << z << endl;
        return 2;
    }

    return -1;
}

//void optimalTraversal(vector<tuple <int, double,int, double > >& traversal, double **table, int m1, int m2, std::vector<std::tuple<int,double>>& v1, std::vector<std::tuple<int,double>>& v2){
void optimalTraversal(vector<tuple <unsigned int, double,unsigned int, double > >& traversal, double **table, int m1, int m2, std::vector<std::tuple<unsigned int,double>>& v1, std::vector<std::tuple<unsigned int,double>>& v2){


    int index;
    int Pi = m1 - 1;
    int Qi = m2 - 1;
    
    traversal.insert(traversal.begin(), {get<0>(v1[Pi]), get<1>(v1[Pi]),get<0>(v2[Qi]),get<1>(v2[Qi])});
    
    // for (int i = 0; i < m1; i++)
    // {
    //     for (int j = 0; j < m2; j++)
    //     {
    //         cout << "optimal table " << i << ' ' << j << ' ' << table[i][j] << endl;
    //     }
    // }
    
    //cout << get<0>(v1[(0)]) << "  " << get<1>(v1[0]) << "  " << get<0>(v2[0]) << "  " << get<1>(v2[0]) << "  " << endl;

    while (Pi != 0 && Qi != 0)
    {
        int check1 = Pi;
        int check2 = Qi;
        index = minIndex(table[Pi - 1][Qi], table[Pi][Qi - 1], table[Pi - 1][Qi - 1] );
        // myLogFile << "id0, id1, id2 " << table[check1 - 1][check2] <<" "<< table[check1][check2 - 1] << " " << table[check1 - 1][check2 - 1] << endl;
        // myLogFile << "index is: " << index << endl;
        // myLogFile << "pi, qi "<< Pi <<" " << Qi << endl;
        
        //cout << get<0>(v1[(--check1)]) << "  " << get<1>(v1[--check1]) << "  " << get<0>(v2[--check2]) << "  " << get<1>(v2[--check2]) << "  " << endl;
        if (index == 0)
        {
            // traversal.insert(traversal.begin(), {--Pi, Qi});
            int P_dec = --Pi;
            traversal.insert(traversal.begin(), {get<0>(v1[P_dec]), get<1>(v1[P_dec]),get<0>(v2[Qi]),get<1>(v2[Qi])});
        }
        else if (index == 1)
        {
            int Q_dec = --Qi;
            traversal.insert(traversal.begin(), {get<0>(v1[Pi]), get<1>(v1[Pi]),get<0>(v2[Q_dec]),get<1>(v2[Q_dec])});
        }
        else if (index == 2)
        {
            int P_dec = --Pi;
            int Q_dec = --Qi;
            traversal.insert(traversal.begin(), {get<0>(v1[P_dec]), get<1>(v1[P_dec]),get<0>(v2[Q_dec]),get<1>(v2[Q_dec])});
        }
        else
        {
            cout << "sth went really wrong" << endl;
        }
    }

    //cout << "here" << endl;


    //for (const auto &i : traversal)
    //    cout << get<0>(i) << "  " << get<1>(i) << "  " << get<2>(i) << "  " << get<3>(i) << "  " << endl;

    //myLogFile << traversal.size() << endl;

}

CurveElement* MeanCurve(CurveElement* P, CurveElement* Q){


    if (Q==NULL){
        return P;
    }

    if (P == NULL && Q == NULL){
        return NULL;
    }

    int i = P->arrayElementTwoD.size();
    
    double **table = new double *[i];
    for (int row = 0; row < i; row++) table[row] = new double[i];

    discreteFrechet(table,i,i,P->arrayElementTwoD,Q->arrayElementTwoD);         

    //vector<tuple < int, double, int, double > > traversal;
    vector<tuple < unsigned int, double, unsigned int, double > > traversal;
    optimalTraversal(traversal,table,i,i,P->arrayElementTwoD,Q->arrayElementTwoD);
    //myLogFile << traversal.size() << endl;

    CurveElement* Mean_Curve = new CurveElement(traversal);
    
    for (int row = 0; row < i; row++) delete[] table[row];
    delete[] table;


    //vector< <tuple <unsigned int, double> > v = Mean_Curve->arrayElementTwoD;
    double e = 0.0;

    while (Mean_Curve->arrayElementTwoD.size() != i){

        //e++;
        e = e + 0.001;

        auto it = Mean_Curve->arrayElementTwoD.begin();
        while (it != Mean_Curve->arrayElementTwoD.end()){

            if (it >= Mean_Curve->arrayElementTwoD.end() || it+1 >= Mean_Curve->arrayElementTwoD.end() || it+2 >= Mean_Curve->arrayElementTwoD.end()){
                break;
            }
            
            double a = get<1>(*it);
            double b = get<1>(*(it+1));
            double c = get<1>(*(it+2));

            // myLogFile << "a b c "<< a << " " << b << " "<< c << endl;

            double res1 = abs(a-b);
            double res2 = abs(b-c);

            //it = it+3;
            if (res1 <= e && res2 <= e){
                Mean_Curve->arrayElementTwoD.erase(it+1);
                if (Mean_Curve->arrayElementTwoD.size()==i){
                    break;
                }
            }else{
                it = it+1;
            }

        }

    }

    //Mean_Curve->arrayElementTwoD = v;

    //cout << Mean_Curve->arrayElementTwoD.size() << endl;

    return Mean_Curve;

}

