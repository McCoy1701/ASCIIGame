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
    
    // Rate limit to 1 log per 2 seconds to reduce spam during rapid testing
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 
                      1, 2.0, "EFFECT TRACE: mock_heal_effect triggered with value: %d", value);
}

void mock_poison_tick(uint8_t value) {
    poison_tick_called++;
    last_effect_value = value;
    
    // Rate limit poison ticks even more aggressively since they can happen frequently
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 3.0, "EFFECT TRACE: mock_poison_tick triggered with value: %d", value);
}

void mock_strength_end(uint8_t value) {
    strength_end_called++;
    last_effect_value = value;
    
    // End effects are rarer, so allow more frequent logging
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 1.0, "EFFECT TRACE: mock_strength_end triggered with value: %d", value);
}

void mock_no_effect(uint8_t value) {
    (void)value;
    
    // This should rarely need logging, so very aggressive rate limiting
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 5.0, "EFFECT TRACE: mock_no_effect (do-nothing callback) triggered.");
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

    d_LogInfo("Creating and destroying 50 consumables in a rapid loop...");
    for (int i = 0; i < 50; i++) {
        // Rate-limited progress reporting every 10 iterations
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 1.0, "Memory safety test progress: %d/50 consumables processed", i + 1);
        
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
// ADVANCED ITEM USAGE TESTS
// =============================================================================

int test_consumable_stacking_behavior_during_usage(void) {
    d_LogWarning("BUG HUNT: Verifying consumable behavior when used from stacked inventory slots.");
    dLogContext_t* ctx = d_PushLogContext("StackingUsage");
    reset_effect_tracking();
    
    // Create inventory and multiple identical consumables
    Inventory_t* inventory = create_inventory(10);
    Item_t* health_potion = create_consumable("Health Potion", "health_potion", 30, mock_heal_effect, 'H');
    
    TEST_ASSERT(inventory != NULL, "Inventory should be created");
    TEST_ASSERT(health_potion != NULL, "Health potion should be created");
    
    d_LogDebug("Adding 5 health potions to inventory...");
    bool added = add_item_to_inventory(inventory, health_potion, 5);
    TEST_ASSERT(added, "Should be able to add 5 potions to inventory");
    
    // Find the stacked slot
    Inventory_slot_t* slot = find_item_in_inventory(inventory, "health_potion");
    TEST_ASSERT(slot != NULL, "Should find health potion in inventory");
    TEST_ASSERT(slot->quantity == 5, "Slot should contain 5 potions");
    
    d_LogDebug("Using consumable from stacked slot...");
    bool used = use_consumable(&slot->item);
    TEST_ASSERT(used, "Should be able to use stacked consumable");
    TEST_ASSERT(heal_effect_called == 1, "Heal effect should be called once");
    TEST_ASSERT(last_effect_value == 30, "Effect should receive correct value");
    
    // Note: In a real system, inventory management would handle quantity reduction
    d_LogDebug("Verifying item data integrity after use...");
    TEST_ASSERT(slot->item.data.consumable.value == 30, "Item value should remain unchanged");
    TEST_ASSERT(slot->quantity == 5, "Quantity unchanged (handled separately by inventory system)");
    
    destroy_inventory(inventory);
    destroy_item(health_potion);
    d_PopLogContext(ctx);
    return 1;
}

