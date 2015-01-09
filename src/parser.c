
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



