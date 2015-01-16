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

static const char* token_names[] = {
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

static const int IDENTIFIER_LENGTH  = 256;;
static const int NUMBER_OF_KEYWORDS  = 256;;
static const int STRING_BUF_SIZE = 256;;

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

lex_context_t lex_init();
token_t lex_token(lex_context_t * ctx, reader_t * r);

