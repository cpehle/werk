CFLAGS:=-g

werk: obj/lexer.o obj/parser.o obj/main.o obj/reader.o
	$(CC) obj/lexer.o obj/parser.o obj/main.o obj/reader.o -o werk

obj/reader.o: reader.c lexer.h
	$(CC) $(CFLAGS) -c reader.c -o obj/reader.o

obj/lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o obj/lexer.o

obj/parser.o: parser.c parser.h lexer.h
	$(CC) $(CFLAGS) -c parser.c -o obj/parser.o

obj/main.o: main.c parser.h lexer.h
	$(CC) $(CFLAGS) -c main.c -o obj/main.o

clean:
	rm obj/*
