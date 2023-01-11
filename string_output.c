/* String output functions. We could use "printf" everywhere but it's just very slow. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef TARGET_EMUTOS
#include <mint/ostruct.h>
#include <osbind.h>
#elif defined (TARGET_MCP)
#include "mcp/syscalls.h"
#endif

#include "value.h"
#include "object.h"
#include "native.h"

void print_cstring(const char *s) {
#ifdef TARGET_EMUTOS
  (void)Cconws(s);
#elif defined (TARGET_MCP)
    sys_chan_write(0, (unsigned char *)s, strlen(s));
#else
  printf("%s", s);
#endif
}

void print_char(const char c) {
#ifdef TARGET_EMUTOS
  Cconout(c);
#elif defined (TARGET_MCP)
{
    char buf = c;
    sys_chan_write(0, (unsigned char)&buf, 1);
}
#else
  printf("%c", c);
#endif
}

void print_string_obj(struct ObjString *s) {
  if (s->length == 1)
    print_char(s->chars[0]);
  else {
#ifdef TARGET_MCP
    sys_chan_write(0, s->chars, s->length);
#else
    print_cstring(s->chars);
#endif
  }
}



