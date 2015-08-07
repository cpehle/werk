typedef enum Tokentype {
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
} Tokentype;

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

#define IDENTIFIER_LENGTH 256
#define NUMBER_OF_KEYWORDS 256
#define STRING_BUF_SIZE 256

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

typedef struct Token {
  Tokentype type;
  union {
    ident_t ident;
    string_t string;
    number_t number;
  };
} Token;


typedef struct key_table_entry {
  Tokentype type;
  char id[12];
} key_table_entry_t;

typedef struct Lexer {
  char ch; // current character
  Token token; // current token
  int err_count;
  int kwx[10];
  key_table_entry_t key_table[NUMBER_OF_KEYWORDS];
} Lexer;

typedef struct position {
  int x;
  int y;
} position_t;

typedef struct Reader {
  char filename[256];
  char * buffer;
  int buffer_length;
  int buffer_position;
  position_t position;
  int eof;
} Reader;

char readchar(Reader * r);
Reader readerinit(char * filename);


Lexer lexerinit();
Token lex_token(Lexer * ctx, Reader * r);
void lex_mark(Reader * r, char * err);
