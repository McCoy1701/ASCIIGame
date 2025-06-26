// ASCIIGame/tests/items/test_items_usage.c
// Test file for item usage and effects system
#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )

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

// Test effect tracking variables (global for testing purposes)
int heal_effect_called = 0;
int poison_tick_called = 0;
int strength_end_called = 0;
uint8_t last_effect_value = 0;

// =============================================================================
// MOCK EFFECT FUNCTIONS FOR TESTING
// =============================================================================

void mock_heal_effect(uint8_t value)
{
    heal_effect_called++;
    last_effect_value = value;
    printf("🩹 Healing for %d points!\n", value);
}

void mock_poison_tick(uint8_t value)
{
    poison_tick_called++;
    last_effect_value = value;
    printf("☠️ Taking %d poison damage!\n", value);
}

void mock_strength_end(uint8_t value)
{
    strength_end_called++;
    last_effect_value = value;
    printf("💪 Strength boost of %d has ended.\n", value);
}

void mock_no_effect(uint8_t value)
{
    (void)value; // Do nothing - for testing NULL-like behavior
}

// Reset tracking variables between tests
void reset_effect_tracking(void)
{
    heal_effect_called = 0;
    poison_tick_called = 0;
    strength_end_called = 0;
    last_effect_value = 0;
}

// =============================================================================
// CONSUMABLE USAGE TESTS
// =============================================================================

int test_use_consumable(void)
{
    reset_effect_tracking();

    // Create test consumables
    Item_t* health_potion = create_consumable("Health Potion", "health_potion", 50, mock_heal_effect, 'H');
    Item_t* null_consumable = create_consumable("Broken Potion", "broken_potion", 25, NULL, 'B');

    printf("null_consumable: %p\n", null_consumable);
    TEST_ASSERT(health_potion != NULL, "Health potion should be created");
    TEST_ASSERT(null_consumable == NULL, "Broken potion should not be created");

    // Test successful consumable use
    bool health_used = use_consumable(health_potion);
    TEST_ASSERT(health_used, "Health potion should be used successfully");
    TEST_ASSERT(heal_effect_called == 1, "Heal effect should be called once");
    TEST_ASSERT(last_effect_value == 50, "Heal effect should receive value 50");

    // Test consumable with NULL callback
    bool broken_used = use_consumable(null_consumable);
    TEST_ASSERT(!broken_used, "Broken potion should fail to be used");

    // Test using non-consumable item
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);
    Item_t* sword = create_weapon("Test Sword", "test_sword", basic_material, 15, 25, 0, 'S');

    bool sword_used = use_consumable(sword);
    TEST_ASSERT(!sword_used, "Sword should not be usable as consumable");

    // Test NULL handling
    bool null_used = use_consumable(NULL);
    TEST_ASSERT(!null_used, "NULL item should not be usable");

    destroy_item(health_potion);
    destroy_item(null_consumable);
    destroy_item(sword);
    return 1;
}

// =============================================================================
// DURATION EFFECT TESTS
// =============================================================================

int test_consumable_duration_effects(void)
{
    reset_effect_tracking();

    // Create a poison with duration effects
    Item_t* poison = create_consumable("Poison Vial", "poison_vial", 5, mock_no_effect, 'P');
    TEST_ASSERT(poison != NULL, "Poison should be created");

    // Manually set up duration effects (since create_consumable doesn't set these)
    poison->data.consumable.on_duration_tick = mock_poison_tick;
    poison->data.consumable.on_duration_end = mock_strength_end; // Reusing for test
    poison->data.consumable.duration_seconds = 3; // 3 seconds duration

    // Test initial state
    TEST_ASSERT(poison->data.consumable.duration_seconds == 3, "Poison should start with 3 seconds duration");

    // Simulate ticking effects
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 1, "Poison tick should be called once");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 2, "Duration should decrease to 2");

    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 2, "Poison tick should be called twice");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 1, "Duration should decrease to 1");

    // Final tick should trigger end effect
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 3, "Poison tick should be called three times");
    TEST_ASSERT(strength_end_called == 1, "End effect should be called once");
    TEST_ASSERT(poison->data.consumable.duration_seconds == 0, "Duration should be 0");

    // Additional ticks should do nothing
    trigger_consumable_duration_tick(poison);
    TEST_ASSERT(poison_tick_called == 3, "Poison tick should not be called again");
    TEST_ASSERT(strength_end_called == 1, "End effect should not be called again");

    destroy_item(poison);
    return 1;
}

