// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "imgproc.h"

int main(int argc, char **argv) {

  // Load Plugins
  struct Plugin *plugins = loadPlugins();
  int numPlugins = sizeof(plugins) / sizeof(struct Plugin);
  
  if (argc <= 1) {
    printUsg();
  } else if (argc == 2 && strcmp("list", argv[1]) == 0) {
    // Print plugin list
    printPlugins(plugins, numPlugins);
  } else if (argc >= 3 && strcmp("exec", argv[1]) == 0) {
    // Load plugin from argv[2] and parse arguments
    //if (args == NULL) { printErr("missing or invalid command line arguments"); }
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
  return strcat(getenv("PATH"), "/plugins");
}

struct Plugin *loadPlugins() {
  const char *pluginDir = getPluginDir();
  struct Plugin *p = malloc(sizeof(struct Plugin));
  
  DIR *openDIR = opendir(pluginDir);
  if (openDIR == NULL) { printErr("opendir failed"); }

  struct dirent *curr_plugin;
  int count = 0;
  do {
    curr_plugin = readdir(openDIR);
    // Process each plugin
    char *name = curr_plugin->d_name;
    int len = strlen(name);
    // Check for proper extension
    if (len > 2 && strcmp((name+len-3), ".so") == 0) {
      p = realloc(p, ++count * sizeof(struct Plugin));

      // Store handle
      char *temp = malloc(256);
      strcpy(temp, pluginDir);
      strcat(temp, name);
      void *h = dlopen(temp, RTLD_LAZY);
      if (!h) { printErr("bad plugin handle"); }
      (p+count)->handle = h;

      // Store function addresses
      loadFuncs(h, p+count);
    }
  } while (curr_plugin != NULL);

  if (closedir(openDIR)) { printErr("closedir failed"); }
  return p;
}

void loadFuncs(void *handle, struct Plugin *p) {
  *(void**) (&(p->get_plugin_name)) = dlsym(handle, "get_plugin_name");
  *(void**) (&(p->get_plugin_desc)) = dlsym(handle, "get_plugin_desc");
  *(void**) (&(p->parse_arguments)) = dlsym(handle, "parse_arguments");
  *(void**) (&(p->transform_image)) = dlsym(handle, "transform_image");

  // Check for failure
  if (!p->get_plugin_name || !p->get_plugin_desc ||
      !p->parse_arguments || !p->transform_image) {
    printErr("failure storing function address");
  }
}

void printPlugins(struct Plugin *p, int num) {
  printf("Loaded %d plugin(s)\n", num);
  for (int i = 0; i < num; i++) {
    printf("%8s: %s\n", p->get_plugin_name(), p->get_plugin_desc());
    p = p + 1;
  }
}
