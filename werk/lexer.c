#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "lexer.h"

void
lex_mark(Reader * r, char * err)
{
  printf("%s:%i:%i: %s\n", r->filename, r->position.y, r->position.x, err);
}

static Token
lex_identifier(Lexer * ctx, Reader * r, char curr)
{
  Token tok;
  int i = 0, k = 0;
  char ch = curr;

  do {
    if(i < IDENTIFIER_LENGTH-1) { tok.ident.name[i] = ch; ++i; }
    ch = readchar(r);
  } while(!(((ch < '0') || (ch > '9')) && ((ch < 'A') || (ch > 'Z')) && ((ch < 'a') || (ch > 'z'))));
  tok.ident.name[i] = 0;
  if (i<10) {
    for(k = ctx->kwx[i-1]; k < ctx->kwx[i]; ++k) {
      if(!strcmp(tok.ident.name, ctx->key_table[k].id)){
	tok.type = ctx->key_table[k].type;
	ctx->ch = ch;
	return tok;
      }
    }
  }
  tok.type = IDENT;
  ctx->ch = ch;
  return tok;
}

static Token
lex_string(Lexer * ctx, Reader * r)
{
  Token tok;
  int i = 0;
  char ch = 0;
  ch = readchar(r);
  while (!(r->eof) && (ch != '"')) {
    if (ch >= ' ') {
      if (i < STRING_BUF_SIZE-1) {
	tok.string.buf[i] = ch;
	i++;
      } else {
	lex_mark(r, "string too long.");
      }
    }
    ch = readchar(r);
  }
  tok.string.buf[i] = 0;
  tok.type = STRING;
  ctx->ch = readchar(r);
  return tok;
}

static Token
lex_hex_string(Lexer * ctx, Reader * r)
{
  Token tok;
  int i = 0;
  return tok;
}

static Token
lex_number(Lexer * ctx, Reader * r, char curr)
{
  Token tok;
  int n = 0; int k = 0;
  double x = 0.0;
  const int max = 2147483647;
  char ch = curr;
  do {
    if (n < 16) {
      tok.number.buf[n] = ch - 0x30;
      n++;
    }
    else {
      lex_mark(r, "too many digits");
      n = 0;
    }
    ch = readchar(r);
  } while (!((ch < '0' || ch > '9') && (ch < 'A' || ch > 'F')));
  switch(ch) {
  case 'H':
  case 'R':
  case 'X':
    for(int i = 0; i<n; ++i) {
      char h = tok.number.buf[i];
      if (h >= 10) { h = h - 7; };
      k = k * 0x10 + h;
    }
    break;
  case '.':
    ch = readchar(r);
    for(int i = 0; i<n; ++i) {
      x = x * 10.0 + tok.number.buf[i];
    }
    int e = 0;
    while(ch >= '0' && ch <= 9) {
      x = x * 10.0 + ch - 0x30;
      e--;
      ch = readchar(r);
    }
    tok.type = REAL;
    tok.number.rval = x;
    break;
  default:
    for(int i = 0; i<n; i++) {
      if (tok.number.buf[i] < 10) {
	/* TODO: Check for overflow. */
	k = k * 10 + tok.number.buf[i];
      } else {
	lex_mark(r, "bad integer");
      }
    }
    tok.type = INT;
    tok.number.ival = k;
    break;
  }
  ctx->ch = ch;
  return tok;
}

static void
lex_comment(Lexer * ctx, Reader *r)
{
  char ch = readchar(r);
  do {
    while((ch != '*') && !(r->eof)) {
      if (ch == '(') {
    ch = readchar(r);
	if (ch == '*') {
	  lex_comment(ctx, r);
	  ch = ctx->ch;
	} 
      } else {
    ch = readchar(r);
      }
    }
    while (ch == '*') { ch = readchar(r); }
  } while(!(ch == ')' || r->eof));
  if (!r->eof) {
    ch = readchar(r);
  } else {
    lex_mark(r, "unterminated comment");
  }
  ctx->ch = ch;
}

