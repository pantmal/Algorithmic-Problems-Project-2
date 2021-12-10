#include <cmath>
#include <bits/stdc++.h>

#include "CurveElement.h"
#include "Helpers.h"

using namespace std;

//std::ofstream myLogFile;

//Debug method
void CurveElement::displayId()
{
    // myLogFile << "id is: " << id << std::endl;
}

//Displaying each column/value of the vector
void CurveElement::displayVectorElementArray()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    for(const auto &i : arrayElementOneD)
        myLogFile<<i <<endl;

}

void CurveElement::displayVectorElementGrid()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    //for(const auto &i : gridElementTwoD)
        //myLogFile<<get<0>(i)<<"  "<<get<1>(i) <<endl;

    myLogFile << "size " << gridElementTwoD.size() << endl;
}

CurveElement::CurveElement(int dimensions, std::string fileLine, int numberOfHashTables, std::string variant)
{

    assigned = false;

    QueryTrickid = new unsigned int[numberOfHashTables]; //gets set when the vector element gets into an LSH bucket
    
    id = "0";
    double temp;
    int counter = 0;
    distanceCurrQ = 0;
    
    size = dimensions;
   // arrayVectorElement = new double[size];
    
    std::stringstream sso(fileLine);
    sso >> id; //Get the id
    while (sso >> temp) //And the vector values in the array field
    {
        // arrayVectorElement[counter] = temp;
        if (variant == "discrete"){
            arrayElementTwoD.push_back({counter+1,temp});
            gridElementTwoD.push_back({0,0.0});
        }else{
            arrayElementOneD.push_back(temp);
            filteredElementOneD.push_back(0.0);
        }    
        counter++;
    }
    

}

//Debug method
void CurveElement::displayDistanceCurrQ()
{
    cout << distanceCurrQ << endl;
}

//Debug method
void CurveElement::setDistanceRandom()
{
    unsigned long x;
    x = rand();
    x <<= 15;
    x ^= rand();
    x %= 10000000;
    distanceCurrQ = x + 1;
}

//Getting the L2 distance with the q/argument specified and setting it to the field.
void CurveElement::getL2Distance(CurveElement *q)
{
    double temp;
    double square;
    double distance = 0.0;
    for (int i = 0; i < size; i++)
    {
    //    temp = this->arrayVectorElement[i] - q->arrayVectorElement[i];
        square = pow(temp, 2.0);
        distance += square;
    }

    this->distanceCurrQ = sqrt(distance);
}

//Destructor
CurveElement::~CurveElement()
{

   // delete[] arrayVectorElement;
    delete[] QueryTrickid;
}