#include <list>
#include <ostream>
#include <iostream>

#include "TableF.h"

//TableF has two buckets, one for the values that got 0 and the ones that got 1
TableF::TableF(){

  this->buckets = 2;
  table = new list<int >[buckets];
    
}

//Check if the item is already in the table return its pos (meaning its bucket (meaning 0 or 1))
int TableF::checkItem(int item){

    int pos = -1;
    for (int i = 0; i < buckets; i++){
        list<int>::iterator hitr1;
        
        for (hitr1= table[i].begin();hitr1 != table[i].end(); hitr1++) {
            if (*hitr1 == item){
                pos = i;
                break;
            }
        }
    }

    return pos;
}

//Add item
void TableF::insertItem(int item, int pos){

    table[pos].push_back(item);

}

//Debug method
void TableF::displayHash(){
    for (int i = 0; i < buckets; i++){
    cout << "INDEX: " << i << endl;
    for (auto x : table[i])
    {
      cout << " --> " << x << endl;
    }
  }

}

//Destructor
TableF::~TableF(){

    delete[] table;
}