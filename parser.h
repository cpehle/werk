typedef struct Parser {
   Lexer *lex;
   Reader * r;
   Token tok;
   Token peek;
} Parser;
Parser parserinit(Lexer *lex, Reader *reader);
void parseprogram(Parser *c);
