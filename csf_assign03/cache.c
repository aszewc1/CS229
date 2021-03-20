// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "cache.h"

SimulationParams * parse_args(int argc, char** argv) {
  if (argc != 7) { return NULL; }

  int num_sets = atoi(argv[1]);
  int num_blocks = atoi(argv[2]);
  int num_bytes = atoi(argv[3]);
  
  if (!is_pp2(num_sets) || !is_pp2(num_blocks) ||
      !is_pp2(num_bytes) || num_bytes < 4) { return NULL; }

  bool write_allocate = false;
  bool write_through = false;
  bool least_recently_used = false;
   //checks user command line input to  determine type of cache policy.
  if (strcmp("write-allocate", argv[4]) == 0) { write_allocate = true; }
  else if (strcmp("no-write-allocate", argv[4]) != 0) { return NULL; }

  if (strcmp("write-through", argv[5]) == 0) { write_through = true; }
  else if ((strcmp("write-back", argv[5]) != 0) ||
	   ((strcmp("write-back", argv[5]) == 0) &&
	     !write_allocate)) { return NULL; }

  if (strcmp("lru", argv[6]) == 0) { least_recently_used = true; }
  else if (strcmp("fifo", argv[6]) != 0) { return NULL; }
  //allocates memory
  SimulationParams * sim = malloc(sizeof(SimulationParams));
  sim->sets = num_sets;
  sim->blocks = num_blocks;
  sim->bytes = num_bytes;
  sim->allocate = write_allocate;
  sim->through = write_through;
  sim->evict_lfu = least_recently_used;
  return sim;
}

Cache *create_cache(int num_sets, int set_assoc) {
  Cache *c = malloc(sizeof(Cache)); //allocates memory 
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
  /*Initialize stat values */
  c->loads = 0;
  c->stores = 0;
  c->load_hit = 0;
  c->load_miss = 0;
  c->store_hit = 0;
  c->store_miss = 0;
  c->cycles = 0;
  c->ts = 0;
  return c;
}

void destroy_cache(Cache *c) {
  int n_s = c->num_sets;
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

void load_block(Block *b, uint32_t t, int ts) {
  b->tag = t;
  b->valid = true;
  b->dirty = false; //false since load does not need write
  b->load_ts = ts; //load time stamp
  b->access_ts = ts; //access time stamp
}

bool is_pp2 (int num) { return ((num > 0) && ((num & (num - 1)) == 0)); }

int find_pow(int num) {
  int count = 0;
  while (num > 1) {
    count++;
    num = num >> 1;
  }
  return count;
}

uint32_t bitmask(uint32_t source, int length, int lower) {
  uint32_t mask = (((uint32_t) 1) << length) - 1;
  mask <<= lower;
  mask &= source;
  mask >>= lower;
  return mask;
}

Block *is_hit(Cache *c, Set *s, int slots, uint32_t t) {
  Block *curr_block = s->blocks;
  for (int i = 0; i < slots; i++) {
    curr_block = s->blocks + i;
    if (curr_block->valid && curr_block->tag == t) {
      curr_block->access_ts = c->ts;
      return curr_block;
    }
  }
  return NULL;
}

Block *evict(bool lru, Set *s, int slots) {
  // check if blocks are valid, load if not
  // otherwise check for lru or fifo, select for each
  // update data structure and "clean" blocks
  int lru_index = -1;
  unsigned int lru_ts = 0;
  int fifo_index = -1;
  unsigned int fifo_ts = 0;
  Block *b = s->blocks;
  
  for (int i = 0; i < slots; i++) {
    b = s->blocks + i;
    if (!(b->valid)) {
      return b;
    }
    else {
      if (lru) {
	if (lru_index == -1 || b->access_ts < lru_ts) {
	  lru_ts = b->access_ts;
	  lru_index = i;
	}
      }
      else {
	if (fifo_index == -1 || b->load_ts < fifo_ts) {
	  fifo_ts = b->load_ts;
	  fifo_index = i;
	}
      }
    }
  }

  if (lru) { b =  s->blocks + lru_index; }
  else { b = s->blocks + fifo_index; }
  return b;
}

Block *handle_write_back(SimulationParams *p, Cache *c,
			 Set *s, uint32_t t) {
  Block *b = evict(p->evict_lfu, s, p->blocks);
  if (!p->through && b->dirty) {
    c->cycles += 100 * p->bytes / 4;
  }
  load_block(b, t, c->ts);
  c->cycles += 100 * p->bytes / 4;
  return b;
}

void handle_load(SimulationParams *p, Cache *c,
		 Set *s, uint32_t t, bool hit) {
  c->loads++;
  c->cycles++;
  if (hit) { c->load_hit++; }
  else {
    c->load_miss++;
    handle_write_back(p, c, s, t);
  }
}

void handle_store(SimulationParams *p, Cache *c,
		  Set *s, Block *b, uint32_t t, bool hit) {
  c->stores++;
  if (hit) {
    c->store_hit++;
    c->cycles++;
    if (p->through) { // write through
      c->cycles += 100 * p->bytes / 4;
    }
    else { // write back
      b->dirty = true;
    }
  }
  else {
    c->store_miss++;
    if (p->allocate) { // write-allocate
      Block *bl = handle_write_back(p, c, s, t);
      if (p->through) { c->cycles += 100 * p->bytes / 4; }
      else { bl->dirty = true; }
      c->cycles++;
    }
    else { // no write-allocate
      c->cycles += 100 * p->bytes / 4;
    }
  }
}
