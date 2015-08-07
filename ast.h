typedef enum {
        Int,
        Float,
        Binop,
        Fun
} Tag;

typedef struct {
        Tag t;
        union {
                int x;
                float f;
        } v;
} Term;
