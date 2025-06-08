CC = gcc
ECC = emcc
CINC = -Iinclude/
CFLAGS = -Wall -Wextra -Iinclude/ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
EFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj
LIB_DIR=lib
INDEX_DIR=index


.PHONY: all
all: always $(INDEX_DIR)/index

$(OBJ_DIR)/em_main.o: $(SRC_DIR)/main.c
	$(ECC) -c $< $(CINC) $(EFLAGS) -o $@

$(OBJ_DIR)/em_game.o: $(SRC_DIR)/game.c
	$(ECC) -c $< $(CINC) $(EFLAGS) -o $@

$(INDEX_DIR)/index: $(OBJ_DIR)/em_main.o $(OBJ_DIR)/em_game.o $(LIB_DIR)/libArchimedes.a
	mkdir $(INDEX_DIR)
	$(ECC) $^ -s WASM=1 --shell-file htmlTemplate/template.html --preload-file resources/ -o $@.html $(EFLAGS)


.PHONY: native
native: always $(BIN_DIR)/native

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(OBJ_DIR)/game.o: $(SRC_DIR)/game.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(BIN_DIR)/native: $(OBJ_DIR)/main.o $(OBJ_DIR)/game.o
	$(CC) $^ -ggdb -lArchimedes $(CFLAGS) -o $@


.PHONY: bear
bear:
	bear -- make


.PHONY: bearclean
bearclean:
	rm compile_commands.json


.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(INDEX_DIR)
	clear


.PHONY: always
always:
	mkdir $(OBJ_DIR) $(BIN_DIR)
