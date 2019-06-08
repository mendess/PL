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

# Remove the build it rules to avoid code overwriting
%.c: %.y
%.c: %.l

# COMPILING
release: __release_dir __output_dir $(GIC_OBJ_R) $(OBJECTS) target/release/libsati.$(EXT)
	cc $(OPTIMISE) $(CFLAGS) $(OBJECTS) $(GIC_OBJ_R) -I$(HEADERS) target/release/libsati.a -lpthread -ldl -o $(T_RELEASE)/$(EXE)

$(OBJECTS): $(T_RELEASE)/%.o : $(SOURCES_DIR)/%.c
	cc $(OPTIMISE) $(CFLAGS) -I$(HEADERS) -c $< -o $@

debug: __debug_dir __output_dir $(GIC_OBJ_D) $(DEBUG_OBJECTS) target/debug/libsati.$(EXT)
	cc $(DEBUG) $(CFLAGS) $(DEBUG_OBJECTS) $(GIC_OBJ_D) -I$(HEADERS) target/debug/libsati.a -lpthread -ldl -o $(T_DEBUG)/$(EXE)

$(DEBUG_OBJECTS): $(T_DEBUG)/%.o : $(SOURCES_DIR)/%.c
	cc $(DEBUG) $(CFLAGS) -I$(HEADERS) -c $< -o $@

install: release
	sudo mkdir -p $(DESTDIR)/usr/local/bin/
	sudo cp $(T_RELEASE)/sati $(DESTDIR)/usr/local/bin/
	sudo mkdir -p $(DESTDIR)/usr/local/man/man1/
	sudo cp assets/sati.1 $(DESTDIR)/usr/local/man/man1/
	sudo mandb

uninstall:
	sudo rm $(DESTDIR)/usr/local/bin/sati
	sudo rm $(DESTDIR)/usr/share/local/man/man1/sati.1
	sudo mandb

__lex: $(SOURCES_DIR)/sati.l
	@mkdir -p $(T_GIC)
	flex -o $(LEX_OUT) $<

$(GIC_OBJ_R): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	yacc $< -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h --graph=graph.y
	-dot graph.y -Tjpg > report/sati.jpg
	rm graph.y
	cc $(OPTIMISE) -I$(HEADERS) -I$(T_GIC) -c $(GIC_OUT) -o $(GIC_OBJ_R)

$(GIC_OBJ_D): $(SOURCES_DIR)/sati.y __lex __include_dir
	@mkdir -p $(T_GIC)
	yacc $< -o $(GIC_OUT) --defines=$(HEADERS)/y.tab.h
	cc $(DEBUG) -I$(HEADERS) -c $(GIC_OUT) -o $(GIC_OBJ_D)

target/release/libsati.$(EXT): src/sati.rs Cargo.toml
	cargo build --release

target/debug/libsati.$(EXT): src/sati.rs Cargo.toml
	cargo build

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
