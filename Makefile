CC = gcc
ECC = emcc
CINC = -Iinclude/
CFLAGS = -Wall -Wextra -Iinclude/ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
EFLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2

SRC_DIR=src
INC_DIR=include
BIN_DIR=bin
OBJ_DIR=obj
EMS_DIR=ems_obj
LIB_DIR=lib
INDEX_DIR=index
EDITOR_DIR=editor

.PHONY: all
all: $(INDEX_DIR)/index

EMS_OBJS =\
					$(EMS_DIR)/main.o\
					$(EMS_DIR)/game.o

$(EMS_DIR)/%.o: $(SRC_DIR)/%.c | $(EMS_DIR)
	$(ECC) -c $< -o $@ $(CINC)


$(INDEX_DIR)/index: $(EMS_OBJS) $(LIB_DIR)/libArchimedes.a | $(INDEX_DIR)
	# Create a deployment version of the template with corrected paths

	# ========================================================================
	# HTML TEMPLATE PATH TRANSFORMATION FOR DEPLOYMENT
	# ========================================================================
	# Problem: The template.html uses "../htmlTemplate/" paths which work during
	# development but break when deployed because the directory structure changes.
	#
	# Solution: Use 'sed' (stream editor) to find and replace paths in the template:
	#
	# Transformation 1: Regular resources (CSS, JS files)
	#   FROM: ../htmlTemplate/css/style.css
	#   TO:   htmlTemplate/css/style.css
	#   Why:  Removes "../" to make paths relative to deployment directory
	#
	# Transformation 2: ES6 Module imports (JavaScript modules)
	#   FROM: from 'htmlTemplate/module_loader.mjs'
	#   TO:   from './htmlTemplate/module_loader.mjs'
	#   Why:  ES6 modules require explicit relative path indicators (./ or ../)
	#         Without it, browsers treat "htmlTemplate/" as a bare specifier (package name)
	#
	# sed command breakdown:
	#   -e: allows multiple expressions in one command
	#   's|pattern|replacement|g': substitute pattern with replacement globally
	#   |: delimiter (using | instead of / to avoid escaping forward slashes)
	#   '\''': way to include single quotes in shell command (end quote, escaped quote, start quote)
	# ========================================================================

	sed -e 's|../htmlTemplate/|htmlTemplate/|g' -e 's|from '\''htmlTemplate/|from '\''./htmlTemplate/|g' htmlTemplate/template.html > $(INDEX_DIR)/template_deploy.html

	# Compile C code to WebAssembly with the modified HTML template
	$(ECC) $^ -s WASM=1 --shell-file $(INDEX_DIR)/template_deploy.html --preload-file resources/ -o $(INDEX_DIR)/index.html -sALLOW_MEMORY_GROWTH $(EFLAGS)
	# Copy the htmlTemplate directory to deployment location
	# This ensures all CSS, JS, and other assets are available at runtime
	cp -r htmlTemplate $(INDEX_DIR)/

	# Clean up the temporary modified template file
	rm $(INDEX_DIR)/template_deploy.html

.PHONY: native
native: $(BIN_DIR)/native

NATIVE_OBJS = \
							$(OBJ_DIR)/main.o\
							$(OBJ_DIR)/game.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)


$(BIN_DIR)/native: $(NATIVE_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb -lArchimedes -lDaedalus $(CFLAGS) -o $@


.PHONY: editor
editor: $(BIN_DIR)/editor

EDITOR_OBJS = \
							$(OBJ_DIR)/editor.o\
							$(OBJ_DIR)/init_editor.o\
							$(OBJ_DIR)/world_editor.o\
							$(OBJ_DIR)/items_editor.o\
							$(OBJ_DIR)/entity_editor.o\
							$(OBJ_DIR)/color_editor.o\
							$(OBJ_DIR)/ui_editor.o\
							#$(OBJ_DIR)/save_editor.o

$(OBJ_DIR)/%.o: $(EDITOR_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(BIN_DIR)/editor: $(EDITOR_OBJS)  | $(BIN_DIR)
	$(CC) $^ -ggdb -lArchimedes $(CFLAGS) -o $@

$(EMS_DIR):
	mkdir -p $(EMS_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(INDEX_DIR):
	mkdir -p $(INDEX_DIR)

.PHONY: bear
bear:
	bear -- make


.PHONY: bearclean
bearclean:
	rm compile_commands.json


.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(INDEX_DIR) $(EMS_DIR)
	@if [ -t 1 ]; then clear; fi


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
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_creation_destruction $(TEST_DIR)/items/test_items_creation_destruction.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-type-checking
test-items-type-checking: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_type_checking $(TEST_DIR)/items/test_items_type_checking_and_access.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-material-system
test-items-material-system: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_material_system $(TEST_DIR)/items/test_items_material_system.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-properties
test-items-properties: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_properties $(TEST_DIR)/items/test_items_properties.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-durability
test-items-durability: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_durability $(TEST_DIR)/items/test_items_durability.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-inventory
test-items-inventory: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_inventory $(TEST_DIR)/items/test_items_inventory.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-usage
test-items-usage: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_usage $(TEST_DIR)/items/test_items_usage.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes


.PHONY: test-items-helper-functions
test-items-helper-functions: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_helper_functions $(TEST_DIR)/items/test_items_helper_functions.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes

.PHONY: test-items-integration-tests
test-items-integration-tests: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_integration_tests $(TEST_DIR)/items/test_items_integration_tests.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes

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

.PHONY: run-test-items-helper-functions
run-test-items-helper-functions: test-items-helper-functions
	@./$(BIN_DIR)/test_items_helper_functions

.PHONY: run-test-items-integration-tests
run-test-items-integration-tests: test-items-integration-tests
	@./$(BIN_DIR)/test_items_integration_tests

# Demo test for framework improvements
.PHONY: test-framework-demo
test-framework-demo: always
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_framework_demo test_framework_demo.c -lm

.PHONY: run-test-framework-demo
run-test-framework-demo: test-framework-demo
	@./$(BIN_DIR)/test_framework_demo

# Test for comma formatting and XP requirements
.PHONY: test-comma-xp
test-comma-xp: always
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_comma_and_xp test_comma_and_xp.c -lm

.PHONY: run-test-comma-xp
run-test-comma-xp: test-comma-xp
	@./$(BIN_DIR)/test_comma_and_xp

# Global test runner (summary output)
.PHONY: test
test:
	@./run_tests.sh

# Test help
.PHONY: test-help
test-help:
	@echo "Available test commands:"
	@echo "  make test                              - Run all tests with global summary"
	@echo "  make run-test-framework-demo           - Run test framework demo (shows new features)"
	@echo "  make run-test-items-creation-destruction - Run item creation/destruction tests (detailed)"
	@echo "  make run-test-items-type-checking        - Run type checking/access tests (detailed)"
	@echo "  make run-test-items-material-system      - Run material system tests (detailed)"
	@echo "  make run-test-items-properties            - Run item properties tests (detailed)"
	@echo "  make run-test-items-durability            - Run item durability tests (detailed)"
	@echo "  make run-test-items-inventory             - Run inventory management tests (detailed)"
	@echo "  make run-test-items-usage                 - Run item usage & effects tests (detailed)"
	@echo "  make run-test-items-helper-functions      - Run item helper functions tests (detailed)"
	@echo "  make test-help                         - Show this help message"
