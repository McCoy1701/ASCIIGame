// ASCIIGame/tests/items/test_items_usage.c
// Test file for item usage and effects system

#include "../../include/tests.h"
#include "../../include/items.h"
#include "../../include/structs.h"
#include "../../include/defs.h"
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
    Lock_t chest_lock = {"chest_lock", "A sturdy chest lock", 100, 0};
    Lock_t door_lock = {"door_lock", "A simple door lock", 50, 0};
    Lock_t jammed_lock = {"jammed_lock", "A jammed lock", 200, 5}; // Jammed for 5 seconds

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
    Lock_t magic_lock = {"magic_lock", "A magical lock", 255, 0};
    Item_t* magic_key = create_key("Magic Key", "magic_key", magic_lock, 'M');

    bool magic_works = can_key_open_lock(magic_key, &magic_lock);
    TEST_ASSERT(magic_works, "Magic key should open magic lock");

    // Test key doesn't work on different lock
    Lock_t different_lock = {"different_lock", "A different lock", 100, 0};
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
    Lock_t unpickable_lock = {"unpickable", "Cannot be picked", 255, 0};
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

    TEST_SUITE_END();
}
