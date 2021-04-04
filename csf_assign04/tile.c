#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
  unsigned N;
};

const char *get_plugin_name(void) {
  return "tile";
}

const char *get_plugin_desc(void) {
  return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
  if (num_args != 1) {
    return NULL;
  }
  int n = atoi(*args);
  if (!n) {
    return NULL;
  }
  
  void * arg_struct = malloc(sizeof(struct Arguments));
  ((struct Arguments *) arg_struct)->N = n;
  return arg_struct;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
  unsigned N = args->N;
  
  // Allocate a result Image
  struct Image *out = img_create(source->width, source->height);
  if (!out) {
    free(args);
    return NULL;
  }

  for (unsigned nj = 0; nj < N; nj++) {
    for (unsigned ni = 0; ni < N; ni++) {
      for (unsigned j = 0; j < source->height; j+=N) {
	for (unsigned i = 0; i < source->width; i+=N) {
	  int output_index = (source->width) * (nj * (source->height/N) + j/N)
	    + (ni * source->width / N) + (i/N);
	  int source_index = (source->width) * j + i;
	  
	  out->data[output_index] = source->data[source_index];
	}
      }
    }
  }
    
  free(args);
  
  return out;
}
