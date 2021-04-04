#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;
	// just define a single dummy field.
	int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorh";
}

const char *get_plugin_desc(void) {
	return "mirror image horizontally";
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

  for (int j = 0; j < source->height; j++) {
    for (int i = 0; i < source->width / 2; i++) {
      int left_index = i * j + i;
      int right_index = source->width - left_index;
      uint32_t temp = out->data[left_index];
      out->data[left_index] = out->data[right_index];
      out->data[right_index] = temp;
    }
  }
  
  free(args);
  
  return out;
}
