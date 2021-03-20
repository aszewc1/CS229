// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cache.h"

/* Make structure for params
*set is the number of sets in cache.
*block is block number.
*bytes is block size.
*allocate is true if set to write-allocate.
*through is true if set to write-through.
*evict-lfu is true when block needs to be evicted.
*/
struct SimulationParams {
  int sets;
  int blocks;
  int bytes;
  bool allocate;
  bool through;
  bool evict_lfu;
};

// Return whether an integer is a positive power of 2
bool is_pp2 (int num) { return ((num > 0) && ((num & (num - 1)) == 0)); }

/* Constructor to initialize values*/
struct SimulationParams * parse_args(int argc, char** argv) {
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
  struct SimulationParams * sim = malloc(sizeof(struct SimulationParams));
  sim->sets = num_sets;
  sim->blocks = num_blocks;
  sim->bytes = num_bytes;
  sim->allocate = write_allocate;
  sim->through = write_through;
  sim->evict_lfu = least_recently_used;
  return sim;
}

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

Block *handle_write_back(struct SimulationParams *p, Cache *c,
			 Set *s, uint32_t t) {
  Block *b = evict(p->evict_lfu, s, p->blocks);
  if (!p->through && b->dirty) {
    c->cycles += 100 * p->bytes / 4;
  }
  load_block(b, t, c->ts);
  c->cycles += 100 * p->bytes / 4;
  return b;
}

int main(int argc, char** argv) {
  struct SimulationParams *params = parse_args(argc, argv);
  if (!params) {
    fprintf(stderr, "Invalid or nonsensical simulation parameters\n");
    return 1;
  }

  int o_bits = find_pow(params->bytes); // offset bits
  int i_bits = find_pow(params->sets);  // index bits

  Cache *c = create_cache(params->sets, params->blocks);

  char type;
  char hex[9];
  int nothing;
  
  while(scanf("%c 0x%s %d\n", &type, hex, &nothing) == 3) {
    c->ts++;
    // bit mask to extract tag bits
    uint32_t adr = strtoul(hex, NULL, 16);
    uint32_t tag = bitmask(adr, 32 - i_bits - o_bits, i_bits + o_bits);
    uint32_t index = bitmask(adr, i_bits, o_bits);
    bool hit = false;

    Set *curr_set = c->sets + index;
    Block *curr_block = curr_set->blocks;
    for (int i = 0; i < params->blocks; i++) {
      curr_block = curr_set->blocks + i;
      if (curr_block->valid && curr_block->tag == tag) {
	hit = true;
	curr_block->access_ts = c->ts;
	break;
      }
    }
    /*loops below count cycles for different policy. 
    * WA + WB (write-back) has 1, 25n+1, 1, 25n+1 for load-hit, load-miss, store-hit, store-miss respectively.
    * WA + WT has 1, 25n+1, 25n+1, 50n+1 for load-hit, load-miss, store-hit, store-miss respectively.
    * NWA + WT has 1, 25n+1, 1, 25n for load-hit, load-miss, store-hit, store-miss respectively.
    */
    if (type == 'l') {
      c->loads++;
      c->cycles++;
      if (hit) { c->load_hit++; }
      else {
	c->load_miss++;
	handle_write_back(params, c, curr_set, tag);
      }
    }
    else if (type == 's') {
      c->stores++;
      if (hit) {
	c->store_hit++;
	c->cycles++;
	if (params->through) { // write through
	  c->cycles += 100 * params->bytes / 4;
	}
	else { // write back
	  curr_block->dirty = true;
	}
      }
      else {
	c->store_miss++;
	if (params->allocate) { // write-allocate
	  Block *b = handle_write_back(params, c, curr_set, tag);
	  if (params->through) { c->cycles += 100 * params->bytes / 4; }
	  else { b->dirty = true; }
	  c->cycles++;
	}
	else { // no write-allocate
	  c->cycles += 100 * params->bytes / 4;
	}
      }
    }
  }

  summary(c);

  destroy_cache(c);
  free(params);
  return 0;
}
