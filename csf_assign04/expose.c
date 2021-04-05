//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
  double factor;
};

const char *get_plugin_name(void) {
  return "expose";
}

const char *get_plugin_desc(void) {
  return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
  if (num_args != 1) {
    return NULL;
  }
  double n = atof(*args);
  if (n <= 0) {
    return NULL;
  }
  
  void * arg_struct = malloc(sizeof(struct Arguments));
  ((struct Arguments *) arg_struct)->factor = n;
  return arg_struct;
}

// Helper function to apply factor to pixels.
static uint32_t expose(uint32_t pix, double f) {
  uint8_t r, g, b, a;
  img_unpack_pixel(pix, &r, &g, &b, &a);

  // Cap values at 255
  r = (f * r > 255) ? 255 : f * r;
  b = (f * b > 255) ? 255 : f * b;
  g = (f * g > 255) ? 255 : f * g;
  
  return img_pack_pixel(r, g, b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
  struct Arguments *args = arg_data;
  
  // Allocate a result Image
  struct Image *out = img_create(source->width, source->height);
  if (!out) {
    free(args);
    return NULL;
  }
  
  unsigned num_pixels = source->width * source->height;
  for (unsigned i = 0; i < num_pixels; i++) {
    out->data[i] = expose(source->data[i], args->factor);
  }
  
  free(args);
  
  return out;
}
