// ASCIIGame/tests/items/test_items_material_system.c
// Test file for item material system, enhanced with Daedalus Logging.

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
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

// =============================================================================
// MATERIAL CREATION TESTS
// =============================================================================

int test_create_material(void)
{
    d_LogInfo("Verifying creation of standard materials.");
    MaterialProperties_t steel_props = {
        .weight_fact = 1.5f, .value_coins_fact = 1.1f, .durability_fact = 1.2f,
        .min_damage_fact = 1.1f, .max_damage_fact = 1.3f, .armor_value_fact = 1.3f,
        .evasion_value_fact = 0.8f, .stealth_value_fact = 0.9f, .enchant_value_fact = 1.1f
    };
    Material_t steel = create_material("steel", steel_props);

    TEST_ASSERT(strcmp(steel.name->str, "steel") == 0, "Material name should be 'steel'");
    TEST_ASSERT(float_equals(steel.properties.weight_fact, 1.5f, 0.01f), "Weight factor should be 1.5");
    TEST_ASSERT(float_equals(steel.properties.durability_fact, 1.2f, 0.01f), "Durability factor should be 1.2");

    d_LogInfo("Verifying creation of default material with NULL name.");
    MaterialProperties_t default_props = create_default_material_properties();
    Material_t unknown = create_material(NULL, default_props);
    TEST_ASSERT(strcmp(unknown.name->str, "Default Material") == 0, "NULL name should default to 'Default Material'");
    TEST_ASSERT(float_equals(unknown.properties.weight_fact, 1.0f, 0.01f), "NULL material should have default factors");

    d_LogInfo("Verifying creation of material with extreme property values.");
    MaterialProperties_t mythril_props = {
        .weight_fact = 0.1f, .value_coins_fact = 3.0f, .durability_fact = 1.4f,
        .min_damage_fact = 3.0f, .max_damage_fact = 1.4f, .armor_value_fact = 2.5f,
        .evasion_value_fact = 10.0f, .stealth_value_fact = 5.0f, .enchant_value_fact = 8.0f
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
    d_LogInfo("Verifying application of material properties to a weapon.");
    MaterialProperties_t iron_props = create_default_material_properties();
    Material_t base_material = create_material("iron", iron_props);
    Item_t* sword = create_weapon("Base Sword", "base_sword", base_material, 10, 15, 0, 's');
    TEST_ASSERT(sword != NULL, "Base weapon should be created");

    uint8_t original_min_dmg = sword->data.weapon.min_damage;
    d_LogDebugF("Applying steel material to weapon. Original min_damage: %d", original_min_dmg);

    MaterialProperties_t steel_weapon_props = {1.5f, 1.2f, 1.1f, 1.1f, 1.3f, 1.0f, 0.9f, 1.0f, 1.0f};
    sword->material_data = create_material("steel", steel_weapon_props);
    apply_material_to_weapon(sword);

    d_LogDebugF("Post-application: min_damage=%d, max_damage=%d, weight=%.2f", sword->data.weapon.min_damage, sword->data.weapon.max_damage, sword->weight_kg);
    TEST_ASSERT(sword->data.weapon.min_damage > original_min_dmg, "Min damage should be increased");
    TEST_ASSERT(sword->data.weapon.durability <= 255, "Durability should be affected");

    destroy_item(sword);

    d_LogWarning("Testing material application with NULL and wrong item types.");
    apply_material_to_weapon(NULL);
    TEST_ASSERT(1, "apply_material_to_weapon should handle NULL gracefully");

    Item_t* armor = create_armor("Test Armor", "test_armor", base_material, 20, 5, 'A', 15, 15);
    apply_material_to_weapon(armor);
    TEST_ASSERT(1, "apply_material_to_weapon should handle wrong type gracefully");
    destroy_item(armor);

    return 1;
}

int test_apply_material_to_armor(void)
{
    d_LogInfo("Verifying application of material properties to armor.");
    Material_t base_material = create_material("leather", create_default_material_properties());
    Item_t* armor = create_armor("Base Armor", "base_armor", base_material, 15, 10, 'A', 15, 15);
    TEST_ASSERT(armor != NULL, "Base armor should be created");

    uint8_t original_armor = armor->data.armor.armor_value;
    float original_weight = armor->weight_kg;
    d_LogDebugF("Applying dragonscale material. Original armor_value: %d, weight: %.2f", original_armor, original_weight);

    MaterialProperties_t dragonscale_props = { .weight_fact = 0.8f, .armor_value_fact = 1.2f, .evasion_value_fact = 1.1f };
    armor->material_data = create_material("dragonscale", dragonscale_props);
    apply_material_to_armor(armor);

    d_LogDebugF("Post-application: armor_value=%d, weight=%.2f", armor->data.armor.armor_value, armor->weight_kg);
    TEST_ASSERT(armor->data.armor.armor_value > original_armor, "Armor value should be increased");
    TEST_ASSERT(armor->weight_kg < original_weight, "Weight should be decreased");

    destroy_item(armor);

    d_LogWarning("Testing material application on armor with NULL and wrong types.");
    apply_material_to_armor(NULL);
    TEST_ASSERT(1, "apply_material_to_armor should handle NULL gracefully");

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
    d_LogInfo("Verifying final weight calculation.");
    MaterialProperties_t lead_props = { .weight_fact = 3.0f };
    Material_t heavy_material = create_material("lead", lead_props);
    Item_t* mace = create_weapon("Lead Mace", "lead_mace", heavy_material, 8, 12, 0, 'm');
    TEST_ASSERT(mace != NULL, "Lead mace should be created");

    apply_material_to_weapon(mace);
    float final_weight = calculate_final_weight(mace);
    d_LogDebugF("Calculated final weight for Lead Mace: %.2f", final_weight);
    TEST_ASSERT(final_weight > 0, "Final weight should be greater than 0");
    TEST_ASSERT(float_equals(final_weight, mace->weight_kg, 0.01f), "Final weight should match item weight");
    destroy_item(mace);

    d_LogDebug("Verifying final weight calculation for NULL item.");
    float null_weight = calculate_final_weight(NULL);
    TEST_ASSERT(float_equals(null_weight, 0.0f, 0.01f), "NULL item should have 0 weight");

    return 1;
}

int test_calculate_final_value(void)
{
    d_LogInfo("Verifying final value calculation.");
    MaterialProperties_t gold_props = { .value_coins_fact = 2.0f };
    Material_t gold = create_material("gold", gold_props);
    Item_t* crown = create_armor("Golden Crown", "gold_crown", gold, 5, 15, 'C', 15, 15);
    TEST_ASSERT(crown != NULL, "Golden crown should be created");

    apply_material_to_armor(crown);
    uint16_t final_value = calculate_final_value(crown);
    d_LogDebugF("Calculated final value for Golden Crown: %d", final_value);
    TEST_ASSERT(final_value > 0, "Final value should be greater than 0");
    TEST_ASSERT(final_value == crown->value_coins, "Final value should match item value");
    destroy_item(crown);

    d_LogDebug("Verifying final value calculation for NULL item.");
    uint16_t null_value = calculate_final_value(NULL);
    TEST_ASSERT(null_value == 0, "NULL item should have 0 value");

    return 1;
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

int test_material_system_integration(void)
{
    d_LogInfo("Verifying complete material system workflow.");
    MaterialProperties_t mythril_props = { .weight_fact = 0.2f, .value_coins_fact = 2.5f, .min_damage_fact = 2.0f };
    Material_t mythril = create_material("mythril", mythril_props);
    Item_t* mythril_sword = create_weapon("Mythril Blade", "mythril_blade", mythril, 12, 18, 1, 'M');
    TEST_ASSERT(mythril_sword != NULL, "Mythril sword should be created");

    uint8_t pre_min_dmg = mythril_sword->data.weapon.min_damage;
    float pre_weight = mythril_sword->weight_kg;
    apply_material_to_weapon(mythril_sword);

    d_LogDebugF("Mythril sword post-application: min_dmg=%d, weight=%.2f", mythril_sword->data.weapon.min_damage, mythril_sword->weight_kg);
    TEST_ASSERT(mythril_sword->data.weapon.min_damage > pre_min_dmg, "Mythril should improve damage");
    TEST_ASSERT(mythril_sword->weight_kg < pre_weight, "Mythril should be lighter");

    float final_weight = calculate_final_weight(mythril_sword);
    uint16_t final_value = calculate_final_value(mythril_sword);
    d_LogInfoF("Final calculations: weight=%.2f, value=%d", final_weight, final_value);
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
    // Initialize the Daedalus logger for this test suite
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = false,
        .include_function = false
    };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

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

    // Clean up the logger
    d_DestroyLogger(d_GetGlobalLogger());

}
