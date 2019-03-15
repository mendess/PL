EXE=mkhtml
CFLAGS=-O2
DEBUG=-g
LEX_OUT=lex_out.c

default: lex
	cc $(CFLAGS) $(LEX_OUT) -o $(EXE)

debug: lex
	cc $(DEBUG) $(LEX_OUT) -o $(EXE)

lex:
	flex -o $(LEX_OUT) main.l

run: default
	./$(EXE)

clean:
	rm -f $(LEX_OUT)
	rm -f *.o
	rm -f *.hmtl
