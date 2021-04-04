// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "imgproc.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    printUsg();
  } else if (argc == 2 && strcmp("list", argv[1]) == 0) {
    // Load Plugins
    struct Plugin **plugins = malloc(50 * sizeof(struct Plugin));
    int numPlugins = loadPlugins(plugins);
    // Print plugin list
    printPlugins(plugins, numPlugins);
    destroyPlugins(plugins, numPlugins);
  } else if (argc >= 5 && strcmp("exec", argv[1]) == 0) {
    // Load Plugins
    struct Plugin **plugins = malloc(sizeof(struct Plugin));
    int numPlugins = loadPlugins(plugins);
    // Load plugin from argv[2] and parse arguments
    struct Plugin *myP = findPlugin(plugins, numPlugins, argv[2]);
    if (myP == NULL) { printErr("plugin not found"); }
    void *args = myP->parse_arguments(argc - 5, argv + 5);
    if (args == NULL) { printErr("missing or invalid command line arguments"); }
    struct Image *in = img_read_png(argv[3]);
    if (in == NULL) { printErr("image read fail"); }
    struct Image *out = myP->transform_image(in, args);
    if (out == NULL) { printErr("image transform fail"); }
    if (!img_write_png(out, argv[4])) { printErr("image write fail"); }
    img_destroy(in);
    img_destroy(out);
    destroyPlugins(plugins, numPlugins);
  } else {
    printErr("unknown command name");
  }

  return 0;
}

void printUsg() {
  printf("Usage: imgproc <command> [<command args...>]\n");
  printf("Commands are:\n");
  printf("  list\n");
  printf("  exec <plugin> <input img> <output img> [<plugin args...>]\n");
}

void printErr(const char *msg) {
  printf("Error: %s\n", msg);
  exit(1);
}

char *getPluginDir() {
  char *dir = getenv("PLUGIN_DIR");
  if (dir != NULL) {
    return dir;
  }
  return "./plugins/";
}

int loadPlugins(struct Plugin **p) {
  const char *pluginDir = getPluginDir();
  
  DIR *openDIR = opendir(pluginDir);
  if (openDIR == NULL) { printErr("opendir failed"); }

  struct dirent *curr_plugin;
  int c = 0;
  while ((curr_plugin = readdir(openDIR)) != NULL) {
    // Process each plugin
    char *name = curr_plugin->d_name;
    int len = strlen(name);
    // Check for proper extension
    if (len > 2 && strcmp((name+len-3), ".so") == 0) {
      //p = realloc(p, (c + 1) * sizeof(struct Plugin));
      *(p + c) = createPlugin();

      // Store handle
      char *temp = malloc(256);
      strcpy(temp, pluginDir);
      strcat(temp, name);
      (*(p + c))->handle = dlopen(temp, RTLD_LAZY);
      free(temp);
      if (!(*(p + c))->handle) { printErr("bad plugin handle"); }
      
      // Store function addresses
      loadFuncs((*(p + c))->handle, *(p + c));
      c++;
    }
  }

  if (closedir(openDIR)) { printErr("closedir failed"); }

  return c;
}

struct Plugin *createPlugin() {
  struct Plugin *p = malloc(sizeof(struct Plugin));
  return p;
}

void destroyPlugins(struct Plugin **p, int size) {
  for (int i = 0; i < size; i++) {
    if (dlclose((*(p+i))->handle)) { printErr("could not unload lib"); }
    free(*(p+i));
  }
  free(p);
}

void loadFuncs(void *handle, struct Plugin *p) {
  *(void**) (&p->get_plugin_name) = dlsym(handle, "get_plugin_name");
  *(void**) (&p->get_plugin_desc) = dlsym(handle, "get_plugin_desc");
  *(void**) (&p->parse_arguments) = dlsym(handle, "parse_arguments");
  *(void**) (&p->transform_image) = dlsym(handle, "transform_image");

  // Check for failure
  if (!p->get_plugin_name || !p->get_plugin_desc ||
      !p->parse_arguments || !p->transform_image) {
    printErr("failure storing function address");
  }
}

void printPlugins(struct Plugin **p, int num) {
  printf("Loaded %d plugin(s)\n", num);
  for (int i = 0; i < num; i++) {
    printf("%8s: %s\n",
	   (*(p+i))->get_plugin_name(),
	   (*(p+i))->get_plugin_desc());
  }
}

struct Plugin *findPlugin(struct Plugin **p,
			  int size, const char *name) {
  for (int i = 0; i < size; i++) {
    if(strcmp((*(p+i))->get_plugin_name(), name) == 0) {
      return *(p+i);
    }
  }
  return NULL;
}
