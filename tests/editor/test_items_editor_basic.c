// tests/editor/test_items_editor_basic.c
// Basic, Edge Case, and Epic functionality tests for Items Editor subsystem

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include "Archimedes.h"
#include "editor.h"
#include "items_editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =============================================================================
// TEST SETUP
// =============================================================================

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// By including the .c file, we gain access to its static functions 
// (e_ItemEditorDoLoop, e_ItemEditorRenderLoop) and static global variables 
// (items_database, items_count, items_status_text) for precise unit testing.
#include "../../editor/items_editor.c"


/**
 * @brief Resets the editor's state before each test.
 */
static void reset_items_editor_state(void)
{
    // Manually clean up state to ensure tests are isolated
    if (items_database) {
        ie_FreeItemsDatabase(items_database, items_count);
        items_database = NULL;
        items_count = 0;
    }
    if (items_status_text) {
        free(items_status_text);
        items_status_text = NULL;
    }
    // Reset app state
    memset(&app, 0, sizeof(aApp_t));
}

// =============================================================================
// TEST CASES (Existing & New)
// =============================================================================

int test_items_editor_initialization(void) {
    d_LogInfo("Testing items editor initialization...");
    reset_items_editor_state();
    e_InitItemEditor();
    TEST_ASSERT(app.delegate.logic == e_ItemEditorDoLoop, "Logic delegate should be set");
    TEST_ASSERT(app.delegate.draw == e_ItemEditorRenderLoop, "Draw delegate should be set");
    TEST_ASSERT(items_database != NULL, "Item database should be loaded on init");
    reset_items_editor_state();
    return 1;
}

int test_items_database_loading(void) {
    d_LogInfo("Starting items database loading test...");
    reset_items_editor_state();
    int loaded_count = 0;
    Item_t* db = load_items_database(&loaded_count);
    TEST_ASSERT(db != NULL, "Database should be loaded successfully");
    TEST_ASSERT(loaded_count > 0, "Should load at least one item");
    ie_FreeItemsDatabase(db, loaded_count);
    return 1;
}

int test_items_database_cleanup_safety(void) {
    d_LogInfo("Starting items database cleanup safety test...");
    ie_FreeItemsDatabase(NULL, 10);
    TEST_ASSERT(1, "NULL database cleanup should not crash");
    Item_t* empty_db = malloc(sizeof(Item_t));
    ie_FreeItemsDatabase(empty_db, 0); 
    TEST_ASSERT(1, "Zero count cleanup should not crash");
    return 1;
}

int test_editor_mode_placeholders(void) {
    d_LogInfo("Starting editor mode placeholder functions test...");
    reset_items_editor_state();
    // These placeholder functions just log messages and should not crash
    ie_save();
    TEST_ASSERT(1, "Save function placeholder should not crash");
    ie_load();
    TEST_ASSERT(1, "Load function placeholder should not crash");
    return 1;
}

// =============================================================================
// EPIC & EDGE CASE TESTS
// =============================================================================

/**
 * @brief Tests that the main logic loop is safe against corrupted global state.
 */
int test_loop_logic_safety(void)
{
    d_LogInfo("Epic Test: Verifying logic loop safety against corrupt state...");
    reset_items_editor_state();
    e_InitItemEditor(); // Set up delegates

    // Corrupt the state: database is NULL but count is not
    ie_FreeItemsDatabase(items_database, items_count);
    items_database = NULL;
    items_count = 5; // This is a corrupt state

    // The logic loop should handle this without crashing
    e_ItemEditorDoLoop(0.016f);
    TEST_ASSERT(true, "Logic loop should not crash with NULL database and positive count");

    reset_items_editor_state();
    return 1;
}

/**
 * @brief Tests that the main render loop is safe against corrupted global state.
 */
int test_render_loop_safety(void)
{
    d_LogInfo("Epic Test: Verifying render loop safety against corrupt state...");
    reset_items_editor_state();
    e_InitItemEditor(); // Set up delegates

    // Corrupt the state: status text is NULL
    free(items_status_text);
    items_status_text = NULL;

    // The render loop should handle this without crashing
    e_ItemEditorRenderLoop(0.016f);
    TEST_ASSERT(true, "Render loop should not crash with NULL status text");

    reset_items_editor_state();
    return 1;
}

/**
 * @brief Verifies that e_DestroyItemEditor fully cleans up and NULLs global pointers.
 */
int test_destroy_fully_nulls_globals(void)
{
    d_LogInfo("Epic Test: Verifying e_DestroyItemEditor fully NULLs global pointers...");
    reset_items_editor_state();
    e_InitItemEditor(); // This allocates and sets the globals

    TEST_ASSERT(items_database != NULL, "Prerequisite: database should be non-NULL after init");
    TEST_ASSERT(items_status_text != NULL, "Prerequisite: status text should be non-NULL after init");

    e_DestroyItemEditor(); // Destroy the state

    TEST_ASSERT(items_database == NULL, "items_database pointer should be NULL after destroy");
    TEST_ASSERT(items_status_text == NULL, "items_status_text pointer should be NULL after destroy");
    
    return 1;
}

