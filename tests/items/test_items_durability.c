// ASCIIGame/tests/items/test_items_durability.c
// Test file for item durability system functions, enhanced with Daedalus Logging.

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "Daedalus.h" // The architect's toolkit, including the logging system.
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper function for float comparison
static bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// Dummy callback for creating consumables
static void dummy_consume_callback(uint8_t value)
{
    (void)value; // Suppress unused parameter warning
}

// =============================================================================
// DURABILITY DAMAGE TESTS
// =============================================================================

int test_damage_item_durability(void)
{
    // Create test items
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);
    Lock_t lock = create_lock("Test Lock", "test_lock", 10, 10);

    Item_t* sword = create_weapon("Test Sword", "test_sword", basic_material, 20, 30, 0, 'S', 0, 0, "common");
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 25, 15, 'A', 10, 10, "common");
    Item_t* key = create_key("Test Key", "test_key", lock, 'k', 0, "common");

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    TEST_ASSERT(key != NULL, "Key should be created");
    if (!sword || !armor || !key) {
        d_LogError("Failed to create one or more items for test.");
        return 0; // Can't proceed if creation fails
    }

    // Check initial durability
    d_LogInfoF("Initial State: Sword Durability=%d, Armor Durability=%d", get_durability(sword), get_durability(armor));
    TEST_ASSERT(get_durability(sword) == 255, "Sword should start with full durability");
    TEST_ASSERT(get_durability(armor) == 255, "Armor should start with full durability");

    // Damage the items
    d_LogDebug("Applying 50 damage to sword, 30 to armor, 100 to key.");
    damage_item_durability(sword, 50);
    damage_item_durability(armor, 30);
    damage_item_durability(key, 100); // Should have no effect

    // Check durability after damage
    d_LogInfoF("After Damage: Sword Durability=%d, Armor Durability=%d, Key Durability=%d", get_durability(sword), get_durability(armor), get_durability(key));
    TEST_ASSERT(get_durability(sword) == 205, "Sword durability should be 255-50=205");
    TEST_ASSERT(get_durability(armor) == 225, "Armor durability should be 255-30=225");
    TEST_ASSERT(get_durability(key) == 255, "Key durability should be unaffected");

    // Test massive damage (should cap at 0)
    d_LogWarning("Applying massive damage (256) to sword to test breaking.");
    damage_item_durability(sword, 256);
    d_LogInfoF("Broken Sword Durability: %d", get_durability(sword));
    TEST_ASSERT(get_durability(sword) == 0, "Sword should be broken (durability 0)");

    // Test NULL handling
    damage_item_durability(NULL, 50);
    TEST_ASSERT(1, "damage_item_durability should handle NULL gracefully");

    destroy_item(sword);
    destroy_item(armor);
    destroy_item(key);
    return 1;
}

// =============================================================================
// DURABILITY REPAIR TESTS
// =============================================================================

int test_repair_item(void)
{
    // Create test items
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Damaged Sword", "damaged_sword", basic_material, 15, 25, 0, 'S', 0, 0, "common");
    Item_t* armor = create_armor("Damaged Armor", "damaged_armor", basic_material, 20, 10, 'A', 10, 10, "common");
    Item_t* consumable = create_consumable("Potion", "potion", 50, dummy_consume_callback, 'p', 0, "common");

    TEST_ASSERT(sword && armor && consumable, "All test items should be created");
    if (!sword || !armor || !consumable) { return 0; }

    // Damage items first
    damage_item_durability(sword, 100);
    damage_item_durability(armor, 75);
    d_LogDebugF("Post Initial Damage: Sword Durability=%d, Armor Durability=%d", get_durability(sword), get_durability(armor));
    TEST_ASSERT(get_durability(sword) == 155, "Sword should be damaged to 155");
    TEST_ASSERT(get_durability(armor) == 180, "Armor should be damaged to 180");

    // Repair the items
    repair_item(sword, 50);
    repair_item(armor, 25);
    repair_item(consumable, 100); // Should have no effect
    TEST_ASSERT(get_durability(sword) == 205, "Sword should be repaired to 205");
    TEST_ASSERT(get_durability(armor) == 205, "Armor should be repaired to 205");
    TEST_ASSERT(get_durability(consumable) == 255, "Consumable durability should be unaffected");

    // Test over-repair (should cap at 255)
    d_LogWarning("Testing over-repair to ensure durability caps at 255.");
    repair_item(sword, 100);
    TEST_ASSERT(get_durability(sword) == 255, "Sword should be capped at max durability");

    // Test repairing broken item
    damage_item_durability(armor, 255); // Break it completely
    d_LogInfoF("Broken Armor Durability: %d", get_durability(armor));
    TEST_ASSERT(get_durability(armor) == 0, "Armor should be broken");
    repair_item(armor, 100);
    TEST_ASSERT(get_durability(armor) == 100, "Broken armor should be repairable");

    // Test NULL handling
    repair_item(NULL, 50);
    TEST_ASSERT(1, "repair_item should handle NULL gracefully");

    destroy_item(sword);
    destroy_item(armor);
    destroy_item(consumable);
    return 1;
}

