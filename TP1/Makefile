#FLAGS
OPTIMISE=-O2
CFLAGS=--std=c11 -pedantic -W -Wall -Wextra `pkg-config --cflags --libs glib-2.0`
DEBUG=-g

#COLORS
PURPLE=@echo -en "\033[35m"
GREEN=@echo -en "\033[32m"
RESET=@echo -en "\033[0m"
CYAN=@echo -en "\033[36m"

#DIRECTORIES
T_RELEASE=target/release
T_DEBUG=target/debug
SOURCES_DIR=src
T_LEX=target/lex

#FILES
LEX_OUT=$(T_LEX)/lex_out.c
LEX_OBJ=$(T_LEX)/lex_out.o
SOURCES=$(wildcard $(SOURCES_DIR)/*.c)
HEADERS=include
OBJECTS=$(patsubst $(SOURCES_DIR)/%.c,$(T_RELEASE)/%.o,$(SOURCES))
DEBUG_OBJECTS=$(patsubst $(SOURCES_DIR)/%.c,$(T_DEBUG)/%.o,$(SOURCES))
EXE=mkhtml

# COMPILING

release: release_dir output_dir lex $(OBJECTS)
	$(CYAN)
	cc $(OPTIMISE) $(CFLAGS) $(OBJECTS) $(LEX_OBJ) -I$(HEADERS) -o $(T_RELEASE)/$(EXE)
	$(RESET)

$(OBJECTS): $(T_RELEASE)/%.o : $(SOURCES_DIR)/%.c
	$(GREEN)
	cc $(OPTIMISE) $(CFLAGS) -I$(HEADERS) -c $< -o $@
	$(RESET)

debug: debug_dir output_dir lex_debug $(DEBUG_OBJECTS)
	$(CYAN)
	cc $(DEBUG) $(CFLAGS) $(DEBUG_OBJECTS) $(LEX_OBJ) -I$(HEADERS) -o $(T_DEBUG)/$(EXE)
	$(RESET)

$(DEBUG_OBJECTS): $(T_DEBUG)/%.o : $(SOURCES_DIR)/%.c
	$(GREEN)
	cc $(DEBUG) $(CFLAGS) -I$(HEADERS) -c $< -o $@
	$(CYAN)

lex: $(SOURCES_DIR)/main.l
	@mkdir -p $(T_LEX)
	$(PURPLE)
	flex -o $(LEX_OUT) $(SOURCES_DIR)/main.l
	$(GREEN)
	cc $(OPTIMISE) -I$(HEADERS) -c $(LEX_OUT) -o $(LEX_OBJ)
	$(RESET)

lex_debug: $(SOURCES_DIR)/main.l
	@mkdir -p $(T_LEX)
	$(PURPLE)
	flex -o $(LEX_OUT) $(SOURCES_DIR)/main.l
	$(GREEN)
	cc $(DEBUG) -I$(HEADERS) -c $(LEX_OUT) -o $(LEX_OBJ)
	$(RESET)

# DIRECTORIES

release_dir:
	@mkdir -p $(T_RELEASE)

debug_dir:
	@mkdir -p $(T_DEBUG)

output_dir:
	@mkdir -p noticias/tags/

# OTHER SCRIPTS
grind: debug unicode
	valgrind --leak-check=full --show-leak-kinds=all target/debug/mkhtml input/folha8.OUT.clean.txt

bench: release unicode
	@mkdir -p benchmarks
	./run-benchmarks.sh

unicode:
	./clean_unicode.sh input/folha8.OUT.txt

clean:
	rm -rf $(T_RELEASE)
	rm -rf $(T_DEBUG)
	rm -rf $(T_LEX)
	rm -rf noticias/*
	rm -f $(EXE)
