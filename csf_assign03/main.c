// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

int main(int argc, char** argv) {
  struct SimulationParams *params = parse_args(argc, argv);
  if (!params) {
    fprintf(stderr, "Invalid or nonsensical simulation parameters\n");
    return 1;
  }

  int o_bits = find_pow(params->bytes);
  int i_bits = find_pow(params->sets);
  int t_bits = 32 - o_bits - i_bits;
  t_bits = t_bits;

  Cache *c = create_cache(o_bits, i_bits, find_pow(params->blocks));

  char type;
  char hex[9];
  int nothing;
  
  while(scanf("%c 0x%s %d\n", &type, hex, &nothing) == 3) {
    unsigned int adr = strtoul(hex, NULL, 16);
    adr = adr;
    if (type == 'l') {
      c->loads++;
      // Handle reads
    }
    else if (type == 's') {
      c->stores++;
      // Handle different write policies
    }
  }

  destroy_cache(c);
  free(params);
  return 0;
}
