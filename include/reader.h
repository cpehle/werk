#include <stdbool.h>

typedef struct position {
  int x;
  int y;
} position_t;

typedef struct reader {
  char filename[256];
  char * buffer;
  int buffer_length;
  int buffer_position;
  position_t position;
  bool eof;
} reader_t;

char read_char(reader_t * r);
reader_t reader_init(char * filename);
