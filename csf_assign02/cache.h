#ifndef CACHE.H
#define CACHE.H


#include <stdio.h>
#include <iostream>

using namespace std;

class Cache
{
  public:
    Cache (); //constructor
    ~Cache ();//deconstructor
    void print() const;
  private:
    int totLoads; //total number of loads
    int totStores; // total number of stores
    int loadHit; //number of hits during load
    int loadMiss; //number of misses during load
    int storeHit; //number of hits during store
    int storeMiss; // number of misses during store
    int cycles; //total cycles
};

#endif CACHE.H