int test_duration_end_trigger(void)
{
    reset_effect_tracking();

    // Create item with only end effect
    Item_t* buff = create_consumable("Strength Buff", "strength_buff", 10, mock_no_effect, 'S');
    TEST_ASSERT(buff != NULL, "Buff should be created");

    buff->data.consumable.on_duration_end = mock_strength_end;
    buff->data.consumable.duration_seconds = 5;

    // Manually trigger end effect
    trigger_consumable_duration_end(buff);
    TEST_ASSERT(strength_end_called == 1, "End effect should be called");
    TEST_ASSERT(last_effect_value == 10, "End effect should receive correct value");
    TEST_ASSERT(buff->data.consumable.duration_seconds == 0, "Duration should be reset to 0");

    // Test with NULL callbacks
    Item_t* simple = create_consumable("Simple Item", "simple", 1, mock_no_effect, 's');
    simple->data.consumable.on_duration_end = NULL;
    simple->data.consumable.duration_seconds = 1;

    trigger_consumable_duration_end(simple);
    TEST_ASSERT(1, "Should handle NULL end callback gracefully");

    // Test NULL handling
    trigger_consumable_duration_tick(NULL);
    trigger_consumable_duration_end(NULL);
    TEST_ASSERT(1, "Should handle NULL items gracefully");

    destroy_item(buff);
    destroy_item(simple);
    return 1;
}

// =============================================================================
// KEY/LOCK INTERACTION TESTS
// =============================================================================

int test_key_lock_interaction(void)
{
    // Create test locks
    Lock_t chest_lock = create_lock("chest_lock", "A sturdy chest lock", 100, 0);
    Lock_t door_lock = create_lock("door_lock", "A simple door lock", 50, 0);
    Lock_t jammed_lock = create_lock("jammed_lock", "A jammed lock", 200, 5);

    // Create test keys
    Item_t* chest_key = create_key("Chest Key", "chest_key", chest_lock, 'k');
    Item_t* door_key = create_key("Door Key", "door_key", door_lock, 'd');
    Item_t* wrong_key = create_key("Wrong Key", "wrong_key", chest_lock, 'w');

    TEST_ASSERT(chest_key != NULL, "Chest key should be created");
    TEST_ASSERT(door_key != NULL, "Door key should be created");
    TEST_ASSERT(wrong_key != NULL, "Wrong key should be created");

    // Test correct key opens correct lock
    bool chest_opens = can_key_open_lock(chest_key, &chest_lock);
    TEST_ASSERT(chest_opens, "Chest key should open chest lock");

    bool door_opens = can_key_open_lock(door_key, &door_lock);
    TEST_ASSERT(door_opens, "Door key should open door lock");

    // Test wrong key doesn't open lock
    bool wrong_opens_door = can_key_open_lock(chest_key, &door_lock);
    TEST_ASSERT(!wrong_opens_door, "Chest key should not open door lock");

    bool door_opens_chest = can_key_open_lock(door_key, &chest_lock);
    TEST_ASSERT(!door_opens_chest, "Door key should not open chest lock");

    // Test jammed lock cannot be opened
    bool jammed_opens = can_key_open_lock(chest_key, &jammed_lock);
    TEST_ASSERT(!jammed_opens, "No key should open jammed lock");

    // Test non-key items cannot open locks
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);
    Item_t* sword = create_weapon("Sword", "sword", basic_material, 10, 15, 0, 'S');

    bool sword_opens = can_key_open_lock(sword, &chest_lock);
    TEST_ASSERT(!sword_opens, "Sword should not open locks");

    // Test NULL handling
    TEST_ASSERT(!can_key_open_lock(NULL, &chest_lock), "NULL key should not open lock");
    TEST_ASSERT(!can_key_open_lock(chest_key, NULL), "Key should not open NULL lock");
    TEST_ASSERT(!can_key_open_lock(NULL, NULL), "NULL key and lock should fail");

    destroy_item(chest_key);
    destroy_item(door_key);
    destroy_item(wrong_key);
    destroy_item(sword);
    return 1;
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