int test_key_lock_concurrent_access_simulation(void) {
    d_LogWarning("BUG HUNT: Simulating rapid key/lock checks for race condition detection.");
    dLogContext_t* ctx = d_PushLogContext("ConcurrentKeyLock");
    
    d_LogDebug("Creating master key system with multiple locks...");
    Lock_t master_lock = create_lock("master_lock", "A master lock system", 150, 0);
    Lock_t decoy_lock1 = create_lock("decoy_lock_1", "First decoy lock", 75, 0);
    Lock_t decoy_lock2 = create_lock("decoy_lock_2", "Second decoy lock", 125, 0);
    
    Item_t* master_key = create_key("Master Key", "master_key", master_lock, 'M');
    Item_t* wrong_key = create_key("Wrong Key", "wrong_key", decoy_lock1, 'W');
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 
                      1, 5.0, "Performing %d rapid key/lock verification cycles...", 1000);
    
    int correct_matches = 0;
    int incorrect_rejections = 0;
    int jammed_failures = 0; // Track expected failures due to jamming
    
    for (int i = 0; i < 1000; i++) {
        // Simulate lock state changes (jamming/unjamming) BEFORE testing
        if (i % 100 == 0) {
            master_lock.jammed_seconds = (i % 200 == 0) ? 5 : 0;
            if (master_lock.jammed_seconds > 0) {
                d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                                  1, 2.0, "Lock jammed at iteration %d", i);
            }
        }
        
        // Test correct key/lock pair (accounting for jamming)
        if (can_key_open_lock(master_key, &master_lock)) {
            correct_matches++;
        } else if (master_lock.jammed_seconds > 0) {
            jammed_failures++; // Expected failure due to jamming
        }
        
        // Test incorrect key/lock pairs (these should always fail)
        if (!can_key_open_lock(wrong_key, &master_lock)) {
            incorrect_rejections++;
        }
        if (!can_key_open_lock(master_key, &decoy_lock2)) {
            incorrect_rejections++;
        }
    }
    
    // Calculate expected correct matches (total - jammed periods)
    int expected_correct = 1000 - jammed_failures;
    
    TEST_ASSERT(correct_matches == expected_correct, "All non-jammed correct key/lock pairs should match");
    TEST_ASSERT(incorrect_rejections == 2000, "All incorrect pairs should be rejected");
    TEST_ASSERT(jammed_failures > 0, "Should have encountered some jammed lock scenarios");
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO,
                      1, 1.0, "Performance results: %d correct, %d rejections, %d jammed failures", 
                      correct_matches, incorrect_rejections, jammed_failures);
    
    destroy_item(master_key);
    destroy_item(wrong_key);
    destroy_lock(&master_lock);
    destroy_lock(&decoy_lock1);
    destroy_lock(&decoy_lock2);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_consumable_callback_exception_handling(void) {
    d_LogWarning("BUG HUNT: Testing system stability when consumable callbacks behave unexpectedly.");
    dLogContext_t* ctx = d_PushLogContext("CallbackExceptions");
    reset_effect_tracking();
    
    // Create a consumable with a callback that modifies global state
    Item_t* chaos_potion = create_consumable("Chaos Potion", "chaos_potion", 100, mock_heal_effect, 'C');
    TEST_ASSERT(chaos_potion != NULL, "Chaos potion should be created");
    
    d_LogDebug("Testing normal callback behavior...");
    bool normal_use = use_consumable(chaos_potion);
    TEST_ASSERT(normal_use, "Normal use should succeed");
    TEST_ASSERT(heal_effect_called == 1, "Callback should be executed");
    
    d_LogDebug("Testing multiple rapid callback executions...");
    reset_effect_tracking();
    for (int i = 0; i < 10; i++) {
        use_consumable(chaos_potion);
    }
    TEST_ASSERT(heal_effect_called == 10, "All callbacks should execute successfully");
    
    d_LogDebug("Testing callback with maximum value edge case...");
    chaos_potion->data.consumable.value = 255;
    reset_effect_tracking();
    use_consumable(chaos_potion);
    TEST_ASSERT(last_effect_value == 255, "Maximum value should be passed correctly");
    
    d_LogDebug("Testing duration callback chain integrity...");
    chaos_potion->data.consumable.on_duration_tick = mock_poison_tick;
    chaos_potion->data.consumable.on_duration_end = mock_strength_end;
    chaos_potion->data.consumable.duration_seconds = 3;
    
    reset_effect_tracking();
    
    // Execute full duration chain
    trigger_consumable_duration_tick(chaos_potion); // 3->2
    trigger_consumable_duration_tick(chaos_potion); // 2->1  
    trigger_consumable_duration_tick(chaos_potion); // 1->0, triggers end
    
    TEST_ASSERT(poison_tick_called == 3, "Duration tick should be called 3 times");
    TEST_ASSERT(strength_end_called == 1, "End effect should be called once");
    TEST_ASSERT(chaos_potion->data.consumable.duration_seconds == 0, "Duration should be zeroed");
    
    d_LogDebug("Verifying system state after complex callback chain...");
    TEST_ASSERT(chaos_potion->data.consumable.value == 255, "Item value should remain stable");
    
    destroy_item(chaos_potion);
    d_PopLogContext(ctx);
    return 1;
}
// =============================================================================
// MISSING TEST FUNCTIONS - COMPLETING THE ARCHITECTURAL VISION
// =============================================================================

