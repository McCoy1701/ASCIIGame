// ASCIIGame/tests/items/test_items_properties.c
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
void dummy_consume_callback(uint8_t value) // consumables cannot have on_consume = NULL
{
    // Dummy callback for testing
    (void)value; // Suppress unused parameter warning
}
// Helper function for float comparison
bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// =============================================================================
// WEAPON STATS TESTS
// =============================================================================

int test_weapon_stats(void)
{
    // Create a weapon for testing
    MaterialProperties_t iron_props = create_default_material_properties();
    Material_t iron = create_material("iron", iron_props);
    Item_t* sword = create_weapon("Test Sword", "test_sword", iron, 15, 25, 1, 'S');
    TEST_ASSERT(sword != NULL, "Sword should be created");

    // Test weapon damage functions
    uint8_t min_dmg = get_weapon_min_damage(sword);
    uint8_t max_dmg = get_weapon_max_damage(sword);
    uint8_t range = get_weapon_range(sword);

    TEST_ASSERT(min_dmg == 15, "Min damage should be 15");
    TEST_ASSERT(max_dmg == 25, "Max damage should be 25");
    TEST_ASSERT(range == 1, "Range should be 1");

    // Test with NULL
    TEST_ASSERT(get_weapon_min_damage(NULL) == 0, "NULL weapon should have 0 min damage");
    TEST_ASSERT(get_weapon_max_damage(NULL) == 0, "NULL weapon should have 0 max damage");
    TEST_ASSERT(get_weapon_range(NULL) == 0, "NULL weapon should have 0 range");

    // Test with non-weapon item
    Item_t* armor = create_armor("Test Armor", "test_armor", iron, 20, 10, 'A');
    TEST_ASSERT(get_weapon_min_damage(armor) == 0, "Armor should have 0 weapon damage");
    TEST_ASSERT(get_weapon_max_damage(armor) == 0, "Armor should have 0 weapon damage");
    TEST_ASSERT(get_weapon_range(armor) == 0, "Armor should have 0 weapon range");

    destroy_item(sword);
    destroy_item(armor);
    return 1;
}

int test_weapon_ammo(void)
{
    // Create weapons for testing
    MaterialProperties_t wood_props = create_default_material_properties();
    Material_t wood = create_material("wood", wood_props);

    // Create melee weapon
    Item_t* sword = create_weapon("Sword", "sword", wood, 10, 15, 0, 'S');
    TEST_ASSERT(sword != NULL, "Sword should be created");

    // Create ranged weapon
    Item_t* bow = create_weapon("Bow", "bow", wood, 5, 10, 5, 'B');
    printf("Bow created\n %p", &bow->data.ammo);
    TEST_ASSERT(bow != NULL, "Bow should be created");

    // Create ammunition
    Item_t* arrow = create_ammunition("Arrow", "arrow", wood, 3, 5, 'a');
    TEST_ASSERT(arrow != NULL, "Arrow should be created");

    // Set Bow's ammuniton to Arrow
    //bow->data.weapon.ammo = arrow;

    // Test ammo requirements
    bool sword_needs_ammo = weapon_needs_ammo(sword);
    bool bow_needs_ammo = weapon_needs_ammo(bow);

    TEST_ASSERT(!sword_needs_ammo, "Sword should not need ammo");
    printf("Bow needs ammo: %d\n", bow_needs_ammo);
    TEST_ASSERT(bow_needs_ammo, "Bow should need ammo");

    // Test ammo compatibility
    bool sword_can_use_arrow = weapon_can_use_ammo(sword, arrow);
    bool bow_can_use_arrow = weapon_can_use_ammo(bow, arrow);

    TEST_ASSERT(!sword_can_use_arrow, "Sword should not be able to use arrows");
    TEST_ASSERT(bow_can_use_arrow, "Bow should be able to use arrows");

    // Test with NULL
    TEST_ASSERT(!weapon_needs_ammo(NULL), "NULL weapon should not need ammo");
    TEST_ASSERT(!weapon_can_use_ammo(NULL, arrow), "NULL weapon should not use ammo");
    TEST_ASSERT(!weapon_can_use_ammo(bow, NULL), "Weapon should not use NULL ammo");

    destroy_item(sword);
    destroy_item(bow);
    destroy_item(arrow);
    return 1;
}

