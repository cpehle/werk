#include "../src/lexer.h"
typedef struct {
  int count;
  char * file_names[64];
} lex_test_cases_t;

typedef struct {
  int expected_tokens[256];
} lex_expected_t;


const lex_test_cases_t test_cases = {2, {"test.kant",""}};
const lex_expected_t lex_expect[2] = {{.expected_tokens =
                                       {LPAREN,
                                        MODULE,
                                        IDENT,
                                        COLON,
                                        PERIOD,
                                        LPAREN,
                                        VAR,
                                        IDENT,
                                        IDENT,
                                        IDENT,
                                        COLON,
                                        PERIOD,
                                        INT,
                                        INT,
                                        INT,
                                        RPAREN,
                                        INT,
                                        INT,
                                        INT,
                                        INT,
                                        INT,
                                        INT,
                                        RPAREN,
                                        NNULL}},
                                      {},};

int test_lexer() {
  int success = 1;
  for(int i = 0; i < test_cases.count; ++i) {
    reader_t r = reader_init(test_cases.file_names[i]);
    lex_context_t ctx = lex_init();
    token_t tok;
    int j = 0;
    do {
       tok = lex_token(&ctx, &r);
       success = success && (lex_expect[i].expected_tokens[j] == tok.type);
       j++;
     } while (!r.eof);
  }
  return success;
}
