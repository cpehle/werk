#include "../include/u.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct value val;
struct value {
  u64 tag;
  u64 d[62];
  union {
    u64 last;
    val * next;
  };
};

global_variable const val nil = {};

val conc (const val a, const val b) {
  val c = a;
  val c0 = c;
  val * d;
  while (c.tag & 1) {
    d = malloc(sizeof(val));
    memcpy(d,c.next,sizeof(val));
    c.next = d;
    c = *c.next;
  }
  c.next = &b;

  return c0;
}

void print_val (val * a) {
  assert(a != 0);
  for( ;a->tag & 1; a = a->next) {
    for (int i = 1; i<62; ++i) {
      printf("%ld", a->d[i]);
    }
  }
  printf("%ld", a->last);
}

void gen_val(val * a, size_t n) {
  
}
