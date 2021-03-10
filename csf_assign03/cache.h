#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int loads; // Total number of loads
  int stores; // Total number of stores
  int load_hit; // Number of hits during load
  int load_miss; // Number of misses during load
  int store_hit; // Number of hits during store
  int store_miss; // Number of misses during store
  int cycles; // Total cycles
} Cache;

Cache *create_cache(); // Constructor
void destroy_cache(Cache *c); // Destructor
void summary(Cache *c);

#endif // CACHE_H
