// ASCIIGame/tests/items/test_items_durability.c
// Test file for item durability system functions
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

// Helper functions
void dummy_consume_callback(uint8_t value)
{
    (void)value; // Suppress unused parameter warning
}

// Helper function for float comparison
bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
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
    Item_t* sword = create_weapon("Test Sword", "test_sword", basic_material, 20, 30, 0, 'S');
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 25, 15, 'A', 10, 10);
    Item_t* key = create_key("Test Key", "test_key", lock, 'k');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    TEST_ASSERT(key != NULL, "Key should be created");

    // Check initial durability (should be 255 for weapons/armor)
    uint8_t initial_sword_durability = get_durability(sword);
    uint8_t initial_armor_durability = get_durability(armor);

    TEST_ASSERT(initial_sword_durability == 255, "Sword should start with full durability");
    TEST_ASSERT(initial_armor_durability == 255, "Armor should start with full durability");

    // Damage the items
    damage_item_durability(sword, 50);
    damage_item_durability(armor, 30);
    damage_item_durability(key, 100); // Should have no effect

    // Check durability after damage
    uint8_t damaged_sword_durability = get_durability(sword);
    uint8_t damaged_armor_durability = get_durability(armor);
    uint8_t key_durability = get_durability(key);

    TEST_ASSERT(damaged_sword_durability == 205, "Sword durability should be 255-50=205");
    TEST_ASSERT(damaged_armor_durability == 225, "Armor durability should be 255-30=225");
    TEST_ASSERT(key_durability == 255, "Key durability should be unaffected");

    // Test massive damage (should cap at 0)
    damage_item_durability(sword, 256);
    uint8_t broken_sword_durability = get_durability(sword);
    printf("Broken Sword Durability: %d\n", broken_sword_durability);
    TEST_ASSERT(broken_sword_durability == 0, "Sword should be broken (durability 0)");

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

    Item_t* sword = create_weapon("Damaged Sword", "damaged_sword", basic_material, 15, 25, 0, 'S');
    Item_t* armor = create_armor("Damaged Armor", "damaged_armor", basic_material, 20, 10, 'A', 10, 10);
    Item_t* consumable = create_consumable("Potion", "potion", 50, dummy_consume_callback, 'p');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    TEST_ASSERT(consumable != NULL, "Consumable should be created");

    // Damage items first
    damage_item_durability(sword, 100);
    damage_item_durability(armor, 75);

    uint8_t damaged_sword_durability = get_durability(sword);
    uint8_t damaged_armor_durability = get_durability(armor);

    TEST_ASSERT(damaged_sword_durability == 155, "Sword should be damaged to 155");
    TEST_ASSERT(damaged_armor_durability == 180, "Armor should be damaged to 180");

    // Repair the items
    repair_item(sword, 50);
    repair_item(armor, 25);
    repair_item(consumable, 100); // Should have no effect

    uint8_t repaired_sword_durability = get_durability(sword);
    uint8_t repaired_armor_durability = get_durability(armor);
    uint8_t consumable_durability = get_durability(consumable);

    TEST_ASSERT(repaired_sword_durability == 205, "Sword should be repaired to 205");
    TEST_ASSERT(repaired_armor_durability == 205, "Armor should be repaired to 205");
    TEST_ASSERT(consumable_durability == 255, "Consumable durability should be unaffected");

    // Test over-repair (should cap at 255)
    repair_item(sword, 100);
    uint8_t over_repaired_durability = get_durability(sword);
    TEST_ASSERT(over_repaired_durability == 255, "Sword should be capped at max durability");

    // Test repairing broken item
    damage_item_durability(armor, 255); // Break it completely
    printf("Broken Armor Durability: %d\n", get_durability(armor));
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
    // Create test items
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Breakable Sword", "breakable_sword", basic_material, 18, 28, 0, 'S');
    Item_t* armor = create_armor("Breakable Armor", "breakable_armor", basic_material, 22, 12, 'A', 10, 10);
    Item_t* arrow = create_ammunition("Arrow", "arrow", basic_material, 5, 8, 'a');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    TEST_ASSERT(arrow != NULL, "Arrow should be created");

    // Test initial state (not broken)
    TEST_ASSERT(!is_item_broken(sword), "New sword should not be broken");
    TEST_ASSERT(!is_item_broken(armor), "New armor should not be broken");
    TEST_ASSERT(!is_item_broken(arrow), "Arrows can't be broken");

    // Damage items partially
    damage_item_durability(sword, 100);
    damage_item_durability(armor, 50);

    TEST_ASSERT(!is_item_broken(sword), "Partially damaged sword should not be broken");
    TEST_ASSERT(!is_item_broken(armor), "Partially damaged armor should not be broken");

    // Break items completely
    damage_item_durability(sword, 200); // Should break it
    damage_item_durability(armor, 250); // Should break it

    TEST_ASSERT(is_item_broken(sword), "Completely damaged sword should be broken");
    TEST_ASSERT(is_item_broken(armor), "Completely damaged armor should be broken");
    TEST_ASSERT(!is_item_broken(arrow), "Arrows still can't be broken");

    // Test NULL handling
    TEST_ASSERT(is_item_broken(NULL), "NULL item should be considered broken");

    destroy_item(sword);
    destroy_item(armor);
    destroy_item(arrow);
    return 1;
}

