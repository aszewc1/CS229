#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Make structure for params
 * set is the number of sets in cache.
 * block is block number.
 * bytes is block size.
 * allocate is true if set to write-allocate.
 * through is true if set to write-through.
 * evict-lfu is true when block needs to be evicted.
 */
typedef struct {
  int sets;
  int blocks;
  int bytes;
  bool allocate;
  bool through;
  bool evict_lfu;
} SimulationParams;

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
  int ts;                 // Time stamp for lru and fifo
} Cache;

/* Constructor to initialize simulation params */
SimulationParams *parse_args(int argc, char** argv);

/* Constructor to create cache simulator data sctructure */
Cache *create_cache(int num_sets, int set_assoc);

/* Destructor for cache simulator data structure */
void destroy_cache(Cache *c);

/* Print summary statistics for cache simulation */
void summary(Cache *c);

/* ---------- ADDITIONAL FUNCTIONS ---------- */

/* Simulates loading of block from main memory
 * by setting timestamps clearing dirty bool
 * and restoring defaults otherwise.
 */
void load_block(Block *b, uint32_t t, int ts);

/* Returns whether integer is positive power of 2 */
bool is_pp2 (int num);

/* Returns power of 2 of a positive integer */
int find_pow(int num);

/* Applies bitmask to extract length of bits from
 * source offset by lower integer from lowest bit.
 */
uint32_t bitmask(uint32_t source, int length, int lower);

/* Loops through blocks in a set of the cache to
 * see if there are any hits with the given tag t.
 */
Block *is_hit(Cache *c, Set *s, int slots, uint32_t t);

/* Evicts the appropriate block from a set with
 * given number of slots depending on lru or fifo
 * write policy.
 */
Block *evict(bool lru, Set *s, int slots);

Block *handle_write_back(SimulationParams *p, Cache *c,
			 Set *s, uint32_t t);

void handle_load(SimulationParams *p, Cache *c,
		  Set *s, uint32_t t, bool hit);

void handle_store(SimulationParams *p, Cache *c,
		  Set *s, Block *b, uint32_t t, bool hit);

#endif // CACHE_H
