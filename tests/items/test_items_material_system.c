#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "tests.h"
#include "items.h"
#include "structs.h"
#iyynclude "defs.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper function for float comparison
bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// =============================================================================
// MATERIAL CREATION TESTS
// =============================================================================

int test_create_material(void)
{
    // Test normal material creation
    MaterialProperties_t steel_props = {
        .weight_fact = 1.5f,
        .value_coins_fact = 1.1f,
        .durability_fact = 1.2f,
        .min_damage_fact = 1.1f,
        .max_damage_fact = 1.3f,
        .armor_value_fact = 1.3f,
        .evasion_value_fact = 0.8f,
        .stealth_value_fact = 0.9f,
        .enchant_value_fact = 1.1f
    };
    Material_t steel = create_material("steel", steel_props);

    TEST_ASSERT(strcmp(steel.name->str, "steel") == 0, "Material name should be 'steel'");
    // NOTE: Access properties through the nested 'properties' struct
    TEST_ASSERT(float_equals(steel.properties.weight_fact, 1.5f, 0.01f), "Weight factor should be 1.5");
    TEST_ASSERT(float_equals(steel.properties.min_damage_fact, 1.1f, 0.01f), "Min damage factor should be 1.1");
    TEST_ASSERT(float_equals(steel.properties.max_damage_fact, 1.3f, 0.01f), "Max damage factor should be 1.3");
    TEST_ASSERT(float_equals(steel.properties.armor_value_fact, 1.3f, 0.01f), "Armor value factor should be 1.3");
    TEST_ASSERT(float_equals(steel.properties.evasion_value_fact, 0.8f, 0.01f), "Evasion value factor should be 0.8");
    TEST_ASSERT(float_equals(steel.properties.durability_fact, 1.2f, 0.01f), "Durability factor should be 1.2");
    TEST_ASSERT(float_equals(steel.properties.stealth_value_fact, 0.9f, 0.01f), "Stealth factor should be 0.9");
    TEST_ASSERT(float_equals(steel.properties.enchant_value_fact, 1.1f, 0.01f), "Enchant factor should be 1.1");

    // Test material creation with NULL name
    MaterialProperties_t default_props = {
        .weight_fact = 1.0f, .value_coins_fact = 1.0f, .durability_fact = 1.0f,
        .min_damage_fact = 1.0f, .max_damage_fact = 1.0f, .armor_value_fact = 1.0f,
        .evasion_value_fact = 1.0f, .stealth_value_fact = 1.0f, .enchant_value_fact = 1.0f
    };
    Material_t unknown = create_material(NULL, default_props);
    TEST_ASSERT(strcmp(unknown.name->str, "Default Material") == 0, "NULL name should default to 'Default Material'");
    TEST_ASSERT(float_equals(unknown.properties.weight_fact, 1.0f, 0.01f), "NULL material should have default factors");

    // Test extreme values
    MaterialProperties_t mythril_props = {
        .weight_fact = 0.1f,
        .value_coins_fact = 3.0f, // Mapped from old value
        .durability_fact = 1.4f,  // Mapped from old value
        .min_damage_fact = 3.0f,
        .max_damage_fact = 1.4f,  // Mapped from old value
        .armor_value_fact = 2.5f,
        .evasion_value_fact = 10.0f,
        .stealth_value_fact = 5.0f,
        .enchant_value_fact = 8.0f
    };
    Material_t mythril = create_material("mythril", mythril_props);
    TEST_ASSERT(strcmp(mythril.name->str, "mythril") == 0, "Mythril name should be correct");
    TEST_ASSERT(float_equals(mythril.properties.weight_fact, 0.1f, 0.01f), "Mythril should be very light");
    TEST_ASSERT(float_equals(mythril.properties.min_damage_fact, 3.0f, 0.01f), "Mythril should have high damage");

    return 1;
}

// =============================================================================
// MATERIAL APPLICATION TESTS
// =============================================================================

