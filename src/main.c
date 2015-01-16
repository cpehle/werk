#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../include/reader.h"
#include "../include/lexer.h"


int main(){
  srandom(12);
  //write_test_file("test2.ob");
  reader_t r = reader_init("tests/test.kant");
  lex_context_t ctx = lex_init();
  token_t tok;
  clock_t begin = clock();
  do {
    tok = lex_token(&ctx, &r);
    printf("%s %s\n", token_names[tok.type], (tok.type == IDENT) ? tok.ident.name : "");
  } while (!r.eof);
  printf("time passed: %ld\n", clock() - begin);
  return 0;
}
