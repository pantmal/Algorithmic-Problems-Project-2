#include "IdDistancePair.h"
#include <iostream>
#include <string>

using namespace std;

//Functions for the idDistancePair class used during output.

bool idDistancePair::operator==(idDistancePair &Pair1)
{
    if (this->getId() != Pair1.getId())
        return false;
    return true;
}

idDistancePair::idDistancePair(string x, double y)
{
    this->id = x;
    this->distance = y;
}

void idDistancePair::setId(string x)
{
    id = x;
}

void idDistancePair::setDistance(int x)
{
    id = x;
}

string idDistancePair::getId()
{
    return id;
}

double idDistancePair::getDistance()
{
    return distance;
}