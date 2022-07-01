#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcp/syscalls.h"
#include "mcp/fsys.h"
#include "line_edit.h"
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl() {
  char line[CLI_MAX_COMMAND_SIZE];
  sys_chan_ioctrl(0, 0x03, 0, 0);
  for (;;) {
    printf("> ");

    short rc = cli_readline(0, line);
    switch(rc) {
      case -1:
        // ctx switch
        break;
      case -2:
        // help
        break;
      case -4:
        // ctrl-c
        break;
      default:
        printf("\n");
        if (strcmp(line, "exit") == 0) {
          return;
        } else if (strcmp(line, "help") == 0) {
          //print_help();
        } else if (strcmp(line, "copyright") == 0) {
          //print_copyright();
        } else if (strcmp(line, "license") == 0) {
          //print_license();
        } else {
          interpret(line);
        }                
    }    
  }
}

static char* readFile(const char* path) {
  t_file_info fileInfo;
  if (sys_fsys_stat(path, &fileInfo) != 0) {
    fprintf(stderr, "Unable to sys_fsys_stat: \"%s\".\n", path);
    exit(74);
  }
  long fileSize = fileInfo.size;

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  char* buffer = (char*)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

static void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source); // [owner]

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
  initVM();

  srand(sys_time_jiffies());

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: flox [path]\n");
    exit(64);
  }
  
  freeVM();

  return 0;
}
