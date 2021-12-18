#include <cmath>

#include "VectorElement.h"
#include "Helpers.h"

using namespace std;

//std::ofstream myLogFile;

VectorElement::VectorElement(int dimensions, std::string fileLine, int numberOfHashTables)
{

    assigned = false;

    QueryTrickid = new unsigned int[numberOfHashTables]; //gets set when the vector element gets into an LSH bucket
    
    id = "0";
    double temp;
    int counter = 0;
    distanceCurrQ = 0;
    
    size = dimensions;
    arrayVectorElement = new double[size];
    
    std::stringstream sso(fileLine);
    sso >> id; //Get the id
    while (sso >> temp) //And the vector values in the array field
    {
        arrayVectorElement[counter] = temp;
        counter++;
    }
}

//Debug method
void VectorElement::displayId()
{
    // myLogFile << "id is: " << id << std::endl;
}

//Displaying each column/value of the vector
void VectorElement::displayVectorElementArray()
{
    // myLogFile << "---ELEMENT ARRAY---" << std::endl;
    for (int i = 0; i < size; i++)
    {
        myLogFile << arrayVectorElement[i] << ", ";
    }
    
}


//Debug method
void VectorElement::displayDistanceCurrQ()
{
    cout << distanceCurrQ << endl;
}

//Debug method
void VectorElement::setDistanceRandom()
{
    unsigned long x;
    x = rand();
    x <<= 15;
    x ^= rand();
    x %= 10000000;
    distanceCurrQ = x + 1;
}

//Getting the L2 distance with the q/argument specified and setting it to the field.
void VectorElement::getL2Distance(VectorElement *q)
{
    double temp;
    double square;
    double distance = 0.0;
    for (int i = 0; i < size; i++)
    {
        temp = this->arrayVectorElement[i] - q->arrayVectorElement[i];
        square = pow(temp, 2.0);
        distance += square;
    }

    this->distanceCurrQ = sqrt(distance);
}

//Destructor
VectorElement::~VectorElement()
{

    delete[] arrayVectorElement;
    delete[] QueryTrickid;
}