typedef struct Parser {
   Lexer *lex;
   Reader * r;
   Token token;
   Token peek;
} Parser;
Parser parserinit(Lexer *lex, Reader *reader);
void parseprogram(Parser *c, TermDesc* v);
