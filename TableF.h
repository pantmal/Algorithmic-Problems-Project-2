
#ifndef TABLEF_H
#define TABLEF_H

#include <list>
#include <ostream>
#include <iostream>

// #include "VectorElement.h"
// #include "Helpers.h"
// #include "Neighbours.h"

using namespace std;

//TableF class
class TableF
{

    int buckets; 
    list<int > *table;    
    int temp;

public:
    TableF();

    int checkItem(int);
    void insertItem(int, int );
    void displayHash();

    ~TableF();
};

#endif /* TABLEF_H */
