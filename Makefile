CFLAGS=-g -std=c11 -I werk/
CHEADERS=$(wildcard werk/*.h)
CFILES=$(wildcard werk/*.c)
OBJFILES=$(patsubst build/%.o,%c,$(CFILES))

werk.native: $(OBJFILES)
	mkdir build/
	$(CC) $(CFLAGS) $(OBJFILES) -o werk.native


build/%.o: werk/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm werk.native
	rm -rf build/
