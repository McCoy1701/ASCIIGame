// tests/editor/test_items_editor_creation.c
// Unit tests for the Items Editor Creation Mode

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
// TEST SETUP & MOCKING
// =============================================================================

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// --- Mocking Flags & Stubs ---
// These flags and stub functions will help us verify that our creation code
// is calling the other parts of the editor as expected.
static bool g_create_item_called = false;
static bool g_add_to_db_called = false;
static bool g_init_items_editor_called = false;

void ie_AddItemToDatabase(Item_t* item) {
    d_LogInfo("STUB: ie_AddItemToDatabase called");
    g_add_to_db_called = true;
    // In a real scenario, we'd clean up the dummy item's memory here.
    // For this test, it's a static variable, so no cleanup needed.
}

void e_InitItemsEditor(void) {
    d_LogInfo("STUB: e_InitItemsEditor called");
    g_init_items_editor_called = true;
}

void ie_DrawItemCreationPreview(int, int, int, int, int) {
    // This is just a stub to prevent linker errors.
}

void ie_edit(void) {
    d_LogInfo("STUB: ie_edit called");
}

// C-file inclusion for testing static functions and variables.
// This is a powerful technique that makes 'private' module internals visible to the test suite.
#include "../../editor/items_editor/creation.c"

// =============================================================================
// TEST CASES
// =============================================================================

/**
 * @brief Resets all global mock flags before each test.
 */
static void reset_mock_flags(void)
{
    g_create_item_called = false;
    g_add_to_db_called = false;
    g_init_items_editor_called = false;

    // Also reset keyboard state
    for(int i = 0; i < MAX_KEYBOARD_KEYS; i++) app.keyboard[i] = 0;
}

/**
 * @brief Tests if the creation mode initializes correctly.
 */
int test_creation_mode_initialization(void)
{
    d_LogInfo("Testing creation mode initialization...");
    reset_mock_flags();

    ie_creation();

    TEST_ASSERT(app.delegate.logic == ie_CreationDoLoop, "Logic delegate should be set to creation loop");
    TEST_ASSERT(app.delegate.draw == ie_CreationRenderLoop, "Draw delegate should be set to creation render");
    
    // Because we included the .c file, we can directly test its static variables
    TEST_ASSERT(item_type_index == 0, "Item type index should be reset to 0");
    TEST_ASSERT(rarity_index == 0, "Rarity index should be reset to 0");
    
    d_LogInfo("...initialization test completed.");
    return 1;
}

/**
 * @brief Tests if the hotkeys for cycling parameters work correctly.
 */
int test_creation_hotkey_cycling(void)
{
    d_LogInfo("Testing creation mode hotkey cycling...");
    reset_mock_flags();
    ie_creation(); // Initialize to reset indices

    // --- Test Type Cycling (Key '1') ---
    TEST_ASSERT(item_type_index == 0, "Initial type index should be 0");
    app.keyboard[SDL_SCANCODE_1] = 1; // Simulate key press
    ie_CreationDoLoop(0.016f);
    TEST_ASSERT(item_type_index == 1, "Type index should cycle to 1");

    // --- Test Rarity Cycling (Key '2') ---
    TEST_ASSERT(rarity_index == 0, "Initial rarity index should be 0");
    app.keyboard[SDL_SCANCODE_2] = 1; // Simulate key press
    ie_CreationDoLoop(0.016f);
    TEST_ASSERT(rarity_index == 1, "Rarity index should cycle to 1");

    d_LogInfo("...hotkey cycling test completed.");
    return 1;
}

/**
 * @brief Tests the random item generator's logical flow.
 */
int test_generate_random_item_flow(void)
{
    d_LogInfo("Testing random item generation logic flow...");
    reset_mock_flags();

    iec_GenerateRandomItem();

    // This implicitly proves that the creation step was successful.
    TEST_ASSERT(g_add_to_db_called, "iec_GenerateRandomItem should create an item and add it to the database");

    d_LogInfo("...random generation flow test completed.");
    return 1;
}

/**
 * @brief Tests that the ESC key correctly exits creation mode.
 */
