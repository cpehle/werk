typedef enum {
        Int,
        Float,
        Binop,
        Fun
} TermKind;

typedef struct {
        TermKind t;
        union {
                struct {
                        short idx;
                } index;
                int x;
                float f;
        } u;
} TermDesc;
