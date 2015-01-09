#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../include/reader.h"
#include "../include/lexer.h"

const char *
token_names[] = {
  "NNULL",
  "TIMES",
  "RDIV",
  "DIV",
  "MOD",
  "AND",
  "PLUS",
  "MINUS",
  "OR",
  "EQL",
  "NEQ",
  "LSS",
  "LEQ",
  "GTR",
  "GEQ",
  "IN",
  "IS",
  "ARROW",
  "PERIOD",
  "CHAR",
  "INT",
  "REAL",
  "FALSE",
  "TRUE",
  "NIL",
  "STRING",
  "NOT",
  "LPAREN",
  "LBRAK",
  "LBRACE",
  "IDENT",
  "IF",
  "WHILE",
  "REPEAT",
  "CASE",
  "FOR",
  "COMMA",
  "COLON",
  "BECOMES",
  "UPTO",
  "RPAREN",
  "RBRAK",
  "RBRACE",
  "THEN",
  "OF",
  "DO",
  "TO",
  "BY",
  "SEMICOLON",
  "END",
  "BAR",
  "ELSE",
  "ELSEIF",
  "UNTIL",
  "RETURN",
  "ARRAY",
  "RECORD",
  "POINTER",
  "CONST",
  "TYPE",
  "VAR",
  "PROCEDURE",
  "BEGIN",
  "IMPORT",
  "MODULE"
};


int main(){
  srandom(12);
  //write_test_file("test2.ob");
  reader_t r = reader_init("test.ob");
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
