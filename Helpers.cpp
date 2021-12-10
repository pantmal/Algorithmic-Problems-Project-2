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

double discreteFrechet(int i, int j, vector<tuple<int,double>> v1, vector<tuple<int,double>> v2) //how_many_columns -1 
{
    double table[i][j]{-1.0};

    double x1 = get<0>(v1[0]);
    double y1 = get<1>(v1[0]);
    double x2 = get<0>(v2[0]);
    double y2 = get<1>(v2[0]);
    double c = pointL2(x1,y1,x2,y2); //abs(v1[0] - v2[0]);
    
    
    for (int l = 0; l < j; l++)
    {
        if (l == 0)
        { //first row first column
            table[0][0] = c;
            break;
        }

        double x1 = get<0>(v1[0]);
        double y1 = get<1>(v1[0]);
        double x2 = get<0>(v2[l]);
        double y2 = get<1>(v2[l]);

        double temp = pointL2(x1,y1,x2,y2); //abs(v1[0] - v2[l]);
        table[0][l] = max(table[0][l - 1], temp); //first row (except the first column)
    }

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

                double temp =  pointL2(x1,y1,x2,y2); //abs(v1[row] - v2[0]);
                table[row][col] = max(table[row - 1][1], temp);
            } //for every row the first column
            

            double x1 = get<0>(v1[row]);
            double y1 = get<1>(v1[row]);
            double x2 = get<0>(v2[col]);
            double y2 = get<1>(v2[col]);

            double temp2 =  pointL2(x1,y1,x2,y2); //abs(v1[row] - v2[col]);
            
            
            table[row][col] = max(min_3(table[row - 1][col], table[row - 1][col - 1], table[row][col - 1]), temp2);
        }
    }

    // for (int r1 = 0; r1 < i; r1++)
    // {
    //     for (int c1 = 0; c1 < i; c1++)
    //     {
    //         //cout << "table: " << table[r1][c1] << " ";
    //     }
    //     //cout << endl;
    // }

    //cout << "Discrete Frechet Distance is: " << table[i - 1][j - 1] << endl;
    
    return table[i-1][j-1];
}

std::ofstream myLogFile;
