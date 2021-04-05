// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
  // This plugin doesn't accept any command line arguments;
  // just define a single dummy field.
  int dummy;
};

const char *get_plugin_name(void) {
  return "mirrorv";
}

const char *get_plugin_desc(void) {
  return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
  (void) args; // this is just to avoid a warning about an unused parameter
  
  if (num_args != 0) {
    return NULL;
  }
  return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
  
  // Allocate a result Image
  struct Image *out = img_create(source->width, source->height);
  if (!out) {
    free(args);
    return NULL;
  }

  for (unsigned j = 0; j < source->height; j++) {
    for (unsigned i = 0; i < source->width; i++) {
      int top_index = (source->width) * j + i;
      int bottom_index = (source->width) * (source->height - 1 - j) + i;

      out->data[top_index] = source->data[bottom_index];
    }
  }
  
  free(args);
  
  return out;
}
