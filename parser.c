#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdio.h>

static int accept(Parser *c, Tokentype t) {
  if (c->tok.type == t) {
    c->tok = lex_token(c->lex, c->r);
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

static int variable(Parser *c) {
  if (c->tok.type == IDENT) {
    printf(" %s", c->tok.ident.name);
    c->tok = lex_token(c->lex, c->r);
    return 1;
  } else {
    return 0;
  }
}

static int literal(Parser *c) {
  if (c->tok.type == REAL) {
    printf(" %f", c->tok.number.rval);
    c->tok = lex_token(c->lex, c->r);
    return 1;
  } else if (c->tok.type == INT) {
    printf(" %d", c->tok.number.ival);
    c->tok = lex_token(c->lex, c->r);
    return 1;
  } else {
    return 0;
  }
}

static void expression(Parser *c) {
  if (accept(c, LPAREN)) {
    if (accept(c, VAR)) {
      printf(" (var");
      variable(c);
      while (variable(c)) {
      };
      if (accept(c, COLON)) {
        printf(": ");
        type_signature(c);
      } else
        (expect(c, PERIOD, "Expected type signature or \'.\'."));
      do {
        expression(c);
      } while (c->tok.type != RPAREN);

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

void parseprogram(Parser *c) {
  printf("(module");
  expect(c, LPAREN, "Expected ( at beginning of file.");
  expect(c, MODULE, "Expected keyword \"module\" at beginning of file.");
  variable(c);
  if (accept(c, COLON)) {
    printf(": ");
    module_signature(c);
  }
  do {
    expression(c);
  } while (c->tok.type != RPAREN);
  printf(")\n");
}

Parser parserinit(Lexer *lex, Reader *reader) {
  Parser ctx = {};
  ctx.lex = lex;
  ctx.r = reader;
  ctx.tok = lex_token(ctx.lex, ctx.r);
  return ctx;
}
