// ASCIIGame/tests/items/test_items_durability.c
// Test file for item durability system functions

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

    Item_t* sword = create_weapon("Test Sword", "test_sword", basic_material, 20, 30, 0, 'S');
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 25, 15, 'A');
    Item_t* key = create_key("Test Key", "test_key", (Lock_t){"lock", "A lock", 50, 0}, 'k');

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
    Item_t* armor = create_armor("Damaged Armor", "damaged_armor", basic_material, 20, 10, 'A');
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
    Item_t* armor = create_armor("Breakable Armor", "breakable_armor", basic_material, 22, 12, 'A');
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
    Item_t* armor = create_armor("Percentage Armor", "percentage_armor", basic_material, 18, 8, 'A');
    Item_t* key = create_key("Test Key", "test_key", (Lock_t){"lock", "A lock", 30, 0}, 'k');

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
    TEST_ASSERT(initial_durability < 255, "Glass sword should have reduced initial durability");
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
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Durability System Tests");

    // Run durability system tests
    RUN_TEST(test_damage_item_durability);
    RUN_TEST(test_repair_item);
    RUN_TEST(test_is_item_broken);
    RUN_TEST(test_get_durability_percentage);
    RUN_TEST(test_durability_system_integration);

    TEST_SUITE_END();
}