int test_usage_system_integration(void)
{
    reset_effect_tracking();

    // Create a complex consumable with all effects
    Item_t* super_potion = create_consumable("Super Potion", "super_potion", 100, mock_heal_effect, 'S');
    TEST_ASSERT(super_potion != NULL, "Super potion should be created");

    // Set up all duration effects
    super_potion->data.consumable.on_duration_tick = mock_poison_tick; // Reusing for test
    super_potion->data.consumable.on_duration_end = mock_strength_end;
    super_potion->data.consumable.duration_seconds = 2;

    // Use the consumable (should trigger immediate effect)
    bool used = use_consumable(super_potion);
    TEST_ASSERT(used, "Super potion should be used successfully");
    TEST_ASSERT(heal_effect_called == 1, "Immediate heal effect should trigger");

    // Simulate duration effects
    trigger_consumable_duration_tick(super_potion);
    TEST_ASSERT(poison_tick_called == 1, "Duration tick should work");
    TEST_ASSERT(super_potion->data.consumable.duration_seconds == 1, "Duration should decrease");

    // Final tick should end effect
    trigger_consumable_duration_tick(super_potion);
    TEST_ASSERT(strength_end_called == 1, "End effect should trigger");
    TEST_ASSERT(super_potion->data.consumable.duration_seconds == 0, "Duration should end");

    // Test lock interaction integration
    Lock_t magic_lock = create_lock("magic_lock", "A magical lock", 255, 0);
    Item_t* magic_key = create_key("Magic Key", "magic_key", magic_lock, 'M');

    bool magic_works = can_key_open_lock(magic_key, &magic_lock);
    TEST_ASSERT(magic_works, "Magic key should open magic lock");

    // Test key doesn't work on different lock
    Lock_t different_lock = create_lock("different_lock", "A different lock", 100, 0);
    bool wrong_lock = can_key_open_lock(magic_key, &different_lock);
    TEST_ASSERT(!wrong_lock, "Magic key should not open different lock");

    destroy_item(super_potion);
    destroy_item(magic_key);
    return 1;
}

// =============================================================================
// EDGE CASE TESTS
// =============================================================================

int test_usage_edge_cases(void)
{
    reset_effect_tracking();

    // Test consumable with 0 duration
    Item_t* instant = create_consumable("Instant Effect", "instant", 25, mock_heal_effect, 'I');
    instant->data.consumable.on_duration_tick = mock_poison_tick;
    instant->data.consumable.duration_seconds = 0; // No duration

    trigger_consumable_duration_tick(instant);
    TEST_ASSERT(poison_tick_called == 0, "Zero duration should not trigger tick");

    // Test consumable with only tick, no end
    Item_t* tick_only = create_consumable("Tick Only", "tick_only", 10, mock_no_effect, 'T');
    tick_only->data.consumable.on_duration_tick = mock_poison_tick;
    tick_only->data.consumable.on_duration_end = NULL;
    tick_only->data.consumable.duration_seconds = 1;

    trigger_consumable_duration_tick(tick_only);
    TEST_ASSERT(poison_tick_called == 1, "Tick should work without end callback");
    TEST_ASSERT(tick_only->data.consumable.duration_seconds == 0, "Duration should end");

    // Test lock with 255 pick difficulty (unpickable)
    Lock_t unpickable_lock = create_lock("unpickable", "Cannot be picked", 255, 0);
    Item_t* any_key = create_key("Any Key", "any_key", unpickable_lock, 'A');

    bool unpickable_opens = can_key_open_lock(any_key, &unpickable_lock);
    TEST_ASSERT(unpickable_opens, "Matching key should still open unpickable lock");

    // Test very high duration
    Item_t* long_effect = create_consumable("Long Effect", "long_effect", 1, mock_no_effect, 'L');
    long_effect->data.consumable.on_duration_tick = mock_poison_tick;
    long_effect->data.consumable.duration_seconds = 65535; // Max uint16_t

    trigger_consumable_duration_tick(long_effect);
    TEST_ASSERT(long_effect->data.consumable.duration_seconds == 65534, "High duration should decrement correctly");

    destroy_item(instant);
    destroy_item(tick_only);
    destroy_item(any_key);
    destroy_item(long_effect);
    return 1;
}

