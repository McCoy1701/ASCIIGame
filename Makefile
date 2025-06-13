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
EDITOR_DIR=editor

.PHONY: all
all: always $(INDEX_DIR)/index

$(OBJ_DIR)/em_main.o: $(SRC_DIR)/main.c
	$(ECC) -c $< $(CINC) $(EFLAGS) -o $@

$(OBJ_DIR)/em_game.o: $(SRC_DIR)/game.c
	$(ECC) -c $< $(CINC) $(EFLAGS) -o $@

$(INDEX_DIR)/index: $(OBJ_DIR)/em_main.o $(OBJ_DIR)/em_game.o $(LIB_DIR)/libArchimedes.a
	mkdir $(INDEX_DIR)
	$(ECC) $^ -s WASM=1 --shell-file htmlTemplate/template.html --preload-file resources/ -o $@.html -sALLOW_MEMORY_GROWTH $(EFLAGS)


.PHONY: native
native: always $(BIN_DIR)/native

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(OBJ_DIR)/game.o: $(SRC_DIR)/game.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(BIN_DIR)/native: $(OBJ_DIR)/main.o $(OBJ_DIR)/game.o
	$(CC) $^ -ggdb -lArchimedes $(CFLAGS) -o $@


.PHONY: editor
editor: always $(BIN_DIR)/editor

$(OBJ_DIR)/init_editor.o: $(EDITOR_DIR)/init_editor.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(OBJ_DIR)/save_editor.o: $(EDITOR_DIR)/save_editor.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(OBJ_DIR)/world_editor.o: $(EDITOR_DIR)/world_editor.c
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(BIN_DIR)/editor: $(OBJ_DIR)/world_editor.o $(OBJ_DIR)/save_editor.o $(OBJ_DIR)/init_editor.o
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
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# =============================================================================
# TESTING
# =============================================================================
TEST_DIR=tests
TEST_CFLAGS = -Wall -Wextra -ggdb $(CINC)

# Individual test targets
.PHONY: test-items-creation-destruction
test-items-creation-destruction: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_creation_destruction $(TEST_DIR)/items/test_items_creation_destruction.c $(OBJ_DIR)/items.o

.PHONY: test-items-type-checking
test-items-type-checking: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_type_checking $(TEST_DIR)/items/test_items_type_checking_and_access.c $(OBJ_DIR)/items.o

.PHONY: test-items-material-system
test-items-material-system: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_material_system $(TEST_DIR)/items/test_items_material_system.c $(OBJ_DIR)/items.o -lm

.PHONY: test-items-properties
test-items-properties: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_properties $(TEST_DIR)/items/test_items_properties.c $(OBJ_DIR)/items.o -lm

.PHONY: test-items-durability
test-items-durability: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_durability $(TEST_DIR)/items/test_items_durability.c $(OBJ_DIR)/items.o -lm

.PHONY: test-items-inventory
test-items-inventory: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_inventory $(TEST_DIR)/items/test_items_inventory.c $(OBJ_DIR)/items.o -lm

.PHONY: test-items-usage
test-items-usage: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_usage $(TEST_DIR)/items/test_items_usage.c $(OBJ_DIR)/items.o -lm


# Compile items.c as object file for testing
$(OBJ_DIR)/items.o: $(SRC_DIR)/items.c
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

# Individual test runners (for detailed output)
.PHONY: run-test-items-creation-destruction
run-test-items-creation-destruction: test-items-creation-destruction
	@./$(BIN_DIR)/test_items_creation_destruction

.PHONY: run-test-items-type-checking
run-test-items-type-checking: test-items-type-checking
	@./$(BIN_DIR)/test_items_type_checking

.PHONY: run-test-items-material-system
run-test-items-material-system: test-items-material-system
	@./$(BIN_DIR)/test_items_material_system

.PHONY: run-test-items-properties
run-test-items-properties: test-items-properties
	@./$(BIN_DIR)/test_items_properties

.PHONY: run-test-items-durability
run-test-items-durability: test-items-durability
	@./$(BIN_DIR)/test_items_durability

.PHONY: run-test-items-inventory
run-test-items-inventory: test-items-inventory
	@./$(BIN_DIR)/test_items_inventory

.PHONY: run-test-items-usage
run-test-items-usage: test-items-usage
	@./$(BIN_DIR)/test_items_usage

# Global test runner (summary output)
.PHONY: test
test:
	@./run_tests.sh

# Test help
.PHONY: test-help
test-help:
	@echo "Available test commands:"
	@echo "  make test                              - Run all tests with global summary"
	@echo "  make run-test-items-creation-destruction - Run item creation/destruction tests (detailed)"
	@echo "  make run-test-items-type-checking        - Run type checking/access tests (detailed)"
	@echo "  make run-test-items-material-system      - Run material system tests (detailed)"
	@echo "  make run-test-items-properties            - Run item properties tests (detailed)"
	@echo "  make run-test-items-durability            - Run item durability tests (detailed)"
	@echo "  make run-test-items-inventory             - Run inventory management tests (detailed)"
	@echo "  make run-test-items-usage                 - Run item usage & effects tests (detailed)"
	@echo "  make test-help                         - Show this help message"
