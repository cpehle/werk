#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

char
readchar(Reader * r){
  char c;
  if(r->buffer_position < r->buffer_length) {
    c = r->buffer[r->buffer_position];
    r->buffer_position++;
    if(c == '\n') {
      r->position.x = 0;
      r->position.y++;
    } else {
      r->position.x++;
    }
  } else {
    r->eof = true;
    c = -1;
  }
  return c;
}

Reader
readerinit(char * filename)
{
  FILE * f = fopen(filename, "rb");
  if (!f) {
    Reader r = {};
    return r;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char * buf =  malloc(fsize + 1);
  fread(buf , fsize, 1, f);
  Reader r = { .filename = "", .buffer = buf, .buffer_length = fsize, .buffer_position = 0, .position = { .x = 0, .y = 1}, .eof = false };
  strncpy(r.filename, filename, 256);
  return r;
}
