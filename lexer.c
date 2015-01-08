#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct position {
  int x;
  int y;
} position_t;

typedef struct reader {
  char filename[256];
  char * buffer;
  int buffer_length;
  int buffer_position;
  position_t position;
  bool eof;
} reader_t;

char
read_char(reader_t * r){
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

reader_t
reader_init(char * filename)
{
  FILE * f = fopen(filename, "rb");
  if (!f) {
    reader_t r = {};
    return r;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char * buf =  malloc(fsize + 1);
  fread(buf , fsize, 1, f);
  reader_t r = { .filename = "", .buffer = buf, .buffer_length = fsize, .buffer_position = 0, .position = { .x = 0, .y = 1}, .eof = false };
  strncpy(r.filename, filename, 256);
  return r;
}

typedef enum token_type {
  NNULL = 0,
  TIMES,
  RDIV,
  DIV,
  MOD,
  AND,
  PLUS,
  MINUS,
  OR,
  EQL,
  NEQ,
  LSS,
  LEQ,
  GTR,
  GEQ,
  IN,
  IS,
  ARROW,
  PERIOD,
  CHAR,
  INT,
  REAL,
  FALSE,
  TRUE,
  NIL,
  STRING,
  NOT,
  LPAREN,
  LBRAK,
  LBRACE,
  IDENT,
  IF,
  WHILE,
  REPEAT,
  CASE,
  FOR,
  COMMA,
  COLON,
  BECOMES,
  UPTO,
  RPAREN,
  RBRAK,
  RBRACE,
  THEN,
  OF,
  DO,
  TO,
  BY,
  SEMICOLON,
  END,
  BAR,
  ELSE,
  ELSEIF,
  UNTIL,
  RETURN,
  ARRAY,
  RECORD,
  POINTER,
  CONST,
  TYPE,
  VAR,
  PROCEDURE,
  BEGIN,
  IMPORT,
  MODULE
} token_type_t;

const int IDENTIFIER_LENGTH = 32;
const int NUMBER_OF_KEYWORDS = 33;
const int STRING_BUF_SIZE = 256;

typedef struct identifier {
  char name[IDENTIFIER_LENGTH];
} ident_t;

typedef struct string {
  char buf[STRING_BUF_SIZE];
} string_t;

typedef struct number {
  char buf[16];
  double rval;
  int    ival;
} number_t;

typedef struct token {
  token_type_t type;
  union {
    ident_t ident;
    string_t string;
    number_t number;
  };
} token_t;

typedef struct key_table_entry {
  token_type_t type;
  char id[12];
} key_table_entry_t;

typedef struct lex_context {
  char ch; /* last character read */
  int err_count;
  int kwx[10];
  key_table_entry_t key_table[NUMBER_OF_KEYWORDS];
} lex_context_t;

void
lex_mark(reader_t * r, char * err)
{
  printf("%s:%i:%i: %s\n", r->filename, r->position.y, r->position.x, err);
}

token_t
lex_identifier(lex_context_t * ctx, reader_t * r, char curr)
{
  token_t tok;
  int i = 0, k = 0;
  char ch = curr;
  do {
    if(i < IDENTIFIER_LENGTH-1) { tok.ident.name[i] = ch; ++i; }
    ch = read_char(r);
  } while(!(((ch < '0') || (ch > '9')) && ((ch < 'A') || (ch > 'Z')) && ((ch < 'a') || (ch > 'z'))));
  tok.ident.name[i] = 0;
  
  if (i<10) {
    for(k = ctx->kwx[i-1]; k < ctx->kwx[i]; ++k) {
      if(!strcmp(tok.ident.name, ctx->key_table[k].id)){
	tok.type = ctx->key_table[k].type;
	return tok;
      }
    }
  }
  tok.type = IDENT;
  ctx->ch = ch;
  return tok;
}

token_t
lex_string(lex_context_t * ctx, reader_t * r)
{
  token_t tok;
  int i = 0;
  char ch = 0;
  ch = read_char(r);
  while (!(r->eof) && (ch != '"')) {
    if (ch >= ' ') {
      if (i < STRING_BUF_SIZE-1) {
	tok.string.buf[i] = ch;
	i++;
      } else {
	lex_mark(r, "string too long.");
      }
    }
    ch = read_char(r);
  }
  tok.string.buf[i] = 0;
  tok.type = STRING;
  ctx->ch = read_char(r);
  return tok;
}

token_t
lex_hex_string(lex_context_t * ctx, reader_t * r)
{
  token_t tok;
  int i = 0;
  return tok;
}

token_t
lex_number(lex_context_t * ctx, reader_t * r, char curr)
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
      lex_mark(r, "too many digits");
      n = 0;
    }
    ch = read_char(r);
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
    ch = read_char(r);
    for(int i = 0; i<n; ++i) {
      x = x * 10.0 + tok.number.buf[i];
    }
    int e = 0;
    while(ch >= '0' && ch <= 9) {
      x = x * 10.0 + ch - 0x30;
      e--;
      ch = read_char(r);
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

void
lex_comment(lex_context_t * ctx, reader_t *r)
{
  char ch = read_char(r);
  do {
    while((ch != '*') && !(r->eof)) {
      if (ch == '(') {
	ch = read_char(r);
	if (ch == '*') {
	  lex_comment(ctx, r);
	  ch = ctx->ch;
	} 
      } else {
	ch = read_char(r);
      }
    }
    while (ch == '*') { ch = read_char(r); }
  } while(!(ch == ')' || r->eof));
  if (!r->eof) {
    ch = read_char(r);
  } else {
    lex_mark(r, "unterminated comment");
  }
  ctx->ch = ch;
}

token_t
lex_token(lex_context_t * ctx, reader_t * r) {
  token_t tok;
  char ch = ctx->ch;
  do {
    while (!(r->eof) && (ch <= 0x20)) {
      ch = read_char(r);
    } /* skip whitespace and control characters */
    if (ch < 'A') {
      if (ch < '0') {
	switch (ch) {
	case '"':
	  tok = lex_string(ctx, r);
	  ch = ctx->ch;
	  break;
	case '#':
	  ch = read_char(r);
	  tok.type = NEQ;
	  break;
	case '$':
	  tok = lex_hex_string(ctx, r);
	  ch = ctx->ch;
	  break;
	case '&':
	  ch = read_char(r);
	  tok.type = AND;
	  break;
	case '(':
	  ch = read_char(r);
	  if (ch == '*') { tok.type = NNULL; lex_comment(ctx, r); ch = ctx->ch;} else { tok.type = LPAREN; };
	  break;
	case ')':
	  ch = read_char(r);
	  tok.type = RPAREN;
	  break;
	case '*':
 	  ch = read_char(r);
	  tok.type = TIMES;
	  break;
	case '+':
	  ch = read_char(r);
	  tok.type = PLUS;
	  break;
	case ',':
	  ch = read_char(r);
	  tok.type = COMMA;
	  break;
	case '-':
	  ch = read_char(r);
	  tok.type = MINUS;
	  break;
	case '.':
	  ch = read_char(r);
	  if (ch == '.') { ch = read_char(r); tok.type = UPTO; } else { tok.type = PERIOD; };
	  break;
	case '/':
	  ch = read_char(r);
	  tok.type = RDIV;
	  break;
	default:
	  ch = read_char(r);
	  tok.type = NNULL;
	}
      } else if (ch < ':') {
	tok = lex_number(ctx, r, ch);
	ch = ctx->ch;
      } else {
	switch (ch) {
	case ':':
	  ch = read_char(r);
	  if(ch == '=') { ch = read_char(r); tok.type = BECOMES; } else { tok.type = COLON; }
	  break;
	case ';':
	  ch = read_char(r);
	  tok.type = SEMICOLON;
	  break;
	case '<':
	  ch = read_char(r);
	  if(ch == '=') { ch = read_char(r); tok.type = LEQ; } else { tok.type = LSS; };
	  break;
	case '=':
	  ch = read_char(r);
	  tok.type = EQL;
	  break;
	case '>':
	  ch = read_char(r);
	  if(ch == '=') { ch = read_char(r); tok.type = GEQ; } else { tok.type = GTR; };
	  break;
	default:
	  ch = read_char(r);
	  tok.type = NNULL;
	}
      }
    } else if (ch < '[') {
      ctx->ch = ch;
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
      ch = read_char(r);
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
      ch = read_char(r);
    }
  } while((tok.type == NNULL) && (r->eof == false));
  ctx->ch = ch;
  return tok;
}

void
lex_enter_keyword(lex_context_t * ctx, token_type_t type, const char * name, int k)
{
  strncpy(ctx->key_table[k].id, name, 12);
  ctx->key_table[k].type = type;
}

lex_context_t
lex_init(void)
{
  lex_context_t ctx = { .ch = 0, .err_count = 0, .kwx = {}, .key_table = {}};
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

const char *
token_name[] = {
  "NNULL     ",
  "*         ",
  "RDIV      ",
  "%         ",
  "MOD       ",
  "&         ",
  "+         ",
  "-         ",
  "or        ",
  "=         ",
  "#         ",
  "<         ",
  "<=        ",
  ">         ",
  ">=        ",
  "in        ",
  "is        ",
  "^         ",
  ":         ",
  "char      ",
  "123322234 ",
  "REAL      ",
  "false     ",
  "true      ",
  "nil       ",
  "\"test 2\"",
  "~         ",
  "(         ",
  "[         ",
  "{         ",
  "testing   ",
  "if        ",
  "while     ",
  "repeat    ",
  "case      ",
  "for       ",
  ",         ",
  ":         ",
  ":=        ",
  "..        ",
  ")         ",
  "]         ",
  "}         ",
  "then      ",
  "of        ",
  "do        ",
  "to        ",
  "by        ",
  ";         ",
  "end       ",
  "bar       ",
  "else      ",
  "elseif    ",
  "until     ",
  "return    ",
  "array     ",
  "record    ",
  "pointer   ",
  "const     ",
  "type      ",
  "var       ",
  "procdure  ",
  "begin     ",
  "import    ",
  "module    "
};

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
  ident_t name;
  long    val;
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


/* void */
/* qualident() */
/* { */

/* } */


/* void */
/* check_int(reader_t * r, item_t * x) */
/* { */
/*   if (x->type->form != FINT) { */
/*     lex_mark(r, "not Integer"); */
/*     x->type = &Int_Type; */
/*   } */
/* } */

/* void */
/* check_real(reader_t *r, item_t * x) */
/* { */
/*   if (x->type->form != FREAL) { */
/*     lex_mark(r, "not Real"); */
/*     x->type = &Real_Type; */
/*   } */
/* } */


/* void */
/* compatible_types() */
/* { */
  
/* } */


  
/* void */
/* factor(lex_context_t * ctx, reader_t * r, token_t * tok) */
/* { */
/*   if (tok->type < CHAR || tok->type > IDENT) { */
/*     lex_mark(r, "expression expected."); */
/*     do { lex_token(ctx, r, tok); } while (tok->type < CHAR || tok->type > IDENT); */
/*   } */
/*   switch (tok->type) { */
/*   case IDENT: */
/*     qualident(); */
/*   case INT:  make_const_item(); lex_token(ctx, r, tok); break; */
/*   case REAL: make_real_item(); lex_token(ctx, r, tok); break; */
/*   case CHAR: make_const_item(); lex_token(ctx, r, tok); break; */
/*   case LPAREN: lex_token(ctx, r, tok); expression(); check(RPAREN, "no )"); break; */
/*   case RBRACE: lex_token(ctx, r, tok); set(); check(RBRACE, "no )"); break; */
/*   case NOT: lex_token(ctx, r, tok); factor(ctx, r, tok); check_bool(); gen_not(); break; */
/*   case FALSE: make_const_item(); lex_token(ctx, r, tok); break; */
/*   case TRUE:  make_const_item(); lex_token(ctx, r, tok); break; */
/*   default: lex_mark(r,"not a factor"); */
/*   } */
/* } */

/* void */
/* term(lex_context_t * ctx, reader_t * r, token_t * tok) */
/* { */
/*   factor(ctx, r, tok); */
/*   item_t x; // dummy for now. */
/*   form_t f = x.type->form; */
/*   while(tok->type >= TIMES && tok->type <= AND) { */
/*     token_type_t op = tok->type; lex_token(ctx, r, tok); */
/*     switch (op) { */
/*     case TIMES: */
/*       switch (f) { */
/*       case FINT: */
/* 	factor(ctx, r, tok); check_int(); mul_op(); break; */
/*       case FREAL: */
/* 	factor(ctx, r, tok); check_real(); real_op(); break; */
/*       case FSET: */
/*       default: */
/* 	lex_mark(r, "bad type"); */
/*       } */
/*     case DIV: */
/*     case MOD: */
/*       check_int(); factor(); check_int(); div_op(); break; */
/*     default: check_bool(); and1(x); factor(); check_bool(); and2(); break; */
/*     } */
/*   } */
/* } */

/* void */
/* simple_expression() */
/* { */

/* } */

/* void */
/* module(lex_context_t * ctx, reader_t *restrict r, writer_t *restrict w) */
/* { */
/*   token_t tok; */
/*   tok = lex_token(ctx, r); */
/*   if (tok.type == MODULE) { */
/*     tok = lex_token(ctx, r); */
/*   } else { */
/*     lex_mark(r, "must start with module"); */
/*   } */
/* } */


void
write_test_file(char * filename)
{
  FILE* f = fopen(filename, "w");
  int i;
  for(int j = 0; j < 100000000; ++j) {
      i = random()%MODULE;
      fwrite(token_name[i], sizeof(char), 10, f);
  }
}


int main(){
  srandom(12);
  //write_test_file("test2.ob");
  reader_t r = reader_init("test2.ob");
  lex_context_t ctx = lex_init();
  token_t tok;
  clock_t begin = clock();
  do {
    tok = lex_token(&ctx, &r);
    //printf("%s\n", token_name[tok.type]);
  } while (!r.eof);
  printf("time passed: %ld\n", clock() - begin);
  return 0;
}
