// Alexandra Szewc    aszewc1
// Victor Wen         vwen2

#include "imgproc.h"

int main(int argc, char **argv) {

  // Load Plugins
  
  if (argc <= 1) {
    printUsg();
  } else if (argc == 2 && strcmp("list", argv[1]) == 0) {
    // Print plugin list
  } else if (argc >= 3 && strcmp("exec", argv[1]) == 0) {
    // Load plugin from argv[2] and parse arguments
    if (args == NULL) { printErr("missing or invalid command line arguments"); }
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
  char *dir = getnv("PLUGIN_DIR");
  if (dir != NULL) {
    return dir;
  }
  return strcat(getnv("PATH"), "/plugins");
}
