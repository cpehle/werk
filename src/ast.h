typedef union literal {
  
} literal_t;

typedef union ast {
  
} ast_t;

void ast_literal(ast_t * ast);
void ast_module(ast_t * ast);
void ast_expression(ast_t * ast);
void ast_program(ast_t * ast);
