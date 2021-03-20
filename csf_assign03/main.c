// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "cache.h"

int main(int argc, char** argv) {
  SimulationParams *params = parse_args(argc, argv);
  if (!params) {
    fprintf(stderr, "Invalid or nonsensical simulation parameters\n");
    return 1;
  }

  Cache *c = create_cache(params->sets, params->blocks);
  int o_bits = find_pow(params->bytes); // offset bits
  int i_bits = find_pow(params->sets);  // index bits
  char type;
  char hex[9];
  int nothing;
  
  while(scanf("%c 0x%s %d\n", &type, hex, &nothing) == 3) {
    c->ts++;
    // bit mask to extract tag bits
    uint32_t adr = strtoul(hex, NULL, 16);
    uint32_t tag = bitmask(adr, 32 - i_bits - o_bits, i_bits + o_bits);
    uint32_t index = bitmask(adr, i_bits, o_bits);
    Set *curr_set = c->sets + index;
    Block *curr_block = is_hit(c, curr_set, params->blocks, tag);

    if (type == 'l') {
      handle_load(params, c, curr_set, tag, curr_block != NULL);
    }
    else if (type == 's') {
      handle_store(params, c, curr_set, curr_block, tag, curr_block != NULL);
    }
  }

  summary(c);
  destroy_cache(c);
  free(params);
  return 0;
}
