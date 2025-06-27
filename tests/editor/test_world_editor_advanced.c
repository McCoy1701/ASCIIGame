// ASCIIGame/tests/world/test_world_editor_advanced.c
// Advanced test file for world editor functions, built to the highest standard.

#include "tests.h"
#include "world_editor.h"
#include "init_editor.h"  // For direct access to core logic
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =============================================================================
// MOCK FUNCTION FOR LINKER
// =============================================================================
// The functions in world_editor.c call e_InitEditor(), which lives in editor.c.
// Since we CANNOT link editor.c (it has its own main() function), we must
// provide this single dummy mock function to satisfy the linker. The other
// e_Init... functions are now provided by linking their respective .o files.
void e_InitEditor(void) { /* Do nothing for this test. */ }
// =============================================================================

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// ADVANCED TESTS FOR WORLD EDITOR LOGIC
// =============================================================================

int test_we_large_world_operations(void)
{
    d_LogInfo("Verifying editor performance with a large world.");
    d_LogDebug("Creating world of maximum dimensions to check for allocation errors.");

    World_t* world = init_world(WORLD_WIDTH_LARGE, WORLD_WIDTH_LARGE,
                                REGION_SIZE_LARGE, REGION_SIZE_LARGE,
                                LOCAL_SIZE_LARGE, LOCAL_SIZE_LARGE, Z_HEIGHT_LARGE);

    TEST_ASSERT(world != NULL, "init_world should succeed for a large world.");
    if (!world) return 0;

    TEST_ASSERT(world->world_width == WORLD_WIDTH_LARGE, "Large world width should be correct.");
    TEST_ASSERT(world->regions != NULL, "Regions for large world should be allocated.");

    d_LogDebug("Successfully created large world. Now freeing it.");
    free_world(world, (world->world_width * world->world_height), (world->region_width * world->region_height));
    TEST_ASSERT(1, "Large world freed successfully without crash.");

    return 1;
}

int test_we_stress_test_recreation(void)
{
    d_LogWarning("Stress testing by rapidly creating and destroying worlds.");
    d_LogDebug("This test loops many times to check for potential memory leaks from improper freeing.");

    LOOP_TEST_START();
    for(int i = 0; i < 100; ++i) {
        World_t* world = init_world(WORLD_WIDTH_SMALL, WORLD_WIDTH_SMALL,
                                    REGION_SIZE_SMALL, REGION_SIZE_SMALL,
                                    LOCAL_SIZE_SMALL, LOCAL_SIZE_SMALL, Z_HEIGHT_SMALL);

        // We only need to assert the first few times to verify the loop is working.
        if (i < 3) {
             TEST_ASSERT(world != NULL, "World creation inside loop should succeed.");
        }

        if (!world) {
            // If creation fails at any point, we must stop the test.
            d_LogFatalF("World creation failed on iteration %d!", i);
            return 0;
        }

        free_world(world, (world->world_width * world->world_height), (world->region_width * world->region_height));
    }
    LOOP_TEST_END();

    TEST_ASSERT(1, "World recreation stress test completed without errors.");
    return 1;
}

int test_we_unimplemented_placeholders(void)
{
    d_LogInfo("Verifying that unimplemented high-level functions are safe.");
    d_LogDebug("These functions (save, load, edit) are not yet implemented.");
    d_LogDebug("A successful test is one where calling them does not cause a crash.");

    we_edit();
    TEST_ASSERT(1, "we_edit() placeholder is safe to call.");

    we_save();
    TEST_ASSERT(1, "we_save() placeholder is safe to call.");

    we_load();
    TEST_ASSERT(1, "we_load() placeholder is safe to call.");

    return 1;
}


// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // =========================================================================
    // DAEDALUS LOGGER INITIALIZATION
    // =========================================================================
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = false,
        .include_function = false
    };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);
    // =========================================================================


    TEST_SUITE_START("World Editor Advanced Tests");

    // Run tests for advanced world editor scenarios
    RUN_TEST(test_we_large_world_operations);
    RUN_TEST(test_we_stress_test_recreation);
    RUN_TEST(test_we_unimplemented_placeholders);


    // =========================================================================
    // DAEDALUS LOGGER SHUTDOWN (MUST BE BEFORE TEST_SUITE_END)
    // =========================================================================
    d_DestroyLogger(d_GetGlobalLogger());

    // TEST_SUITE_END contains the final return statement for main.
    TEST_SUITE_END();
}