// =============================================================================
// ENHANCED USAGE SYSTEM BUG HUNTING TESTS - WITH PROPER LOGGING
// =============================================================================

/*
 * Test consumable creation and usage with extreme values
 */
int test_consumable_extreme_values(void)
{
    LOG("Starting consumable extreme values test");
    reset_effect_tracking();

    // Test with zero value consumable
    LOG("Testing zero value consumable");
    Item_t* zero_potion = create_consumable("Zero Potion", "zero_potion", 0, mock_heal_effect, 'Z');
    TEST_ASSERT(zero_potion != NULL, "Zero value consumable should be created");

    if (zero_potion) {
        dString_t* log_msg = d_InitString();
        d_AppendString(log_msg, "Zero potion value: ", 0);
        d_AppendInt(log_msg, zero_potion->data.consumable.value);
        LOG(log_msg->str);
        d_DestroyString(log_msg);

        bool zero_used = use_consumable(zero_potion);
        TEST_ASSERT(zero_used, "Zero value consumable should be usable");
        TEST_ASSERT(last_effect_value == 0, "Zero value should be passed to effect");
        destroy_item(zero_potion);
    }

    // Test with maximum uint8_t value
    LOG("Testing maximum value consumable");
    Item_t* max_potion = create_consumable("Max Potion", "max_potion", 255, mock_heal_effect, 'M');
    TEST_ASSERT(max_potion != NULL, "Max value consumable should be created");

    if (max_potion) {
        bool max_used = use_consumable(max_potion);
        TEST_ASSERT(max_used, "Max value consumable should be usable");
        TEST_ASSERT(last_effect_value == 255, "Max value should be passed correctly");
        destroy_item(max_potion);
    }

    // Test with value that might overflow uint8_t
    LOG("Testing potential overflow value consumable");
    Item_t* overflow_potion = create_consumable("Overflow Potion", "overflow_potion", 65535, mock_heal_effect, 'O');
    TEST_ASSERT(overflow_potion != NULL, "Overflow value consumable should be created");

    if (overflow_potion) {
        dString_t* log_msg = d_InitString();
        d_AppendString(log_msg, "Overflow potion actual value: ", 0);
        d_AppendInt(log_msg, overflow_potion->data.consumable.value);
        d_AppendString(log_msg, " (should be clamped to uint8_t range)", 0);
        LOG(log_msg->str);
        d_DestroyString(log_msg);

        TEST_ASSERT(overflow_potion->data.consumable.value <= 255, "Value should be clamped to uint8_t");
        destroy_item(overflow_potion);
    }

    LOG("Consumable extreme values test completed");
    return 1;
}

/*
 * Test duration effects with extreme and edge case values
 */
int test_duration_extreme_cases(void)
{
    LOG("Starting duration extreme cases test");
    reset_effect_tracking();

    // Test with zero duration
    LOG("Testing zero duration consumable");
    Item_t* instant = create_consumable("Instant Effect", "instant", 25, mock_heal_effect, 'I');
    TEST_ASSERT(instant != NULL, "Instant effect should be created");

    instant->data.consumable.on_duration_tick = mock_poison_tick;
    instant->data.consumable.on_duration_end = mock_strength_end;
    instant->data.consumable.duration_seconds = 0;

    LOG("Triggering tick on zero duration item");
    trigger_consumable_duration_tick(instant);
    TEST_ASSERT(poison_tick_called == 0, "Zero duration should not trigger tick");

    // Test with maximum duration value
    LOG("Testing maximum duration consumable");
    Item_t* eternal = create_consumable("Eternal Effect", "eternal", 50, mock_no_effect, 'E');
    TEST_ASSERT(eternal != NULL, "Eternal effect should be created");

    eternal->data.consumable.on_duration_tick = mock_poison_tick;
    eternal->data.consumable.duration_seconds = 65535; // Max uint16_t

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Eternal effect initial duration: ", 0);
    d_AppendInt(log_msg, eternal->data.consumable.duration_seconds);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    LOG("Triggering tick on maximum duration item");
    trigger_consumable_duration_tick(eternal);
    TEST_ASSERT(eternal->data.consumable.duration_seconds == 65534, "Max duration should decrement correctly");

    // Test rapid duration consumption
    LOG("Testing rapid duration consumption");
    reset_effect_tracking();
    Item_t* rapid = create_consumable("Rapid Effect", "rapid", 10, mock_no_effect, 'R');
    rapid->data.consumable.on_duration_tick = mock_poison_tick;
    rapid->data.consumable.on_duration_end = mock_strength_end;
    rapid->data.consumable.duration_seconds = 100;

    // Consume all duration rapidly
    int ticks = 0;
    while (rapid->data.consumable.duration_seconds > 0 && ticks < 200) { // Safety limit
        trigger_consumable_duration_tick(rapid);
        ticks++;
    }

    log_msg = d_InitString();
    d_AppendString(log_msg, "Rapid consumption took ", 0);
    d_AppendInt(log_msg, ticks);
    d_AppendString(log_msg, " ticks, poison_tick_called=", 0);
    d_AppendInt(log_msg, poison_tick_called);
    d_AppendString(log_msg, ", strength_end_called=", 0);
    d_AppendInt(log_msg, strength_end_called);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(ticks == 100, "Should take exactly 100 ticks");
    TEST_ASSERT(poison_tick_called == 100, "Tick should be called 100 times");
    TEST_ASSERT(strength_end_called == 1, "End should be called once");

    destroy_item(instant);
    destroy_item(eternal);
    destroy_item(rapid);

    LOG("Duration extreme cases test completed");
    return 1;
}

