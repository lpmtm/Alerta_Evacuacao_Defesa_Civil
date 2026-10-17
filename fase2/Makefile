CC     := gcc
CFLAGS := -Wall -O2 -Isrc
LDLIBS := -lm
BIN    := bin
DATA   := data/mapa_brumadinho.geojson

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

HEADERS  := $(wildcard src/*.h)
LIB_SRC  := $(filter-out $(wildcard src/main_*.c),$(wildcard src/*.c))
LIB_OBJ  := $(patsubst src/%.c,$(BIN)/obj/%.o,$(LIB_SRC))
MAIN_BIN := $(patsubst src/%.c,$(BIN)/%$(EXE),$(wildcard src/main_*.c))
TEST_BIN := $(patsubst tests/%.c,$(BIN)/%$(EXE),$(wildcard tests/teste_*.c))

.PHONY: all clean run help
.SECONDARY:

all: $(MAIN_BIN) $(TEST_BIN)

$(BIN)/obj/%.o: src/%.c $(HEADERS) | $(BIN)/obj
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%$(EXE): src/%.c $(LIB_OBJ) $(HEADERS) | $(BIN)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@ $(LDLIBS)

$(BIN)/%$(EXE): tests/%.c $(LIB_OBJ) $(HEADERS) | $(BIN)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@ $(LDLIBS)

$(BIN) $(BIN)/obj:
	mkdir -p $@

run: all
	@test -n "$(T)" || { echo "Use: make run T=nome_do_executavel"; exit 1; }
	./$(BIN)/$(T)$(EXE) $(DATA)

clean:
	rm -rf $(BIN)

help:
	@echo "make               compila todos os executaveis (src/main_*.c e tests/teste_*.c)"
	@echo "make run T=nome    compila e roda bin/nome com o dataset"
	@echo "make clean         apaga a pasta bin/"
