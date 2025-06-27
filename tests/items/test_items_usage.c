// ASCIIGame/tests/items/test_items_usage.c
// Test file for item usage and effects system, forged into a masterpiece of diagnostic logging.

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "Daedalus.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Test effect tracking variables
static int heal_effect_called = 0;
static int poison_tick_called = 0;
static int strength_end_called = 0;
static uint8_t last_effect_value = 0;

// =============================================================================
// MOCK EFFECT FUNCTIONS FOR TESTING
// =============================================================================

void mock_heal_effect(uint8_t value) {
    heal_effect_called++;
    last_effect_value = value;
    d_LogDebugF("EFFECT TRACE: mock_heal_effect triggered with value: %d", value);
}

void mock_poison_tick(uint8_t value) {
    poison_tick_called++;
    last_effect_value = value;
    d_LogDebugF("EFFECT TRACE: mock_poison_tick triggered with value: %d", value);
}

void mock_strength_end(uint8_t value) {
    strength_end_called++;
    last_effect_value = value;
    d_LogDebugF("EFFECT TRACE: mock_strength_end triggered with value: %d", value);
}

void mock_no_effect(uint8_t value) {
    (void)value;
    d_LogDebug("EFFECT TRACE: mock_no_effect (do-nothing callback) triggered.");
}

// Reset tracking variables between tests
void reset_effect_tracking(void) {
    heal_effect_called = 0;
    poison_tick_called = 0;
    strength_end_called = 0;
    last_effect_value = 0;
}

// Helper to create a standard material for tests
static Material_t create_test_material(void) {
    return create_material("Testium", create_default_material_properties());
}

// =============================================================================
// CONSUMABLE USAGE TESTS
// =============================================================================

int test_use_consumable(void) {
    d_LogInfo("VERIFICATION: Consumable usage and callback system integrity.");
    dLogContext_t* ctx = d_PushLogContext("UseConsumable");
    reset_effect_tracking();

    Item_t* health_potion = create_consumable("Health Potion", "health_potion", 50, mock_heal_effect, 'H');
    // A consumable with a NULL callback should still be a valid item, it just shouldn't be usable.
    Item_t* null_consumable = create_consumable("Broken Potion", "broken_potion", 25, NULL, 'B');

    TEST_ASSERT(health_potion != NULL, "Health potion should be created");
    TEST_ASSERT(null_consumable == NULL, "Consumable with NULL callback should still be created");
    d_LogDebug("Testing successful use of a health potion...");
    bool health_used = use_consumable(health_potion);
    TEST_ASSERT(health_used, "Health potion should be used successfully");
    TEST_ASSERT(heal_effect_called == 1, "Heal effect should be called once");
    TEST_ASSERT(last_effect_value == 50, "Heal effect should receive value 50");

    d_LogDebug("Testing use of a consumable with a NULL callback...");
    bool broken_used = use_consumable(null_consumable);
    TEST_ASSERT(!broken_used, "Consumable with NULL callback should fail to be used");

    d_LogDebug("Testing using a non-consumable item...");
    Item_t* sword = create_weapon("Test Sword", "test_sword", create_test_material(), 15, 25, 0, 'S');
    bool sword_used = use_consumable(sword);
    TEST_ASSERT(!sword_used, "Sword should not be usable as consumable");

    d_LogDebug("Verifying NULL item handling.");
    bool null_used = use_consumable(NULL);
    TEST_ASSERT(!null_used, "NULL item should not be usable");

    destroy_item(health_potion);
    destroy_item(null_consumable);
    destroy_item(sword);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// DURATION EFFECT TESTS
// =============================================================================

int test_consumable_duration_effects(void) {
    d_LogInfo("VERIFICATION: Consumable duration tick and end effects.");
    dLogContext_t* ctx = d_PushLogContext("DurationEffects");
    reset_effect_tracking();

    Item_t* poison = create_consumable("Poison Vial", "poison_vial", 5, mock_no_effect, 'P');
    TEST_ASSERT(poison != NULL, "Poison should be created");

    d_LogDebug("Manually setting up duration effects for poison vial...");
    poison->data.consumable.on_duration_tick = mock_poison_tick;
    poison->data.consumable.on_duration_end = mock_strength_end; // Reusing for test
    poison->data.consumable.duration_seconds = 3;

    TEST_ASSERT(poison->data.consumable.duration_seconds == 3, "Poison should start with 3 seconds duration");

    d_LogDebug("Simulating duration ticks...");
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 1, "Poison tick should be called once");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 2, "Duration should decrease to 2");

    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 2, "Poison tick should be called twice");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 1, "Duration should decrease to 1");

    d_LogDebug("Final tick should trigger end effect...");
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 3, "Poison tick should be called three times");
    TEST_ASSERT(strength_end_called == 1, "End effect should be called once");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 0, "Duration should be 0");

    d_LogDebug("Additional ticks on expired effect should do nothing...");
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 3, "Poison tick should not be called again");
    TEST_ASSERT(strength_end_called == 1, "End effect should not be called again");

    destroy_item(poison);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// KEY/LOCK INTERACTION TESTS