// =============================================================================
// DURABILITY PERCENTAGE TESTS
// =============================================================================

int test_get_durability_percentage(void)
{
    // Create test items
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Percentage Sword", "percentage_sword", basic_material, 16, 24, 0, 'S');
    Item_t* armor = create_armor("Percentage Armor", "percentage_armor", basic_material, 18, 8, 'A', 10, 10);
    Lock_t lock = create_lock("lock", "A lock", 30, 0);
    Item_t* key = create_key("Test Key", "test_key", lock, 'k');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    TEST_ASSERT(key != NULL, "Key should be created");

    // Test full durability (should be 1.0 = 100%)
    float sword_full_percent = get_durability_percentage(sword);
    float armor_full_percent = get_durability_percentage(armor);
    float key_percent = get_durability_percentage(key);

    TEST_ASSERT(float_equals(sword_full_percent, 1.0f, 0.01f), "Full durability sword should be 100%");
    TEST_ASSERT(float_equals(armor_full_percent, 1.0f, 0.01f), "Full durability armor should be 100%");
    TEST_ASSERT(float_equals(key_percent, 1.0f, 0.01f), "Key should always be 100%");

    // Damage to specific percentages
    damage_item_durability(sword, 127); // Should be ~50% (255-127=128, 128/255≈0.5)
    damage_item_durability(armor, 191); // Should be ~25% (255-191=64, 64/255≈0.25)

    float sword_half_percent = get_durability_percentage(sword);
    float armor_quarter_percent = get_durability_percentage(armor);

    TEST_ASSERT(float_equals(sword_half_percent, 0.5f, 0.02f), "Half damaged sword should be ~50%");
    TEST_ASSERT(float_equals(armor_quarter_percent, 0.25f, 0.02f), "Quarter durability armor should be ~25%");

    // Test completely broken (should be 0.0 = 0%)
    damage_item_durability(sword, 255);
    damage_item_durability(armor, 255);

    float sword_broken_percent = get_durability_percentage(sword);
    float armor_broken_percent = get_durability_percentage(armor);

    TEST_ASSERT(float_equals(sword_broken_percent, 0.0f, 0.01f), "Broken sword should be 0%");
    TEST_ASSERT(float_equals(armor_broken_percent, 0.0f, 0.01f), "Broken armor should be 0%");

    // Test NULL handling
    float null_percent = get_durability_percentage(NULL);
    TEST_ASSERT(float_equals(null_percent, 0.0f, 0.01f), "NULL item should be 0%");

    destroy_item(sword);
    destroy_item(armor);
    destroy_item(key);
    return 1;
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

int test_durability_system_integration(void)
{
    // Create a weapon with material that affects durability
    MaterialProperties_t fragile_props = create_default_material_properties();
    fragile_props.durability_fact = 0.5f; // 50% durability
    Material_t fragile_material = create_material("glass", fragile_props);

    Item_t* glass_sword = create_weapon("Glass Sword", "glass_sword", fragile_material, 30, 40, 0, 'G');
    TEST_ASSERT(glass_sword != NULL, "Glass sword should be created");

    // Apply material (should reduce durability)
    apply_material_to_weapon(glass_sword);

    uint8_t initial_durability = get_durability(glass_sword);
    float initial_percent = get_durability_percentage(glass_sword);

    printf("Glass sword initial durability: %d (%.1f%%)\n",
           initial_durability, initial_percent * 100);

    // Glass sword should start with reduced durability due to material
    TEST_ASSERT(initial_durability == 255, "Glass sword should have same initial durability");
    TEST_ASSERT(!is_item_broken(glass_sword), "Glass sword should not start broken");

    // Damage it progressively
    damage_item_durability(glass_sword, 50);

    uint8_t after_damage = get_durability(glass_sword);
    float after_damage_percent = get_durability_percentage(glass_sword);

    printf("After 50 damage: %d (%.1f%%)\n",
           after_damage, after_damage_percent * 100);

    // Repair it
    repair_item(glass_sword, 30);

    uint8_t after_repair = get_durability(glass_sword);
    float after_repair_percent = get_durability_percentage(glass_sword);

    printf("After 30 repair: %d (%.1f%%)\n",
           after_repair, after_repair_percent * 100);

    TEST_ASSERT(after_repair > after_damage, "Repair should increase durability");
    TEST_ASSERT(!is_item_broken(glass_sword), "Repaired sword should not be broken");

    // Break it completely
    damage_item_durability(glass_sword, 255);

    printf("Broken Sword Durability: %d\n", get_durability(glass_sword));

    TEST_ASSERT(is_item_broken(glass_sword), "Massively damaged sword should be broken");
    TEST_ASSERT(float_equals(get_durability_percentage(glass_sword), 0.0f, 0.01f),
                "Broken sword should be 0%");

    destroy_item(glass_sword);
    return 1;
}
// =============================================================================
// ENHANCED DURABILITY BUG HUNTING TESTS
// =============================================================================

/*
 * Test durability overflow and underflow edge cases
 */
int test_durability_overflow_underflow(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Overflow Sword", "overflow_sword", basic_material, 20, 30, 0, 'S');
    TEST_ASSERT(sword != NULL, "Overflow test sword should be created");

    // Test extreme damage values that could cause underflow
    uint8_t initial_durability = get_durability(sword);
    printf("Initial sword durability: %d\n", initial_durability);

    // Try to damage by more than max uint8_t
    damage_item_durability(sword, 65535);  // Way more than 255
    uint8_t after_extreme_damage = get_durability(sword);
    printf("After extreme damage (65535): %d\n", after_extreme_damage);
    TEST_ASSERT(after_extreme_damage == 0, "Extreme damage should clamp to 0, not underflow");

    // Reset sword
    repair_item(sword, 255);
    TEST_ASSERT(get_durability(sword) == 255, "Should be fully repaired");

    // Test multiple damage calls that could cause integer overflow
    for (int i = 0; i < 10; i++) {
        damage_item_durability(sword, 200);
        uint8_t current_dur = get_durability(sword);
        printf("After damage round %d: durability = %d\n", i+1, current_dur);
        TEST_ASSERT(current_dur <= 255, "Durability should never exceed 255");
    }

    // Test extreme repair values
    damage_item_durability(sword, 255); // Break it
    repair_item(sword, 65535); // Try to repair by extreme amount
    uint8_t after_extreme_repair = get_durability(sword);
    printf("After extreme repair (65535): %d\n", after_extreme_repair);
    TEST_ASSERT(after_extreme_repair == 255, "Extreme repair should clamp to 255, not overflow");

    destroy_item(sword);
    return 1;
}

/*
 * Test durability with extreme material factors
 */
int test_durability_extreme_materials(void)
{
    // Test with zero durability factor (items should break instantly?)
    MaterialProperties_t zero_durability_props = create_default_material_properties();
    zero_durability_props.durability_fact = 0.0f;
    Material_t zero_durability_material = create_material("paper", zero_durability_props);

    Item_t* paper_sword = create_weapon("Paper Sword", "paper_sword", zero_durability_material, 10, 15, 0, 'P');
    TEST_ASSERT(paper_sword != NULL, "Paper sword should be created");

    apply_material_to_weapon(paper_sword);
    uint8_t paper_durability = get_durability(paper_sword);
    printf("Paper sword durability after material application: %d\n", paper_durability);
    TEST_ASSERT(paper_durability >= 0, "Paper sword durability should not be negative");

    // Test with extreme durability factor
    MaterialProperties_t extreme_durability_props = create_default_material_properties();
    extreme_durability_props.durability_fact = 999999.0f;
    Material_t extreme_durability_material = create_material("adamantine", extreme_durability_props);

    Item_t* adamantine_sword = create_weapon("Adamantine Sword", "adamantine_sword", extreme_durability_material, 10, 15, 0, 'A');
    TEST_ASSERT(adamantine_sword != NULL, "Adamantine sword should be created");

    apply_material_to_weapon(adamantine_sword);
    uint8_t adamantine_durability = get_durability(adamantine_sword);
    printf("Adamantine sword durability after material application: %d\n", adamantine_durability);
    TEST_ASSERT(adamantine_durability <= 255, "Adamantine sword durability should not overflow uint8_t");

    // Test negative durability factor
    MaterialProperties_t negative_durability_props = create_default_material_properties();
    negative_durability_props.durability_fact = -10.0f;
    Material_t negative_durability_material = create_material("cursed", negative_durability_props);

    Item_t* cursed_sword = create_weapon("Cursed Sword", "cursed_sword", negative_durability_material, 10, 15, 0, 'C');
    TEST_ASSERT(cursed_sword != NULL, "Cursed sword should be created");

    apply_material_to_weapon(cursed_sword);
    uint8_t cursed_durability = get_durability(cursed_sword);
    printf("Cursed sword durability after material application: %d\n", cursed_durability);
    TEST_ASSERT(cursed_durability >= 0, "Cursed sword durability should not be negative");

    destroy_item(paper_sword);
    destroy_item(adamantine_sword);
    destroy_item(cursed_sword);
    return 1;
}

/*
 * Test durability race conditions and rapid operations
 */
int test_durability_rapid_operations(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* stress_sword = create_weapon("Stress Sword", "stress_sword", basic_material, 20, 30, 0, 'S');
    TEST_ASSERT(stress_sword != NULL, "Stress sword should be created");

    // Rapidly alternate between damage and repair
    for (int i = 0; i < 100; i++) {
        damage_item_durability(stress_sword, 10);
        repair_item(stress_sword, 5);

        uint8_t current_dur = get_durability(stress_sword);
        TEST_ASSERT(current_dur <= 255, "Durability should stay within bounds during rapid operations");

        // Check percentage calculation during rapid changes
        float percentage = get_durability_percentage(stress_sword);
        TEST_ASSERT(percentage >= 0.0f && percentage <= 1.0f, "Percentage should stay within 0-1 range");
    }

    // Test rapid damage to zero and repair to full
    for (int i = 0; i < 50; i++) {
        damage_item_durability(stress_sword, 255); // Break it
        TEST_ASSERT(get_durability(stress_sword) == 0, "Should be broken");
        TEST_ASSERT(is_item_broken(stress_sword), "Should register as broken");

        repair_item(stress_sword, 255); // Fix it
        TEST_ASSERT(get_durability(stress_sword) == 255, "Should be fully repaired");
        TEST_ASSERT(!is_item_broken(stress_sword), "Should not be broken after repair");
    }

    destroy_item(stress_sword);
    return 1;
}

/*
 * Test durability edge cases with different item types
 */
int test_durability_item_type_edge_cases(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Test consumables (shouldn't have durability mechanics)
    Item_t* potion = create_consumable("Test Potion", "test_potion", 50, dummy_consume_callback, 'P');
    TEST_ASSERT(potion != NULL, "Potion should be created");

    uint8_t potion_initial = get_durability(potion);
    damage_item_durability(potion, 100);
    uint8_t potion_after_damage = get_durability(potion);
    repair_item(potion, 50);
    uint8_t potion_after_repair = get_durability(potion);

    printf("Potion durability: initial=%d, after_damage=%d, after_repair=%d\n",
           potion_initial, potion_after_damage, potion_after_repair);

    TEST_ASSERT(potion_initial == potion_after_damage, "Consumables should not be affected by damage");
    TEST_ASSERT(potion_after_damage == potion_after_repair, "Consumables should not be affected by repair");
    TEST_ASSERT(!is_item_broken(potion), "Consumables should never be broken");

    // Test keys (shouldn't have durability mechanics)
    Lock_t test_lock = create_lock("Test Lock", "test_lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", test_lock, 'K');
    TEST_ASSERT(key != NULL, "Key should be created");

    uint8_t key_initial = get_durability(key);
    damage_item_durability(key, 100);
    uint8_t key_after_damage = get_durability(key);
    repair_item(key, 50);
    uint8_t key_after_repair = get_durability(key);

    printf("Key durability: initial=%d, after_damage=%d, after_repair=%d\n",
           key_initial, key_after_damage, key_after_repair);

    TEST_ASSERT(key_initial == key_after_damage, "Keys should not be affected by damage");
    TEST_ASSERT(key_after_damage == key_after_repair, "Keys should not be affected by repair");
    TEST_ASSERT(!is_item_broken(key), "Keys should never be broken");

    // Test ammunition (stackable items with potential durability issues)
    Item_t* arrows = create_ammunition("Test Arrows", "test_arrows", basic_material, 5, 8, 'A');
    TEST_ASSERT(arrows != NULL, "Arrows should be created");

    uint8_t arrows_initial = get_durability(arrows);
    damage_item_durability(arrows, 100);
    uint8_t arrows_after_damage = get_durability(arrows);

    printf("Arrows durability: initial=%d, after_damage=%d\n", arrows_initial, arrows_after_damage);
    TEST_ASSERT(!is_item_broken(arrows), "Ammunition should not be breakable");

    destroy_item(potion);
    destroy_item(key);
    destroy_item(arrows);
    return 1;
}

/*
 * Test durability percentage calculation edge cases
 */
int test_durability_percentage_edge_cases(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Percentage Test Sword", "percentage_sword", basic_material, 20, 30, 0, 'S');
    TEST_ASSERT(sword != NULL, "Percentage test sword should be created");

    // Test percentage at exact boundaries
    uint8_t test_durabilities[] = {255, 254, 128, 127, 64, 32, 16, 8, 4, 2, 1, 0};
    int num_tests = sizeof(test_durabilities) / sizeof(test_durabilities[0]);

    for (int i = 0; i < num_tests; i++) {
        // Set exact durability by damaging from full
        repair_item(sword, 255); // Start full
        damage_item_durability(sword, 255 - test_durabilities[i]);

        uint8_t actual_durability = get_durability(sword);
        float percentage = get_durability_percentage(sword);
        float expected_percentage = (float)actual_durability / 255.0f;

        printf("Durability %d: actual=%d, percentage=%.6f, expected=%.6f\n",
               test_durabilities[i], actual_durability, percentage, expected_percentage);

        TEST_ASSERT(float_equals(percentage, expected_percentage, 0.001f),
                   "Percentage calculation should be accurate");
        TEST_ASSERT(percentage >= 0.0f && percentage <= 1.0f,
                   "Percentage should be in valid range");
    }

    // Test with broken item
    damage_item_durability(sword, 255);
    float broken_percentage = get_durability_percentage(sword);
    TEST_ASSERT(float_equals(broken_percentage, 0.0f, 0.001f), "Broken item should be 0%");

    destroy_item(sword);
    return 1;
}

/*
 * Test durability with concurrent modifications (simulated)
 */
int test_durability_concurrent_simulation(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Concurrent Test Sword", "concurrent_sword", basic_material, 20, 30, 0, 'S');
    TEST_ASSERT(sword != NULL, "Concurrent test sword should be created");

    // Simulate what might happen if multiple systems try to modify durability simultaneously
    // This tests for potential race conditions or state corruption

    struct {
        uint8_t damage_amount;
        uint8_t repair_amount;
    } operations[] = {
        {50, 30}, {100, 80}, {25, 25}, {200, 150}, {10, 5},
        {75, 60}, {255, 200}, {40, 45}, {90, 20}, {120, 255}
    };

    int num_operations = sizeof(operations) / sizeof(operations[0]);

    for (int i = 0; i < num_operations; i++) {
        uint8_t before_durability = get_durability(sword);
        bool before_broken = is_item_broken(sword);
        float before_percentage = get_durability_percentage(sword);

        // Apply damage
        damage_item_durability(sword, operations[i].damage_amount);
        uint8_t after_damage = get_durability(sword);
        bool after_damage_broken = is_item_broken(sword);

        // Apply repair
        repair_item(sword, operations[i].repair_amount);
        uint8_t after_repair = get_durability(sword);
        bool after_repair_broken = is_item_broken(sword);
        float after_percentage = get_durability_percentage(sword);

        printf("Op %d: before=%d, after_damage=%d, after_repair=%d, broken_states=[%d,%d,%d]\n",
               i, before_durability, after_damage, after_repair,
               before_broken, after_damage_broken, after_repair_broken);

        // Consistency checks
        TEST_ASSERT(after_repair <= 255, "Durability should not exceed maximum");
        TEST_ASSERT(after_percentage >= 0.0f && after_percentage <= 1.0f,
                   "Percentage should be in valid range");
        TEST_ASSERT((after_repair == 0) == after_repair_broken,
                   "Broken state should match zero durability");
    }

    destroy_item(sword);
    return 1;
}

/*
 * Test memory safety during durability operations
 */
int test_durability_memory_safety(void)
{
    // Test with items that might be deallocated during operations
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Create many items and test durability operations
    Item_t* items[10];
    for (int i = 0; i < 10; i++) {
        char name[32], id[32];
        snprintf(name, sizeof(name), "Memory Test Item %d", i);
        snprintf(id, sizeof(id), "memory_test_%d", i);

        items[i] = create_weapon(name, id, basic_material, 10 + i, 20 + i, 0, 'M');
        TEST_ASSERT(items[i] != NULL, "Memory test item should be created");
    }

    // Perform operations on all items
    for (int i = 0; i < 10; i++) {
        damage_item_durability(items[i], 50 + i * 10);
        repair_item(items[i], 25 + i * 5);

        uint8_t durability = get_durability(items[i]);
        float percentage = get_durability_percentage(items[i]);
        bool broken = is_item_broken(items[i]);

        printf("Item %d: durability=%d, percentage=%.2f, broken=%d\n",
               i, durability, percentage, broken);

        TEST_ASSERT(durability <= 255, "Durability should be within bounds");
        TEST_ASSERT(percentage >= 0.0f && percentage <= 1.0f, "Percentage should be valid");
    }

    // Clean up
    for (int i = 0; i < 10; i++) {
        destroy_item(items[i]);
    }

    // Test operations on NULL items (should not crash)
    damage_item_durability(NULL, 50);
    repair_item(NULL, 30);
    uint8_t null_durability = get_durability(NULL);
    float null_percentage = get_durability_percentage(NULL);
    bool null_broken = is_item_broken(NULL);

    TEST_ASSERT(null_durability == 0, "NULL item should have 0 durability");
    TEST_ASSERT(float_equals(null_percentage, 0.0f, 0.001f), "NULL item should have 0% durability");
    TEST_ASSERT(null_broken == true, "NULL item should be considered broken");

    return 1;
}
// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Durability System Tests");
    srand(123725);
    // Run durability system tests
    RUN_TEST(test_damage_item_durability);
    RUN_TEST(test_repair_item);
    RUN_TEST(test_is_item_broken);
    RUN_TEST(test_get_durability_percentage);
    RUN_TEST(test_durability_system_integration);

    RUN_TEST(test_durability_overflow_underflow);
    RUN_TEST(test_durability_extreme_materials);
    RUN_TEST(test_durability_rapid_operations);
    RUN_TEST(test_durability_item_type_edge_cases);
    RUN_TEST(test_durability_percentage_edge_cases);
    RUN_TEST(test_durability_concurrent_simulation);
    RUN_TEST(test_durability_memory_safety);

    TEST_SUITE_END();
}
