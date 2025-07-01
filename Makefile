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
WEO_DIR=obj/world_editor
LIB_DIR=lib
INDEX_DIR=index
EDITOR_DIR=editor

.PHONY: all
all: $(INDEX_DIR)/index

EMS_OBJS =\
					$(EMS_DIR)/main.o\
					$(EMS_DIR)/game.o

$(EMS_DIR)/%.o: $(SRC_DIR)/%.c | $(EMS_DIR)
	$(ECC) -c $< -o $@ $(CINC) $(EFLAGS)


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
							$(OBJ_DIR)/world_editor/creation.o\
							$(OBJ_DIR)/world_editor/edit.o\
							$(OBJ_DIR)/world_editor/utils.o\
							$(OBJ_DIR)/color_editor.o\
							$(OBJ_DIR)/editor.o\
							$(OBJ_DIR)/entity_editor.o\
							$(OBJ_DIR)/init_editor.o\
							$(OBJ_DIR)/items_editor.o\
							$(OBJ_DIR)/save_editor.o\
							$(OBJ_DIR)/ui_editor.o\
							$(OBJ_DIR)/world_editor.o

$(OBJ_DIR)/%.o: $(EDITOR_DIR)/%.c | $(OBJ_DIR) $(WEO_DIR)
	$(CC) -c $< -o $@ -ggdb $(CFLAGS)

$(BIN_DIR)/editor: $(EDITOR_OBJS) | $(BIN_DIR)
	$(CC) $^ -ggdb -lArchimedes -lDaedalus $(CFLAGS) -o $@

$(WEO_DIR):
	mkdir -p $(WEO_DIR)

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

# --- Object File Rules for All Testable Code ---
# Rule for compiling dStrings needed for tests
$(OBJ_DIR)/dStrings.o: src/dStrings.c
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

# Rule for compiling main game sources needed for some tests
$(OBJ_DIR)/items.o: src/items.c
	$(CC) -c $< -o $@ $(TEST_CFLAGS)

# A list of ALL editor modules that our tests will need to link against.
# CRITICAL FIX: We have REMOVED editor.o from this list because it contains a 'main' function
# which conflicts with the test's own 'main' function. The undefined references this
# creates must be solved in the test file itself (see Step 2 below).
EDITOR_MODULE_OBJS = \
		$(OBJ_DIR)/world_editor/creation.o\
		$(OBJ_DIR)/world_editor/utils.o\
    $(OBJ_DIR)/world_editor.o \
    $(OBJ_DIR)/init_editor.o \
    $(OBJ_DIR)/items_editor.o \
    $(OBJ_DIR)/entity_editor.o \
    $(OBJ_DIR)/color_editor.o \
    $(OBJ_DIR)/ui_editor.o

# Generic pattern rule to build any editor object file from its source file.
# This avoids conflicts with other rules and keeps the Makefile clean.
$(OBJ_DIR)/%.o: editor/%.c | $(WEO_DIR)
	$(CC) -c $< -o $@ $(TEST_CFLAGS)


# --- Individual Test Targets ---

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

.PHONY: test-items-integration-tests
test-items-integration-tests: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_integration_tests $(TEST_DIR)/items/test_items_integration_tests.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes

.PHONY: test-items-usage
test-items-usage: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_usage \
		$(TEST_DIR)/items/test_items_usage.c \
		$(OBJ_DIR)/items.o \
		-lDaedalus -lArchimedes -lm

.PHONY: test-items-helper-functions
test-items-helper-functions: always $(OBJ_DIR)/items.o
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_items_helper_functions $(TEST_DIR)/items/test_items_helper_functions.c $(OBJ_DIR)/items.o -lm -lDaedalus -lArchimedes

# The world editor tests depend on ALL other editor modules.
.PHONY: test-world-editor-basic
test-world-editor-basic: always $(EDITOR_MODULE_OBJS)
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_world_editor_basic tests/editor/test_world_editor_basic.c $(EDITOR_MODULE_OBJS) -lm -lDaedalus -lArchimedes

.PHONY: test-world-editor-advanced
test-world-editor-advanced: always $(EDITOR_MODULE_OBJS)
	$(CC) $(TEST_CFLAGS) -o $(BIN_DIR)/test_world_editor_advanced tests/editor/test_world_editor_advanced.c $(EDITOR_MODULE_OBJS) -lm -lDaedalus -lArchimedes


# --- Individual Test Runners (for detailed output) ---
.PHONY: run-test-items-creation-destruction
run-test-items-creation-destruction: test-items-creation-destruction
	@./$(BIN_DIR)/test_items_creation_destruction

.PHONY: run-test-items-type-checking
run-test-items-type-checking: test-items-type-checking
	@./$(BIN_DIR)/test_items_type_checking

.PHONY: run-test-items-usage
run-test-items-usage: test-items-usage
	@./$(BIN_DIR)/test_items_usage

.PHONY: run-test-items-helper-functions
run-test-items-helper-functions: test-items-helper-functions
	@./$(BIN_DIR)/test_items_helper_functions

.PHONY: run-test-items-material-system
run-test-items-material-system: test-items-material-system
	@./$(BIN_DIR)/test_items_material_system

.PHONY: run-test-items-durability
run-test-items-durability: test-items-durability
	@./$(BIN_DIR)/test_items_durability

.PHONY: run-test-items-inventory
run-items-inventory: test-items-inventory
	@./$(BIN_DIR)/test_items_inventory

.PHONY: run-test-items-properties
run-test-items-properties: test-items-properties
	@./$(BIN_DIR)/test_items_properties

.PHONY: run-test-items-integration-tests
run-test-items-integration-tests: test-items-integration-tests
	@./$(BIN_DIR)/test_items_integration_tests

.PHONY: run-test-world-editor-basic
run-test-world-editor-basic: test-world-editor-basic
	@./$(BIN_DIR)/test_world_editor_basic


.PHONY: run-test-world-editor-advanced
run-test-world-editor-advanced: test-world-editor-advanced
	@./$(BIN_DIR)/test_world_editor_advanced


# --- Global Test Runner ---
.PHONY: test
test:
	@./run_tests.sh
