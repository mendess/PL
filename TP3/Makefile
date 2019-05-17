#FLAGS
OPTIMISE=-O2
CFLAGS=--std=c11 -pedantic -W -Wall -Wextra
DEBUG=-g

ifeq ($(shell uname),Darwin)
    EXT := dylib
else
    EXT := so
endif

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
release: __release_dir __output_dir $(GIC_OBJ_R) $(OBJECTS) target/release/libsati.$(EXT)
	cc $(OPTIMISE) $(CFLAGS) $(OBJECTS) $(GIC_OBJ_R) -I$(HEADERS) -L ./target/release/ -lsati -o $(T_RELEASE)/$(EXE)

$(OBJECTS): $(T_RELEASE)/%.o : $(SOURCES_DIR)/%.c
	cc $(OPTIMISE) $(CFLAGS) -I$(HEADERS) -c $< -o $@

debug: __debug_dir __output_dir $(GIC_OBJ_D) $(DEBUG_OBJECTS) target/debug/libsati.$(EXT)
	cc $(DEBUG) $(CFLAGS) $(DEBUG_OBJECTS) $(GIC_OBJ_D) -I$(HEADERS) -L ./target/debug/ -lsati -o $(T_DEBUG)/$(EXE)

$(DEBUG_OBJECTS): $(T_DEBUG)/%.o : $(SOURCES_DIR)/%.c
	cc $(DEBUG) $(CFLAGS) -I$(HEADERS) -c $< -o $@

__lex: $(SOURCES_DIR)/sati.l
	@mkdir -p $(T_GIC)
	flex -o $(LEX_OUT) $<

$(GIC_OBJ_R): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	yacc $< -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h
	cc $(OPTIMISE) -I$(HEADERS) -I$(T_GIC) -c $(GIC_OUT) -o $(GIC_OBJ_R)

$(GIC_OBJ_D): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	yacc $< -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h
	cc $(DEBUG) -I$(HEADERS) -c $(GIC_OUT) -o $(GIC_OBJ_D)

target/release/libsati.$(EXT): src/sati.rs Cargo.toml
	cargo build --release

target/debug/libsati.$(EXT): src/sati.rs Cargo.toml
	cargo build --release

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
	cargo clean
