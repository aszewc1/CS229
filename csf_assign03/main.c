// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cache.h"

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

  if (strcmp("write-allocate", argv[4]) == 0) { write_allocate = true; }
  else if (strcmp("no-write-allocate", argv[4]) != 0) { return NULL; }

  if (strcmp("write-through", argv[5]) == 0) { write_through = true; }
  else if ((strcmp("write-back", argv[5]) != 0) ||
	   ((strcmp("write-back", argv[5]) == 0) &&
	     !write_allocate)) { return NULL; }

  if (strcmp("lru", argv[6]) == 0) { least_recently_used = true; }
  else if (strcmp("fifo", argv[6]) != 0) { return NULL; }

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

Block *evict(bool lru, Set *s, int slots) {
  // check if blocks are valid, load if not
  // otherwise check for lru or fifo, select for each
  // update data structure and "clean" blocks
  bool wrote_invalid = false;
  int lru_index = -1;
  int lru_ts = -1;
  int fifo_index = -1;
  int fifo_ts = -1;
  Block *b = s->blocks;
  
  for (int i = 0; i < slots; i++) {
    b = s->blocks + i;
    if (!(b->valid)) {
      return b;
    }
    else {
      if (lru) {
	if (lru_ts == -1 || b->access_ts < lru_ts) {
	  lru_ts = b->access_ts;
	  lru_index = i;
	}
      }
      else {
	if (fifo_ts == -1 || b->load_ts < fifo_ts) {
	  fifo_ts = b->access_ts;
	  fifo_index = i;
	}
      }
    }
  }

  if (!wrote_invalid) {
    if (lru) { b =  s->blocks + lru_index; }
    else { b = s->blocks + fifo_index; }
  }
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
  int t_bits = 32 - o_bits - i_bits;    // tag bits

  Cache *c = create_cache(o_bits, params->sets, params->blocks);
  Set *cache = c->sets;

  char type;
  char hex[9];
  int nothing;
  int ts = 0;
  
  while(scanf("%c 0x%s %d\n", &type, hex, &nothing) == 3) {
    ts++;
    // bit mask to extract tag bits
    uint32_t adr = strtoul(hex, NULL, 16);
    uint32_t tag = ((((uint32_t)(0 - 1)) << (32 - t_bits)) & adr) >> (32 - t_bits);
    uint32_t index = ((((1 << i_bits) - 1) << o_bits) & adr) >> o_bits;
    bool hit = false;

    Set *curr_set = &(cache[index]);
    Block *curr_block = curr_set->blocks;
    for (int i = 0; i < params->blocks; i++) {
      curr_block = curr_set->blocks + i;
      if (curr_block->valid && curr_block->tag == tag) {
	hit = true;
	curr_block->access_ts = ts;
      }
    }
    
    if (type == 'l') {
      c->loads++;
      if (hit) {
	c->load_hit++;
	c->cycles++;
      }
      else {
	c->load_miss++;
	c->cycles += 100 * params->bytes / 4 + 1;
	load_block(evict(params->evict_lfu, curr_set, params->blocks),
		   tag, ts);
      }
    }
    else if (type == 's') {
      c->stores++;
      if (hit) {
	c->store_hit++;
	if (params->through) {
	  // write through
	  c->cycles += 100 * params->bytes / 4;
	}
	else {
	  // write back
	  curr_block->dirty = true;
	  c->cycles++;
	}
      }
      else {
	c->store_miss++;
	if (params->allocate) {
	  // write-allocate
	  Block *b = evict(params->evict_lfu, curr_set, params->blocks);
	  load_block(b, tag, ts);
	  b->dirty = true; // only need for write back
	  c->cycles += 100 * params->bytes / 4;
	  if (params->through) { c->cycles += 100 * params->bytes / 4; }
	  else { c->cycles++; }
	}
	else {
	  // no write-allocate
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
