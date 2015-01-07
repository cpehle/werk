#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

void
read_char(reader_t *restrict r, char * c){
  if(r->buffer_position < r->buffer_length) {
    *c = r->buffer[r->buffer_position];
    r->buffer_position++;
    if(*c == '\n') {
      r->position.x = 0;
      r->position.y++;
    }
  } else {
    r->eof = true;
    *c = -1;
  }
}

reader_t
reader_init(char * filename)
{
  FILE * f = fopen("test.ob", "rb");
  if (!f) {
    reader_t r = {};
    return r;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char * buf =  malloc(fsize + 1);
  fread(buf , fsize, 1, f);
  reader_t r = { .filename = "", .buffer = buf, .buffer_length = fsize, .buffer_position = 0, .position = { .x = 0, .y = 0}, .eof = false };
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

typedef struct token {
  token_type_t type;
  union {
    ident_t ident;
    string_t string;
  };
} token_t;

typedef struct key_table_entry {
  token_type_t type;
  char id[12];
} key_table_entry_t;



typedef struct lex_context {
  char ch; /* last character read */
  position_t err_pos;
  int err_count;
  int kwx[10];
  key_table_entry_t key_table[NUMBER_OF_KEYWORDS];
} lex_context_t;


void
lex_mark(reader_t * r, char * err)
{
  printf("%s:%i:%i: %s\n", r->filename, r->position.y, r->position.x, err);
}

void
lex_identifier(lex_context_t * ctx, reader_t * r, token_t * tok)
{
  int i = 0, k = 0;
  char * cp = &ctx->ch; 
  do {
    if(i < IDENTIFIER_LENGTH-1) { tok->ident.name[i] = *cp; ++i; }
    read_char(r, cp);
  } while(!(((*cp < '0') || (*cp > '9')) && ((*cp < 'A') || (*cp > 'Z')) && ((*cp < 'a') || (*cp > 'z'))));
  tok->ident.name[i] = 0;
  
  if (i<10) {
    for(k = ctx->kwx[i-1]; k < ctx->kwx[i]; ++k) {
      if(!strcmp(tok->ident.name, ctx->key_table[k].id)){
	tok->type = ctx->key_table[k].type;
	return;
      }
    }
  }
  tok->type = IDENT;
}

void
lex_string(lex_context_t * ctx, reader_t * r, token_t * tok)
{
  int i = 0;
  char ch = ctx->ch;
  read_char(r, &ch);
  while (!(r->eof) && (ch != '"')) {
    if (ch >= ' ') {
      if (i < STRING_BUF_SIZE-1) {
	tok->string.buf[i] = ch;
	i++;
      } else {
	lex_mark(r, "string too long.");
      }
    }
  }
}

void
lex_hex_string(lex_context_t * ctx, reader_t * r, token_t * tok)
{
  int i = 0;
}

void
lex_number(lex_context_t * ctx, reader_t * r, token_t * tok)
{
  int i = 0;
}

void
lex_comment(lex_context_t * ctx, reader_t *r)
{

}


void
lex_token(lex_context_t * ctx, reader_t * r, token_t * tok) {
  char * cp = &ctx->ch;
  do {
    while (!(r->eof) && (*cp <= 0x20)) {
      read_char(r,cp);
    } /* skip whitespace and control characters */
    if (*cp < 'A') {
      if (*cp < '0') {
	switch (*cp) {
	case '"':
	  lex_string(ctx, r, tok);
	  break;
	case '#':
	  read_char(r,cp);
	  tok->type = NEQ;
	  break;
	case '$':
	  lex_hex_string(ctx, r, tok);
	  break;
	case '&':
	  read_char(r,cp);
	  tok->type = AND;
	  break;
	case '(':
	  read_char(r,cp);
	  if (*cp == '*') { tok->type = NNULL; lex_comment(ctx, r); } else { tok->type = LPAREN; };
	  break;
	case ')':
	  read_char(r,cp);
	  tok->type = RPAREN;
	  break;
	case '*':
	  read_char(r,cp);
	  tok->type = TIMES;
	  break;
	case '+':
	  read_char(r,cp);
	  tok->type = PLUS;
	  break;
	case ',':
	  read_char(r,cp);
	  tok->type = COMMA;
	  break;
	case '-':
	  read_char(r,cp);
	  tok->type = MINUS;
	  break;
	case '.':
	  read_char(r,cp);
	  if (*cp == '.') { read_char(r, cp); tok->type = UPTO; } else { tok->type = PERIOD; };
	  break;
	case '/':
	  read_char(r,cp);
	  tok->type = RDIV;
	  break;
	default:
	  read_char(r,cp);
	  tok->type = NNULL;
	}
      } else if (*cp < ':') {
	lex_number(ctx, r, tok);
      } else {
	switch (*cp) {
	case ':':
	  read_char(r,cp);
	  if(*cp == '=') { read_char(r,cp); tok->type = BECOMES; } else { tok->type = COLON; }
	  break;
	case ';':
	  read_char(r,cp);
	  tok->type = SEMICOLON;
	  break;
	case '<':
	  read_char(r,cp);
	  if(*cp == '=') { read_char(r, cp); tok->type = LEQ; } else { tok->type = LSS; };
	  break;
	case '=':
	  read_char(r,cp);
	  tok->type = EQL;
	  break;
	case '>':
	  read_char(r,cp);
	  if(*cp == '=') { read_char(r, cp); tok->type = GEQ; } else { tok->type = GTR; };
	  break;
	default:
	  read_char(r,cp);
	  tok->type = NNULL;
	}
      }
    } else if (*cp < '[') {
      lex_identifier(ctx, r, tok);
    } else if (*cp < 'a') {
      switch (*cp) {
      case '[':
	  tok->type = RBRAK;
	  break;
      case ']':
	tok->type = LBRAK;
	break;
      case '^':
	tok->type = ARROW;
	break;
      default:
	tok->type = NNULL;
      }
      read_char(r,cp);
    } else if (*cp < '{') {
      lex_identifier(ctx, r, tok);
    } else {
      switch (*cp) {
      case '{':
	tok->type = LBRACE;
	break;
      case '}':
	tok->type = RBRACE;
	break;
      case '|':
	tok->type = BAR;
	break;
      case '~':
	tok->type = NOT;
	break;
      default:
	tok->type = NNULL;
      }
      read_char(r,cp);
    }
  } while((tok->type == NNULL) && (r->eof == false));
}

void
lex_enter_keyword(lex_context_t * ctx, token_type_t type, const char * name, int k)
{
  strcpy(ctx->key_table[k].id, name);
  ctx->key_table[k].type = type;
}

lex_context_t
lex_init(void)
{

  lex_context_t ctx = { .ch = 0, .err_pos = { .x = 0, .y = 0 }, .err_count = 0, .kwx = {}, .key_table = {}};
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
  reader_t r = reader_init("test.ob");
  lex_context_t ctx = lex_init();
  for(int i = 0; i < 10; ++i) {
    printf("ctx.kwx[%i] = %i\n", i, ctx.kwx[i]);
  }
  for(int i = 0; i < NUMBER_OF_KEYWORDS; ++i) {
    printf("ctx.keyword_table[%i] = %s\n", i, ctx.key_table[i].id);
  }
  token_t tok = { .type = NNULL, .ident = { .name = "" }};
  do {
    lex_token(&ctx, &r, &tok);
    printf("%s\n", token_name[tok.type]);
  } while (!r.eof);

  return 0;
}
