#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <iostream>

class Cache {
  public:
    Cache (); // Constructor
    ~Cache ();// Deconstructor
    void print() const;
  private:
    int totLoads; // Total number of loads
    int totStores; // Total number of stores
    int loadHit; // Number of hits during load
    int loadMiss; // Number of misses during load
    int storeHit; // Number of hits during store
    int storeMiss; // Number of misses during store
    int cycles; // Total cycles
};

#endif // CACHE_H