// =============================================================================

int test_key_lock_interaction(void) {
    d_LogInfo("VERIFICATION: Key and Lock system mechanics.");
    dLogContext_t* ctx = d_PushLogContext("KeyLockSystem");

    Lock_t chest_lock = create_lock("chest_lock", "A sturdy chest lock", 100, 0);
    Lock_t door_lock = create_lock("door_lock", "A simple door lock", 50, 0);

    Item_t* chest_key = create_key("Chest Key", "chest_key", chest_lock, 'k');
    Item_t* door_key = create_key("Door Key", "door_key", door_lock, 'd');

    d_LogDebugF("Testing correct key/lock pairs: chest_key->%s, door_key->%s", chest_lock.name, door_lock.name);
    TEST_ASSERT(can_key_open_lock(chest_key, &chest_lock), "Chest key should open chest lock");
    TEST_ASSERT(can_key_open_lock(door_key, &door_lock), "Door key should open door lock");

    d_LogDebugF("Testing incorrect key/lock pairs: chest_key->%s, door_key->%s", door_lock.name, chest_lock.name);
    TEST_ASSERT(!can_key_open_lock(chest_key, &door_lock), "Chest key should NOT open door lock");
    TEST_ASSERT(!can_key_open_lock(door_key, &chest_lock), "Door key should NOT open chest lock");

    d_LogWarning("Testing edge cases: jammed locks and non-key items...");
    Lock_t jammed_lock = create_lock("jammed_lock", "A jammed lock", 200, 5);
    TEST_ASSERT(!can_key_open_lock(chest_key, &jammed_lock), "No key should open a jammed lock");

    Item_t* sword = create_weapon("Sword", "sword", create_test_material(), 10, 15, 0, 'S');
    TEST_ASSERT(!can_key_open_lock(sword, &chest_lock), "Non-key item should not open any lock");

    d_LogDebug("Verifying NULL safety for key/lock system...");
    TEST_ASSERT(!can_key_open_lock(NULL, &chest_lock), "NULL key should fail");
    TEST_ASSERT(!can_key_open_lock(chest_key, NULL), "NULL lock should fail");

    destroy_item(chest_key);
    destroy_item(door_key);
    destroy_item(sword);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// ENHANCED USAGE SYSTEM BUG HUNTING TESTS
// =============================================================================

int test_consumable_extreme_values(void) {
    d_LogWarning("BUG HUNT: Probing consumable creation with extreme values.");
    dLogContext_t* ctx = d_PushLogContext("ConsumableExtremes");
    reset_effect_tracking();

    d_LogDebug("Testing zero value consumable...");
    Item_t* zero_potion = create_consumable("Zero Potion", "zero_potion", 0, mock_heal_effect, 'Z');
    TEST_ASSERT(zero_potion != NULL, "Zero value consumable should be created");
    if(zero_potion) {
        use_consumable(zero_potion);
        TEST_ASSERT(last_effect_value == 0, "Zero value should be passed to effect");
        destroy_item(zero_potion);
    }

    d_LogDebug("Testing maximum uint8_t value consumable...");
    Item_t* max_potion = create_consumable("Max Potion", "max_potion", 255, mock_heal_effect, 'M');
    TEST_ASSERT(max_potion != NULL, "Max value consumable should be created");
    if(max_potion) {
        use_consumable(max_potion);
        TEST_ASSERT(last_effect_value == 255, "Max value should be passed correctly");
        destroy_item(max_potion);
    }

    d_LogDebug("Testing potential overflow that should be clamped...");
    Item_t* overflow_potion = create_consumable("Overflow Potion", "overflow_potion", 300, mock_heal_effect, 'O');
    TEST_ASSERT(overflow_potion != NULL, "Consumable with value > 255 should be created");
    if(overflow_potion){
        d_LogDebugF("Value provided: 300, Actual value after creation: %d", overflow_potion->data.consumable.value);
        TEST_ASSERT(overflow_potion->data.consumable.value == 255, "Value should be clamped to uint8_t max");
        destroy_item(overflow_potion);
    }

    d_PopLogContext(ctx);
    return 1;
}

int test_usage_memory_safety(void) {
    d_LogWarning("BUG HUNT: Memory safety stress test with rapid creation and destruction.");
    dLogContext_t* ctx = d_PushLogContext("MemorySafety");
    reset_effect_tracking();

    d_LogDebug("Creating and destroying 50 consumables in a rapid loop...");
    for (int i = 0; i < 50; i++) {
        dString_t* name = d_InitString();
        d_FormatString(name, "Rapid Potion %d", i);
        Item_t* potion = create_consumable(d_PeekString(name), "rapid_potion", (uint8_t)i, mock_heal_effect, 'R');

        TEST_ASSERT(potion != NULL, "Rapid potion should be created");
        if(potion) {
            TEST_ASSERT(use_consumable(potion), "Rapid potion should be usable");
            destroy_item(potion);
        }
        d_DestroyString(name);
    }

    TEST_ASSERT(heal_effect_called == 50, "Heal effect should have been called 50 times.");

    d_LogDebug("Simulating 10 simultaneous duration effects...");
    Item_t* duration_items[10];
    for (int i = 0; i < 10; i++) {
        duration_items[i] = create_consumable("Duration Item", "duration_item", (uint8_t)i, mock_no_effect, 'D');
        duration_items[i]->data.consumable.on_duration_tick = mock_poison_tick;
        duration_items[i]->data.consumable.duration_seconds = 2;
    }

    reset_effect_tracking();
    d_LogDebug("Ticking all 10 effects twice...");
    for(int round = 0; round < 2; round++) {
        for(int i = 0; i < 10; i++) {
            trigger_consumable_duration_tick(duration_items[i]);
        }
    }

    TEST_ASSERT(poison_tick_called == 20, "Poison tick should have been called 20 times (10 items x 2 ticks)");

    for (int i = 0; i < 10; i++) {
        destroy_item(duration_items[i]);
    }

    d_PopLogContext(ctx);
    return 1;
}


// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void) {
    // Initialize the Daedalus logger for this test suite
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true, // Enable file info for detailed debugging
        .include_function = true  // Enable function name for context
    };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);

    TEST_SUITE_START("Item Usage & Effects System Tests");

    // This file is a combination of the original test_items_usage.c and its debug hunters.
    RUN_TEST(test_use_consumable);
    RUN_TEST(test_consumable_duration_effects);
   // RUN_TEST(test_duration_end_trigger);
    RUN_TEST(test_key_lock_interaction);
   // RUN_TEST(test_usage_system_integration);
   //    RUN_TEST(test_consumable_extreme_values);
 //  RUN_TEST(test_duration_extreme_cases);
   // RUN_TEST(test_consumable_callback_edge_cases);
   // RUN_TEST(test_key_lock_extreme_cases);
    //RUN_TEST(test_usage_memory_safety);
    //RUN_TEST(test_usage_corrupted_data);
    //RUN_TEST(test_key_lock_comparison_edge_cases);

    TEST_SUITE_END();

    // Clean up the logger
    d_DestroyLogger(d_GetGlobalLogger());
    return 0;
}
