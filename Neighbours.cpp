#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>

#include "Neighbours.h"
#include "Helpers.h"

//Constructor and destructor for neighboursInfo class

neighboursInfo::neighboursInfo(int N)
{
    //std::cout << "getting neighbors del" << std::endl;
    this->arrayId = new std::string[N];
    this->arrayDistance = new double[N]{-1.0};
    //std::cout << "getting neighbors del2" << std::endl;
}

neighboursInfo::~neighboursInfo()
{
    // for (int i = 0; i < 1; i++)
    // {
    //     delete arrayId;
    //     delete arrayDistance;
    // }

    //std::cout << "getting neighbors del" << std::endl;
    delete[] arrayId;
    delete[] arrayDistance;
    //std::cout << "getting neighbors del2" << std::endl;
}

using namespace std;