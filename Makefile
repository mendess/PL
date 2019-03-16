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

default: release_dir lex $(OBJECTS)
	$(CYAN)
	cc $(OPTIMISE) $(CFLAGS) $(OBJECTS) $(LEX_OBJ) -I$(HEADERS) -o $(T_RELEASE)/$(EXE)
	$(RESET)

$(OBJECTS): $(T_RELEASE)/%.o : $(SOURCES_DIR)/%.c
	$(GREEN)
	cc $(OPTIMISE) $(CFLAGS) -I$(HEADERS) -c $< -o $@
	$(RESET)

debug: debug_dir lex $(DEBUG_OBJECTS)
	$(CYAN)
	cc $(DEBUG) $(CFLAGS) $(OBJECTS) $(LEX_OBJ) -I$(HEADERS) -o $(T_DEBUG)/$(EXE)
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

release_dir:
	@mkdir -p $(T_RELEASE)

debug_dir:
	@mkdir -p $(T_DEBUG)

clean:
	rm -rf $(T_RELEASE)
	rm -rf $(T_DEBUG)
	rm -rf $(T_LEX)
	rm -f noticias/*
	rm -f $(EXE)
