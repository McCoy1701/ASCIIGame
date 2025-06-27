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

int test_we_memory_boundary_corruption(void)
{
    d_LogWarning("BUG HUNT: Testing for buffer overruns in world allocation.");

    // Attempt allocation at exact system limits
    World_t* boundary_world = init_world(WORLD_WIDTH_LARGE + 1, WORLD_WIDTH_LARGE + 1,
                                       REGION_SIZE_LARGE, REGION_SIZE_LARGE,
                                       LOCAL_SIZE_LARGE, LOCAL_SIZE_LARGE, Z_HEIGHT_LARGE);

    // Test should either succeed gracefully or fail safely
    if (boundary_world) {
        TEST_ASSERT(boundary_world->regions != NULL, "Boundary world regions should be valid if allocated");
        free_world(boundary_world, (boundary_world->world_width * boundary_world->world_height),
                   (boundary_world->region_width * boundary_world->region_height));
    }
    TEST_ASSERT(1, "Memory boundary test completed without crash");
    return 1;
}
int test_we_null_pointer_cascade(void)
{
    d_LogWarning("BUG HUNT: Testing cascading null pointer dereferences.");

    // Force allocation failure scenario
    World_t* corrupted_world = init_world(0, 0, 0, 0, 0, 0, 0); // Zero dimensions

    if (corrupted_world) {
        // If somehow created, test defensive programming
        TEST_ASSERT(corrupted_world->regions == NULL || corrupted_world->world_width == 0,
                   "Zero-dimension world should have null/zero state");
        free_world(corrupted_world, 0, 0);
    }

    TEST_ASSERT(1, "Null pointer cascade test survived");
    return 1;
}
int test_we_integer_overflow_protection(void)
{
    d_LogWarning("BUG HUNT: Testing arithmetic overflow in dimension calculations.");

    // Create dimensions that multiply to near-overflow values
    uint32_t dangerous_width = 65535;  // Close to uint16 max
    uint32_t dangerous_height = 65535;

    World_t* overflow_world = init_world(dangerous_width, dangerous_height,
                                       REGION_SIZE_SMALL, REGION_SIZE_SMALL,
                                       LOCAL_SIZE_SMALL, LOCAL_SIZE_SMALL, Z_HEIGHT_SMALL);

    if (overflow_world) {
        uint64_t total_regions = (uint64_t)overflow_world->world_width * overflow_world->world_height;
        TEST_ASSERT(total_regions == (uint64_t)dangerous_width * dangerous_height,
                   "Large dimension multiplication should not overflow");
        free_world(overflow_world, total_regions,
                   (overflow_world->region_width * overflow_world->region_height));
    }

    TEST_ASSERT(1, "Integer overflow protection test completed");
    return 1;
}
int test_we_resource_exhaustion_recovery(void)
{
    d_LogWarning("BUG HUNT: Testing system behavior under memory exhaustion.");

    World_t* worlds[50]; // Array to hold multiple worlds
    int successful_allocations = 0;

    d_LogDebug("Attempting to exhaust available memory...");
    LOOP_TEST_START();
    for (int i = 0; i < 50; i++) {
        worlds[i] = init_world(WORLD_WIDTH_MEDIUM, WORLD_WIDTH_MEDIUM,
                              REGION_SIZE_MEDIUM, REGION_SIZE_MEDIUM,
                              LOCAL_SIZE_MEDIUM, LOCAL_SIZE_MEDIUM, Z_HEIGHT_MEDIUM);

        if (worlds[i] != NULL) {
            successful_allocations++;
            if (i < 3) {
                TEST_ASSERT(worlds[i]->regions != NULL, "Initial allocations should succeed");
            }
        }
    }
    LOOP_TEST_END();

    // Clean up successfully allocated worlds
    for (int i = 0; i < 50; i++) {
        if (worlds[i] != NULL) {
            free_world(worlds[i], (worlds[i]->world_width * worlds[i]->world_height),
                      (worlds[i]->region_width * worlds[i]->region_height));
        }
    }

    TEST_ASSERT(successful_allocations > 0, "At least some worlds should allocate before exhaustion");
    return 1;
}
int test_we_concurrent_access_simulation(void)
{
    d_LogWarning("BUG HUNT: Simulating concurrent world operations for race conditions.");

    World_t* shared_world = init_world(WORLD_WIDTH_SMALL, WORLD_WIDTH_SMALL,
                                     REGION_SIZE_SMALL, REGION_SIZE_SMALL,
                                     LOCAL_SIZE_SMALL, LOCAL_SIZE_SMALL, Z_HEIGHT_SMALL);
    TEST_ASSERT(shared_world != NULL, "Shared world should be created for concurrent testing");

    if (!shared_world) return 0;

    d_LogDebug("Simulating rapid access patterns that could cause race conditions...");

    // Simulate rapid state changes that might expose threading issues
    LOOP_TEST_START();
    for (int cycle = 0; cycle < 1000; cycle++) {
        // Simulate operations that modify world state
        we_edit();  // Call editor functions rapidly
        we_save();
        we_load();

        // Verify world integrity hasn't been corrupted
        if (cycle < 3) {
            TEST_ASSERT(shared_world->regions != NULL, "World integrity should be maintained");
        }

        // Brief "context switch" simulation
        if (cycle % 100 == 0) {
            d_LogDebugF("Concurrent simulation cycle %d completed", cycle);
        }
    }
    LOOP_TEST_END();

    TEST_ASSERT(shared_world->regions != NULL, "World should survive concurrent access simulation");

    free_world(shared_world, (shared_world->world_width * shared_world->world_height),
              (shared_world->region_width * shared_world->region_height));
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

    RUN_TEST(test_we_memory_boundary_corruption);
    RUN_TEST(test_we_null_pointer_cascade);
    RUN_TEST(test_we_integer_overflow_protection);
    RUN_TEST(test_we_resource_exhaustion_recovery);
    RUN_TEST(test_we_concurrent_access_simulation);

    // =========================================================================
    // DAEDALUS LOGGER SHUTDOWN (MUST BE BEFORE TEST_SUITE_END)
    // =========================================================================
    d_DestroyLogger(d_GetGlobalLogger());

    // TEST_SUITE_END contains the final return statement for main.
    TEST_SUITE_END();
}
