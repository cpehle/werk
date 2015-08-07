#include <stdlib.h>

#include <stdio.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

int main() {
  Reader r = readerinit("tests/test.kant");
  Lexer l = lexerinit();
  Token tok;
  clock_t begin = clock();
  do {
    tok = lex_token(&l, &r);
  } while (!r.eof);
  printf("time passed: %ld\n", clock() - begin);
  r = readerinit("tests/test.kant");
  l = lexerinit();
  Parser pctx = parserinit(&l, &r);
  parseprogram(&pctx);
  return 0;
}