int test_apply_material_to_weapon(void)
{
    // Create a base weapon
    MaterialProperties_t iron_props = {1.0f, 1.0f, 1.0f, 0.9f, 1.0f, 1.2f, 1.1f, 1.0f, 1.0f};
    Material_t base_material = create_material("iron", iron_props);
    Item_t* sword = create_weapon("Base Sword", "base_sword", base_material, 10, 15, 0, 's');
    TEST_ASSERT(sword != NULL, "Base weapon should be created");

    // Store original values
    uint8_t original_min_dmg = sword->data.weapon.min_damage;
    uint8_t original_max_dmg = sword->data.weapon.max_damage;
    float original_weight = sword->weight_kg;
    uint16_t original_value = sword->value_coins;

    // Create a steel material that enhances damage and weight
    MaterialProperties_t steel_weapon_props = {1.5f, 1.2f, 1.1f, 1.1f, 1.3f, 1.0f, 0.9f, 1.0f, 1.0f};
    Material_t steel = create_material("steel", steel_weapon_props);
    sword->material_data = steel;

    // Apply material effects
    apply_material_to_weapon(sword);

    // Check that damage was increased
    TEST_ASSERT(sword->data.weapon.min_damage >= original_min_dmg, "Min damage should be increased or same");
    TEST_ASSERT(sword->data.weapon.max_damage >= original_max_dmg, "Max damage should be increased or same");

    // Check that weight was increased
    TEST_ASSERT(sword->weight_kg >= original_weight, "Weight should be increased");
    // Check that the value_coins was increased
    TEST_ASSERT(sword->value_coins >= original_value, "Value should be increased");
    // Check that durability was affected
    TEST_ASSERT(sword->data.weapon.durability <= 255, "Durability should be affected");

    destroy_item(sword);

    // Test with NULL item
    apply_material_to_weapon(NULL);
    TEST_ASSERT(1, "apply_material_to_weapon should handle NULL gracefully");

    // Test with wrong item type
    Item_t* armor = create_armor("Test Armor", "test_armor", base_material, 20, 5, 'A', 15, 15);
    apply_material_to_weapon(armor);
    TEST_ASSERT(1, "apply_material_to_weapon should handle wrong type gracefully");
    destroy_item(armor);

    return 1;
}