/**
 * @brief Tests that re-initializing the editor does not reload an existing database.
 */
int test_reinitialization_with_existing_database(void)
{
    d_LogInfo("Epic Test: Verifying re-initialization does not reload the database...");
    reset_items_editor_state();
    e_InitItemEditor(); // First initialization

    Item_t* first_db_pointer = items_database;
    TEST_ASSERT(first_db_pointer != NULL, "Database should be loaded on first init");

    // Call init again. The internal 'if (items_database == NULL)' check should prevent a reload.
    e_InitItemEditor();
    
    TEST_ASSERT(items_database == first_db_pointer, "Database pointer should not change on re-init");

    reset_items_editor_state();
    return 1;
}
// =============================================================================
// DEBUG HUNTING & EDGE CASE TESTS
// =============================================================================

/**
 * @brief Tests that calling destroy twice does not cause a crash.
 */
int test_double_destroy_is_safe(void)
{
    d_LogInfo("Debug Hunt: Testing that double-destroy is safe...");
    reset_items_editor_state();
    e_InitItemEditor(); // Initialize once

    e_DestroyItemEditor(); // First destroy
    e_DestroyItemEditor(); // Second destroy should be handled gracefully

    TEST_ASSERT(true, "Calling e_DestroyItemEditor twice should not crash");
    
    return 1;
}

/**
 * @brief Tests the render loop's safety with an invalid item type in the database.
 */
int test_render_loop_with_invalid_item_type(void)
{
    d_LogInfo("Debug Hunt: Testing render loop with an out-of-bounds item type...");
    reset_items_editor_state();
    e_InitItemEditor();

    // Manually corrupt an item in the database with an invalid type
    if (items_database != NULL && items_count > 0) {
        items_database[0].type = (ItemType_t)99;
    }

    // The render loop's switch statement should use its 'default' case
    // and not crash.
    e_ItemEditorRenderLoop(0.016f);
    TEST_ASSERT(true, "Render loop should not crash when processing an invalid item type");

    reset_items_editor_state();
    return 1;
}

/**
 * @brief Tests that the logic loop is safe if status text is NULL.
 */
int test_logic_loop_handles_null_status_text(void)
{
    d_LogInfo("Debug Hunt: Testing logic loop with NULL status text...");
    reset_items_editor_state();
    e_InitItemEditor();

    // Manually free and NULL out the status text to simulate a corrupt state
    free(items_status_text);
    items_status_text = NULL;

    // The logic loop has a NULL check and should not crash.
    e_ItemEditorDoLoop(0.016f);
    TEST_ASSERT(true, "Logic loop should not crash if status text is NULL");

    reset_items_editor_state();
    return 1;
}

/**
 * @brief Tests how e_InitItemEditor handles a failure from load_items_database.
 */
int test_init_handles_load_failure(void)
{
    d_LogInfo("Debug Hunt: Testing init when database loading fails...");
    reset_items_editor_state();
    
    // Use our new "chaos flag" to command the loader to fail
    g_force_load_failure = true;

    // e_InitItemEditor should call the failing loader and handle the NULL return
    e_InitItemEditor();

    // Reset the flag so it doesn't affect other tests
    g_force_load_failure = false;

    TEST_ASSERT(items_database == NULL, "items_database should be NULL after a load failure");
    TEST_ASSERT(app.delegate.logic == e_ItemEditorDoLoop, "Delegates should still be set even if DB load fails");
    
    reset_items_editor_state();
    return 1;
}


// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger with appropriate configuration
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true,
        .include_function = false
    };
    
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

    d_LogInfo("Starting Items Editor Basic Functionality Tests");
    
    TEST_SUITE_START("Items Editor Basic Functionality Tests");

    // Core functionality tests
    RUN_TEST(test_items_editor_initialization);
    RUN_TEST(test_items_database_loading);
    RUN_TEST(test_items_database_cleanup_safety);
    RUN_TEST(test_editor_mode_placeholders);
    
    // Epic and edge case tests
    RUN_TEST(test_loop_logic_safety);
    RUN_TEST(test_render_loop_safety);
    RUN_TEST(test_destroy_fully_nulls_globals);
    RUN_TEST(test_reinitialization_with_existing_database);

    RUN_TEST(test_double_destroy_is_safe);
    RUN_TEST(test_render_loop_with_invalid_item_type);
    RUN_TEST(test_logic_loop_handles_null_status_text);
    RUN_TEST(test_init_handles_load_failure);

    // Clean up the logger before finishing
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}