Token
lex_token(Lexer * ctx, Reader * r) {
  Token tok;
  char ch = ctx->ch;
  do {
    while (!(r->eof) && (ch <= 0x20)) {
      ch = readchar(r);
    } /* skip whitespace and control characters */
    if (ch < 'A') {
      if (ch < '0') {
	switch (ch) {
	case '"':
	  tok = lex_string(ctx, r);
	  ch = ctx->ch;
	  break;
	case '#':
      ch = readchar(r);
	  tok.type = NEQ;
	  break;
	case '$':
	  tok = lex_hex_string(ctx, r);
	  ch = ctx->ch;
	  break;
	case '&':
      ch = readchar(r);
	  tok.type = AND;
	  break;
	case '(':
      ch = readchar(r);
	  if (ch == '*') { tok.type = NNULL; lex_comment(ctx, r); ch = ctx->ch;} else { tok.type = LPAREN; };
	  break;
	case ')':
      ch = readchar(r);
	  tok.type = RPAREN;
	  break;
	case '*':
      ch = readchar(r);
	  tok.type = TIMES;
	  break;
	case '+':
      ch = readchar(r);
	  tok.type = PLUS;
	  break;
	case ',':
      ch = readchar(r);
	  tok.type = COMMA;
	  break;
	case '-':
      ch = readchar(r);
	  tok.type = MINUS;
	  break;
	case '.':
      ch = readchar(r);
      if (ch == '.') { ch = readchar(r); tok.type = UPTO; } else { tok.type = PERIOD; };
	  break;
	case '/':
      ch = readchar(r);
	  tok.type = RDIV;
	  break;
	default:
      ch = readchar(r);
	  tok.type = NNULL;
	}
      } else if (ch < ':') {
	tok = lex_number(ctx, r, ch);
	ch = ctx->ch;
      } else {
	switch (ch) {
	case ':':
      ch = readchar(r);
      if(ch == '=') { ch = readchar(r); tok.type = BECOMES; } else { tok.type = COLON; }
	  break;
	case ';':
      ch = readchar(r);
	  tok.type = SEMICOLON;
	  break;
	case '<':
      ch = readchar(r);
      if(ch == '=') { ch = readchar(r); tok.type = LEQ; } else { tok.type = LSS; };
	  break;
	case '=':
      ch = readchar(r);
	  tok.type = EQL;
	  break;
	case '>':
      ch = readchar(r);
      if(ch == '=') { ch = readchar(r); tok.type = GEQ; } else { tok.type = GTR; };
	  break;
	default:
      ch = readchar(r);
	  tok.type = NNULL;
	}
      }
    } else if (ch < '[') {
      tok = lex_identifier(ctx, r, ch);
      ch = ctx->ch;
    } else if (ch < 'a') {
      switch (ch) {
      case '[':
	  tok.type = RBRAK;
	  break;
      case ']':
	tok.type = LBRAK;
	break;
      case '^':
	tok.type = ARROW;
	break;
      default:
	tok.type = NNULL;
      }
      ch = readchar(r);
    } else if (ch < '{') {
      tok = lex_identifier(ctx, r, ch);
      ch = ctx->ch;
    } else {
      switch (ch) {
      case '{':
	tok.type = LBRACE;
	break;
      case '}':
	tok.type = RBRACE;
	break;
      case '|':
	tok.type = BAR;
	break;
      case '~':
	tok.type = NOT;
	break;
      default:
	tok.type = NNULL;
      }
      ch = readchar(r);
    }
  } while((tok.type == NNULL) && (r->eof == false));
  ctx->ch = ch;
  return tok;
}

void
lex_enter_keyword(Lexer * ctx, Tokentype type, const char * name, int k)
{
  strncpy(ctx->key_table[k].id, name, 12);
  ctx->key_table[k].type = type;
}

Lexer
lexerinit(void)
{
  Lexer ctx = { .ch = 0, .err_count = 0, .kwx = {}, .key_table = {}};
  int k = 0;
  ctx.kwx[0] = 0;
  ctx.kwx[1] = 0;
  lex_enter_keyword(&ctx, IF, "if", k++);
  lex_enter_keyword(&ctx, DO, "do", k++);
  lex_enter_keyword(&ctx, OF, "of", k++);
  lex_enter_keyword(&ctx, OR, "or", k++);
  lex_enter_keyword(&ctx, TO, "to", k++);
  lex_enter_keyword(&ctx, IN, "in", k++);
  lex_enter_keyword(&ctx, IS, "is", k++);
  lex_enter_keyword(&ctx, BY, "by", k++);
  ctx.kwx[2] = k;
  lex_enter_keyword(&ctx, END, "end", k++);
  lex_enter_keyword(&ctx, NIL, "nil", k++);
  lex_enter_keyword(&ctx, VAR, "var", k++);
  lex_enter_keyword(&ctx, DIV, "div", k++);
  lex_enter_keyword(&ctx, MOD, "mod", k++);
  lex_enter_keyword(&ctx, FOR, "for", k++);
  ctx.kwx[3] = k;
  lex_enter_keyword(&ctx, ELSE, "else", k++);
  lex_enter_keyword(&ctx, THEN, "then", k++);
  lex_enter_keyword(&ctx, TRUE, "true", k++);
  lex_enter_keyword(&ctx, TYPE, "type", k++);
  lex_enter_keyword(&ctx, CASE, "case", k++);
  ctx.kwx[4] = k;
  lex_enter_keyword(&ctx, ELSEIF, "elseif", k++);
  lex_enter_keyword(&ctx, FALSE, "false", k++);
  lex_enter_keyword(&ctx, ARRAY, "array", k++);
  lex_enter_keyword(&ctx, BEGIN, "begin", k++);
  lex_enter_keyword(&ctx, CONST, "const", k++);
  lex_enter_keyword(&ctx, UNTIL, "until", k++);
  lex_enter_keyword(&ctx, WHILE, "while", k++);
  ctx.kwx[5] = k;
  lex_enter_keyword(&ctx, RECORD, "record", k++);
  lex_enter_keyword(&ctx, REPEAT, "repeat", k++);
  lex_enter_keyword(&ctx, RETURN, "return", k++);
  lex_enter_keyword(&ctx, IMPORT, "import", k++);
  lex_enter_keyword(&ctx, MODULE, "module", k++);
  ctx.kwx[6] = k;
  lex_enter_keyword(&ctx, POINTER, "pointer", k++);
  ctx.kwx[7] = k; ctx.kwx[8] = k;
  lex_enter_keyword(&ctx, PROCEDURE, "procedure", k++);
  ctx.kwx[9] = k;
  return ctx;
}



