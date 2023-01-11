#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TARGET_EMUTOS
#include <mint/ostruct.h>
#include <osbind.h>
#elif defined (TARGET_MCP)
#include "mcp/syscalls.h"
#include "mcp/fsys.h"
#include "line_edit.h"
#endif

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "string_output.h"
#include "vm.h"

static void repl() {
#ifdef TARGET_MCP
  sys_chan_ioctrl(0, 0x03, 0, 0);
#endif
  for (;;) {
    print_cstring("> ");

  short rc;
#ifdef TARGET_MCP
  char line[CLI_MAX_COMMAND_SIZE];
  rc = cli_readline(0, line);
#elif defined(TARGET_EMUTOS)
  _CCONLINE l;
  l.maxlen = 254;
  Cconrs(&l);
  char *line = l.buffer;
  line[l.actuallen] = '\0';
  rc = 5; /* whatever */
#endif
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
        print_cstring(CRLF);
        if (strlen(line) == 0)
          continue;
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
  long fileSize;

#ifdef TARGET_MCP
  t_file_info fileInfo;
  if (sys_fsys_stat(path, &fileInfo) != 0) {
    fprintf(stderr, "Unable to sys_fsys_stat: \"%s\".\n", path);
    exit(74);
  }
  fileSize = fileInfo.size;
#endif

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

#ifndef TARGET_MCP
  fseek(file, 0L, SEEK_END);
  fileSize = ftell(file);
  rewind(file);
#endif

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

static void random_seed(void) {
  long seed;
#ifndef TARGET_EMUTOS
  seed = sys_time_jiffies();
#else
  seed = *((unsigned long*const)0x4ba);
#endif
  srand(seed);
}

// This is a bit of a hack, so we can call Supexec on EmuTOS to run the program
// in supervisor mode. This allows to read system variables (e.g. the 200Hz timer).
static int l_argc;
char* *l_argv;
int do_main(void) {
  initVM();

  random_seed();
#ifdef TARGET_MCP
  const char *edit_filename = sys_var_get("edit_filename");

  if (strlen(edit_filename) > 0) {
    runFile(edit_filename);
  } else
#endif
  if (l_argc == 1) {
    repl();
  } else if (l_argc == 2) {
    runFile(l_argv[1]);
  } else {
    fprintf(stderr, "Usage: flox [path]\n");
    exit(64);
  }
  
  freeVM();

  return 0;
}

int main(int argc, const char* argv[]) {
  l_argc = argc;
  l_argv = (char**)argv;

#ifdef TARGET_EMUTOS
  return (int)Supexec(do_main);
#else
  return do_main();
#endif
}
