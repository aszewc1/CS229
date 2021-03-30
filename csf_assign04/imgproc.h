// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "image.h"
#include "pnglite.h"

/*
 * Method to print usage message on incorrect
 * command line arguments.
 */
void printUsg();

/*
 * Method to print error message on any
 * error condition.
 */
void printErr(const char *msg);


/*
 * Method to find the appropriate plugin
 * directory based on whether the PLUGIN_DIR
 * environment variable is set.
 */
char *getPluginDir();

/* Plugin struct for plugin handling */
struct Plugin {
  void *handle;
  const char *(*get_plugin_name)(void);
  const char *(*get_plugin_desc)(void);
  void *(*parse_arguments)(int num_args, char *args[]);
  struct Image *(*transform_image)(struct Image *source, void *arg_data);
};