int test_escape_from_creation_mode(void)
{
    d_LogInfo("Testing escape key behavior...");
    reset_mock_flags();
    ie_creation(); // Enter creation mode

    app.keyboard[SDL_SCANCODE_ESCAPE] = 1; // Simulate ESC press
    ie_CreationDoLoop(0.016f);

    TEST_ASSERT(g_init_items_editor_called, "Escape key should call e_InitItemsEditor to exit");

    d_LogInfo("...escape key test completed.");
    return 1;
}

// =============================================================================
// DEBUG HUNTING & EDGE CASE TESTS (Continued)
// =============================================================================

/**
 * @brief Ensures generating an item without a UI does not crash.
 */
int test_generate_item_safely_without_ui(void)
{
    d_LogInfo("Debug Hunt: Testing iec_GenerateItem without a UI container...");
    reset_mock_flags();
    
    // In our test setup, the container widget is NULL.
    // The now-defensive function should handle this gracefully and not crash.
    iec_GenerateItem();
    
    TEST_ASSERT(g_add_to_db_called == false, "Item should not be added to DB if UI is missing");
    
    d_LogInfo("...safe generation test completed.");
    return 1;
}

/**
 * @brief Tests creating an item with an invalid, out-of-bounds type.
 */
int test_create_item_with_invalid_type(void)
{
    d_LogInfo("Debug Hunt: Testing item creation with an invalid type enum...");
    reset_mock_flags();
    
    // Call the creator with an out-of-bounds type
    Item_t* bad_item = ie_CreateItemWithParameters((ItemType_t)99, 0, 0, 0, 0);
    
    TEST_ASSERT(bad_item == NULL, "ie_CreateItemWithParameters should return NULL for an invalid type");
    
    d_LogInfo("...invalid type creation test completed.");
    return 1;
}

/**
 * @brief Tests that the parameter cycling correctly wraps around.
 */
int test_parameter_hotkey_wrapping(void)
{
    d_LogInfo("Debug Hunt: Testing hotkey parameter wrapping...");
    reset_mock_flags();
    ie_creation(); // Initializes indices to 0

    // Set rarity index to the last valid value (4)
    rarity_index = 4;
    
    // Press the cycle key (2) again
    app.keyboard[SDL_SCANCODE_2] = 1;
    ie_CreationDoLoop(0.016f);
    app.keyboard[SDL_SCANCODE_2] = 0; // Prevent re-triggering

    TEST_ASSERT(rarity_index == 0, "Rarity index should wrap around from 4 to 0");
    
    d_LogInfo("...hotkey wrapping test completed.");
    return 1;
}

/**
 * @brief Ensures the render loop doesn't crash with an out-of-bounds index.
 */
int test_render_loop_safety_with_bad_index(void)
{
    d_LogInfo("Debug Hunt: Testing render loop with an out-of-bounds index...");
    reset_mock_flags();
    ie_creation();

    // Manually set an index to a bad value
    item_type_index = 99;

    // The now-defensive render loop should execute without crashing.
    ie_CreationRenderLoop(0.016f);

    TEST_ASSERT(true, "Render loop should not crash with an out-of-bounds index");
    
    d_LogInfo("...render loop safety test completed.");
    return 1;
}

// =============================================================================
// EPIC & STRESS TESTS
// =============================================================================

/**
 * @brief Stress tests the creation and destruction flow to hunt for memory leaks.
 */
int test_creation_memory_leak_stress_test(void)
{
    d_LogInfo("Epic Test: Memory leak stress test for item creation...");
    
    LOOP_TEST_START();
    for (int i = 0; i < 500; i++) {
        // Create an item using the high-level function
        Item_t* item = ie_CreateItemWithParameters(ITEM_TYPE_WEAPON, 0, 0, 0, 0);
        TEST_ASSERT(item != NULL, "Item should be created successfully in loop");
        
        // Immediately destroy it to check for leaks over many cycles
        if(item) {
            destroy_item(item);
        }
    }
    LOOP_TEST_END();
    
    d_LogInfo("...memory leak stress test completed.");
    return 1;
}

/**
 * @brief Tests the full logic of iec_GenerateItem by mocking UI widgets.
 */
