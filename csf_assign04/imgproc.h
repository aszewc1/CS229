// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include "image.h"
#include "pnglite.h"

/* Plugin struct for plugin handling */
struct Plugin {
  void *handle;
  const char *(*get_plugin_name)(void);
  const char *(*get_plugin_desc)(void);
  void *(*parse_arguments)(int num_args, char *args[]);
  struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

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

/*
 * Method to create dynamically allocated
 * array of Plugins structures with all relevant
 * information for Plugin manipulation
 */
int loadPlugins(struct Plugin **p);

/*
 * Dynamically allocate memory for
 * a single Plugin and its elements.
 */
struct Plugin *createPlugin();

/*
 * Method to free dynamically allocated
 * plugin structs.
 */
void destroyPlugins(struct Plugin **p, int size);

/*
 * Load functions associated with a
 * plugin handle.
 */
void loadFuncs(void *handle, struct Plugin *p);

/*
 * Function to print list of plugins
 * given the plugin-storing data structure
 * and the total number of plugins.
 */
void printPlugins(struct Plugin **p, int num);

/*
 * Method to return pointer to specific
 * plugin required by exec/
 */
struct Plugin *findPlugin(struct Plugin **p,
			  int size, const char *name);
