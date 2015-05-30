CFLAGS=-g -std=c99

werk: lexer.o parser.o main.o reader.o
	$(CC) lexer.o parser.o main.o reader.o -o werk

reader.o: reader.c lexer.h
	$(CC) $(CFLAGS) -c reader.c -o reader.o

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o lexer.o

parser.o: parser.c parser.h lexer.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

main.o: main.c parser.h lexer.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm *.o
