EXE=mkhtml
CFLAGS=-O2
DEBUG=-g
LEX_OUT=src/lex_out.c
OBJECTS=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))

default: lex $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) -o $(EXE)

debug: lex $(OBJECTS)
	cc $(DEBUG) $(OBJECTS) -o $(EXE)

lex:
	flex -o $(LEX_OUT) src/main.l

clean:
	rm -f src/$(LEX_OUT)
	rm -f src/*.o
	rm -f noticias/*.hmtl
	rm $(EXE)
