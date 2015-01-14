#include <stdint.h>
#define global_variable static
#define local_persist static
#define internal static

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int   s32;
typedef long  s64;
typedef short s16;
typedef char  s8;

typedef double r64;
typedef float  r32;

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

typedef struct identifier {
  int len;
  char name[];
} ident_t;

typedef struct string {
  int len;
  char buf[];
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

struct lex_position {
  int x, y;
};

typedef struct lex_context {
  char ch; // last character read
  int idx_in;  // index into kant_input buffer
  int idx_out; // index into kant_output buffer
  char filename[32];
  struct lex_position pos;
  int eof;
  int err_count;
  int kwx[10];
  key_table_entry_t key_table[MODULE+1];
} lex_context_t;

typedef struct kant_memory {
  u64 permanent_storage_size;
  void * permanent_storage;
  u64 transient_storage_size;
  void * transient_storage;
} kant_memory_t;

typedef struct kant_input {
  int buffer_length;
  char * buffer;
} kant_input_t;

typedef struct kant_output {
  int token_count;
  token_t * tokens;
} kant_output_t;

typedef struct kant_state {
  lex_context_t lex_context;

} kant_state_t;

kant_state_t lex_init();
void lex_buf(kant_memory_t *, kant_state_t *restrict, const kant_input_t *restrict, kant_output_t *restrict);
