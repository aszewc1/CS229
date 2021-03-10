// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "cache.h"

Cache *create_cache() {
  Cache *c = malloc(sizeof(cache));
  c->loads = 0;
  c->stores = 0;
  c->load_hit = 0;
  c->load_miss = 0;
  c->store_hit = 0;
  c->store_miss = 0;
  c->cycles = 0;
  return cache;
}

void destroy_cache(Cache *c) { free(c); }

void summary(Cache *c) {
  printf("Total loads: %d\nTotal stores: %d\nLoad hits: %d\n",
	 c->loads, c->stores, c->load_hit);
  printf("Load misses: %d\nStore hits: %d\nStore misses: %d\n",
	 c->load_miss, c->store_hit, c->store_miss);
  printf("Total cycles: %d\n", c->cycles);
}
