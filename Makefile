SDIR=src/
ODIR=obj/
LDIR=lib/
BDIR=bin/
LIBS=
IDIR=include/
CC=clang
CFLAGS=-I$(IDIR) -g

_DEPS = lexer.h reader.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = reader.o lexer.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kant: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
