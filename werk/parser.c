#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdio.h>

static int accept(Parser *c, Tokentype t) {
  if (c->token.type == t) {
    c->token = lex_token(c->lex, c->r);
    return 1;
  }
  return 0;
}

static int expect(Parser *c, Tokentype t, char *error_msg) {
  if (accept(c, t)) {
    return 1;
  }
  lex_mark(c->r, error_msg);
  return 0;
}

static void type_signature(Parser *c) {
  printf(".");
  expect(c, PERIOD, "Expected \".\" at end of type signature.");
}

static int variable(Parser *c, char * varname) {
  if (c->token.type == IDENT) {
    printf(" %s", c->token.ident.name);
    varname = c->token.ident.name;
    c->token = lex_token(c->lex, c->r);
    return 1;
  } else {
    return 0;
  }
}

static int literal(Parser *c) {
  if (c->token.type == REAL) {
    printf(" %f", c->token.number.rval);
    c->token = lex_token(c->lex, c->r);
    return 1;
  } else if (c->token.type == INT) {
    printf(" %d", c->token.number.ival);
    c->token = lex_token(c->lex, c->r);
    return 1;
  } else {
    return 0;
  }
}

static void expression(Parser *c) {
  if (accept(c, LPAREN)) {
    if (accept(c, VAR)) {
      printf(" (var");
      char varname[256];
      variable(c, varname);
      while (variable(c, varname)) {
      };
      if (accept(c, COLON)) {
        printf(": ");
        type_signature(c);
      } else
        (expect(c, PERIOD, "Expected type signature or \'.\'."));
      do {
        expression(c);
      } while (c->token.type != RPAREN);

    } else if (accept(c, PROCEDURE)) {
    }
    printf(")\n");
    expect(c, RPAREN, "No closing ).");
  } else if (accept(c, IDENT)) {

  } else if (literal(c)) {
  } else {
  }
}



static void module_signature(Parser *c) {
  printf(".");
  expect(c, PERIOD, "Expected \".\" at end of module signature.");
}

typedef struct {
	char name[256];
} Module;

void parseprogram(Parser *c, TermDesc* v) {
  Module m = {};
  printf("(module");
  expect(c, LPAREN, "Expected ( at beginning of file.");
  expect(c, MODULE, "Expected keyword \"module\" at beginning of file.");
  //variable(c, m.varname);
  if (accept(c, COLON)) {
    printf(": ");
    module_signature(c);
  }
  do {
    expression(c);
  } while (c->token.type != RPAREN);
  printf(")\n");
}

Parser parserinit(Lexer *lex, Reader *reader) {
  Parser ctx = {};
  ctx.lex = lex;
  ctx.r = reader;
  ctx.token = lex_token(ctx.lex, ctx.r);
  return ctx;
}
