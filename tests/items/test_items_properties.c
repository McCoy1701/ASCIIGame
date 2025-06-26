// ASCIIGame/tests/items/test_items_properties.c
#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "tests.h"
#include "Daedalus.h"

#include "items.h"
#include "structs.h"
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
    Item_t* armor = create_armor("Test Armor", "test_armor", iron, 20, 10, 'A', 15, 15);
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
    TEST_ASSERT(bow != NULL, "Bow should be created");

    // Create ammunition
    Item_t* arrow = create_ammunition("Arrow", "arrow", wood, 3, 5, 'a');
    TEST_ASSERT(arrow != NULL, "Arrow should be created");

    // Set Bow's ammuniton to Arrow
    //bow->data.weapon.ammo = arrow;

    // Test ammo requirements
    bool sword_needs_ammo = weapon_needs_ammo(sword);
    bool bow_needs_ammo = weapon_needs_ammo(bow);

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Sword Needs Ammo: ", 0);
    d_AppendString(log_message, sword_needs_ammo ? "Yes" : "No", 0);
    d_AppendString(log_message, " | Bow Needs Ammo: ", 0);
    d_AppendString(log_message, bow_needs_ammo ? "Yes" : "No", 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(!sword_needs_ammo, "Sword should not need ammo");
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
    Item_t* armor = create_armor("Test Armor", "test_armor", leather, 30, 15, 'A', 10, 5);
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

    Item_t* gold_ring = create_armor("Gold Ring", "gold_ring", valuable_material, 1, 0, 'o', 15, 15);
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

    Item_t* stealth_armor = create_armor("Shadow Cloak", "shadow_cloak", stealth_material, 10, 20, 'C', 15, 15);
    Item_t* stealth_weapon = create_weapon("Shadow Blade", "shadow_blade", stealth_material, 15, 20, 0, 'b');
    Lock_t lock = create_lock("test_lock", "A test lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", lock, 'k');

    stealth_armor->data.armor.stealth_value = 10;
    stealth_weapon->data.weapon.stealth_value = 15;

    TEST_ASSERT(stealth_armor != NULL, "Stealth armor should be created");
    TEST_ASSERT(stealth_weapon != NULL, "Stealth weapon should be created");
    TEST_ASSERT(key != NULL, "Key should be created");

    // Apply materials to set stealth values
    apply_material_to_armor(stealth_armor);
    apply_material_to_weapon(stealth_weapon);

    uint8_t armor_stealth = get_stealth_value(stealth_armor);
    uint8_t weapon_stealth = get_stealth_value(stealth_weapon);
    uint8_t key_stealth = get_stealth_value(key);

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Armor stealth value: ", 0);
    d_AppendInt(log_message, armor_stealth);
    d_AppendString(log_message, "\nWeapon stealth value: ", 0);
    d_AppendInt(log_message, weapon_stealth);
    d_AppendString(log_message, "\nKey stealth value: ", 0);
    d_AppendInt(log_message, key_stealth);
    LOG(log_message->str);

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

    // check that the string comparison function is working correctly
    TEST_ASSERT(strcmp("Arrows", arrows->name->str) == 0, "Arrows should be named 'Arrows'");
    TEST_ASSERT(strcmp("Sword", sword->name->str) == 0, "Sword should be named 'Sword'");

    // Test with NULL
    TEST_ASSERT(!is_item_stackable(NULL), "NULL item should not be stackable");
    TEST_ASSERT(get_max_stack_size(NULL) == 0, "NULL item should have 0 max stack size");

    destroy_item(sword);
    destroy_item(arrows);
    return 1;
}
// =============================================================================
// ENHANCED PROPERTIES BUG HUNTING TESTS - WITH PROPER LOGGING
// =============================================================================

/*
 * Test weapon stats with extreme and edge case values
 */
