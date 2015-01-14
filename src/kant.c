
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 
#include "kant.h"

global_variable const int IDENTIFIER_LENGTH = 32;
global_variable const int STRING_BUF_SIZE = 32;

global_variable const char* token_names[] = {
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

internal char read_char(kant_state_t *restrict s, const kant_input_t *restrict r){
  char c;
  if(s->lex_context.idx_in < r->buffer_length) {
    c = r->buffer[s->lex_context.idx_in++];
    if(c == '\n') {
      s->lex_context.pos.x = 0;
      s->lex_context.pos.y++;
    } else {
      s->lex_context.pos.x++;
    }
  } else {
    s->lex_context.eof = 1;
    c = -1;
  }
  return c;
}

internal void lex_mark(const lex_context_t ctx, char * err)
{
  printf("%s:%i:%i: %s\n", ctx.filename, ctx.pos.y, ctx.pos.x, err);
}

internal token_t lex_identifier(kant_state_t * s, const kant_input_t * in, char curr)
{
  token_t tok;
  int i = 0, k = 0;
  char ch = curr;

  do {
    if(i < IDENTIFIER_LENGTH-1) { tok.ident.name[i] = ch; ++i; }
    ch = read_char(s,in);
  } while(!(((ch < '0') || (ch > '9')) && ((ch < 'A') || (ch > 'Z')) && ((ch < 'a') || (ch > 'z'))));
  tok.ident.name[i] = 0;
 
  if (i<10) {
    for(k = s->lex_context.kwx[i-1]; k < s->lex_context.kwx[i]; ++k) {
      if(!strcmp(tok.ident.name, s->lex_context.key_table[k].id)){
	tok.type = s->lex_context.key_table[k].type;
	s->lex_context.ch = ch;
	return tok;
      }
    }
  } 
  tok.type = IDENT;
  s->lex_context.ch = ch;
  return tok;
}

internal token_t lex_string(kant_state_t * s, const kant_input_t * in)
{
  token_t tok;
  int i = 0;
  char ch = 0;
  ch = read_char(s,in);
  while (!(s->lex_context.eof) && (ch != '"')) {
    if (ch >= ' ') {
      if (i < STRING_BUF_SIZE-1) {
	tok.string.buf[i] = ch;
	i++;
      } else {
	lex_mark(s->lex_context, "string too long.");
      }
    }
    ch = read_char(s,in);
  }
  tok.string.buf[i] = 0;
  tok.type = STRING;
  s->lex_context.ch = read_char(s,in);
  return tok;
}

internal token_t lex_number(kant_state_t * s, const kant_input_t * in, char curr)
{
  token_t tok;
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
      lex_mark(s->lex_context, "too many digits");
      n = 0;
    }
    ch = read_char(s,in);
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
    ch = read_char(s,in);
    for(int i = 0; i<n; ++i) {
      x = x * 10.0 + tok.number.buf[i];
    }
    int e = 0;
    while(ch >= '0' && ch <= 9) {
      x = x * 10.0 + ch - 0x30;
      e--;
      ch = read_char(s,in);
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
	lex_mark(s->lex_context, "bad integer");
      }
    }
    tok.type = INT;
    tok.number.ival = k;
    break;
  }
  s->lex_context.ch = ch;
  return tok;
}

internal void lex_comment(kant_state_t * s, const kant_input_t * in)
{
  char ch = read_char(s,in);
  do {
    while((ch != '*') && !(s->lex_context.eof)) {
      if (ch == '(') {
	ch = read_char(s,in);
	if (ch == '*') {
	  lex_comment(s, in);
	  ch = s->lex_context.ch;
	} 
      } else {
	ch = read_char(s,in);
      }
    }
    while (ch == '*') { ch = read_char(s,in); }
  } while(!(ch == ')' || s->lex_context.eof));
  if (!s->lex_context.eof) {
    ch = read_char(s,in);
  } else {
    lex_mark(s->lex_context, "unterminated comment");
  }
  s->lex_context.ch = ch;
}

