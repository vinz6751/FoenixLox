#ifndef clox_string_output_h
#define clox_string_output_h

#include "value.h"

#ifdef TARGET_EMUTOS
  #define CRLF "\r\n"
#else
  #define CRLF "\n"
#endif

void print_cstring(const char *s);
void print_char(const char c);
void print_string_obj(struct ObjString *s);

#endif
