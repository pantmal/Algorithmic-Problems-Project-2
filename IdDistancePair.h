#ifndef IDDISTANCEPAIR_H
#define IDDISTANCEPAIR_H

#include <string>

//idDistancePair class used during output.
class idDistancePair
{
    std::string id;
    double distance;

public:
    idDistancePair(std::string, double);
    void setId(std::string);
    void setDistance(int);
    std::string getId();
    double getDistance();
    bool operator==(idDistancePair &);
};

#endif //IDDISTANCEPAIR_H