// =============================================================================
// ARMOR STATS TESTS
// =============================================================================

int test_armor_stats(void)
{
    // Create armor for testing
    MaterialProperties_t leather_props = create_default_material_properties();
    Material_t leather = create_material("leather", leather_props);
    Item_t* armor = create_armor("Test Armor", "test_armor", leather, 30, 15, 'A');
    TEST_ASSERT(armor != NULL, "Armor should be created");

    // Test armor stat functions
    uint8_t armor_val = get_armor_value(armor);
    uint8_t evasion_val = get_evasion_value(armor);

    TEST_ASSERT(armor_val == 30, "Armor value should be 30");
    TEST_ASSERT(evasion_val == 15, "Evasion value should be 15");

    // Test with NULL
    TEST_ASSERT(get_armor_value(NULL) == 0, "NULL armor should have 0 armor value");
    TEST_ASSERT(get_evasion_value(NULL) == 0, "NULL armor should have 0 evasion value");

    // Test with non-armor item
    Item_t* sword = create_weapon("Test Sword", "test_sword", leather, 10, 15, 0, 'S');
    TEST_ASSERT(get_armor_value(sword) == 0, "Weapon should have 0 armor value");
    TEST_ASSERT(get_evasion_value(sword) == 0, "Weapon should have 0 evasion value");

    destroy_item(armor);
    destroy_item(sword);
    return 1;
}

// =============================================================================
// GENERAL ITEM PROPERTIES TESTS
// =============================================================================

int test_item_weight(void)
{
    // Create items with different weights
    MaterialProperties_t heavy_props = {
        .weight_fact = 2.0f, .value_coins_fact = 1.0f, .durability_fact = 1.0f,
        .min_damage_fact = 1.0f, .max_damage_fact = 1.0f, .armor_value_fact = 1.0f,
        .evasion_value_fact = 1.0f, .stealth_value_fact = 1.0f, .enchant_value_fact = 1.0f
    };
    Material_t heavy_material = create_material("heavy", heavy_props);

    Item_t* heavy_sword = create_weapon("Heavy Sword", "heavy_sword", heavy_material, 20, 30, 0, 'H');
    TEST_ASSERT(heavy_sword != NULL, "Heavy sword should be created");

    // Apply material to affect weight
    apply_material_to_weapon(heavy_sword);

    float weight = get_item_weight(heavy_sword);
    TEST_ASSERT(weight > 0.0f, "Weight should be greater than 0");
    TEST_ASSERT(float_equals(weight, heavy_sword->weight_kg, 0.01f), "get_item_weight should match item's weight_kg");

    // Test with NULL
    float null_weight = get_item_weight(NULL);
    TEST_ASSERT(float_equals(null_weight, 0.0f, 0.01f), "NULL item should have 0 weight");

    destroy_item(heavy_sword);
    return 1;
}

int test_item_value(void)
{
    // Create items with different values
    MaterialProperties_t valuable_props = {
        .weight_fact = 1.0f, .value_coins_fact = 3.0f, .durability_fact = 1.0f,
        .min_damage_fact = 1.0f, .max_damage_fact = 1.0f, .armor_value_fact = 1.0f,
        .evasion_value_fact = 1.0f, .stealth_value_fact = 1.0f, .enchant_value_fact = 1.0f
    };
    Material_t valuable_material = create_material("gold", valuable_props);

    Item_t* gold_ring = create_armor("Gold Ring", "gold_ring", valuable_material, 1, 0, 'o');
    TEST_ASSERT(gold_ring != NULL, "Gold ring should be created");

    // Apply material to affect value
    apply_material_to_armor(gold_ring);

    uint8_t value = get_item_value_coins(gold_ring);
    TEST_ASSERT(value > 0, "Value should be greater than 0");
    TEST_ASSERT(value == gold_ring->value_coins, "get_item_value_coins should match item's value_coins");

    // Test with NULL
    uint8_t null_value = get_item_value_coins(NULL);
    TEST_ASSERT(null_value == 0, "NULL item should have 0 value");

    destroy_item(gold_ring);
    return 1;
}