/*
 * Test consumable callback edge cases and potential crashes
 */
int test_consumable_callback_edge_cases(void)
{
    LOG("Starting consumable callback edge cases test");
    reset_effect_tracking();

    // Test with NULL callbacks (should already be prevented in creation)
    LOG("Testing NULL callback handling");
    Item_t* null_callback = create_consumable("Null Callback", "null_callback", 25, NULL, 'N');
    TEST_ASSERT(null_callback == NULL, "NULL callback consumable should not be created");

    // Test with all callbacks NULL for duration effects
    LOG("Testing consumable with NULL duration callbacks");
    Item_t* partial_null = create_consumable("Partial Null", "partial_null", 30, mock_heal_effect, 'P');
    TEST_ASSERT(partial_null != NULL, "Partial null should be created");

    partial_null->data.consumable.on_duration_tick = NULL;
    partial_null->data.consumable.on_duration_end = NULL;
    partial_null->data.consumable.duration_seconds = 5;

    LOG("Using consumable with NULL duration callbacks");
    bool used = use_consumable(partial_null);
    TEST_ASSERT(used, "Should be usable despite NULL duration callbacks");

    LOG("Triggering duration operations with NULL callbacks");
    for (int i = 0; i < 10; i++) {
        trigger_consumable_duration_tick(partial_null);
        trigger_consumable_duration_end(partial_null);
    }
    TEST_ASSERT(1, "NULL callback operations should not crash");

    // Test with callbacks that might cause issues
    LOG("Testing consumable with edge case callbacks");
    Item_t* edge_case = create_consumable("Edge Case", "edge_case", 255, mock_heal_effect, 'E');
    TEST_ASSERT(edge_case != NULL, "Edge case consumable should be created");

    // Set up callbacks and test multiple uses
    edge_case->data.consumable.on_duration_tick = mock_poison_tick;
    edge_case->data.consumable.on_duration_end = mock_strength_end;
    edge_case->data.consumable.duration_seconds = 3;

    LOG("Multiple sequential uses of same consumable");
    for (int i = 0; i < 5; i++) {
        reset_effect_tracking();
        use_consumable(edge_case);

        dString_t* log_msg = d_InitString();
        d_AppendString(log_msg, "Use ", 0);
        d_AppendInt(log_msg, i+1);
        d_AppendString(log_msg, ": heal_effect_called=", 0);
        d_AppendInt(log_msg, heal_effect_called);
        LOG(log_msg->str);
        d_DestroyString(log_msg);

        TEST_ASSERT(heal_effect_called == 1, "Heal effect should be called each use");
    }

    destroy_item(partial_null);
    destroy_item(edge_case);

    LOG("Consumable callback edge cases test completed");
    return 1;
}

/*
 * Test key and lock system with extreme and malformed data
 */
