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
T_GIC=target/gic

#FILES
LEX_OUT=$(T_GIC)/lex.yy.c
GIC_OUT=$(T_GIC)/y.tab.c
GIC_OBJ_R=$(T_RELEASE)/y.tab.o
GIC_OBJ_D=$(T_DEBUG)/y.tab.o
SOURCES=$(wildcard $(SOURCES_DIR)/*.c)
HEADERS=include
OBJECTS=$(patsubst $(SOURCES_DIR)/%.c,$(T_RELEASE)/%.o,$(SOURCES))
DEBUG_OBJECTS=$(patsubst $(SOURCES_DIR)/%.c,$(T_DEBUG)/%.o,$(SOURCES))
EXE=sati

# COMPILING

release: __release_dir __output_dir $(GIC_OBJ_R) $(OBJECTS)
	$(CYAN)
	cc $(OPTIMISE) $(CFLAGS) $(OBJECTS) $(GIC_OBJ_R) -I$(HEADERS) -o $(T_RELEASE)/$(EXE)
	$(RESET)

$(OBJECTS): $(T_RELEASE)/%.o : $(SOURCES_DIR)/%.c
	$(GREEN)
	cc $(OPTIMISE) $(CFLAGS) -I$(HEADERS) -c $< -o $@
	$(RESET)

debug: __debug_dir __output_dir $(GIC_OBJ_D) $(DEBUG_OBJECTS)
	$(CYAN)
	cc $(DEBUG) $(CFLAGS) $(DEBUG_OBJECTS) $(GIC_OBJ_D) -I$(HEADERS) -o $(T_DEBUG)/$(EXE)
	$(RESET)

$(DEBUG_OBJECTS): $(T_DEBUG)/%.o : $(SOURCES_DIR)/%.c
	$(GREEN)
	cc $(DEBUG) $(CFLAGS) -I$(HEADERS) -c $< -o $@
	$(CYAN)

__lex: $(SOURCES_DIR)/sati.l
	@mkdir -p $(T_GIC)
	$(PURPLE)
	flex -o $(LEX_OUT) $<
	$(RESET)

$(GIC_OBJ_R): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	$(PURPLE)
	yacc -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h $<
	$(GREEN)
	cc $(OPTIMISE) -I$(HEADERS) -I$(T_GIC) -c $(GIC_OUT) -o $(GIC_OBJ_R)
	$(RESET)

$(GIC_OBJ_D): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	$(PURPLE)
	yacc -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h $<
	$(GREEN)
	cc $(DEBUG) -I$(HEADERS) -c $(GIC_OUT) -o $(GIC_OBJ_D)
	$(RESET)

# DIRECTORIES

__release_dir:
	@mkdir -p $(T_RELEASE)

__debug_dir:
	@mkdir -p $(T_DEBUG)

__output_dir:
	@mkdir -p anotated

__include_dir:
	@mkdir -p include

# OTHER SCRIPTS
clean:
	rm -rf $(T_RELEASE)
	rm -rf $(T_DEBUG)
	rm -rf $(T_GIC)
	rm -rf noticias/*
	rm -f $(EXE)