internal token_t lex_token(kant_state_t * s, const kant_input_t * in) {
  token_t tok;
  char ch = s->lex_context.ch;
  do {
    while (!(s->lex_context.eof) && (ch <= 0x20)) {
      ch = read_char(s,in);
    } /* skip whitespace and control characters */
    if (ch < 'A') {
      if (ch < '0') {
	switch (ch) {
	case '"':
	  tok = lex_string(s, in);
	  ch = s->lex_context.ch;
	  break;
	case '#':
	  ch = read_char(s,in);
	  tok.type = NEQ;
	  break;
	/* case '$': */
	/*   tok = lex_hex_string(s->lex_context, in); */
	/*   ch = ctx->ch; */
	/*   break; */
	case '&':
	  ch = read_char(s,in);
	  tok.type = AND;
	  break;
	case '(':
	  ch = read_char(s,in);
	  if (ch == '*') { tok.type = NNULL; lex_comment(s, in); ch = s->lex_context.ch;} else { tok.type = LPAREN; };
	  break;
	case ')':
	  ch = read_char(s,in);
	  tok.type = RPAREN;
	  break;
	case '*':
 	  ch = read_char(s,in);
	  tok.type = TIMES;
	  break;
	case '+':
	  ch = read_char(s,in);
	  tok.type = PLUS;
	  break;
	case ',':
	  ch = read_char(s,in);
	  tok.type = COMMA;
	  break;
	case '-':
	  ch = read_char(s,in);
	  tok.type = MINUS;
	  break;
	case '.':
	  ch = read_char(s,in);
	  if (ch == '.') { ch = read_char(s,in); tok.type = UPTO; } else { tok.type = PERIOD; };
	  break;
	case '/':
	  ch = read_char(s,in);
	  tok.type = RDIV;
	  break;
	default:
	  ch = read_char(s,in);
	  tok.type = NNULL;
	}
      } else if (ch < ':') {
	tok = lex_number(s, in, ch);
	ch = s->lex_context.ch;
      } else {
	switch (ch) {
	case ':':
	  ch = read_char(s,in);
	  if(ch == '=') { ch = read_char(s,in); tok.type = BECOMES; } else { tok.type = COLON; }
	  break;
	case ';':
	  ch = read_char(s,in);
	  tok.type = SEMICOLON;
	  break;
	case '<':
	  ch = read_char(s,in);
	  if(ch == '=') { ch = read_char(s,in); tok.type = LEQ; } else { tok.type = LSS; };
	  break;
	case '=':
	  ch = read_char(s,in);
	  tok.type = EQL;
	  break;
	case '>':
	  ch = read_char(s,in);
	  if(ch == '=') { ch = read_char(s,in); tok.type = GEQ; } else { tok.type = GTR; };
	  break;
	default:
	  ch = read_char(s,in);
	  tok.type = NNULL;
	}
      }
    } else if (ch < '[') {
      tok = lex_identifier(s, in, ch);
      ch = s->lex_context.ch;
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
      ch = read_char(s,in);
    } else if (ch < '{') {
      tok = lex_identifier(s, in, ch);
      ch = s->lex_context.ch;
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
      ch = read_char(s,in);
    }
  } while((tok.type == NNULL) && (s->lex_context.eof == 0));
  s->lex_context.ch = ch;
  return tok;
}

internal void
lex_enter_keyword(lex_context_t * ctx, token_type_t type, const char * name, int k)
{
  strncpy(ctx->key_table[k].id, name, 12);
  ctx->key_table[k].type = type;
}