int test_key_lock_extreme_cases(void)
{
    LOG("Starting key/lock extreme cases test");

    // Test locks with extreme difficulty values
    LOG("Testing locks with extreme difficulty values");
    Lock_t zero_difficulty = create_lock("Zero Lock", "zero_lock", 0, 0);
    Lock_t max_difficulty = create_lock("Max Lock", "max_lock", 255, 0);
    Lock_t jammed_max = create_lock("Jammed Max", "jammed_max", 255, 255);

    // Test keys for extreme locks
    Item_t* zero_key = create_key("Zero Key", "zero_key", zero_difficulty, 'Z');
    Item_t* max_key = create_key("Max Key", "max_key", max_difficulty, 'M');
    Item_t* jammed_key = create_key("Jammed Key", "jammed_key", jammed_max, 'J');

    TEST_ASSERT(zero_key != NULL, "Zero difficulty key should be created");
    TEST_ASSERT(max_key != NULL, "Max difficulty key should be created");
    TEST_ASSERT(jammed_key != NULL, "Jammed max key should be created");

    // Test key-lock compatibility with extreme values
    LOG("Testing key-lock compatibility with extreme values");
    bool zero_opens = can_key_open_lock(zero_key, &zero_difficulty);
    bool max_opens = can_key_open_lock(max_key, &max_difficulty);
    bool jammed_opens = can_key_open_lock(jammed_key, &jammed_max);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Lock opening results: zero=", 0);
    d_AppendString(log_msg, zero_opens ? "true" : "false", 0);
    d_AppendString(log_msg, ", max=", 0);
    d_AppendString(log_msg, max_opens ? "true" : "false", 0);
    d_AppendString(log_msg, ", jammed=", 0);
    d_AppendString(log_msg, jammed_opens ? "true" : "false", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(zero_opens, "Zero difficulty lock should open with matching key");
    TEST_ASSERT(max_opens, "Max difficulty lock should open with matching key");
    TEST_ASSERT(!jammed_opens, "Jammed lock should not open even with matching key");

    // Test cross-compatibility (wrong keys)
    LOG("Testing wrong key compatibility");
    bool wrong1 = can_key_open_lock(zero_key, &max_difficulty);
    bool wrong2 = can_key_open_lock(max_key, &zero_difficulty);
    bool wrong3 = can_key_open_lock(jammed_key, &zero_difficulty);

    TEST_ASSERT(!wrong1, "Zero key should not open max lock");
    TEST_ASSERT(!wrong2, "Max key should not open zero lock");
    TEST_ASSERT(!wrong3, "Jammed key should not open zero lock");

    // Test with corrupted/extreme lock data
    LOG("Testing locks with extreme string lengths");
    char long_name[1000];
    char long_desc[1000];
    memset(long_name, 'A', 999);
    memset(long_desc, 'B', 999);
    long_name[999] = '\0';
    long_desc[999] = '\0';

    Lock_t long_lock = create_lock(long_name, long_desc, 128, 0);
    Item_t* long_key = create_key("Long Key", "long_key", long_lock, 'L');
    TEST_ASSERT(long_key != NULL, "Key for long-named lock should be created");

    bool long_opens = can_key_open_lock(long_key, &long_lock);
    TEST_ASSERT(long_opens, "Long-named lock should work normally");

    destroy_item(zero_key);
    destroy_item(max_key);
    destroy_item(jammed_key);
    destroy_item(long_key);

    LOG("Key/lock extreme cases test completed");
    return 1;
}

/*
 * Test usage system memory safety and concurrent-like operations
 */
int test_usage_memory_safety(void)
{
    LOG("Starting usage system memory safety test");
    reset_effect_tracking();

    // Test creating and using many consumables rapidly
    LOG("Creating and using multiple consumables rapidly");
    for (int i = 0; i < 50; i++) {
        dString_t* name = d_InitString();
        dString_t* id = d_InitString();

        d_AppendString(name, "Rapid Potion ", 0);
        d_AppendInt(name, i);
        d_AppendString(id, "rapid_potion_", 0);
        d_AppendInt(id, i);

        Item_t* potion = create_consumable(name->str, id->str, i % 256, mock_heal_effect, 'R');
        TEST_ASSERT(potion != NULL, "Rapid potion should be created");

        // Use and destroy immediately
        bool used = use_consumable(potion);
        TEST_ASSERT(used, "Rapid potion should be usable");

        destroy_item(potion);
        d_DestroyString(name);
        d_DestroyString(id);

        if (i % 10 == 9) {
            dString_t* log_msg = d_InitString();
            d_AppendString(log_msg, "Completed rapid cycle ", 0);
            d_AppendInt(log_msg, i+1);
            LOG(log_msg->str);
            d_DestroyString(log_msg);
        }
    }

    // Test with many duration effects running "simultaneously"
    LOG("Testing multiple duration effects");
    Item_t* duration_items[10];
    for (int i = 0; i < 10; i++) {
        dString_t* name = d_InitString();
        dString_t* id = d_InitString();

        d_AppendString(name, "Duration Item ", 0);
        d_AppendInt(name, i);
        d_AppendString(id, "duration_item_", 0);
        d_AppendInt(id, i);

        duration_items[i] = create_consumable(name->str, id->str, 20 + i, mock_no_effect, 'D');
        TEST_ASSERT(duration_items[i] != NULL, "Duration item should be created");

        duration_items[i]->data.consumable.on_duration_tick = mock_poison_tick;
        duration_items[i]->data.consumable.on_duration_end = mock_strength_end;
        duration_items[i]->data.consumable.duration_seconds = 5 + (i % 3);

        d_DestroyString(name);
        d_DestroyString(id);
    }

    LOG("Simulating concurrent duration ticking");
    reset_effect_tracking();
    int total_ticks = 0;

    // Simulate concurrent ticking by round-robin through all items
    for (int round = 0; round < 10; round++) {
        for (int i = 0; i < 10; i++) {
            if (duration_items[i]->data.consumable.duration_seconds > 0) {
                trigger_consumable_duration_tick(duration_items[i]);
                total_ticks++;
            }
        }
    }

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Total ticks processed: ", 0);
    d_AppendInt(log_msg, total_ticks);
    d_AppendString(log_msg, ", poison_tick_called=", 0);
    d_AppendInt(log_msg, poison_tick_called);
    d_AppendString(log_msg, ", strength_end_called=", 0);
    d_AppendInt(log_msg, strength_end_called);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(poison_tick_called > 0, "Some poison ticks should have occurred");
    TEST_ASSERT(strength_end_called > 0, "Some end effects should have occurred");

    // Clean up
    for (int i = 0; i < 10; i++) {
        destroy_item(duration_items[i]);
    }

    LOG("Usage memory safety test completed");
    return 1;
}

/*
 * Test usage system with malformed and corrupted data
 */
int test_usage_corrupted_data(void)
{
    LOG("Starting usage corrupted data test");
    reset_effect_tracking();

    // Test consumable with corrupted duration data
    LOG("Testing consumable with manipulated internal data");
    Item_t* corrupt = create_consumable("Corrupt Item", "corrupt", 50, mock_heal_effect, 'C');
    TEST_ASSERT(corrupt != NULL, "Corrupt test item should be created");

    // Manually corrupt some data to test robustness
    corrupt->data.consumable.on_duration_tick = mock_poison_tick;
    corrupt->data.consumable.on_duration_end = mock_strength_end;
    corrupt->data.consumable.duration_seconds = 3;

    LOG("Using corrupted item normally first");
    bool used1 = use_consumable(corrupt);
    TEST_ASSERT(used1, "Corrupted item should be usable");

    // Now corrupt the item type to test type checking
    LOG("Testing with corrupted item type");
    ItemType_t original_type = corrupt->type;
    corrupt->type = ITEM_TYPE_WEAPON; // Wrong type

    bool used2 = use_consumable(corrupt);
    TEST_ASSERT(!used2, "Item with wrong type should not be usable as consumable");

    // Restore type and test duration with corrupted values
    corrupt->type = original_type;
    corrupt->data.consumable.duration_seconds = 65535; // Very high value

    LOG("Testing duration operations with extreme values");
    for (int i = 0; i < 5; i++) {
        trigger_consumable_duration_tick(corrupt);

        dString_t* log_msg = d_InitString();
        d_AppendString(log_msg, "Tick ", 0);
        d_AppendInt(log_msg, i+1);
        d_AppendString(log_msg, ": duration=", 0);
        d_AppendInt(log_msg, corrupt->data.consumable.duration_seconds);
        LOG(log_msg->str);
        d_DestroyString(log_msg);

        TEST_ASSERT(corrupt->data.consumable.duration_seconds <= 65535, "Duration should not overflow");
    }

    destroy_item(corrupt);

    LOG("Usage corrupted data test completed");
    return 1;
}

/*
 * Test edge cases in key comparison and lock matching
 */
int test_key_lock_comparison_edge_cases(void)
{
    LOG("Starting key/lock comparison edge cases test");

    // Test with identical names but different case
    LOG("Testing case sensitivity in lock names");
    Lock_t lower_lock = create_lock("test_lock", "lowercase lock", 50, 0);
    Lock_t upper_lock = create_lock("TEST_LOCK", "uppercase lock", 50, 0);
    Lock_t mixed_lock = create_lock("Test_Lock", "mixed case lock", 50, 0);

    Item_t* lower_key = create_key("Lower Key", "lower_key", lower_lock, 'l');
    Item_t* upper_key = create_key("Upper Key", "upper_key", upper_lock, 'u');
    Item_t* mixed_key = create_key("Mixed Key", "mixed_key", mixed_lock, 'm');

    // Test cross-case compatibility
    bool lower_upper = can_key_open_lock(lower_key, &upper_lock);
    bool upper_lower = can_key_open_lock(upper_key, &lower_lock);
    bool mixed_lower = can_key_open_lock(mixed_key, &lower_lock);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Case sensitivity results: lower->upper=", 0);
    d_AppendString(log_msg, lower_upper ? "true" : "false", 0);
    d_AppendString(log_msg, ", upper->lower=", 0);
    d_AppendString(log_msg, upper_lower ? "true" : "false", 0);
    d_AppendString(log_msg, ", mixed->lower=", 0);
    d_AppendString(log_msg, mixed_lower ? "true" : "false", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(!lower_upper, "Different case locks should not match");
    TEST_ASSERT(!upper_lower, "Different case locks should not match");
    TEST_ASSERT(!mixed_lower, "Different case locks should not match");

    // Test with empty strings
    LOG("Testing empty string locks and keys");
    Lock_t empty_lock = create_lock("", "", 0, 0);
    Item_t* empty_key = create_key("Empty Key", "empty_key", empty_lock, 'e');
    TEST_ASSERT(empty_key != NULL, "Key for empty-named lock should be created");

    bool empty_opens = can_key_open_lock(empty_key, &empty_lock);
    TEST_ASSERT(empty_opens, "Empty-named lock should open with matching key");

    // Test with very similar but different names
    LOG("Testing similar lock names");
    Lock_t similar1 = create_lock("lock_123", "similar lock 1", 75, 0);
    Lock_t similar2 = create_lock("lock_124", "similar lock 2", 75, 0);

    Item_t* similar1_key = create_key("Similar Key 1", "similar1_key", similar1, '1');
    Item_t* similar2_key = create_key("Similar Key 2", "similar2_key", similar2, '2');

    bool similar_cross = can_key_open_lock(similar1_key, &similar2);
    TEST_ASSERT(!similar_cross, "Similar but different locks should not match");

    destroy_item(lower_key);
    destroy_item(upper_key);
    destroy_item(mixed_key);
    destroy_item(empty_key);
    destroy_item(similar1_key);
    destroy_item(similar2_key);

    LOG("Key/lock comparison edge cases test completed");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Usage & Effects System Tests");

    // Run usage system tests
    RUN_TEST(test_use_consumable);
    RUN_TEST(test_consumable_duration_effects);
    RUN_TEST(test_duration_end_trigger);
    RUN_TEST(test_key_lock_interaction);
    RUN_TEST(test_usage_system_integration);
    RUN_TEST(test_usage_edge_cases);

    RUN_TEST(test_consumable_extreme_values);
    RUN_TEST(test_duration_extreme_cases);
    RUN_TEST(test_consumable_callback_edge_cases);
    RUN_TEST(test_key_lock_extreme_cases);
    RUN_TEST(test_usage_memory_safety);
    RUN_TEST(test_usage_corrupted_data);
    RUN_TEST(test_key_lock_comparison_edge_cases);

    TEST_SUITE_END();
}
