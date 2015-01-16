typedef struct parse_ctx {
	lex_context_t *lex;
	reader_t * r;
	token_t tok;
} parse_ctx_t;
parse_ctx_t parser_init(lex_context_t *lex, reader_t *reader);
void parse_program(parse_ctx_t *c);