int test_duration_extreme_cases(void) {
    d_LogWarning("BUG HUNT: Testing duration system with extreme edge cases and boundary conditions.");
    dLogContext_t* ctx = d_PushLogContext("DurationExtremes");
    reset_effect_tracking();
    
    d_LogDebug("Testing consumable with zero duration...");
    Item_t* instant_potion = create_consumable("Instant Potion", "instant_potion", 75, mock_heal_effect, 'I');
    TEST_ASSERT(instant_potion != NULL, "Instant potion should be created");
    
    instant_potion->data.consumable.on_duration_tick = mock_poison_tick;
    instant_potion->data.consumable.on_duration_end = mock_strength_end;
    instant_potion->data.consumable.duration_seconds = 0; // Already expired
    
    d_LogDebug("Triggering tick on already-expired effect...");
    trigger_consumable_duration_tick(instant_potion);
    TEST_ASSERT(poison_tick_called == 0, "Expired effect should not tick");
    TEST_ASSERT(strength_end_called == 0, "Expired effect should not trigger end");
    
    d_LogDebug("Testing consumable with maximum duration...");
    Item_t* eternal_potion = create_consumable("Eternal Potion", "eternal_potion", 200, mock_no_effect, 'E');
    eternal_potion->data.consumable.on_duration_tick = mock_poison_tick;
    eternal_potion->data.consumable.duration_seconds = 65535; // Maximum uint16_t
    
    reset_effect_tracking();
    trigger_consumable_duration_tick(eternal_potion);
    TEST_ASSERT(poison_tick_called == 1, "Maximum duration should still tick");
    TEST_ASSERT(eternal_potion->data.consumable.duration_seconds == 65534, "Duration should decrement from maximum");
    
    d_LogDebug("Testing rapid duration decrements...");
    eternal_potion->data.consumable.duration_seconds = 5;
    reset_effect_tracking();
    
    // Rapidly tick down to test boundary transition
    for (int i = 0; i < 10; i++) {
        trigger_consumable_duration_tick(eternal_potion);
    }
    
    TEST_ASSERT(poison_tick_called == 5, "Should tick exactly 5 times before expiring");
    TEST_ASSERT(strength_end_called == 0, "End effect not set for this test");
    TEST_ASSERT(eternal_potion->data.consumable.duration_seconds == 0, "Duration should be fully depleted");
    
    destroy_item(instant_potion);
    destroy_item(eternal_potion);
    d_PopLogContext(ctx);
    return 1;
}

int test_consumable_callback_edge_cases(void) {
    d_LogWarning("BUG HUNT: Testing consumable callbacks under unusual and edge case scenarios.");
    dLogContext_t* ctx = d_PushLogContext("CallbackEdgeCases");
    reset_effect_tracking();
    
    d_LogDebug("Testing consumable with all callback types set...");
    Item_t* complex_potion = create_consumable("Complex Potion", "complex_potion", 42, mock_heal_effect, 'X');
    TEST_ASSERT(complex_potion != NULL, "Complex potion should be created");
    
    // Set all possible callbacks
    complex_potion->data.consumable.on_duration_tick = mock_poison_tick;
    complex_potion->data.consumable.on_duration_end = mock_strength_end;
    complex_potion->data.consumable.duration_seconds = 2;
    
    d_LogDebug("Using consumable with full callback suite...");
    bool used = use_consumable(complex_potion);
    TEST_ASSERT(used, "Complex potion should be usable");
    TEST_ASSERT(heal_effect_called == 1, "Primary effect should trigger");
    TEST_ASSERT(last_effect_value == 42, "Correct value should be passed");
    
    d_LogDebug("Triggering duration chain with all callbacks...");
    reset_effect_tracking();
    
    trigger_consumable_duration_tick(complex_potion); // 2->1
    TEST_ASSERT(poison_tick_called == 1, "First tick should execute");
    
    trigger_consumable_duration_tick(complex_potion); // 1->0, triggers end
    TEST_ASSERT(poison_tick_called == 2, "Second tick should execute");
    TEST_ASSERT(strength_end_called == 1, "End effect should trigger");
    
    d_LogDebug("Testing callback value consistency across different triggers...");
    TEST_ASSERT(last_effect_value == 42, "All callbacks should receive same value");
    
    d_LogDebug("Testing consumable with only end callback (no tick callback)...");
    Item_t* end_only_potion = create_consumable("End Only", "end_only", 99, mock_no_effect, 'N');
    end_only_potion->data.consumable.on_duration_end = mock_strength_end;
    end_only_potion->data.consumable.duration_seconds = 1;
    
    reset_effect_tracking();
    trigger_consumable_duration_tick(end_only_potion); // Should only trigger end, not tick
    TEST_ASSERT(poison_tick_called == 0, "No tick callback should mean no tick");
    TEST_ASSERT(strength_end_called == 1, "End callback should still trigger");
    
    destroy_item(complex_potion);
    destroy_item(end_only_potion);
    d_PopLogContext(ctx);
    return 1;
}

