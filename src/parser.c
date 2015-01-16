#include "lexer.h"
#include "ast.h"
#include "parser.h"

static int accept(parse_ctx_t * c, token_type_t t) {
	if (c->tok.type == t) {
		c->tok = lex_token(c->lex, c->r);
		return 1;
	}
	return 0;
}
 
static int expect(parse_ctx_t * c, token_type_t t, char * error_msg) {
	if (accept(c, t)) {
		return 1;
	}
	lex_mark(c->r, error_msg);
	return 0;
}

static void type_signature(parse_ctx_t *c)
{
	expect(c, PERIOD, "Expected \".\" at end of type signature.");
}

static void expression(parse_ctx_t *c)
{
	if (accept(c, LPAREN)) {
		if (accept(c, VAR)) {
			while(accept(c, IDENT)) {
      
			}
			if (accept(c, COLON)) {
				type_signature(c,ast);
			}
			do {
				expression(c,ast);
			} while(c->tok.type != RPAREN);
		} else if (accept(c, PROCEDURE)) {
      
		}
		expect(c, RPAREN, "No closing ).");
	} else if (accept(c, IDENT)) {

	} else if (accept(c, INT)) {

	} else if (accept(c, REAL)) {

	} else {

	}
}

static ast_t module_signature(parse_ctx_t *c)
{
	expect(c, PERIOD, "Expected \".\" at end of module signature.");
}

void parse_program(parse_ctx_t *c)
{
	expect(c, LPAREN, "Expected ( at beginning of file.");
	expect(c, MODULE, "Expected keyword \"module\" at beginning of file.");
	expect(c, IDENT, "Expected module identifier at beginning of file.");
	if (accept(c, COLON)) {
	        module_signature(c);  
	}
	do {
	        expression(c);
	} while (c->tok.type != RPAREN);
}

parse_ctx_t parser_init(lex_context_t *lex, reader_t *reader)
{
	parse_ctx_t ctx = {};
	ctx.lex = lex;
	ctx.r = reader;
	ctx.tok = lex_token(ctx.lex, ctx.r);
	return ctx;
}