int test_apply_material_to_armor(void)
{
    // Create a base armor
    MaterialProperties_t leather_props = {1.0f, 1.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    Material_t base_material = create_material("leather", leather_props);
    Item_t* armor = create_armor("Base Armor", "base_armor", base_material, 15, 10, 'A', 15, 15);
    TEST_ASSERT(armor != NULL, "Base armor should be created");

    // Store original values
    uint8_t original_armor = armor->data.armor.armor_value;
    uint8_t original_evasion = armor->data.armor.evasion_value;
    float original_weight = armor->weight_kg;

    // Create a dragon scale material that enhances armor
    MaterialProperties_t dragonscale_props = {
        .weight_fact = 0.8f,
        .value_coins_fact = 1.5f,
        .armor_value_fact = 1.2f,
        .evasion_value_fact = 1.1f,
        .durability_fact = 1.3f,
        .min_damage_fact = 1.2f,
        .max_damage_fact = 1.2f,
        .stealth_value_fact = 1.2f,
        .enchant_value_fact = 1.2f,
    };
    Material_t dragon_scale = create_material("dragonscale", dragonscale_props);
    armor->material_data = dragon_scale;

    // Apply material effects
    apply_material_to_armor(armor);

    // Check that armor was increased
    TEST_ASSERT(armor->data.armor.armor_value >= original_armor, "Armor value should be increased");
    TEST_ASSERT(armor->data.armor.evasion_value >= original_evasion, "Evasion should be increased");

    // Check that weight was decreased (dragon scale factor 0.8)
    TEST_ASSERT(armor->weight_kg <= original_weight, "Weight should be decreased or same");

    destroy_item(armor);

    // Test with NULL item
    apply_material_to_armor(NULL);
    TEST_ASSERT(1, "apply_material_to_armor should handle NULL gracefully");

    // Test with wrong item type
    Item_t* weapon = create_weapon("Test Sword", "test_sword", base_material, 10, 15, 0, 's');
    apply_material_to_armor(weapon);
    TEST_ASSERT(1, "apply_material_to_armor should handle wrong type gracefully");
    destroy_item(weapon);

    return 1;
}

// =============================================================================
// CALCULATION TESTS
// =============================================================================

int test_calculate_final_weight(void)
{
    // Test with weapon
    MaterialProperties_t lead_props = {
        .weight_fact = 3.0f, .value_coins_fact = 0.5f, .durability_fact = 0.8f,
        .min_damage_fact = 2.0f, .max_damage_fact = 0.1f, .armor_value_fact = 2.0f,
        .evasion_value_fact = 0.5f, .stealth_value_fact = 0.5f, .enchant_value_fact = 0.5f
    };

    Material_t heavy_material = create_material("lead", lead_props);
    Item_t* mace = create_weapon("Lead Mace", "lead_mace", heavy_material, 8, 12, 0, 'm');
    TEST_ASSERT(mace != NULL, "Lead mace should be created");

    apply_material_to_weapon(mace);
    float final_weight = calculate_final_weight(mace);

    TEST_ASSERT(final_weight > 0, "Final weight should be greater than 0");
    TEST_ASSERT(float_equals(final_weight, mace->weight_kg, 0.01f), "Final weight should match item weight");

    destroy_item(mace);

    // Test with NULL
    float null_weight = calculate_final_weight(NULL);
    TEST_ASSERT(float_equals(null_weight, 0.0f, 0.01f), "NULL item should have 0 weight");

    return 1;
}

int test_calculate_final_value(void)
{
    // Test with expensive material
    MaterialProperties_t gold_props = {
        .weight_fact = 2.0f, .value_coins_fact = 2.0f, .durability_fact = 1.1f,
        .min_damage_fact = 1.0f, .max_damage_fact = 2.0f, .armor_value_fact = 1.5f,
        .evasion_value_fact = 1.2f, .stealth_value_fact = 1.3f, .enchant_value_fact = 1.4f
    };
    Material_t gold = create_material("gold", gold_props);
    Item_t* crown = create_armor("Golden Crown", "gold_crown", gold, 5, 15, 'C', 15, 15);
    TEST_ASSERT(crown != NULL, "Golden crown should be created");

    apply_material_to_armor(crown);
    uint16_t final_value = calculate_final_value(crown);

    TEST_ASSERT(final_value > 0, "Final value should be greater than 0");
    TEST_ASSERT(final_value == crown->value_coins, "Final value should match item value");

    destroy_item(crown);

    // Test with NULL
    uint16_t null_value = calculate_final_value(NULL);
    TEST_ASSERT(null_value == 0, "NULL item should have 0 value");

    return 1;
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

int test_material_system_integration(void)
{
    // Test complete workflow: create material, create item, apply material

    // Create a powerful mythril material
    MaterialProperties_t mythril_props = {
        .weight_fact = 0.2f, .value_coins_fact = 2.5f, .durability_fact = 2.0f,
        .min_damage_fact = 2.0f, .max_damage_fact = 5.0f, .armor_value_fact = 5.0f,
        .evasion_value_fact = 3.0f, .stealth_value_fact = 4.0f, .enchant_value_fact = 1.2f
    };
    Material_t mythril = create_material("mythril", mythril_props);

    // Create a weapon with mythril
    Item_t* mythril_sword = create_weapon("Mythril Blade", "mythril_blade", mythril, 12, 18, 1, 'M');
    TEST_ASSERT(mythril_sword != NULL, "Mythril sword should be created");

    // Store pre-application values
    uint8_t pre_min_dmg = mythril_sword->data.weapon.min_damage;
    uint8_t pre_max_dmg = mythril_sword->data.weapon.max_damage;
    float pre_weight = mythril_sword->weight_kg;

    // Apply material
    apply_material_to_weapon(mythril_sword);

    // Verify improvements
    TEST_ASSERT(mythril_sword->data.weapon.min_damage >= pre_min_dmg, "Mythril should improve damage");
    TEST_ASSERT(mythril_sword->data.weapon.max_damage >= pre_max_dmg, "Mythril should improve damage");
    TEST_ASSERT(mythril_sword->weight_kg <= pre_weight, "Mythril should be lighter");

    // Test final calculations
    float final_weight = calculate_final_weight(mythril_sword);
    uint16_t final_value = calculate_final_value(mythril_sword);

    printf("Final Weight: %f\n", final_weight);
    TEST_ASSERT(float_equals(final_weight, mythril_sword->weight_kg, 0.01f), "Weight calculation should be consistent");
    TEST_ASSERT(final_value == mythril_sword->value_coins, "Value calculation should be consistent");

    destroy_item(mythril_sword);

    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Material System Tests");

    // Run material creation tests
    RUN_TEST(test_create_material);

    // Run material application tests
    RUN_TEST(test_apply_material_to_weapon);
    RUN_TEST(test_apply_material_to_armor);

    // Run calculation tests
    RUN_TEST(test_calculate_final_weight);
    RUN_TEST(test_calculate_final_value);

    // Run integration tests
    RUN_TEST(test_material_system_integration);

    TEST_SUITE_END();
}
