typedef struct parse_ctx {
	lex_context_t *lex;
    Reader * r;
	token_t tok;
        token_t peek;
} parse_ctx_t;
parse_ctx_t parser_init(lex_context_t *lex, Reader *reader);
void parse_program(parse_ctx_t *c);