int test_stealth_value(void)
{
    // Create items with stealth values
    MaterialProperties_t stealth_props = create_default_material_properties();
    stealth_props.stealth_value_fact = 2.0f;
    Material_t stealth_material = create_material("shadow", stealth_props);

    Item_t* stealth_armor = create_armor("Shadow Cloak", "shadow_cloak", stealth_material, 10, 20, 'C');
    Item_t* stealth_weapon = create_weapon("Shadow Blade", "shadow_blade", stealth_material, 15, 20, 0, 'b');
    Item_t* key = create_key("Test Key", "test_key", (Lock_t){"test_lock", "A test lock", 50, 0}, 'k');

    TEST_ASSERT(stealth_armor != NULL, "Stealth armor should be created");
    TEST_ASSERT(stealth_weapon != NULL, "Stealth weapon should be created");
    TEST_ASSERT(key != NULL, "Key should be created");

    // Apply materials to set stealth values
    apply_material_to_armor(stealth_armor);
    apply_material_to_weapon(stealth_weapon);

    uint8_t armor_stealth = get_stealth_value(stealth_armor);
    uint8_t weapon_stealth = get_stealth_value(stealth_weapon);
    uint8_t key_stealth = get_stealth_value(key);

    TEST_ASSERT(armor_stealth >= 0, "Armor should have stealth value");
    TEST_ASSERT(weapon_stealth >= 0, "Weapon should have stealth value");
    TEST_ASSERT(key_stealth == 0, "Key should have no stealth value");

    // Test with NULL
    uint8_t null_stealth = get_stealth_value(NULL);
    TEST_ASSERT(null_stealth == 0, "NULL item should have 0 stealth value");

    destroy_item(stealth_armor);
    destroy_item(stealth_weapon);
    destroy_item(key);
    return 1;
}

int test_durability(void)
{
    MaterialProperties_t durable_props = create_default_material_properties();
    durable_props.durability_fact = 1.5f;
    Material_t durable_material = create_material("adamantine", durable_props);

    Item_t* durable_sword = create_weapon("Adamantine Sword", "adamantine_sword", durable_material, 25, 35, 0, 'A');
    Item_t* consumable = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H');

    TEST_ASSERT(durable_sword != NULL, "Durable sword should be created");
    TEST_ASSERT(consumable != NULL, "Consumable should be created");

    // Apply material
    apply_material_to_weapon(durable_sword);

    uint8_t sword_durability = get_durability(durable_sword);
    uint8_t consumable_durability = get_durability(consumable);

    TEST_ASSERT(sword_durability > 0, "Sword should have durability");
    TEST_ASSERT(consumable_durability == 255, "Consumable should have max durability (no degradation)");

    // Test with NULL
    uint8_t null_durability = get_durability(NULL);
    TEST_ASSERT(null_durability == 0, "NULL item should have 0 durability");

    destroy_item(durable_sword);
    destroy_item(consumable);
    return 1;
}

int test_stackable(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Sword", "sword", basic_material, 10, 15, 0, 'S');
    Item_t* arrows = create_ammunition("Arrows", "arrows", basic_material, 5, 8, 'a');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(arrows != NULL, "Arrows should be created");

    bool sword_stackable = is_item_stackable(sword);
    bool arrows_stackable = is_item_stackable(arrows);

    uint8_t sword_max_stack = get_max_stack_size(sword);
    uint8_t arrows_max_stack = get_max_stack_size(arrows);

    TEST_ASSERT(!sword_stackable, "Weapons should not be stackable");
    TEST_ASSERT(arrows_stackable, "Ammunition should be stackable");
    TEST_ASSERT(sword_max_stack == 0, "Weapons should have 0 max stack size");
    TEST_ASSERT(arrows_max_stack > 0, "Ammunition should have positive max stack size");

    // Test with NULL
    TEST_ASSERT(!is_item_stackable(NULL), "NULL item should not be stackable");
    TEST_ASSERT(get_max_stack_size(NULL) == 0, "NULL item should have 0 max stack size");

    destroy_item(sword);
    destroy_item(arrows);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Properties & Stats Tests");

    // Run weapon stats tests
    RUN_TEST(test_weapon_stats);
    RUN_TEST(test_weapon_ammo);

    // Run armor stats tests
    RUN_TEST(test_armor_stats);

    // Run general item properties tests
    RUN_TEST(test_item_weight);
    RUN_TEST(test_item_value);
    RUN_TEST(test_stealth_value);
    RUN_TEST(test_durability);
    RUN_TEST(test_stackable);

    TEST_SUITE_END();
}
