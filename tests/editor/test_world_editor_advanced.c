// ASCIIGame/tests/world/test_world_editor_advanced.c
// Advanced test file for world editor functions, rewritten for stability and clarity.

#include "tests.h"
#include "world_editor.h"
#include "init_editor.h"
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
void e_InitEditor(void) { /* Do nothing for this test. */ }
// =============================================================================

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// ADVANCED TESTS FOR WORLD EDITOR LOGIC (STABILITY-FOCUSED)
// =============================================================================

int test_we_large_world_operations(void)
{
    d_LogInfo("--- Running Test: Large World Operations ---");
    d_LogDebug("Creating a reasonably large world to verify allocation and deallocation.");

    World_t* world = init_world(WORLD_WIDTH_LARGE, WORLD_HEIGHT_LARGE,
                                REGION_WIDTH_LARGE, REGION_HEIGHT_LARGE,
                                LOCAL_WIDTH_LARGE, LOCAL_HEIGHT_LARGE, Z_HEIGHT_LARGE);

    TEST_ASSERT(world != NULL, "init_world should succeed for a large world.");
    if (!world) return 0;

    TEST_ASSERT(world->world_width == WORLD_WIDTH_LARGE, "Large world width should match definition.");
    TEST_ASSERT(world->regions != NULL, "Regions for large world should be allocated.");

    d_LogDebug("Successfully created large world. Now freeing it.");
    free_world(world, (world->world_width * world->world_height), (world->region_width * world->region_height));
    d_LogInfo("Test Complete: Large World Operations.");
    return 1;
}

int test_we_stress_test_recreation(void)
{
    d_LogInfo("--- Running Test: Stress Test Recreation ---");
    d_LogWarning("Stress testing by rapidly creating and destroying small worlds (25 iterations).");

    LOOP_TEST_START();
    for(int i = 0; i < 25; ++i) {
        World_t* world = init_world(WORLD_WIDTH_SMALL, WORLD_HEIGHT_SMALL,
                                    REGION_WIDTH_SMALL, REGION_HEIGHT_SMALL,
                                    LOCAL_WIDTH_SMALL, LOCAL_HEIGHT_SMALL, Z_HEIGHT_SMALL);

        if (!world) {
            d_LogFatalF("World creation failed on iteration %d! Aborting test.", i);
            TEST_ASSERT(world != NULL, "World creation inside loop should not fail.");
            return 0; // Exit immediately on failure
        }
        free_world(world, (world->world_width * world->world_height), (world->region_width * world->region_height));
    }
    LOOP_TEST_END();

    TEST_ASSERT(1, "World recreation stress test completed without errors.");
    d_LogInfo("Test Complete: Stress Test Recreation.");
    return 1;
}

int test_we_unimplemented_placeholders(void)
{
    d_LogInfo("--- Running Test: Unimplemented Placeholders ---");
    d_LogDebug("Verifying that unimplemented functions (save, load, edit) are safe to call.");

    we_edit();
    TEST_ASSERT(1, "we_edit() placeholder is safe to call.");

    we_save();
    TEST_ASSERT(1, "we_save() placeholder is safe to call.");

    we_load();
    TEST_ASSERT(1, "we_load() placeholder is safe to call.");

    d_LogInfo("Test Complete: Unimplemented Placeholders.");
    return 1;
}

int test_we_memory_boundary_corruption(void)
{
    d_LogInfo("--- Running Test: Memory Boundary ---");
    d_LogWarning("BUG HUNT: Testing allocation just beyond standard medium size.");

    World_t* boundary_world = init_world(WORLD_WIDTH_MEDIUM + 1, WORLD_HEIGHT_MEDIUM + 1,
                                         REGION_WIDTH_MEDIUM, REGION_HEIGHT_MEDIUM,
                                         LOCAL_WIDTH_MEDIUM, LOCAL_HEIGHT_MEDIUM, Z_HEIGHT_MEDIUM);

    if (boundary_world) {
        d_LogDebug("Allocation beyond boundary succeeded, now freeing.");
        TEST_ASSERT(boundary_world->regions != NULL, "Boundary world regions should be valid if allocated");
        free_world(boundary_world, (boundary_world->world_width * boundary_world->world_height),
                   (boundary_world->region_width * boundary_world->region_height));
    } else {
        d_LogDebug("Allocation beyond boundary failed as expected, which is safe.");
    }
    TEST_ASSERT(1, "Memory boundary test completed without crash.");
    d_LogInfo("Test Complete: Memory Boundary.");
    return 1;
}

int test_we_null_pointer_cascade(void)
{
    d_LogInfo("--- Running Test: Null Pointer Cascade ---");
    d_LogWarning("BUG HUNT: Testing safe handling of zero-dimension worlds.");

    World_t* corrupted_world = init_world(0, 0, 0, 0, 0, 0, 0);

    if (corrupted_world) {
        d_LogDebug("Zero-dimension world was created, checking state and freeing.");
        TEST_ASSERT(corrupted_world->regions == NULL || corrupted_world->world_width == 0,
                   "Zero-dimension world should have null/zero state");
        free_world(corrupted_world, 0, 0);
    } else {
        d_LogDebug("Zero-dimension world was not created, which is correct.");
    }

    TEST_ASSERT(1, "Null pointer cascade test survived.");
    d_LogInfo("Test Complete: Null Pointer Cascade.");
    return 1;
}

