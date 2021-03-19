#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint32_t tag;           // Tag stored in block
  bool valid;             // Track whether block has been filled
  bool dirty;             // Track whether block needs write
  unsigned int load_ts;   // Load timestamp for block
  unsigned int access_ts; // Access timestamp for block
  
} Block;

typedef struct {
  Block *blocks;          // Blocks in each set; set-associativity
} Set;

typedef struct {
  Set *sets;              // Dynamically allocated array representing cache
  int num_sets;           // Number of sets in cache
  int set_assoc;          // Number of blocks per set; set-associativity
  int loads;              // Total number of loads
  int stores;             // Total number of stores
  int load_hit;           // Number of hits during load
  int load_miss;          // Number of misses during load
  int store_hit;          // Number of hits during store
  int store_miss;         // Number of misses during store
  int cycles;             // Total cycles
} Cache;

Cache *create_cache(); // Constructor
void destroy_cache(Cache *c); // Destructor
void summary(Cache *c);
void load_block(Block *b, uint32_t t, unsigned int ts);

#endif // CACHE_H