kant_state_t
lex_init()
{
  kant_state_t ctx = { .lex_context = { .ch = 0, .err_count = 0, .kwx = {}, .key_table = {}}}; 
  int k = 0;
  ctx.lex_context.kwx[0] = 0;
  ctx.lex_context.kwx[1] = 0;
  lex_enter_keyword(&ctx.lex_context, IF, "if", k++);
  lex_enter_keyword(&ctx.lex_context, DO, "do", k++);
  lex_enter_keyword(&ctx.lex_context, OF, "of", k++);
  lex_enter_keyword(&ctx.lex_context, OR, "or", k++);
  lex_enter_keyword(&ctx.lex_context, TO, "to", k++);
  lex_enter_keyword(&ctx.lex_context, IN, "in", k++);
  lex_enter_keyword(&ctx.lex_context, IS, "is", k++);
  lex_enter_keyword(&ctx.lex_context, BY, "by", k++);
  ctx.lex_context.kwx[2] = k;
  lex_enter_keyword(&ctx.lex_context, END, "end", k++);
  lex_enter_keyword(&ctx.lex_context, NIL, "nil", k++);
  lex_enter_keyword(&ctx.lex_context, VAR, "var", k++);
  lex_enter_keyword(&ctx.lex_context, DIV, "div", k++);
  lex_enter_keyword(&ctx.lex_context, MOD, "mod", k++);
  lex_enter_keyword(&ctx.lex_context, FOR, "for", k++);
  ctx.lex_context.kwx[3] = k;
  lex_enter_keyword(&ctx.lex_context, ELSE, "else", k++);
  lex_enter_keyword(&ctx.lex_context, THEN, "then", k++);
  lex_enter_keyword(&ctx.lex_context, TRUE, "true", k++);
  lex_enter_keyword(&ctx.lex_context, TYPE, "type", k++);
  lex_enter_keyword(&ctx.lex_context, CASE, "case", k++);
  ctx.lex_context.kwx[4] = k;
  lex_enter_keyword(&ctx.lex_context, ELSEIF, "elseif", k++);
  lex_enter_keyword(&ctx.lex_context, FALSE, "false", k++);
  lex_enter_keyword(&ctx.lex_context, ARRAY, "array", k++);
  lex_enter_keyword(&ctx.lex_context, BEGIN, "begin", k++);
  lex_enter_keyword(&ctx.lex_context, CONST, "const", k++);
  lex_enter_keyword(&ctx.lex_context, UNTIL, "until", k++);
  lex_enter_keyword(&ctx.lex_context, WHILE, "while", k++);
  ctx.lex_context.kwx[5] = k;
  lex_enter_keyword(&ctx.lex_context, RECORD, "record", k++);
  lex_enter_keyword(&ctx.lex_context, REPEAT, "repeat", k++);
  lex_enter_keyword(&ctx.lex_context, RETURN, "return", k++);
  lex_enter_keyword(&ctx.lex_context, IMPORT, "import", k++);
  lex_enter_keyword(&ctx.lex_context, MODULE, "module", k++);
  ctx.lex_context.kwx[6] = k;
  lex_enter_keyword(&ctx.lex_context, POINTER, "pointer", k++);
  ctx.lex_context.kwx[7] = k; ctx.lex_context.kwx[8] = k;
  lex_enter_keyword(&ctx.lex_context, PROCEDURE, "procedure", k++);
  ctx.lex_context.kwx[9] = k;
  return ctx;
}

void
lex_buffer(kant_state_t *restrict s, const kant_input_t *restrict in, kant_output_t *restrict out)
{
  for (int j = s->lex_context.idx_out; j < out->token_count; ++j) {
    out->tokens[j] = lex_token(s, in);
  }
}

typedef enum {
  FBYTE = 0,
  FBOOL,
  FCHAR,
  FINT,
  FREAL,
  FSET,
  FPOINTER,
  FNILTYPE,
  FNOTYPE,
  FPROC,
  FSTRING,
  FARRAY,
  FRECORD
} form_t;

typedef enum {
  CHEAD = 0,
  CCONST,
  CVAR,
  CPAR,
  CFLD,
  CTYP,
  CSPROC,
  CSFUNC,
  CMOD
} class_t;

typedef struct type_desc * type_t;
struct type_desc {
  form_t form;
  int ref, mno;
  int no_param;
  long len;
  type_t base;
  size_t size;
};

const struct type_desc Int_Type = {
  .form = FINT,
  .ref = 0,
  .mno = 0,
  .no_param = 0,
  .base = 0,
  .size = 4
};

const struct type_desc Real_Type = {
  .form = FREAL,
  .ref = 0,
  .mno = 0,
  .no_param = 0,
  .base = 0,
  .size = 4
};



typedef struct object_desc * object_t;
struct object_desc {
  class_t class;
  int lev, exno;
  bool expo, rdo;
  object_t next, dsc;
  type_t type;
  long    val;
  ident_t name;
};

typedef struct module_desc * module_t;
struct module_desc {
  struct object_desc obj;
  ident_t orgname;
};

typedef struct compiler_ctx_desc * compiler_ctx_t;
struct compiler_ctx_desc {
  type_t type_table[64];
};

type_t
type(compiler_ctx_t ctx, int ref, form_t form, size_t size)
{
  struct type_desc td = {
    .form = form,
    .ref  = ref,
    .base = 0,
    .size = size,
  };
  type_t t =  malloc(sizeof(td));
  *t = td;
  ctx->type_table[ref] = t;
  return t;
}

typedef struct writer {
  
} writer_t;

typedef struct item {
  type_t type;
  long a, b, r;
} item_t;