// =============================================================================
// ITEM BROKEN STATUS TESTS
// =============================================================================

int test_is_item_broken(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Breakable Sword", "breakable_sword", basic_material, 18, 28, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Sword should be created");

    // Test initial state
    TEST_ASSERT(!is_item_broken(sword), "New sword should not be broken");

    // Damage partially
    damage_item_durability(sword, 100);
    d_LogDebugF("Sword durability after partial damage: %d", get_durability(sword));
    TEST_ASSERT(!is_item_broken(sword), "Partially damaged sword should not be broken");

    // Break completely
    damage_item_durability(sword, 200); // Should break it (155-200 < 0)
    d_LogWarningF("Sword durability after breaking damage: %d", get_durability(sword));
    TEST_ASSERT(is_item_broken(sword), "Completely damaged sword should be broken");

    // Test NULL handling
    TEST_ASSERT(is_item_broken(NULL), "NULL item should be considered broken");

    destroy_item(sword);
    return 1;
}

// =============================================================================
// DURABILITY PERCENTAGE TESTS
// =============================================================================

int test_get_durability_percentage(void)
{
    Material_t mat = create_material("basic", create_default_material_properties());
    Item_t* sword = create_weapon("Percentage Sword", "p_sword", mat, 16, 24, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Sword should be created");

    // Test full durability
    TEST_ASSERT(float_equals(get_durability_percentage(sword), 1.0f, 0.01f), "Full durability should be 100%");

    // Damage to specific percentages
    damage_item_durability(sword, 127); // ~50%
    d_LogDebugF("~50%% test: dur=%d, perc=%.2f", get_durability(sword), get_durability_percentage(sword));
    TEST_ASSERT(float_equals(get_durability_percentage(sword), 0.5f, 0.02f), "Half damaged sword should be ~50%");

    damage_item_durability(sword, 64); // ~25% relative to remaining (128-64=64)
    d_LogDebugF("~25%% test: dur=%d, perc=%.2f", get_durability(sword), get_durability_percentage(sword));
    TEST_ASSERT(float_equals(get_durability_percentage(sword), 0.25f, 0.02f), "Quarter durability armor should be ~25%");

    // Test completely broken
    damage_item_durability(sword, 255);
    TEST_ASSERT(float_equals(get_durability_percentage(sword), 0.0f, 0.01f), "Broken sword should be 0%");

    // Test NULL handling
    TEST_ASSERT(float_equals(get_durability_percentage(NULL), 0.0f, 0.01f), "NULL item should be 0%");

    destroy_item(sword);
    return 1;
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

int test_durability_system_integration(void)
{
    // Material that affects durability
    MaterialProperties_t fragile_props = create_default_material_properties();
    fragile_props.durability_fact = 0.8f; // Takes more damage from hits
    Material_t fragile_material = create_material("brittle", fragile_props);
    Item_t* brittle_sword = create_weapon("Brittle Sword", "brittle_sword", fragile_material, 30, 40, 0, 'B', 0, 0, "common");
    TEST_ASSERT(brittle_sword != NULL, "Brittle sword should be created");

    uint8_t initial_durability = get_durability(brittle_sword);
    d_LogInfoF("Integration Test Initial: Item='%s', Initial Durability=%d", brittle_sword->name->str, initial_durability);
    TEST_ASSERT(initial_durability == 255, "Brittle sword should have standard initial durability");

    // Damage it
    d_LogInfo("Applying 50 damage to brittle sword...");
    damage_item_durability(brittle_sword, 50);
    uint8_t after_damage = get_durability(brittle_sword);
    d_LogInfoF("Durability after 50 damage: %d.", after_damage);
    TEST_ASSERT(after_damage < initial_durability, "Damage should decrease durability");

    // Repair it
    repair_item(brittle_sword, 30);
    uint8_t after_repair = get_durability(brittle_sword);
    d_LogInfoF("Durability after 30 repair: %d", after_repair);
    TEST_ASSERT(after_repair > after_damage, "Repair should increase durability");

    // Break it completely
    damage_item_durability(brittle_sword, 255);
    d_LogWarningF("Broken Sword Durability: %d", get_durability(brittle_sword));
    TEST_ASSERT(is_item_broken(brittle_sword), "Massively damaged sword should be broken");

    destroy_item(brittle_sword);
    return 1;
}

// =============================================================================
// EDGE CASE & STRESS TESTS
// =============================================================================

int test_durability_overflow_underflow(void)
{
    Material_t mat = create_material("basic", create_default_material_properties());
    Item_t* sword = create_weapon("Overflow Sword", "overflow_sword", mat, 20, 30, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Overflow test sword should be created");

    d_LogWarning("Testing extreme damage values for underflow...");
    damage_item_durability(sword, 65535); // More than max uint8_t
    d_LogInfoF("Durability after extreme damage (65535): %d", get_durability(sword));
    TEST_ASSERT(get_durability(sword) == 0, "Extreme damage should clamp to 0, not underflow");

    repair_item(sword, 255); // Reset
    d_LogWarning("Testing extreme repair values for overflow...");
    repair_item(sword, 65535);
    d_LogInfoF("Durability after extreme repair (65535): %d", get_durability(sword));
    TEST_ASSERT(get_durability(sword) == 255, "Extreme repair should clamp to 255, not overflow");

    destroy_item(sword);
    return 1;
}

int test_durability_rapid_operations(void)
{
    Material_t mat = create_material("basic", create_default_material_properties());
    Item_t* sword = create_weapon("Stress Sword", "stress_sword", mat, 20, 30, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Stress sword should be created");

    LOOP_TEST_START();
    for (int i = 0; i < 100; i++) {
        damage_item_durability(sword, 10);
        repair_item(sword, 5);
        TEST_ASSERT(get_durability(sword) <= 255, "Durability must stay within bounds");
    }
    LOOP_TEST_END();

    destroy_item(sword);
    return 1;
}

int test_durability_percentage_edge_cases(void)
{
    Material_t mat = create_material("basic", create_default_material_properties());
    Item_t* sword = create_weapon("Percentage Edge Sword", "p_edge_sword", mat, 20, 30, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Percentage edge case sword should be created");

    uint8_t test_durabilities[] = {255, 254, 128, 127, 1, 0};
    int num_tests = sizeof(test_durabilities) / sizeof(test_durabilities[0]);

    for (int i = 0; i < num_tests; i++) {
        repair_item(sword, 255); // Full repair to reset state
        damage_item_durability(sword, 255 - test_durabilities[i]);
        float percentage = get_durability_percentage(sword);
        float expected = (float)test_durabilities[i] / 255.0f;
        d_LogDebugF("Testing durability %3d -> Expected %.3f, Got %.3f", test_durabilities[i], expected, percentage);
        TEST_ASSERT(float_equals(percentage, expected, 0.001f), "Percentage calculation must be accurate");
        TEST_ASSERT(percentage >= 0.0f && percentage <= 1.0f, "Percentage must be in valid [0,1] range");
    }

    destroy_item(sword);
    return 1;
}

int test_durability_concurrent_simulation(void)
{
    Material_t mat = create_material("basic", create_default_material_properties());
    Item_t* sword = create_weapon("Concurrent Sword", "conc_sword", mat, 20, 30, 0, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Concurrent test sword should be created");

    typedef struct { uint8_t damage; uint8_t repair; } Op_t;
    Op_t ops[] = { {50, 30}, {100, 80}, {200, 150}, {255, 200}, {120, 255} };
    int num_ops = sizeof(ops) / sizeof(ops[0]);

    for (int i = 0; i < num_ops; i++) {
        uint8_t before_dur = get_durability(sword);
        damage_item_durability(sword, ops[i].damage);
        uint8_t after_dmg = get_durability(sword);
        repair_item(sword, ops[i].repair);
        uint8_t after_rep = get_durability(sword);

        d_LogInfoF("SimOp %d: Before=%3d, Dmg=%3d, AfterDmg=%3d, Rep=%3d, Final=%3d",
            i, before_dur, ops[i].damage, after_dmg, ops[i].repair, after_rep);

        // This assertion is valid and useful. A uint8_t can be > 255 if there is an integer promotion bug.
        TEST_ASSERT(after_rep <= 255, "Durability should not exceed maximum");
        float perc = get_durability_percentage(sword);
        TEST_ASSERT(perc >= 0.0f && perc <= 1.0f, "Percentage should be in valid range");
        TEST_ASSERT((after_rep == 0) == is_item_broken(sword), "Broken state should match zero durability");
    }

    destroy_item(sword);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // =========================================================================
    // DAEDALUS LOGGER INITIALIZATION
    // =========================================================================
    // 1. Create a default configuration for our logger.
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG, // Show all logs from DEBUG up
        .colorize_output = true,
        // Let's keep the test output clean by disabling extra metadata for now
        .include_timestamp = false,
        .include_file_info = false,
        .include_function = false
    };

    // 2. Create the logger instance.
    dLogger_t* logger = d_CreateLogger(config);

    // 3. Set our new logger as the GLOBAL logger for all d_Log... calls.
    d_SetGlobalLogger(logger);

    // 4. Add a handler to the now-existing global logger so it prints to console.
    // We use d_GetGlobalLogger() to safely get the pointer we just set.
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);
    // =========================================================================


    TEST_SUITE_START("Item Durability System Tests");
    srand(62686996); // Seed for randomness in material resistance

    // Run durability system tests
    RUN_TEST(test_damage_item_durability);
    RUN_TEST(test_repair_item);
    RUN_TEST(test_is_item_broken);
    RUN_TEST(test_get_durability_percentage);
    RUN_TEST(test_durability_system_integration);

    // Run advanced edge case and stress tests
    RUN_TEST(test_durability_overflow_underflow);
    RUN_TEST(test_durability_rapid_operations);
    RUN_TEST(test_durability_percentage_edge_cases);
    RUN_TEST(test_durability_concurrent_simulation);

    TEST_SUITE_END();

    // =========================================================================
    // DAEDALUS LOGGER SHUTDOWN
    // =========================================================================
    // 5. Destroy the global logger to free its memory.
    d_DestroyLogger(d_GetGlobalLogger());
    // =========================================================================

}
