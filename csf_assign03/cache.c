// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "cache.h"

Cache *create_cache(int o, int num_sets, int set_assoc) {
  Cache *c = malloc(sizeof(Cache));
  c->sets = malloc(sizeof(Set) * num_sets);
  c->num_sets = num_sets;
  c->set_assoc = set_assoc;

  for (int i = 0; i < num_sets; i++) {
    Set *s = c->sets + i;
    s->blocks = malloc(sizeof(Block) * set_assoc);
    for (int j = 0; j < set_assoc; j++) {
      Block *b = s->blocks + j;
      b->tag = 0;
      b->valid = false;
      b->dirty = false;
      b->load_ts = 0;
      b->access_ts = 0;
    }
  }
  
  c->loads = 0;
  c->stores = 0;
  c->load_hit = 0;
  c->load_miss = 0;
  c->store_hit = 0;
  c->store_miss = 0;
  c->cycles = 0;
  return c;
}

void destroy_cache(Cache *c) {
  int n_s = c->num_sets;
  int s_a = c->set_assoc;
  Set *set_ptr = c->sets;
  
  for (int i = 0; i < n_s; i++) {
    free((set_ptr + i)->blocks);
  }
  free(c->sets);
  free(c);
}

void summary(Cache *c) {
  printf("Total loads: %d\nTotal stores: %d\nLoad hits: %d\n",
	 c->loads, c->stores, c->load_hit);
  printf("Load misses: %d\nStore hits: %d\nStore misses: %d\n",
	 c->load_miss, c->store_hit, c->store_miss);
  printf("Total cycles: %d\n", c->cycles);
}

void load_block(Block *b, uint32_t t, unsigned int ts) {
  b->tag = t;
  b->valid = true;
  b->dirty = false;
  b->load_ts = ts;
  b->access_ts = ts;
}