int test_key_lock_extreme_cases(void) {
    d_LogWarning("BUG HUNT: Testing key/lock system with extreme values and edge conditions.");
    dLogContext_t* ctx = d_PushLogContext("KeyLockExtremes");
    
    d_LogDebug("Testing locks with extreme difficulty values...");
    Lock_t impossible_lock = create_lock("impossible_lock", "An impossibly difficult lock", 255, 0);
    Lock_t trivial_lock = create_lock("trivial_lock", "A trivial lock", 0, 0);
    
    Item_t* master_key = create_key("Master Key", "master_key", impossible_lock, 'M');
    Item_t* simple_key = create_key("Simple Key", "simple_key", trivial_lock, 'S');
    
    TEST_ASSERT(can_key_open_lock(master_key, &impossible_lock), "Master key should open impossible lock");
    TEST_ASSERT(can_key_open_lock(simple_key, &trivial_lock), "Simple key should open trivial lock");
    TEST_ASSERT(!can_key_open_lock(simple_key, &impossible_lock), "Simple key should NOT open impossible lock");
    
    d_LogDebug("Testing locks with extreme jam times...");
    Lock_t super_jammed_lock = create_lock("super_jammed", "Super jammed lock", 100, 255);
    TEST_ASSERT(!can_key_open_lock(master_key, &super_jammed_lock), "No key should open super jammed lock");
    
    // Test unjamming
    super_jammed_lock.jammed_seconds = 0;
    TEST_ASSERT(!can_key_open_lock(master_key, &super_jammed_lock), "Key still shouldn't work with wrong lock");
    
    d_LogDebug("Testing very long lock/key names for buffer safety...");
    dString_t* long_name = d_InitString();
    for (int i = 0; i < 500; i++) {
        d_AppendChar(long_name, 'A' + (i % 26));
    }
    
    Lock_t long_named_lock = create_lock(d_PeekString(long_name), "A lock with a very long name", 50, 0);
    Item_t* long_named_key = create_key("Long Named Key", "long_key", long_named_lock, 'L');
    
    TEST_ASSERT(can_key_open_lock(long_named_key, &long_named_lock), "Long-named key should work with its lock");
    
    d_LogDebug("Testing rapid jam/unjam cycles...");
    Lock_t cycling_lock = create_lock("cycling_lock", "A lock that jams and unjams", 75, 0);
    Item_t* cycling_key = create_key("Cycling Key", "cycling_key", cycling_lock, 'C');
    
    int successful_opens = 0;
    int failed_opens = 0;
    
    for (int i = 0; i < 100; i++) {
        cycling_lock.jammed_seconds = (i % 3 == 0) ? 10 : 0; // Jam every 3rd iteration
        
        if (can_key_open_lock(cycling_key, &cycling_lock)) {
            successful_opens++;
        } else {
            failed_opens++;
        }
    }
    
    TEST_ASSERT(successful_opens > 0, "Should have some successful opens");
    TEST_ASSERT(failed_opens > 0, "Should have some failed opens due to jamming");
    TEST_ASSERT(successful_opens + failed_opens == 100, "All attempts should be accounted for");
    
    d_LogDebugF("Jam/unjam test results: %d successful, %d failed", successful_opens, failed_opens);
    
    destroy_item(master_key);
    destroy_item(simple_key);
    destroy_item(long_named_key);
    destroy_item(cycling_key);
    destroy_lock(&impossible_lock);
    destroy_lock(&trivial_lock);
    destroy_lock(&super_jammed_lock);
    destroy_lock(&long_named_lock);
    destroy_lock(&cycling_lock);
    d_DestroyString(long_name);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_usage_system_integration(void) {
    d_LogInfo("VERIFICATION: Integration testing of consumables, keys, and inventory interactions.");
    dLogContext_t* ctx = d_PushLogContext("SystemIntegration");
    reset_effect_tracking();
    
    d_LogDebug("Creating integrated test environment...");
    Inventory_t* player_inventory = create_inventory(20);
    
    // Create various item types
    Item_t* health_potion = create_consumable("Health Potion", "health_potion", 50, mock_heal_effect, 'H');
    Item_t* mana_potion = create_consumable("Mana Potion", "mana_potion", 75, mock_poison_tick, 'M');
    
    Lock_t chest_lock = create_lock("treasure_chest", "A treasure chest lock", 100, 0);
    Item_t* chest_key = create_key("Treasure Key", "treasure_key", chest_lock, 'K');
    
    d_LogDebug("Adding items to inventory...");
    TEST_ASSERT(add_item_to_inventory(player_inventory, health_potion, 3), "Should add health potions");
    TEST_ASSERT(add_item_to_inventory(player_inventory, mana_potion, 2), "Should add mana potions");
    TEST_ASSERT(add_item_to_inventory(player_inventory, chest_key, 1), "Should add chest key");
    
    d_LogDebug("Testing consumable usage from inventory...");
    Inventory_slot_t* health_slot = find_item_in_inventory(player_inventory, "health_potion");
    TEST_ASSERT(health_slot != NULL, "Should find health potion in inventory");
    TEST_ASSERT(health_slot->quantity == 3, "Should have 3 health potions");
    
    bool used_health = use_consumable(&health_slot->item);
    TEST_ASSERT(used_health, "Should be able to use health potion from inventory");
    TEST_ASSERT(heal_effect_called == 1, "Health effect should trigger");
    
    d_LogDebug("Testing key usage from inventory...");
    Inventory_slot_t* key_slot = find_item_in_inventory(player_inventory, "treasure_key");
    TEST_ASSERT(key_slot != NULL, "Should find key in inventory");
    TEST_ASSERT(can_key_open_lock(&key_slot->item, &chest_lock), "Key from inventory should work");
    
    d_LogDebug("Testing duration effects while items are in inventory...");
    Inventory_slot_t* mana_slot = find_item_in_inventory(player_inventory, "mana_potion");
    mana_slot->item.data.consumable.on_duration_tick = mock_poison_tick;
    mana_slot->item.data.consumable.duration_seconds = 2;
    
    reset_effect_tracking();
    trigger_consumable_duration_tick(&mana_slot->item);
    TEST_ASSERT(poison_tick_called == 1, "Duration effect should work on inventory item");
    
    d_LogDebug("Testing system state after integrated operations...");
    TEST_ASSERT(get_inventory_free_slots(player_inventory) == 17, "Should have correct free slots");
    TEST_ASSERT(!is_inventory_full(player_inventory), "Inventory should not be full");
    
    destroy_inventory(player_inventory);
    destroy_item(health_potion);
    destroy_item(mana_potion);
    destroy_item(chest_key);
    destroy_lock(&chest_lock);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_usage_corrupted_data(void) {
    d_LogWarning("BUG HUNT: Testing system resilience against corrupted or invalid item data.");
    dLogContext_t* ctx = d_PushLogContext("CorruptedData");
    reset_effect_tracking();
    
    d_LogDebug("Testing consumable with corrupted callback pointer...");
    Item_t* corrupt_potion = create_consumable("Corrupt Potion", "corrupt_potion", 25, mock_heal_effect, 'R');
    TEST_ASSERT(corrupt_potion != NULL, "Corrupt potion should be created initially");
    
    // Simulate data corruption
    corrupt_potion->data.consumable.on_consume = NULL; // Corrupted callback
    
    bool corrupt_used = use_consumable(corrupt_potion);
    TEST_ASSERT(!corrupt_used, "Corrupted consumable should fail safely");
    TEST_ASSERT(heal_effect_called == 0, "Corrupted callback should not be called");
    
    d_LogDebug("Testing key with corrupted lock data...");
    Lock_t valid_lock = create_lock("valid_lock", "A valid lock", 50, 0);
    Item_t* corrupt_key = create_key("Corrupt Key", "corrupt_key", valid_lock, 'C');
    
    // Simulate lock name corruption
    if (corrupt_key && corrupt_key->data.key.lock.name) {
        d_ClearString(corrupt_key->data.key.lock.name); // Empty the lock name
    }
    
    bool corrupt_opens = can_key_open_lock(corrupt_key, &valid_lock);
    TEST_ASSERT(!corrupt_opens, "Key with corrupted lock data should fail safely");
    
    d_LogDebug("Testing consumable with invalid duration values...");
    Item_t* duration_potion = create_consumable("Duration Potion", "duration_potion", 100, mock_no_effect, 'D');
    
    // Set up duration effects then corrupt the duration
    duration_potion->data.consumable.on_duration_tick = mock_poison_tick;
    duration_potion->data.consumable.on_duration_end = mock_strength_end;
    duration_potion->data.consumable.duration_seconds = 65535; // Max value
    
    reset_effect_tracking();
    
    // Manually corrupt duration to trigger underflow
    duration_potion->data.consumable.duration_seconds = 1;
    trigger_consumable_duration_tick(duration_potion); // Should handle 1->0 transition safely
    
    TEST_ASSERT(poison_tick_called == 1, "Duration tick should handle boundary correctly");
    TEST_ASSERT(strength_end_called == 1, "End effect should trigger on boundary");
    TEST_ASSERT(duration_potion->data.consumable.duration_seconds == 0, "Duration should be safely zeroed");
    
    d_LogDebug("Testing with completely NULL item structures...");
    bool null_consumable_used = use_consumable(NULL);
    bool null_key_opens = can_key_open_lock(NULL, &valid_lock);
    
    TEST_ASSERT(!null_consumable_used, "NULL consumable should fail safely");
    TEST_ASSERT(!null_key_opens, "NULL key should fail safely");
    
    destroy_item(corrupt_potion);
    destroy_item(corrupt_key);
    destroy_item(duration_potion);
    destroy_lock(&valid_lock);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_key_lock_comparison_edge_cases(void) {
    d_LogWarning("BUG HUNT: Testing edge cases in key/lock name comparison and matching logic.");
    dLogContext_t* ctx = d_PushLogContext("ComparisonEdgeCases");
    
    d_LogDebug("Testing keys and locks with identical names but different descriptions...");
    Lock_t lock_a = create_lock("shared_name", "First lock with shared name", 50, 0);
    Lock_t lock_b = create_lock("shared_name", "Second lock with shared name", 75, 0);
    
    Item_t* key_a = create_key("Key A", "key_a", lock_a, 'A');
    Item_t* key_b = create_key("Key B", "key_b", lock_b, 'B');
    
    // Both keys should work with both locks since names match
    TEST_ASSERT(can_key_open_lock(key_a, &lock_a), "Key A should open Lock A");
    TEST_ASSERT(can_key_open_lock(key_a, &lock_b), "Key A should open Lock B (same name)");
    TEST_ASSERT(can_key_open_lock(key_b, &lock_a), "Key B should open Lock A (same name)");
    TEST_ASSERT(can_key_open_lock(key_b, &lock_b), "Key B should open Lock B");
    
    d_LogDebug("Testing case sensitivity in lock names...");
    Lock_t lower_lock = create_lock("lowercase", "A lowercase lock name", 25, 0);
    Lock_t upper_lock = create_lock("LOWERCASE", "An uppercase lock name", 25, 0);
    
    Item_t* lower_key = create_key("Lower Key", "lower_key", lower_lock, 'l');
    
    // Current implementation should be case-sensitive
    TEST_ASSERT(can_key_open_lock(lower_key, &lower_lock), "Key should open matching case lock");
    TEST_ASSERT(!can_key_open_lock(lower_key, &upper_lock), "Key should NOT open different case lock");
    
    d_LogDebug("Testing locks with empty or whitespace names...");
    Lock_t empty_lock = create_lock("", "Lock with empty name", 30, 0);
    Lock_t space_lock = create_lock(" ", "Lock with space name", 30, 0);
    Lock_t whitespace_lock = create_lock("   ", "Lock with whitespace name", 30, 0);
    
    Item_t* empty_key = create_key("Empty Key", "empty_key", empty_lock, 'e');
    Item_t* space_key = create_key("Space Key", "space_key", space_lock, 's');
    
    TEST_ASSERT(can_key_open_lock(empty_key, &empty_lock), "Empty key should open empty lock");
    TEST_ASSERT(!can_key_open_lock(empty_key, &space_lock), "Empty key should NOT open space lock");
    TEST_ASSERT(!can_key_open_lock(space_key, &whitespace_lock), "Space key should NOT open whitespace lock");
    
    d_LogDebug("Testing locks with special characters in names...");
    Lock_t special_lock = create_lock("lock@#$%^&*()", "Lock with special characters", 40, 0);
    Item_t* special_key = create_key("Special Key", "special_key", special_lock, '@');
    
    TEST_ASSERT(can_key_open_lock(special_key, &special_lock), "Special character key should work");
    
    d_LogDebug("Testing very long vs very short lock names...");
    dString_t* very_long_name = d_InitString();
    for (int i = 0; i < 1000; i++) {
        d_AppendString(very_long_name, "VeryLongLockName", 0);
    }
    
    Lock_t long_lock = create_lock(d_PeekString(very_long_name), "Very long lock name", 60, 0);
    Lock_t short_lock = create_lock("x", "Very short lock name", 60, 0);
    
    Item_t* long_key = create_key("Long Key", "long_key", long_lock, 'L');
    Item_t* short_key = create_key("Short Key", "short_key", short_lock, 'x');
    
    TEST_ASSERT(can_key_open_lock(long_key, &long_lock), "Very long key should work");
    TEST_ASSERT(can_key_open_lock(short_key, &short_lock), "Very short key should work");
    TEST_ASSERT(!can_key_open_lock(long_key, &short_lock), "Long key should NOT open short lock");
    
    // Cleanup
    destroy_item(key_a);
    destroy_item(key_b);
    destroy_item(lower_key);
    destroy_item(empty_key);
    destroy_item(space_key);
    destroy_item(special_key);
    destroy_item(long_key);
    destroy_item(short_key);
    
    destroy_lock(&lock_a);
    destroy_lock(&lock_b);
    destroy_lock(&lower_lock);
    destroy_lock(&upper_lock);
    destroy_lock(&empty_lock);
    destroy_lock(&space_lock);
    destroy_lock(&whitespace_lock);
    destroy_lock(&special_lock);
    destroy_lock(&long_lock);
    destroy_lock(&short_lock);
    
    d_DestroyString(very_long_name);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_duration_end_trigger(void) {
    d_LogInfo("VERIFICATION: Consumable duration end trigger behavior and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("DurationEndTrigger");
    d_LogContext_EnableTiming(ctx);
    reset_effect_tracking();
    
    d_LogDebug("Testing standard duration end trigger sequence...");
    Item_t* timed_potion = create_consumable("Timed Potion", "timed_potion", 88, mock_no_effect, 'T');
    TEST_ASSERT(timed_potion != NULL, "Timed potion should be created");
    
    // Set up duration callbacks
    timed_potion->data.consumable.on_duration_tick = mock_poison_tick;
    timed_potion->data.consumable.on_duration_end = mock_strength_end;
    timed_potion->data.consumable.duration_seconds = 3;
    
    d_LogDebug("Executing complete duration cycle with end trigger...");
    
    // Tick 1: 3 -> 2 (no end trigger)
    trigger_consumable_duration_tick(timed_potion);
    TEST_ASSERT(poison_tick_called == 1, "First tick should execute");
    TEST_ASSERT(strength_end_called == 0, "End should not trigger yet");
    TEST_ASSERT(timed_potion->data.consumable.duration_seconds == 2, "Duration should be 2");
    
    // Tick 2: 2 -> 1 (no end trigger)
    trigger_consumable_duration_tick(timed_potion);
    TEST_ASSERT(poison_tick_called == 2, "Second tick should execute");
    TEST_ASSERT(strength_end_called == 0, "End should still not trigger");
    TEST_ASSERT(timed_potion->data.consumable.duration_seconds == 1, "Duration should be 1");
    
    // Tick 3: 1 -> 0 (END TRIGGER!)
    d_LogDebug("Triggering final tick that should cause end effect...");
    trigger_consumable_duration_tick(timed_potion);
    TEST_ASSERT(poison_tick_called == 3, "Final tick should execute");
    TEST_ASSERT(strength_end_called == 1, "End effect should trigger exactly once");
    TEST_ASSERT(timed_potion->data.consumable.duration_seconds == 0, "Duration should be 0");
    TEST_ASSERT(last_effect_value == 88, "End effect should receive correct value");
    
    d_LogDebug("Verifying no additional triggers after duration ends...");
    trigger_consumable_duration_tick(timed_potion);
    TEST_ASSERT(poison_tick_called == 3, "No additional ticks after end");
    TEST_ASSERT(strength_end_called == 1, "No additional end triggers");
    
    d_LogDebug("Testing direct end trigger without duration ticks...");
    Item_t* instant_end_potion = create_consumable("Instant End", "instant_end", 123, mock_heal_effect, 'E');
    instant_end_potion->data.consumable.on_duration_end = mock_strength_end;
    instant_end_potion->data.consumable.duration_seconds = 0; // Already at end
    
    reset_effect_tracking();
    trigger_consumable_duration_end(instant_end_potion);
    TEST_ASSERT(strength_end_called == 1, "Direct end trigger should work");
    TEST_ASSERT(last_effect_value == 123, "Direct end should receive correct value");
    TEST_ASSERT(instant_end_potion->data.consumable.duration_seconds == 0, "Duration should remain 0");
    
    d_LogDebug("Testing end trigger with NULL callback safety...");
    Item_t* no_end_potion = create_consumable("No End Effect", "no_end", 45, mock_heal_effect, 'N');
    no_end_potion->data.consumable.on_duration_end = NULL; // No end callback
    no_end_potion->data.consumable.duration_seconds = 1;
    
    reset_effect_tracking();
    trigger_consumable_duration_tick(no_end_potion); // Should handle NULL end callback gracefully
    TEST_ASSERT(no_end_potion->data.consumable.duration_seconds == 0, "Duration should still decrement");
    TEST_ASSERT(strength_end_called == 0, "NULL end callback should not crash");
    
    d_LogDebug("Testing rapid end trigger scenarios...");
    Item_t* rapid_potions[5];
    for (int i = 0; i < 5; i++) {
        rapid_potions[i] = create_consumable("Rapid Potion", "rapid", 10 + i, mock_no_effect, 'R');
        rapid_potions[i]->data.consumable.on_duration_end = mock_strength_end;
        rapid_potions[i]->data.consumable.duration_seconds = 1; // All end after 1 tick
    }
    
    reset_effect_tracking();
    d_LogDebug("Triggering 5 rapid end effects simultaneously...");
    for (int i = 0; i < 5; i++) {
        trigger_consumable_duration_tick(rapid_potions[i]);
    }
    
    TEST_ASSERT(strength_end_called == 5, "All 5 end effects should trigger");
    
    // Verify each potion ended properly
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT(rapid_potions[i]->data.consumable.duration_seconds == 0, "Each potion should have 0 duration");
    }
    
    d_LogDebug("Testing end trigger value propagation consistency...");
    Item_t* value_test_potion = create_consumable("Value Test", "value_test", 255, mock_no_effect, 'V');
    value_test_potion->data.consumable.on_duration_end = mock_strength_end;
    
    reset_effect_tracking();
    trigger_consumable_duration_end(value_test_potion);
    TEST_ASSERT(last_effect_value == 255, "Maximum value should propagate correctly to end trigger");
    
    // Cleanup
    destroy_item(timed_potion);
    destroy_item(instant_end_potion);
    destroy_item(no_end_potion);
    destroy_item(value_test_potion);
    for (int i = 0; i < 5; i++) {
        destroy_item(rapid_potions[i]);
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
    RUN_TEST(test_duration_end_trigger);
    RUN_TEST(test_key_lock_interaction);
    RUN_TEST(test_usage_system_integration);
    RUN_TEST(test_consumable_extreme_values);
    RUN_TEST(test_duration_extreme_cases);
    RUN_TEST(test_consumable_callback_edge_cases);
    RUN_TEST(test_key_lock_extreme_cases);
    RUN_TEST(test_usage_memory_safety);
    RUN_TEST(test_usage_corrupted_data);
    RUN_TEST(test_key_lock_comparison_edge_cases);

    RUN_TEST(test_consumable_stacking_behavior_during_usage);
    RUN_TEST(test_key_lock_concurrent_access_simulation);
    RUN_TEST(test_consumable_callback_exception_handling);
    

    TEST_SUITE_END();

    // Clean up the logger
    d_DestroyLogger(d_GetGlobalLogger());
    return 0;
}
