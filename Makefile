werk: obj/lexer.o obj/parser.o obj/main.o obj/reader.o
	clang obj/lexer.o obj/parser.o obj/main.o obj/reader.o -o werk

obj/reader.o: src/reader.c src/lexer.h
	clang -c src/reader.c -o obj/reader.o

obj/lexer.o: src/lexer.c src/lexer.h
	clang -c src/lexer.c -o obj/lexer.o

obj/parser.o: src/parser.c src/parser.h src/lexer.h
	clang -c src/parser.c -o obj/parser.o

obj/main.o: src/main.c src/parser.h src/lexer.h
	clang -c src/main.c -o obj/main.o