int test_generate_item_with_mock_ui_values(void)
{
    d_LogInfo("Epic Test: Testing iec_GenerateItem with mocked UI selections...");
    reset_mock_flags();
    
    // --- Advanced Mocking Setup ---
    // Create fake UI data on the stack to simulate user selections
    aSelectWidget_t mock_type_select = { .value = ITEM_TYPE_ARMOR };
    aSelectWidget_t mock_rarity_select = { .value = 3 }; // EPIC
    
    aWidget_t mock_components[] = {
        { .name = "item_type", .data = &mock_type_select },
        { .name = "rarity_level", .data = &mock_rarity_select }
    };
    
    aContainerWidget_t mock_container = { 
        .components = mock_components, 
        .num_components = 2 
    };
    
    aWidget_t mock_widget = { 
        .name = "item_generation_menu", 
        .data = &mock_container 
    };
    
    // Temporarily point the app's active widget to our fake UI
    app.active_widget = &mock_widget;

    // --- Run Test ---
    iec_GenerateItem();
    
    // --- Assertions ---
    TEST_ASSERT(g_add_to_db_called, "Should have added item to DB with mocked UI values");
    // We can also check if the static indices were updated from the mock UI
    TEST_ASSERT(item_type_index == ITEM_TYPE_ARMOR, "Item type index should be updated from mock UI");
    TEST_ASSERT(rarity_index == 3, "Rarity index should be updated from mock UI");

    // --- Cleanup ---
    app.active_widget = NULL; // Reset the mock
    
    d_LogInfo("...mocked UI generation test completed.");
    return 1;
}

/**
 * @brief Tests if the system handles a failure from the item creator function.
 */
int test_creation_handles_low_level_failure(void)
{
    d_LogInfo("Epic Test: Testing error handling when a low-level creator fails...");
    reset_mock_flags();
    
    // Use our "chaos flag" to force ie_CreateItemWithParameters to fail
    g_force_creation_failure = true;
    
    // This should now get a NULL item back and handle the error correctly.
    iec_GenerateRandomItem();
    
    // Reset the flag so it doesn't affect other tests.
    g_force_creation_failure = false;

    // The key check: was the item added to the database? It shouldn't have been.
    TEST_ASSERT(g_add_to_db_called == false, "Item should NOT be added to DB if creation fails");

    d_LogInfo("...low-level failure handling test completed.");
    return 1;
}

/**
 * @brief Verifies that every single valid item type can be created.
 */
int test_all_item_types_are_creatable(void)
{
    d_LogInfo("Epic Test: Verifying all item types can be created...");
    reset_mock_flags();
    
    LOOP_TEST_START();
    for (int type_idx = 0; type_idx < 5; type_idx++) {
        ItemType_t current_type = (ItemType_t)type_idx;
        Item_t* item = ie_CreateItemWithParameters(current_type, 0, 0, 0, 0);
        
        TEST_ASSERT(item != NULL, "Should successfully create an item of each type");
        if (item) {
            TEST_ASSERT(item->type == current_type, "Created item's type should match the requested type");
            destroy_item(item);
        }
    }
    LOOP_TEST_END();
    
    d_LogInfo("...all item type creation test completed.");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger for test output
    dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG, .colorize_output = true };
    d_SetGlobalLogger(d_CreateLogger(config));

    d_LogInfo("Starting Items Editor Creation Mode Tests");
    
    TEST_SUITE_START("Items Editor Creation Mode Tests");

    RUN_TEST(test_creation_mode_initialization);
    RUN_TEST(test_creation_hotkey_cycling);
    RUN_TEST(test_generate_random_item_flow);
    RUN_TEST(test_escape_from_creation_mode);

    RUN_TEST(test_generate_item_safely_without_ui);
    RUN_TEST(test_create_item_with_invalid_type);
    RUN_TEST(test_parameter_hotkey_wrapping);
    RUN_TEST(test_render_loop_safety_with_bad_index);

    RUN_TEST(test_creation_memory_leak_stress_test);
    RUN_TEST(test_generate_item_with_mock_ui_values);
    RUN_TEST(test_creation_handles_low_level_failure);
    RUN_TEST(test_all_item_types_are_creatable);

    // Clean up the logger before finishing
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}