#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define NUM_SEP '-'

char* source;
char* target;
struct stat buffer;

// ftw crush callback
int crush(const char *name, const struct stat *status, int type);

int main(int argc, char** argv) {
  if(argc != 3) {
    printf("USAGE: %s [directory to crush] [target directory]\n", argv[0]);
    return 0;
  }

  source = argv[1];
  target = argv[2];

  if(stat(source, &buffer) != 0) {
    printf("Source directory doesn't exist.\n");
    return 0;
  }

  if(stat(target, &buffer) == 0) {
    printf("Target directory already exists.\n");
    return 0;
  }
  mkdir(target, 0777);

  printf("Scanning directory %s.\n", source);

  ftw(source, crush, 1);
  return 0;
}

int crush(const char *name, const struct stat *status, int type) {
  char *namec, *file;
  namec = file = strdup(name);
  file = basename(file);

  if(type == FTW_F) {
    int idx = 1;
    char *newpath = malloc(strlen(target) + strlen(file) + 128);
    sprintf(newpath, "%s/%s", target, file);
    while(stat(newpath, &buffer) == 0) {
      sprintf(newpath, "%s/%d%c%s", target, idx++, NUM_SEP, file);
    }
    printf("%s => %s\n", name, newpath);
    int child_pid = fork();
    if(child_pid == 0) {
      execl("/bin/cp", "cp", name, newpath, (char *) 0);
    }
    wait(child_pid);
  }

  free(namec);

  return 0;
}
