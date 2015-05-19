#include <stdio.h>
#include "test_lexer.c"

int main() {
  int success = 1;
  success = success && test_lexer();
  printf("%s\n", success ? "Success" : "Failure");
  return 0;
}