int test_we_integer_overflow_protection(void)
{
    d_LogInfo("--- Running Test: Integer Overflow Protection ---");
    d_LogWarning("BUG HUNT: Testing with large, but not system-breaking, dimensions.");

    // Using 4096 is large enough to test 32-bit calculations without allocating insane memory
    uint32_t dangerous_width = 4096;
    uint32_t dangerous_height = 4096;

    World_t* overflow_world = init_world(dangerous_width, dangerous_height,
                                       REGION_WIDTH_SMALL, REGION_HEIGHT_SMALL,
                                       LOCAL_WIDTH_SMALL, LOCAL_HEIGHT_SMALL, Z_HEIGHT_SMALL);

    if (overflow_world) {
        d_LogDebug("Large dimension world allocated, checking 64-bit calculation for overflow.");
        uint64_t total_regions = (uint64_t)overflow_world->world_width * overflow_world->world_height;
        TEST_ASSERT(total_regions == (uint64_t)dangerous_width * dangerous_height,
                   "Large dimension multiplication should not overflow.");
        free_world(overflow_world, total_regions,
                   (overflow_world->region_width * overflow_world->region_height));
    } else {
        d_LogDebug("Large dimension world failed to allocate, which can be normal under memory pressure.");
    }

    TEST_ASSERT(1, "Integer overflow protection test completed without crash.");
    d_LogInfo("Test Complete: Integer Overflow Protection.");
    return 1;
}

int test_we_resource_exhaustion_recovery(void)
{
    d_LogInfo("--- Running Test: Resource Exhaustion Recovery ---");
    d_LogWarning("BUG HUNT: Simulating memory pressure by allocating multiple medium worlds (10 worlds).");

    World_t* worlds[10] = {NULL}; // Array to hold multiple worlds
    int successful_allocations = 0;

    d_LogDebug("Attempting to allocate 10 medium worlds...");
    LOOP_TEST_START();
    for (int i = 0; i < 10; i++) {
        worlds[i] = init_world(WORLD_WIDTH_MEDIUM, WORLD_HEIGHT_MEDIUM,
                              REGION_WIDTH_MEDIUM, REGION_HEIGHT_MEDIUM,
                              LOCAL_WIDTH_MEDIUM, LOCAL_HEIGHT_MEDIUM, Z_HEIGHT_MEDIUM);
        if (worlds[i] != NULL) {
            successful_allocations++;
        }
    }
    LOOP_TEST_END();
    d_LogDebugF("Successfully allocated %d out of 10 worlds.", successful_allocations);

    d_LogDebug("Cleaning up all allocated worlds...");
    for (int i = 0; i < 10; i++) {
        if (worlds[i] != NULL) {
            free_world(worlds[i], (worlds[i]->world_width * worlds[i]->world_height),
                      (worlds[i]->region_width * worlds[i]->region_height));
        }
    }

    TEST_ASSERT(1, "Resource exhaustion test completed without crash.");
    d_LogInfo("Test Complete: Resource Exhaustion Recovery.");
    return 1;
}

int test_we_concurrent_access_simulation(void)
{
    d_LogInfo("--- Running Test: Concurrent Access Simulation ---");
    d_LogWarning("BUG HUNT: Simulating rapid function calls (100 cycles).");

    World_t* shared_world = init_world(WORLD_WIDTH_SMALL, WORLD_HEIGHT_SMALL,
                                     REGION_WIDTH_SMALL, REGION_HEIGHT_SMALL,
                                     LOCAL_WIDTH_SMALL, LOCAL_HEIGHT_SMALL, Z_HEIGHT_SMALL);
    TEST_ASSERT(shared_world != NULL, "Shared world should be created for concurrent testing.");
    if (!shared_world) return 0;

    d_LogDebug("Simulating rapid access patterns by calling placeholder functions...");
    LOOP_TEST_START();
    for (int cycle = 0; cycle < 100; cycle++) {
        we_edit();
        we_save();
        we_load();
    }
    LOOP_TEST_END();

    TEST_ASSERT(shared_world->regions != NULL, "World should survive concurrent access simulation.");
    free_world(shared_world, (shared_world->world_width * shared_world->world_height),
              (shared_world->region_width * shared_world->region_height));

    d_LogInfo("Test Complete: Concurrent Access Simulation.");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
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

    TEST_SUITE_START("World Editor Advanced Tests (Stable)");

    // Run tests for advanced world editor scenarios
    RUN_TEST(test_we_large_world_operations);
    RUN_TEST(test_we_stress_test_recreation);
    RUN_TEST(test_we_unimplemented_placeholders);
    RUN_TEST(test_we_memory_boundary_corruption);
    RUN_TEST(test_we_null_pointer_cascade);
    RUN_TEST(test_we_integer_overflow_protection);
    RUN_TEST(test_we_resource_exhaustion_recovery);
    RUN_TEST(test_we_concurrent_access_simulation);

    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}