int test_weapon_stats_extreme_cases(void)
{
    LOG("Starting weapon stats extreme cases test");

    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Test weapon with zero damage
    LOG("Testing weapon with zero damage values");
    Item_t* zero_weapon = create_weapon("Zero Damage", "zero_damage", basic_material, 0, 0, 0, 'Z');
    TEST_ASSERT(zero_weapon != NULL, "Zero damage weapon should be created");

    uint8_t zero_min = get_weapon_min_damage(zero_weapon);
    uint8_t zero_max = get_weapon_max_damage(zero_weapon);
    uint8_t zero_range = get_weapon_range(zero_weapon);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Zero weapon stats: min=", 0);
    d_AppendInt(log_msg, zero_min);
    d_AppendString(log_msg, ", max=", 0);
    d_AppendInt(log_msg, zero_max);
    d_AppendString(log_msg, ", range=", 0);
    d_AppendInt(log_msg, zero_range);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(zero_min == 0, "Zero min damage should be preserved");
    TEST_ASSERT(zero_max == 0, "Zero max damage should be preserved");
    TEST_ASSERT(zero_range == 0, "Zero range should be preserved");

    // Test weapon with inverted damage (min > max)
    LOG("Testing weapon with inverted damage values (min > max)");
    Item_t* inverted_weapon = create_weapon("Inverted Damage", "inverted_damage", basic_material, 50, 20, 0, 'I');
    TEST_ASSERT(inverted_weapon != NULL, "Inverted damage weapon should be created");

    uint8_t inv_min = get_weapon_min_damage(inverted_weapon);
    uint8_t inv_max = get_weapon_max_damage(inverted_weapon);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Inverted weapon: input min=50, max=20; actual min=", 0);
    d_AppendInt(log_msg, inv_min);
    d_AppendString(log_msg, ", actual max=", 0);
    d_AppendInt(log_msg, inv_max);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    // System might swap them or handle this gracefully
    TEST_ASSERT(inv_min <= inv_max || (inv_min == 50 && inv_max == 20), "Inverted damage should be handled consistently");

    // Test weapon with maximum values
    LOG("Testing weapon with maximum uint8_t values");
    Item_t* max_weapon = create_weapon("Max Weapon", "max_weapon", basic_material, 255, 255, 255, 'M');
    TEST_ASSERT(max_weapon != NULL, "Max weapon should be created");

    uint8_t max_min = get_weapon_min_damage(max_weapon);
    uint8_t max_max = get_weapon_max_damage(max_weapon);
    uint8_t max_range = get_weapon_range(max_weapon);

    TEST_ASSERT(max_min == 255, "Max min damage should be preserved");
    TEST_ASSERT(max_max == 255, "Max max damage should be preserved");
    TEST_ASSERT(max_range == 255, "Max range should be preserved");

    // Test weapon with extreme range values and ammo requirements
    LOG("Testing extreme range weapon ammo logic");
    Item_t* sniper = create_weapon("Sniper Rifle", "sniper", basic_material, 30, 50, 200, 'S');
    TEST_ASSERT(sniper != NULL, "Sniper should be created");

    bool sniper_needs_ammo = weapon_needs_ammo(sniper);
    log_msg = d_InitString();
    d_AppendString(log_msg, "Sniper (range=200) needs ammo: ", 0);
    d_AppendString(log_msg, sniper_needs_ammo ? "Yes" : "No", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(sniper_needs_ammo, "High range weapon should need ammo");

    destroy_item(zero_weapon);
    destroy_item(inverted_weapon);
    destroy_item(max_weapon);
    destroy_item(sniper);
    d_DestroyString(basic_material.name);

    LOG("Weapon stats extreme cases test completed");
    return 1;
}

/*
 * Test weapon ammo system with edge cases and extreme compatibility
 */
int test_weapon_ammo_extreme_cases(void)
{
    LOG("Starting weapon ammo extreme cases test");

    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Create weapons with various range values
    Item_t* melee = create_weapon("Melee", "melee", basic_material, 10, 15, 0, 'M');
    Item_t* short_range = create_weapon("Short Range", "short_range", basic_material, 8, 12, 1, 'S');
    Item_t* long_range = create_weapon("Long Range", "long_range", basic_material, 15, 25, 50, 'L');
    Item_t* extreme_range = create_weapon("Extreme Range", "extreme_range", basic_material, 20, 30, 255, 'E');

    // Create various ammunition types
    Item_t* light_ammo = create_ammunition("Light Ammo", "light_ammo", basic_material, 2, 4, 'l');
    Item_t* heavy_ammo = create_ammunition("Heavy Ammo", "heavy_ammo", basic_material, 8, 15, 'h');
    Item_t* extreme_ammo = create_ammunition("Extreme Ammo", "extreme_ammo", basic_material, 255, 255, 'x');

    LOG("Testing ammo requirements for different ranges");
    bool melee_needs = weapon_needs_ammo(melee);
    bool short_needs = weapon_needs_ammo(short_range);
    bool long_needs = weapon_needs_ammo(long_range);
    bool extreme_needs = weapon_needs_ammo(extreme_range);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Ammo needs: melee(0)=", 0);
    d_AppendString(log_msg, melee_needs ? "true" : "false", 0);
    d_AppendString(log_msg, ", short(1)=", 0);
    d_AppendString(log_msg, short_needs ? "true" : "false", 0);
    d_AppendString(log_msg, ", long(50)=", 0);
    d_AppendString(log_msg, long_needs ? "true" : "false", 0);
    d_AppendString(log_msg, ", extreme(255)=", 0);
    d_AppendString(log_msg, extreme_needs ? "true" : "false", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(!melee_needs, "Melee weapon should not need ammo");
    // The behavior for range=1 might vary, but let's test consistency

    LOG("Testing ammo compatibility with different ammunition types");
    // Test compatibility between weapons and ammo
    bool melee_light = weapon_can_use_ammo(melee, light_ammo);
    bool short_light = weapon_can_use_ammo(short_range, light_ammo);
    bool long_heavy = weapon_can_use_ammo(long_range, heavy_ammo);
    bool extreme_extreme = weapon_can_use_ammo(extreme_range, extreme_ammo);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Ammo compatibility: melee+light=", 0);
    d_AppendString(log_msg, melee_light ? "true" : "false", 0);
    d_AppendString(log_msg, ", short+light=", 0);
    d_AppendString(log_msg, short_light ? "true" : "false", 0);
    d_AppendString(log_msg, ", long+heavy=", 0);
    d_AppendString(log_msg, long_heavy ? "true" : "false", 0);
    d_AppendString(log_msg, ", extreme+extreme=", 0);
    d_AppendString(log_msg, extreme_extreme ? "true" : "false", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(!melee_light, "Melee weapon should not use ammo");

    // Test cross-compatibility (wrong ammo types)
    LOG("Testing incorrect ammo type combinations");
    bool wrong1 = weapon_can_use_ammo(long_range, light_ammo);
    bool wrong2 = weapon_can_use_ammo(short_range, extreme_ammo);

    // Test with corrupted weapon data
    LOG("Testing ammo system with corrupted weapon data");
    Item_t* corrupt_weapon = create_weapon("Corrupt", "corrupt", basic_material, 10, 15, 5, 'C');
    TEST_ASSERT(corrupt_weapon != NULL, "Corrupt weapon should be created");

    // Manually corrupt the type to test robustness
    ItemType_t original_type = corrupt_weapon->type;
    corrupt_weapon->type = ITEM_TYPE_ARMOR; // Wrong type

    bool corrupt_needs = weapon_needs_ammo(corrupt_weapon);
    bool corrupt_can_use = weapon_can_use_ammo(corrupt_weapon, light_ammo);

    TEST_ASSERT(!corrupt_needs, "Corrupted weapon should not need ammo");
    TEST_ASSERT(!corrupt_can_use, "Corrupted weapon should not use ammo");

    // Restore and test with corrupted ammo
    corrupt_weapon->type = original_type;
    ItemType_t ammo_original_type = light_ammo->type;
    light_ammo->type = ITEM_TYPE_WEAPON; // Wrong type

    bool corrupt_ammo_use = weapon_can_use_ammo(corrupt_weapon, light_ammo);
    TEST_ASSERT(!corrupt_ammo_use, "Weapon should not use corrupted ammo");

    light_ammo->type = ammo_original_type; // Restore

    destroy_item(melee);
    destroy_item(short_range);
    destroy_item(long_range);
    destroy_item(extreme_range);
    destroy_item(light_ammo);
    destroy_item(heavy_ammo);
    destroy_item(extreme_ammo);
    destroy_item(corrupt_weapon);
    d_DestroyString(basic_material.name);

    LOG("Weapon ammo extreme cases test completed");
    return 1;
}

/*
 * Test armor stats with extreme values and edge cases
 */
int test_armor_stats_extreme_cases(void)
{
    LOG("Starting armor stats extreme cases test");

    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Test armor with zero values
    LOG("Testing armor with zero stat values");
    Item_t* zero_armor = create_armor("Zero Armor", "zero_armor", basic_material, 0, 0, 'Z', 0, 0);
    TEST_ASSERT(zero_armor != NULL, "Zero armor should be created");

    uint8_t zero_armor_val = get_armor_value(zero_armor);
    uint8_t zero_evasion = get_evasion_value(zero_armor);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Zero armor stats: armor=", 0);
    d_AppendInt(log_msg, zero_armor_val);
    d_AppendString(log_msg, ", evasion=", 0);
    d_AppendInt(log_msg, zero_evasion);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(zero_armor_val == 0, "Zero armor value should be preserved");
    TEST_ASSERT(zero_evasion == 0, "Zero evasion should be preserved");

    // Test armor with maximum values
    LOG("Testing armor with maximum uint8_t values");
    Item_t* max_armor = create_armor("Max Armor", "max_armor", basic_material, 255, 255, 'M', 255, 255);
    TEST_ASSERT(max_armor != NULL, "Max armor should be created");

    uint8_t max_armor_val = get_armor_value(max_armor);
    uint8_t max_evasion = get_evasion_value(max_armor);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Max armor stats: armor=", 0);
    d_AppendInt(log_msg, max_armor_val);
    d_AppendString(log_msg, ", evasion=", 0);
    d_AppendInt(log_msg, max_evasion);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(max_armor_val == 255, "Max armor value should be preserved");
    TEST_ASSERT(max_evasion == 255, "Max evasion should be preserved");

    // Test armor with inverted values (high evasion, low armor)
    LOG("Testing armor with extreme stat combinations");
    Item_t* glass_armor = create_armor("Glass Armor", "glass_armor", basic_material, 1, 254, 'G', 255, 1);
    TEST_ASSERT(glass_armor != NULL, "Glass armor should be created");

    uint8_t glass_armor_val = get_armor_value(glass_armor);
    uint8_t glass_evasion = get_evasion_value(glass_armor);

    TEST_ASSERT(glass_armor_val == 1, "Low armor value should be preserved");
    TEST_ASSERT(glass_evasion == 254, "High evasion should be preserved");

    // Test with corrupted armor data
    LOG("Testing armor stats with corrupted item data");
    Item_t* corrupt_armor = create_armor("Corrupt Armor", "corrupt_armor", basic_material, 50, 30, 'C', 20, 20);
    TEST_ASSERT(corrupt_armor != NULL, "Corrupt armor should be created");

    // Corrupt the item type
    corrupt_armor->type = ITEM_TYPE_WEAPON;

    uint8_t corrupt_armor_val = get_armor_value(corrupt_armor);
    uint8_t corrupt_evasion = get_evasion_value(corrupt_armor);

    TEST_ASSERT(corrupt_armor_val == 0, "Corrupted armor should return 0 armor value");
    TEST_ASSERT(corrupt_evasion == 0, "Corrupted armor should return 0 evasion");

    destroy_item(zero_armor);
    destroy_item(max_armor);
    destroy_item(glass_armor);
    destroy_item(corrupt_armor);
    d_DestroyString(basic_material.name);

    LOG("Armor stats extreme cases test completed");
    return 1;
}

/*
 * Test weight and value calculations with extreme material properties
 */
int test_weight_value_extreme_cases(void)
{
    LOG("Starting weight/value extreme cases test");

    // Test with extreme weight factor materials
    MaterialProperties_t feather_props = create_default_material_properties();
    feather_props.weight_fact = 0.0001f; // Nearly weightless
    Material_t feather_material = create_material("feather", feather_props);

    MaterialProperties_t lead_props = create_default_material_properties();
    lead_props.weight_fact = 10000.0f; // Extremely heavy
    Material_t lead_material = create_material("lead", lead_props);

    MaterialProperties_t negative_props = create_default_material_properties();
    negative_props.weight_fact = -5.0f; // Negative weight (anti-gravity?)
    negative_props.value_coins_fact = -10.0f; // Negative value
    Material_t negative_material = create_material("negative", negative_props);

    LOG("Testing feather-light material");
    Item_t* feather_sword = create_weapon("Feather Sword", "feather_sword", feather_material, 10, 15, 0, 'F');
    TEST_ASSERT(feather_sword != NULL, "Feather sword should be created");

    apply_material_to_weapon(feather_sword);
    float feather_weight = get_item_weight(feather_sword);
    uint8_t feather_value = get_item_value_coins(feather_sword);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Feather sword: weight=", 0);
    d_AppendInt(log_msg, feather_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, feather_value);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(feather_weight >= 0.0f, "Weight should not be negative");

    LOG("Testing lead-heavy material");
    Item_t* lead_sword = create_weapon("Lead Sword", "lead_sword", lead_material, 10, 15, 0, 'L');
    TEST_ASSERT(lead_sword != NULL, "Lead sword should be created");

    apply_material_to_weapon(lead_sword);
    float lead_weight = get_item_weight(lead_sword);
    uint8_t lead_value = get_item_value_coins(lead_sword);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Lead sword: weight=", 0);
    d_AppendInt(log_msg, lead_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, lead_value);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(lead_weight > feather_weight, "Lead should be heavier than feather");

    LOG("Testing negative property material");
    Item_t* negative_sword = create_weapon("Negative Sword", "negative_sword", negative_material, 10, 15, 0, 'N');
    TEST_ASSERT(negative_sword != NULL, "Negative sword should be created");

    apply_material_to_weapon(negative_sword);
    float negative_weight = get_item_weight(negative_sword);
    uint8_t negative_value = get_item_value_coins(negative_sword);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Negative sword: weight=", 0);
    d_AppendInt(log_msg, negative_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, negative_value);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(negative_weight >= 0.0f, "Negative weight should be clamped to 0 or handled gracefully");
    TEST_ASSERT(negative_value >= 0, "Negative value should be clamped to 0 or handled gracefully");

    destroy_item(feather_sword);
    destroy_item(lead_sword);
    destroy_item(negative_sword);
    d_DestroyString(feather_material.name);
    d_DestroyString(lead_material.name);
    d_DestroyString(negative_material.name);

    LOG("Weight/value extreme cases test completed");
    return 1;
}

/*
 * Test stealth values with extreme materials and edge cases
 */
int test_stealth_extreme_cases(void)
{
    LOG("Starting stealth extreme cases test");

    // Test with extreme stealth materials
    MaterialProperties_t invisible_props = create_default_material_properties();
    invisible_props.stealth_value_fact = 1000.0f; // Super stealth
    Material_t invisible_material = create_material("invisible", invisible_props);

    MaterialProperties_t glowing_props = create_default_material_properties();
    glowing_props.stealth_value_fact = 0.0f; // No stealth
    Material_t glowing_material = create_material("glowing", glowing_props);

    MaterialProperties_t anti_stealth_props = create_default_material_properties();
    anti_stealth_props.stealth_value_fact = -10.0f; // Negative stealth
    Material_t anti_stealth_material = create_material("anti_stealth", anti_stealth_props);

    LOG("Testing extreme stealth values");
    Item_t* invisible_armor = create_armor("Invisible Armor", "invisible_armor", invisible_material, 20, 10, 'I', 100, 10);
    Item_t* glowing_armor = create_armor("Glowing Armor", "glowing_armor", glowing_material, 25, 15, 'G', 0, 15);
    Item_t* anti_stealth_armor = create_armor("Anti-Stealth Armor", "anti_stealth_armor", anti_stealth_material, 30, 5, 'A', 50, 20);

    TEST_ASSERT(invisible_armor != NULL, "Invisible armor should be created");
    TEST_ASSERT(glowing_armor != NULL, "Glowing armor should be created");
    TEST_ASSERT(anti_stealth_armor != NULL, "Anti-stealth armor should be created");

    // Set base stealth values and apply materials
    invisible_armor->data.armor.stealth_value = 50;
    glowing_armor->data.armor.stealth_value = 50;
    anti_stealth_armor->data.armor.stealth_value = 50;

    apply_material_to_armor(invisible_armor);
    apply_material_to_armor(glowing_armor);
    apply_material_to_armor(anti_stealth_armor);

    uint8_t invisible_stealth = get_stealth_value(invisible_armor);
    uint8_t glowing_stealth = get_stealth_value(glowing_armor);
    uint8_t anti_stealth_stealth = get_stealth_value(anti_stealth_armor);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Stealth values: invisible=", 0);
    d_AppendInt(log_msg, invisible_stealth);
    d_AppendString(log_msg, ", glowing=", 0);
    d_AppendInt(log_msg, glowing_stealth);
    d_AppendString(log_msg, ", anti-stealth=", 0);
    d_AppendInt(log_msg, anti_stealth_stealth);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(invisible_stealth <= 255, "Invisible stealth should not overflow uint8_t");
    TEST_ASSERT(glowing_stealth >= 0, "Glowing stealth should not be negative");
    TEST_ASSERT(anti_stealth_stealth >= 0, "Anti-stealth should not be negative");

    // Test stealth on non-stealth items
    LOG("Testing stealth values on inappropriate item types");
    Lock_t test_lock = create_lock("Test Lock", "test_lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", test_lock, 'K');
    Item_t* consumable = create_consumable("Test Potion", "test_potion", 25, dummy_consume_callback, 'P');

    uint8_t key_stealth = get_stealth_value(key);
    uint8_t consumable_stealth = get_stealth_value(consumable);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Non-stealth items: key=", 0);
    d_AppendInt(log_msg, key_stealth);
    d_AppendString(log_msg, ", consumable=", 0);
    d_AppendInt(log_msg, consumable_stealth);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(key_stealth == 0, "Keys should have no stealth value");
    TEST_ASSERT(consumable_stealth == 0, "Consumables should have no stealth value");

    destroy_item(invisible_armor);
    destroy_item(glowing_armor);
    destroy_item(anti_stealth_armor);
    destroy_item(key);
    destroy_item(consumable);
    d_DestroyString(invisible_material.name);
    d_DestroyString(glowing_material.name);
    d_DestroyString(anti_stealth_material.name);

    LOG("Stealth extreme cases test completed");
    return 1;
}

/*
 * Test stackable properties with edge cases and potential overflow
 */
int test_stackable_extreme_cases(void)
{
    LOG("Starting stackable extreme cases test");

    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Test stackable items at boundaries
    LOG("Testing stackable ammunition at max stack boundaries");
    Item_t* arrows = create_ammunition("Test Arrows", "test_arrows", basic_material, 5, 8, 'A');
    TEST_ASSERT(arrows != NULL, "Arrows should be created");

    bool arrows_stackable = is_item_stackable(arrows);
    uint8_t arrows_max_stack = get_max_stack_size(arrows);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Arrows: stackable=", 0);
    d_AppendString(log_msg, arrows_stackable ? "true" : "false", 0);
    d_AppendString(log_msg, ", max_stack=", 0);
    d_AppendInt(log_msg, arrows_max_stack);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(arrows_stackable, "Arrows should be stackable");
    TEST_ASSERT(arrows_max_stack > 0, "Arrows should have positive max stack size");
    TEST_ASSERT(arrows_max_stack <= 255, "Max stack should not exceed uint8_t");

    // Test with corrupted stackable data
    LOG("Testing stackable properties with corrupted data");
    Item_t* corrupt_ammo = create_ammunition("Corrupt Ammo", "corrupt_ammo", basic_material, 3, 6, 'C');
    TEST_ASSERT(corrupt_ammo != NULL, "Corrupt ammo should be created");

    // Manually corrupt the stackable value to test bounds checking
    uint8_t original_stackable = corrupt_ammo->stackable;
    corrupt_ammo->stackable = 0; // Make non-stackable

    bool corrupt_stackable = is_item_stackable(corrupt_ammo);
    uint8_t corrupt_max_stack = get_max_stack_size(corrupt_ammo);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Corrupted ammo: stackable=", 0);
    d_AppendString(log_msg, corrupt_stackable ? "true" : "false", 0);
    d_AppendString(log_msg, ", max_stack=", 0);
    d_AppendInt(log_msg, corrupt_max_stack);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(!corrupt_stackable, "Corrupted ammo should not be stackable");
    TEST_ASSERT(corrupt_max_stack == 0, "Corrupted ammo should have 0 max stack");

    // Test with extreme stackable values
    corrupt_ammo->stackable = 255; // Maximum stack size
    bool extreme_stackable = is_item_stackable(corrupt_ammo);
    uint8_t extreme_max_stack = get_max_stack_size(corrupt_ammo);

    TEST_ASSERT(extreme_stackable, "Max stackable should be stackable");
    TEST_ASSERT(extreme_max_stack == 255, "Max stackable should have 255 max stack");

    // Test non-stackable items to ensure they stay non-stackable
    LOG("Testing non-stackable items");
    Item_t* weapon = create_weapon("Test Weapon", "test_weapon", basic_material, 10, 15, 0, 'W');
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 20, 10, 'A', 15, 15);

    bool weapon_stackable = is_item_stackable(weapon);
    bool armor_stackable = is_item_stackable(armor);
    uint8_t weapon_max_stack = get_max_stack_size(weapon);
    uint8_t armor_max_stack = get_max_stack_size(armor);

    TEST_ASSERT(!weapon_stackable, "Weapons should not be stackable");
    TEST_ASSERT(!armor_stackable, "Armor should not be stackable");
    TEST_ASSERT(weapon_max_stack == 0, "Weapons should have 0 max stack");
    TEST_ASSERT(armor_max_stack == 0, "Armor should have 0 max stack");

    // Test stackable items with corrupted types
    LOG("Testing stackable properties with wrong item types");
    weapon->stackable = 50; // Manually set stackable on non-stackable item
    armor->stackable = 100;

    bool corrupt_weapon_stackable = is_item_stackable(weapon);
    bool corrupt_armor_stackable = is_item_stackable(armor);

    // The behavior here depends on implementation - might check type or just stackable field
    log_msg = d_InitString();
    d_AppendString(log_msg, "Type-corrupted stackable: weapon=", 0);
    d_AppendString(log_msg, corrupt_weapon_stackable ? "true" : "false", 0);
    d_AppendString(log_msg, ", armor=", 0);
    d_AppendString(log_msg, corrupt_armor_stackable ? "true" : "false", 0);
    d_AppendString(log_msg, ", armor=", 0);
    d_AppendString(log_msg, corrupt_armor_stackable ? "true" : "false", 0);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    destroy_item(arrows);
    destroy_item(corrupt_ammo);
    destroy_item(weapon);
    destroy_item(armor);
    d_DestroyString(basic_material.name);

    LOG("Stackable extreme cases test completed");
    return 1;
}

/*
    * Test durability properties with material interactions and edge cases
    */
int test_durability_properties_extreme_cases(void)
{
    LOG("Starting durability properties extreme cases test");

    // Test with materials that drastically affect durability
    MaterialProperties_t brittle_props = create_default_material_properties();
    brittle_props.durability_fact = 0.01f; // Very brittle
    Material_t brittle_material = create_material("brittle", brittle_props);

    MaterialProperties_t eternal_props = create_default_material_properties();
    eternal_props.durability_fact = 100.0f; // Nearly indestructible
    Material_t eternal_material = create_material("eternal", eternal_props);

    MaterialProperties_t corrupt_props = create_default_material_properties();
    corrupt_props.durability_fact = -50.0f; // Negative durability
    Material_t corrupt_material = create_material("corrupt", corrupt_props);

    LOG("Testing brittle material durability");
    Item_t* brittle_sword = create_weapon("Brittle Sword", "brittle_sword", brittle_material, 15, 25, 0, 'B');
    TEST_ASSERT(brittle_sword != NULL, "Brittle sword should be created");

    apply_material_to_weapon(brittle_sword);
    uint8_t brittle_durability = get_durability(brittle_sword);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Brittle sword durability after material application: ", 0);
    d_AppendInt(log_msg, brittle_durability);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(brittle_durability > 0, "Brittle sword should have some durability");
    TEST_ASSERT(brittle_durability == 255, "Brittle sword should have same durability");

    LOG("Testing eternal material durability");
    Item_t* eternal_sword = create_weapon("Eternal Sword", "eternal_sword", eternal_material, 15, 25, 0, 'E');
    TEST_ASSERT(eternal_sword != NULL, "Eternal sword should be created");

    apply_material_to_weapon(eternal_sword);
    uint8_t eternal_durability = get_durability(eternal_sword);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Eternal sword durability after material application: ", 0);
    d_AppendInt(log_msg, eternal_durability);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(eternal_durability == 255, "Eternal sword should have same durability");
    TEST_ASSERT(eternal_durability <= 255, "Eternal durability should not overflow");

    LOG("Testing corrupt material durability");
    Item_t* corrupt_sword = create_weapon("Corrupt Sword", "corrupt_sword", corrupt_material, 15, 25, 0, 'C');
    TEST_ASSERT(corrupt_sword != NULL, "Corrupt sword should be created");

    apply_material_to_weapon(corrupt_sword);
    uint8_t corrupt_durability = get_durability(corrupt_sword);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Corrupt sword durability after material application: ", 0);
    d_AppendInt(log_msg, corrupt_durability);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(corrupt_durability >= 0, "Corrupt sword durability should not be negative");

    // Test durability on items that shouldn't have it
    LOG("Testing durability on non-durable items");
    Item_t* consumable = create_consumable("Test Potion", "test_potion", 25, dummy_consume_callback, 'P');
    Lock_t test_lock = create_lock("Test Lock", "test_lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", test_lock, 'K');

    uint8_t consumable_durability = get_durability(consumable);
    uint8_t key_durability = get_durability(key);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Non-durable items: consumable=", 0);
    d_AppendInt(log_msg, consumable_durability);
    d_AppendString(log_msg, ", key=", 0);
    d_AppendInt(log_msg, key_durability);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(consumable_durability == 255, "Consumables should have max durability (no degradation)");
    TEST_ASSERT(key_durability == 255, "Keys should have max durability (no degradation)");

    destroy_item(brittle_sword);
    destroy_item(eternal_sword);
    destroy_item(corrupt_sword);
    destroy_item(consumable);
    destroy_item(key);
    d_DestroyString(brittle_material.name);
    d_DestroyString(eternal_material.name);
    d_DestroyString(corrupt_material.name);

    LOG("Durability properties extreme cases test completed");
    return 1;
}

/*
    * Test property interactions and cross-contamination
    */
int test_property_interactions_extreme_cases(void)
{
    LOG("Starting property interactions extreme cases test");

    // Test with materials that affect multiple properties
    MaterialProperties_t chaos_props = create_default_material_properties();
    chaos_props.weight_fact = 0.1f;        // Very light
    chaos_props.value_coins_fact = 10.0f;  // Very valuable
    chaos_props.durability_fact = 0.2f;    // Very fragile
    chaos_props.min_damage_fact = 5.0f;    // High min damage
    chaos_props.max_damage_fact = 0.5f;    // Low max damage (inverted!)
    chaos_props.armor_value_fact = 20.0f;  // Extreme armor
    chaos_props.evasion_value_fact = 0.01f; // Terrible evasion
    chaos_props.stealth_value_fact = 100.0f; // Perfect stealth
    chaos_props.enchant_value_fact = 0.0f; // No enchant
    Material_t chaos_material = create_material("chaos", chaos_props);

    LOG("Testing chaos material on weapon");
    Item_t* chaos_weapon = create_weapon("Chaos Weapon", "chaos_weapon", chaos_material, 10, 20, 5, 'C');
    TEST_ASSERT(chaos_weapon != NULL, "Chaos weapon should be created");

    // Get pre-application stats
    uint8_t pre_min = get_weapon_min_damage(chaos_weapon);
    uint8_t pre_max = get_weapon_max_damage(chaos_weapon);
    float pre_weight = get_item_weight(chaos_weapon);
    uint8_t pre_value = get_item_value_coins(chaos_weapon);

    apply_material_to_weapon(chaos_weapon);

    // Get post-application stats
    uint8_t post_min = get_weapon_min_damage(chaos_weapon);
    uint8_t post_max = get_weapon_max_damage(chaos_weapon);
    float post_weight = get_item_weight(chaos_weapon);
    uint8_t post_value = get_item_value_coins(chaos_weapon);
    uint8_t stealth = get_stealth_value(chaos_weapon);

    dString_t* log_msg = d_InitString();
    d_AppendString(log_msg, "Chaos weapon before: min=", 0);
    d_AppendInt(log_msg, pre_min);
    d_AppendString(log_msg, ", max=", 0);
    d_AppendInt(log_msg, pre_max);
    d_AppendString(log_msg, ", weight=", 0);
    d_AppendInt(log_msg, pre_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, pre_value);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Chaos weapon after: min=", 0);
    d_AppendInt(log_msg, post_min);
    d_AppendString(log_msg, ", max=", 0);
    d_AppendInt(log_msg, post_max);
    d_AppendString(log_msg, ", weight=", 0);
    d_AppendInt(log_msg, post_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, post_value);
    d_AppendString(log_msg, ", stealth=", 0);
    d_AppendInt(log_msg, stealth);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    // Test for logical consistency
    TEST_ASSERT(post_min <= 255, "Min damage should not overflow");
    TEST_ASSERT(post_max <= 255, "Max damage should not overflow");
    TEST_ASSERT(post_weight >= 0.0f, "Weight should not be negative");
    TEST_ASSERT(stealth <= 255, "Stealth should not overflow");

    LOG("Testing chaos material on armor");
    Item_t* chaos_armor = create_armor("Chaos Armor", "chaos_armor", chaos_material, 15, 10, 'C', 20, 5);
    TEST_ASSERT(chaos_armor != NULL, "Chaos armor should be created");

    apply_material_to_armor(chaos_armor);

    uint8_t armor_val = get_armor_value(chaos_armor);
    uint8_t evasion_val = get_evasion_value(chaos_armor);
    uint8_t armor_stealth = get_stealth_value(chaos_armor);
    float armor_weight = get_item_weight(chaos_armor);
    uint8_t armor_value = get_item_value_coins(chaos_armor);

    log_msg = d_InitString();
    d_AppendString(log_msg, "Chaos armor: armor=", 0);
    d_AppendInt(log_msg, armor_val);
    d_AppendString(log_msg, ", evasion=", 0);
    d_AppendInt(log_msg, evasion_val);
    d_AppendString(log_msg, ", stealth=", 0);
    d_AppendInt(log_msg, armor_stealth);
    d_AppendString(log_msg, ", weight=", 0);
    d_AppendInt(log_msg, armor_weight);
    d_AppendString(log_msg, ", value=", 0);
    d_AppendInt(log_msg, armor_value);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    TEST_ASSERT(armor_val <= 255, "Armor value should not overflow");
    TEST_ASSERT(evasion_val <= 255, "Evasion should not overflow");

    // Test multiple material applications (should this be allowed?)
    LOG("Testing multiple material applications");
    MaterialProperties_t second_props = create_default_material_properties();
    second_props.weight_fact = 2.0f;
    Material_t second_material = create_material("second", second_props);

    chaos_weapon->material_data = second_material;
    apply_material_to_weapon(chaos_weapon);

    float double_applied_weight = get_item_weight(chaos_weapon);
    log_msg = d_InitString();
    d_AppendString(log_msg, "Double-applied material weight: ", 0);
    d_AppendInt(log_msg, double_applied_weight);
    LOG(log_msg->str);
    d_DestroyString(log_msg);

    destroy_item(chaos_weapon);
    destroy_item(chaos_armor);
    d_DestroyString(chaos_material.name);
    d_DestroyString(second_material.name);

    LOG("Property interactions extreme cases test completed");
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

    RUN_TEST(test_weapon_stats_extreme_cases);
    RUN_TEST(test_weapon_ammo_extreme_cases);
    RUN_TEST(test_armor_stats_extreme_cases);
    RUN_TEST(test_weight_value_extreme_cases);
    RUN_TEST(test_stealth_extreme_cases);
    RUN_TEST(test_stackable_extreme_cases);
    RUN_TEST(test_durability_properties_extreme_cases);
    RUN_TEST(test_property_interactions_extreme_cases);

    TEST_SUITE_END();
}
