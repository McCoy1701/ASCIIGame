// ASCIIGame/tests/world/test_world_editor_basic.c
// A proper test file for basic world editor functions, built to the highest standard.

#include "tests.h"
#include "world_editor.h"
#include "init_editor.h"  // Including this to test the core logic directly
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
// e_Init... functions are provided by their respective .o files.
void e_InitEditor(void) { /* Do nothing for this test. */ }
// =============================================================================


// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// REAL TESTS FOR WORLD EDITOR LOGIC
// =============================================================================

int test_init_and_free_world(void)
{
    d_LogInfo("Verifying the core init_world() and free_world() functions.");

    World_t* world = init_world(WORLD_WIDTH_SMALL, WORLD_WIDTH_SMALL,
                                REGION_SIZE_SMALL, REGION_SIZE_SMALL,
                                LOCAL_SIZE_SMALL, LOCAL_SIZE_SMALL, Z_HEIGHT_SMALL);

    TEST_ASSERT(world != NULL, "init_world should return a valid pointer for a new world.");
    if (!world) return 0;

    d_LogDebugF("World created with dimensions: W=%d, R=%d, L=%d, Z=%d",
                world->world_width, world->region_width, world->local_width, world->z_height);

    TEST_ASSERT(world->world_width == WORLD_WIDTH_SMALL, "World width should be set correctly.");
    TEST_ASSERT(world->region_width == REGION_SIZE_SMALL, "Region width should be set correctly.");
    TEST_ASSERT(world->local_width == LOCAL_SIZE_SMALL, "Local width should be set correctly.");
    TEST_ASSERT(world->z_height == Z_HEIGHT_SMALL, "Z-height should be set correctly.");
    TEST_ASSERT(world->regions != NULL, "World regions buffer should be allocated.");

    d_LogInfo("Verifying free_world() cleans up without errors.");
    // This test implicitly checks if free_world causes a crash.
    free_world(world, (world->world_width * world->world_height), (world->region_width * world->region_height));
    TEST_ASSERT(1, "free_world should execute without crashing.");

    return 1;
}

int test_re_creation_of_world(void)
{
    d_LogInfo("Verifying that creating a world multiple times is handled correctly.");

    // First creation
    World_t* world1 = init_world(WORLD_WIDTH_SMALL, WORLD_WIDTH_SMALL, REGION_SIZE_SMALL, REGION_SIZE_SMALL, LOCAL_SIZE_SMALL, LOCAL_SIZE_SMALL, Z_HEIGHT_SMALL);
    TEST_ASSERT(world1 != NULL, "First world creation should succeed.");
    if(!world1) return 0;

    // The source code shows the `map` variable is global. To simulate `wec_GenerateWorld`'s behavior,
    // we manually free the first world before creating the second, as the real function would.
    d_LogDebug("Freeing first world before creating the second.");
    free_world(world1, (world1->world_width * world1->world_height), (world1->region_width * world1->region_height));

    // Second creation with different dimensions
    d_LogDebug("Creating a second world with different (medium) dimensions.");
    World_t* world2 = init_world(WORLD_WIDTH_MEDIUM, WORLD_WIDTH_MEDIUM, REGION_SIZE_MEDIUM, REGION_SIZE_MEDIUM, LOCAL_SIZE_MEDIUM, LOCAL_SIZE_MEDIUM, Z_HEIGHT_MEDIUM);
    TEST_ASSERT(world2 != NULL, "Second world creation should succeed.");
    if(!world2) return 0;

    TEST_ASSERT(world2->world_width == WORLD_WIDTH_MEDIUM, "Second world should have the new medium width.");

    free_world(world2, (world2->world_width * world2->world_height), (world2->region_width * world2->region_height));
    return 1;
}


int test_unimplemented_functions_are_safe(void)
{
    d_LogWarning("Verifying that unimplemented editor functions do not crash.");

    // These functions are currently empty in the source code.
    // A successful test is one that calls them without causing a segmentation fault.
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


    TEST_SUITE_START("World Editor Basic Logic Tests");

    // Run tests for the core logic of the world editor
    RUN_TEST(test_init_and_free_world);
    RUN_TEST(test_re_creation_of_world);
    RUN_TEST(test_unimplemented_functions_are_safe);

    // =========================================================================
    // DAEDALUS LOGGER SHUTDOWN (MUST BE BEFORE TEST_SUITE_END)
    // =========================================================================
    d_DestroyLogger(d_GetGlobalLogger());

    // TEST_SUITE_END contains the final return statement for main.
    TEST_SUITE_END();
}
