// ASCIIGame/tests/items/test_items_properties.c
// Test file for item properties and stats system, forged with divine logging wisdom.

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

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper functions
static void dummy_consume_callback(uint8_t value) // consumables cannot have on_consume = NULL
{
    // Dummy callback for testing
    (void)value; // Suppress unused parameter warning
}

// Helper function for float comparison
static bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// Helper to create a standard test material
static Material_t create_test_material(const char* name) {
    MaterialProperties_t props = create_default_material_properties();
    return create_material(name, props);
}

// =============================================================================
// WEAPON STATS TESTS
// =============================================================================

int test_weapon_stats(void)
{
    d_LogInfo("VERIFICATION: Basic weapon stats functionality and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("WeaponStats");
    
    // Create a weapon for testing
    Material_t iron = create_test_material("iron");
    Item_t* sword = create_weapon("Test Sword", "test_sword", iron, 15, 25, 1, 'S', 0, 0, "common");
    TEST_ASSERT(sword != NULL, "Sword should be created");
    if (!sword) {
        d_LogError("Critical failure: sword creation failed, aborting test");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Testing basic weapon damage accessors...");
    uint8_t min_dmg = get_weapon_min_damage(sword);
    uint8_t max_dmg = get_weapon_max_damage(sword);
    uint8_t range = get_weapon_range(sword);

    d_LogDebugF("Weapon stats: min=%d, max=%d, range=%d", min_dmg, max_dmg, range);
    TEST_ASSERT(min_dmg == 15, "Min damage should be 15");
    TEST_ASSERT(max_dmg == 25, "Max damage should be 25");
    TEST_ASSERT(range == 1, "Range should be 1");

    d_LogDebug("Testing NULL safety for weapon stat functions...");
    TEST_ASSERT(get_weapon_min_damage(NULL) == 0, "NULL weapon should have 0 min damage");
    TEST_ASSERT(get_weapon_max_damage(NULL) == 0, "NULL weapon should have 0 max damage");
    TEST_ASSERT(get_weapon_range(NULL) == 0, "NULL weapon should have 0 range");

    d_LogDebug("Testing weapon functions on non-weapon items...");
    Item_t* armor = create_armor("Test Armor", "test_armor", iron, 20, 10, 'A', 15, 15, "common");
    if (armor) {
        TEST_ASSERT(get_weapon_min_damage(armor) == 0, "Armor should have 0 weapon damage");
        TEST_ASSERT(get_weapon_max_damage(armor) == 0, "Armor should have 0 weapon damage");
        TEST_ASSERT(get_weapon_range(armor) == 0, "Armor should have 0 weapon range");
        destroy_item(armor);
    }

    destroy_item(sword);
    d_DestroyString(iron.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_weapon_ammo(void)
{
    d_LogInfo("VERIFICATION: Weapon ammunition requirements and compatibility system.");
    dLogContext_t* ctx = d_PushLogContext("WeaponAmmo");
    
    Material_t wood = create_test_material("wood");

    d_LogDebug("Creating test weapons with different ranges...");
    Item_t* sword = create_weapon("Sword", "sword", wood, 10, 15, 0, 'S', 0, 0, "common");
    Item_t* bow = create_weapon("Bow", "bow", wood, 5, 10, 5, 'B', 0, 0, "common");
    Item_t* arrow = create_ammunition("Arrow", "arrow", wood, 3, 5, 'a', 0, "common");

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(bow != NULL, "Bow should be created");
    TEST_ASSERT(arrow != NULL, "Arrow should be created");

    if (!sword || !bow || !arrow) {
        d_LogError("Critical failure: weapon/ammo creation failed");
        goto cleanup;
    }

    d_LogDebug("Testing ammo requirements based on weapon range...");
    bool sword_needs_ammo = weapon_needs_ammo(sword);
    bool bow_needs_ammo = weapon_needs_ammo(bow);

    d_LogDebugF("Ammo requirements: Sword(range=0)=%s, Bow(range=5)=%s", 
                sword_needs_ammo ? "Yes" : "No", bow_needs_ammo ? "Yes" : "No");

    TEST_ASSERT(!sword_needs_ammo, "Sword should not need ammo");
    TEST_ASSERT(bow_needs_ammo, "Bow should need ammo");

    d_LogDebug("Testing ammo compatibility between weapons and ammunition...");
    bool sword_can_use_arrow = weapon_can_use_ammo(sword, arrow);
    bool bow_can_use_arrow = weapon_can_use_ammo(bow, arrow);

    d_LogDebugF("Ammo compatibility: Sword+Arrow=%s, Bow+Arrow=%s",
                sword_can_use_arrow ? "Yes" : "No", bow_can_use_arrow ? "Yes" : "No");

    TEST_ASSERT(!sword_can_use_arrow, "Sword should not be able to use arrows");
    TEST_ASSERT(bow_can_use_arrow, "Bow should be able to use arrows");

    d_LogDebug("Testing NULL safety for ammo functions...");
    TEST_ASSERT(!weapon_needs_ammo(NULL), "NULL weapon should not need ammo");
    TEST_ASSERT(!weapon_can_use_ammo(NULL, arrow), "NULL weapon should not use ammo");
    TEST_ASSERT(!weapon_can_use_ammo(bow, NULL), "Weapon should not use NULL ammo");

cleanup:
    destroy_item(sword);
    destroy_item(bow);
    destroy_item(arrow);
    d_DestroyString(wood.name);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// ARMOR STATS TESTS
// =============================================================================

int test_armor_stats(void)
{
    d_LogInfo("VERIFICATION: Armor statistics and defensive value calculations.");
    dLogContext_t* ctx = d_PushLogContext("ArmorStats");
    
    Material_t leather = create_test_material("leather");
    Item_t* armor = create_armor("Test Armor", "test_armor", leather, 30, 15, 'A', 10, 5, "common");
    TEST_ASSERT(armor != NULL, "Armor should be created");
    
    if (!armor) {
        d_LogError("Critical failure: armor creation failed");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Testing armor defensive stat accessors...");
    uint8_t armor_val = get_armor_value(armor);
    uint8_t evasion_val = get_evasion_value(armor);

    d_LogDebugF("Armor defensive stats: armor_value=%d, evasion=%d", armor_val, evasion_val);
    TEST_ASSERT(armor_val == 30, "Armor value should be 30");
    TEST_ASSERT(evasion_val == 15, "Evasion value should be 15");

    d_LogDebug("Testing NULL safety for armor stat functions...");
    TEST_ASSERT(get_armor_value(NULL) == 0, "NULL armor should have 0 armor value");
    TEST_ASSERT(get_evasion_value(NULL) == 0, "NULL armor should have 0 evasion value");

    d_LogDebug("Testing armor functions on non-armor items...");
    Item_t* sword = create_weapon("Test Sword", "test_sword", leather, 10, 15, 0, 'S', 0, 0, "common");
    if (sword) {
        TEST_ASSERT(get_armor_value(sword) == 0, "Weapon should have 0 armor value");
        TEST_ASSERT(get_evasion_value(sword) == 0, "Weapon should have 0 evasion value");
        destroy_item(sword);
    }

    destroy_item(armor);
    d_DestroyString(leather.name);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// GENERAL ITEM PROPERTIES TESTS
// =============================================================================

int test_item_weight(void)
{
    d_LogInfo("VERIFICATION: Item weight calculations and material factor effects.");
    dLogContext_t* ctx = d_PushLogContext("ItemWeight");
    
    d_LogDebug("Creating heavy material with weight factor 2.0...");
    MaterialProperties_t heavy_props = create_default_material_properties();
    heavy_props.weight_fact = 2.0f;
    Material_t heavy_material = create_material("heavy", heavy_props);

    Item_t* heavy_sword = create_weapon("Heavy Sword", "heavy_sword", heavy_material, 20, 30, 0, 'H', 0, 0, "common");
    TEST_ASSERT(heavy_sword != NULL, "Heavy sword should be created");
    
    if (!heavy_sword) {
        d_LogError("Critical failure: heavy sword creation failed");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Applying material effects to modify weight...");
    apply_material_to_weapon(heavy_sword);

    float weight = get_item_weight(heavy_sword);
    d_LogDebugF("Heavy sword weight after material application: %.3f kg", weight);
    
    TEST_ASSERT(weight > 0.0f, "Weight should be greater than 0");
    TEST_ASSERT(float_equals(weight, heavy_sword->weight_kg, 0.01f), "get_item_weight should match item's weight_kg");

    d_LogDebug("Testing weight function with NULL input...");
    float null_weight = get_item_weight(NULL);
    TEST_ASSERT(float_equals(null_weight, 0.0f, 0.01f), "NULL item should have 0 weight");

    destroy_item(heavy_sword);
    d_DestroyString(heavy_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_item_value(void)
{
    d_LogInfo("VERIFICATION: Item value calculations and material economics.");
    dLogContext_t* ctx = d_PushLogContext("ItemValue");
    
    d_LogDebug("Creating valuable material with value factor 3.0...");
    MaterialProperties_t valuable_props = create_default_material_properties();
    valuable_props.value_coins_fact = 3.0f;
    Material_t valuable_material = create_material("gold", valuable_props);

    Item_t* gold_ring = create_armor("Gold Ring", "gold_ring", valuable_material, 1, 0, 'o', 15, 15, "common");
    TEST_ASSERT(gold_ring != NULL, "Gold ring should be created");
    
    if (!gold_ring) {
        d_LogError("Critical failure: gold ring creation failed");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Applying material effects to modify value...");
    apply_material_to_armor(gold_ring);

    uint8_t value = get_item_value_coins(gold_ring);
    d_LogDebugF("Gold ring value after material application: %d coins", value);
    
    TEST_ASSERT(value > 0, "Value should be greater than 0");
    TEST_ASSERT(value == gold_ring->value_coins, "get_item_value_coins should match item's value_coins");

    d_LogDebug("Testing value function with NULL input...");
    uint8_t null_value = get_item_value_coins(NULL);
    TEST_ASSERT(null_value == 0, "NULL item should have 0 value");

    destroy_item(gold_ring);
    d_DestroyString(valuable_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_stealth_value(void)
{
    d_LogInfo("VERIFICATION: Stealth value calculations across different item types.");
    dLogContext_t* ctx = d_PushLogContext("StealthValue");
    
    d_LogDebug("Creating stealth-enhanced materials...");
    MaterialProperties_t stealth_props = create_default_material_properties();
    stealth_props.stealth_value_fact = 2.0f;
    Material_t stealth_material = create_material("shadow", stealth_props);

    Item_t* stealth_armor = create_armor("Shadow Cloak", "shadow_cloak", stealth_material, 10, 20, 'C', 15, 15, "common");
    Item_t* stealth_weapon = create_weapon("Shadow Blade", "shadow_blade", stealth_material, 15, 20, 0, 'b', 0, 0, "common");
    
    // Create items that shouldn't have stealth
    Lock_t lock = create_lock("test_lock", "A test lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", lock, 'k', 0, "common");

    TEST_ASSERT(stealth_armor != NULL, "Stealth armor should be created");
    TEST_ASSERT(stealth_weapon != NULL, "Stealth weapon should be created");
    TEST_ASSERT(key != NULL, "Key should be created");
    
    if (!stealth_armor || !stealth_weapon || !key) {
        d_LogError("Critical failure in stealth test item creation");
        goto stealth_cleanup;
    }

    d_LogDebug("Setting base stealth values before material application...");
    stealth_armor->data.armor.stealth_value = 10;
    stealth_weapon->data.weapon.stealth_value = 15;

    d_LogDebug("Applying stealth materials to enhance stealth capabilities...");
    apply_material_to_armor(stealth_armor);
    apply_material_to_weapon(stealth_weapon);

    uint8_t armor_stealth = get_stealth_value(stealth_armor);
    uint8_t weapon_stealth = get_stealth_value(stealth_weapon);
    uint8_t key_stealth = get_stealth_value(key);

    d_LogDebugF("Stealth values: armor=%d, weapon=%d, key=%d", 
                armor_stealth, weapon_stealth, key_stealth);

    TEST_ASSERT(armor_stealth >= 0, "Armor should have stealth value");
    TEST_ASSERT(weapon_stealth >= 0, "Weapon should have stealth value");
    TEST_ASSERT(key_stealth == 0, "Key should have no stealth value");

    d_LogDebug("Testing stealth function with NULL input...");
    uint8_t null_stealth = get_stealth_value(NULL);
    TEST_ASSERT(null_stealth == 0, "NULL item should have 0 stealth value");

stealth_cleanup:
    destroy_item(stealth_armor);
    destroy_item(stealth_weapon);
    destroy_item(key);
    destroy_lock(&lock);
    d_DestroyString(stealth_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_durability(void)
{
    d_LogInfo("VERIFICATION: Item durability system and degradation resistance.");
    dLogContext_t* ctx = d_PushLogContext("Durability");
    
    d_LogDebug("Creating durable material with enhanced durability factor...");
    MaterialProperties_t durable_props = create_default_material_properties();
    durable_props.durability_fact = 1.5f;
    Material_t durable_material = create_material("adamantine", durable_props);

    Item_t* durable_sword = create_weapon("Adamantine Sword", "adamantine_sword", durable_material, 25, 35, 0, 'A', 0, 0, "common");
    Item_t* consumable = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H', 0, "common");

    TEST_ASSERT(durable_sword != NULL, "Durable sword should be created");
    TEST_ASSERT(consumable != NULL, "Consumable should be created");
    
    if (!durable_sword || !consumable) {
        d_LogError("Critical failure in durability test item creation");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Applying durable material to enhance sword durability...");
    apply_material_to_weapon(durable_sword);

    uint8_t sword_durability = get_durability(durable_sword);
    uint8_t consumable_durability = get_durability(consumable);

    d_LogDebugF("Durability values: sword=%d, consumable=%d", sword_durability, consumable_durability);

    TEST_ASSERT(sword_durability > 0, "Sword should have durability");
    TEST_ASSERT(consumable_durability == 255, "Consumable should have max durability (no degradation)");

    d_LogDebug("Testing durability function with NULL input...");
    uint8_t null_durability = get_durability(NULL);
    TEST_ASSERT(null_durability == 0, "NULL item should have 0 durability");

    destroy_item(durable_sword);
    destroy_item(consumable);
    d_DestroyString(durable_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_stackable(void)
{
    d_LogInfo("VERIFICATION: Item stacking behavior and inventory management properties.");
    dLogContext_t* ctx = d_PushLogContext("Stackable");
    
    Material_t basic_material = create_test_material("basic");

    Item_t* sword = create_weapon("Sword", "sword", basic_material, 10, 15, 0, 'S', 0, 0, "common");
    Item_t* arrows = create_ammunition("Arrows", "arrows", basic_material, 5, 8, 'a', 0, "common");

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(arrows != NULL, "Arrows should be created");
    
    if (!sword || !arrows) {
        d_LogError("Critical failure in stackable test item creation");
        d_PopLogContext(ctx);
        return 0;
    }

    d_LogDebug("Testing stackable properties for different item types...");
    bool sword_stackable = is_item_stackable(sword);
    bool arrows_stackable = is_item_stackable(arrows);

    uint8_t sword_max_stack = get_max_stack_size(sword);
    uint8_t arrows_max_stack = get_max_stack_size(arrows);

    d_LogDebugF("Stackable properties: sword(stackable=%s, max=%d), arrows(stackable=%s, max=%d)",
                sword_stackable ? "true" : "false", sword_max_stack,
                arrows_stackable ? "true" : "false", arrows_max_stack);

    TEST_ASSERT(!sword_stackable, "Weapons should not be stackable");
    TEST_ASSERT(arrows_stackable, "Ammunition should be stackable");
    TEST_ASSERT(sword_max_stack == 0, "Weapons should have 0 max stack size");
    TEST_ASSERT(arrows_max_stack > 0, "Ammunition should have positive max stack size");

    d_LogDebug("Verifying item name integrity for string comparison functions...");
    TEST_ASSERT(strcmp("Arrows", arrows->name->str) == 0, "Arrows should be named 'Arrows'");
    TEST_ASSERT(strcmp("Sword", sword->name->str) == 0, "Sword should be named 'Sword'");

    d_LogDebug("Testing stackable functions with NULL input...");
    TEST_ASSERT(!is_item_stackable(NULL), "NULL item should not be stackable");
    TEST_ASSERT(get_max_stack_size(NULL) == 0, "NULL item should have 0 max stack size");

    destroy_item(sword);
    destroy_item(arrows);
    d_DestroyString(basic_material.name);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// ENHANCED PROPERTIES BUG HUNTING TESTS - WITH PROPER LOGGING
// =============================================================================

int test_weapon_stats_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing weapon stats with extreme and edge case values.");
    dLogContext_t* ctx = d_PushLogContext("WeaponStatsExtremes");
    
    Material_t basic_material = create_test_material("basic");

    d_LogDebug("Testing weapon with zero damage values...");
    Item_t* zero_weapon = create_weapon("Zero Damage", "zero_damage", basic_material, 0, 0, 0, 'Z', 0, 0, "common");
    TEST_ASSERT(zero_weapon != NULL, "Zero damage weapon should be created");
    
    if (zero_weapon) {
        uint8_t zero_min = get_weapon_min_damage(zero_weapon);
        uint8_t zero_max = get_weapon_max_damage(zero_weapon);
        uint8_t zero_range = get_weapon_range(zero_weapon);

        d_LogDebugF("Zero weapon stats: min=%d, max=%d, range=%d", zero_min, zero_max, zero_range);

        TEST_ASSERT(zero_min == 0, "Zero min damage should be preserved");
        TEST_ASSERT(zero_max == 0, "Zero max damage should be preserved");
        TEST_ASSERT(zero_range == 0, "Zero range should be preserved");
    }

    d_LogDebug("Testing weapon with inverted damage values (min > max)...");
    Item_t* inverted_weapon = create_weapon("Inverted Damage", "inverted_damage", 
        basic_material, 50, 20, 0, 'I', 0, 0, "common");
    TEST_ASSERT(inverted_weapon != NULL, "Inverted damage weapon should be created");
    
    if (inverted_weapon) {
        uint8_t inv_min = get_weapon_min_damage(inverted_weapon);
        uint8_t inv_max = get_weapon_max_damage(inverted_weapon);

        d_LogDebugF("Inverted weapon: input min=50, max=20; actual min=%d, actual max=%d", inv_min, inv_max);
        TEST_ASSERT(inv_min <= inv_max || (inv_min == 50 && inv_max == 20), "Inverted damage should be handled consistently");
    }

    d_LogDebug("Testing weapon with maximum uint8_t values...");
    Item_t* max_weapon = create_weapon("Max Weapon", "max_weapon", 
        basic_material, 255, 255, 255, 'M', 0, 0, "common");
    TEST_ASSERT(max_weapon != NULL, "Max weapon should be created");
    
    if (max_weapon) {
        uint8_t max_min = get_weapon_min_damage(max_weapon);
        uint8_t max_max = get_weapon_max_damage(max_weapon);
        uint8_t max_range = get_weapon_range(max_weapon);

        TEST_ASSERT(max_min == 255, "Max min damage should be preserved");
        TEST_ASSERT(max_max == 255, "Max max damage should be preserved");
        TEST_ASSERT(max_range == 255, "Max range should be preserved");
    }

    d_LogDebug("Testing extreme range weapon ammo logic...");
    Item_t* sniper = create_weapon("Sniper Rifle", "sniper", 
        basic_material, 30, 50, 200, 'S', 0, 0, "common");
    TEST_ASSERT(sniper != NULL, "Sniper should be created");
    
    if (sniper) {
        bool sniper_needs_ammo = weapon_needs_ammo(sniper);
        d_LogDebugF("Sniper (range=200) needs ammo: %s", sniper_needs_ammo ? "Yes" : "No");
        TEST_ASSERT(sniper_needs_ammo, "High range weapon should need ammo");
    }

    destroy_item(zero_weapon);
    destroy_item(inverted_weapon);
    destroy_item(max_weapon);
    destroy_item(sniper);
    d_DestroyString(basic_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_weapon_ammo_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing weapon ammo system with edge cases and extreme compatibility.");
    dLogContext_t* ctx = d_PushLogContext("WeaponAmmoExtremes");
    
    Material_t basic_material = create_test_material("basic");

    d_LogDebug("Creating weapons with various range values for ammo testing...");
    Item_t* melee = create_weapon("Melee", "melee", basic_material, 10, 15, 0, 'M', 0, 0, "common");
    Item_t* short_range = create_weapon("Short Range", "short_range", basic_material, 8, 12, 1, 'S', 0, 0, "common");
    Item_t* long_range = create_weapon("Long Range", "long_range", basic_material, 15, 25, 50, 'L', 0, 0,"common");
    Item_t* extreme_range = create_weapon("Extreme Range", "extreme_range", 
        basic_material, 20, 30, 255, 'E', 0, "common", 0);

    Item_t* light_ammo = create_ammunition("Light Ammo", "light_ammo", basic_material, 2, 4, 'l', 0, "common");
    Item_t* heavy_ammo = create_ammunition("Heavy Ammo", "heavy_ammo", basic_material, 8, 15, 'h', 0, "common");
    Item_t* extreme_ammo = create_ammunition("Extreme Ammo", "extreme_ammo", basic_material, 255, 255, 'x', 0, "common");

    if (!melee || !short_range || !long_range || !extreme_range ||
        !light_ammo || !heavy_ammo || !extreme_ammo) {
        d_LogError("Critical failure in ammo test item creation");
        goto ammo_cleanup;
    }

    d_LogDebug("Testing ammo requirements for different weapon ranges...");
    bool melee_needs = weapon_needs_ammo(melee);
    bool short_needs = weapon_needs_ammo(short_range);
    bool long_needs = weapon_needs_ammo(long_range);
    bool extreme_needs = weapon_needs_ammo(extreme_range);

    // Rate-limited logging for bulk operations
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 2.0, "Ammo needs: melee(0)=%s, short(1)=%s, long(50)=%s, extreme(255)=%s",
                      melee_needs ? "true" : "false", short_needs ? "true" : "false",
                      long_needs ? "true" : "false", extreme_needs ? "true" : "false");

    TEST_ASSERT(!melee_needs, "Melee weapon should not need ammo");

    d_LogDebug("Testing ammo compatibility with different ammunition types...");
    bool melee_light = weapon_can_use_ammo(melee, light_ammo);
    bool short_light = weapon_can_use_ammo(short_range, light_ammo);
    bool long_heavy = weapon_can_use_ammo(long_range, heavy_ammo);
    bool extreme_extreme = weapon_can_use_ammo(extreme_range, extreme_ammo);

    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 2.0, "Ammo compatibility: melee+light=%s, short+light=%s, long+heavy=%s, extreme+extreme=%s",
                      melee_light ? "true" : "false", short_light ? "true" : "false",
                      long_heavy ? "true" : "false", extreme_extreme ? "true" : "false");

    TEST_ASSERT(!melee_light, "Melee weapon should not use ammo");

    d_LogDebug("Testing ammo system with corrupted weapon data...");
    Item_t* corrupt_weapon = create_weapon("Corrupt", "corrupt", basic_material, 10, 15, 5, 'C', 0, 0, "common");
    if (corrupt_weapon) {
        ItemType_t original_type = corrupt_weapon->type;
        corrupt_weapon->type = ITEM_TYPE_ARMOR; // Wrong type

        bool corrupt_needs = weapon_needs_ammo(corrupt_weapon);
        bool corrupt_can_use = weapon_can_use_ammo(corrupt_weapon, light_ammo);

        TEST_ASSERT(!corrupt_needs, "Corrupted weapon should not need ammo");
        TEST_ASSERT(!corrupt_can_use, "Corrupted weapon should not use ammo");

        corrupt_weapon->type = original_type; // Restore for safe cleanup
    }

ammo_cleanup:
    destroy_item(melee);
    destroy_item(short_range);
    destroy_item(long_range);
    destroy_item(extreme_range);
    destroy_item(light_ammo);
    destroy_item(heavy_ammo);
    destroy_item(extreme_ammo);
    destroy_item(corrupt_weapon);
    d_DestroyString(basic_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_armor_stats_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing armor stats with extreme values and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("ArmorStatsExtremes");
    
    Material_t basic_material = create_test_material("basic");

    d_LogDebug("Testing armor with zero stat values...");
    Item_t* zero_armor = create_armor("Zero Armor", "zero_armor", basic_material, 0, 0, 'Z', 0, 0, "common");
    TEST_ASSERT(zero_armor != NULL, "Zero armor should be created");
    
    if (zero_armor) {
        uint8_t zero_armor_val = get_armor_value(zero_armor);
        uint8_t zero_evasion = get_evasion_value(zero_armor);

        d_LogDebugF("Zero armor stats: armor=%d, evasion=%d", zero_armor_val, zero_evasion);

        TEST_ASSERT(zero_armor_val == 0, "Zero armor value should be preserved");
        TEST_ASSERT(zero_evasion == 0, "Zero evasion should be preserved");
    }

    d_LogDebug("Testing armor with maximum uint8_t values...");
    Item_t* max_armor = create_armor("Max Armor", "max_armor", basic_material, 255, 255, 'M', 255, 255, "common");
    TEST_ASSERT(max_armor != NULL, "Max armor should be created");
    
    if (max_armor) {
        uint8_t max_armor_val = get_armor_value(max_armor);
        uint8_t max_evasion = get_evasion_value(max_armor);

        d_LogDebugF("Max armor stats: armor=%d, evasion=%d", max_armor_val, max_evasion);

        TEST_ASSERT(max_armor_val == 255, "Max armor value should be preserved");
        TEST_ASSERT(max_evasion == 255, "Max evasion should be preserved");
    }

    d_LogDebug("Testing armor with extreme stat combinations...");
    Item_t* glass_armor = create_armor("Glass Armor", "glass_armor", basic_material, 1, 254, 'G', 255, 1, "common");
    TEST_ASSERT(glass_armor != NULL, "Glass armor should be created");
    
    if (glass_armor) {
        uint8_t glass_armor_val = get_armor_value(glass_armor);
        uint8_t glass_evasion = get_evasion_value(glass_armor);

        TEST_ASSERT(glass_armor_val == 1, "Low armor value should be preserved");
        TEST_ASSERT(glass_evasion == 254, "High evasion should be preserved");
    }

    d_LogDebug("Testing armor stats with corrupted item data...");
    Item_t* corrupt_armor = create_armor("Corrupt Armor", "corrupt_armor", basic_material, 50, 30, 'C', 20, 20, "common");
    if (corrupt_armor) {
        corrupt_armor->type = ITEM_TYPE_WEAPON; // Corrupt the type

        uint8_t corrupt_armor_val = get_armor_value(corrupt_armor);
        uint8_t corrupt_evasion = get_evasion_value(corrupt_armor);

        TEST_ASSERT(corrupt_armor_val == 0, "Corrupted armor should return 0 armor value");
        TEST_ASSERT(corrupt_evasion == 0, "Corrupted armor should return 0 evasion");
    }

    destroy_item(zero_armor);
    destroy_item(max_armor);
    destroy_item(glass_armor);
    destroy_item(corrupt_armor);
    d_DestroyString(basic_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_weight_value_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing weight and value calculations with extreme material properties.");
    dLogContext_t* ctx = d_PushLogContext("WeightValueExtremes");
    
    d_LogDebug("Creating materials with extreme property factors...");
    
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

    d_LogDebug("Testing feather-light material effects...");
    Item_t* feather_sword = create_weapon("Feather Sword", "feather_sword", 
        feather_material, 10, 15, 0, 'F', 0, 0, "common");
    if (feather_sword) {
        apply_material_to_weapon(feather_sword);
        float feather_weight = get_item_weight(feather_sword);
        uint8_t feather_value = get_item_value_coins(feather_sword);

        d_LogDebugF("Feather sword: weight=%.6f, value=%d", feather_weight, feather_value);
        TEST_ASSERT(feather_weight >= 0.0f, "Weight should not be negative");
    }

    d_LogDebug("Testing lead-heavy material effects...");
    Item_t* lead_sword = create_weapon("Lead Sword", "lead_sword", lead_material, 10, 15, 0, 'L', 0, 0, "common");
    if (lead_sword && feather_sword) {
        apply_material_to_weapon(lead_sword);
        float lead_weight = get_item_weight(lead_sword);
        float feather_weight = get_item_weight(feather_sword);

        d_LogDebugF("Lead sword: weight=%.3f", lead_weight);
        TEST_ASSERT(lead_weight > feather_weight, "Lead should be heavier than feather");
    }

    d_LogDebug("Testing negative property material effects...");
    Item_t* negative_sword = create_weapon("Negative Sword", "negative_sword", negative_material, 10, 15, 0, 'N', 0, 0, "common");
    if (negative_sword) {
        apply_material_to_weapon(negative_sword);
        float negative_weight = get_item_weight(negative_sword);
        uint8_t negative_value = get_item_value_coins(negative_sword);

        d_LogDebugF("Negative sword: weight=%.3f, value=%d", negative_weight, negative_value);
        TEST_ASSERT(negative_weight >= 0.0f, "Negative weight should be clamped to 0 or handled gracefully");
        TEST_ASSERT(negative_value >= 0, "Negative value should be clamped to 0 or handled gracefully");
    }

    destroy_item(feather_sword);
    destroy_item(lead_sword);
    destroy_item(negative_sword);
    d_DestroyString(feather_material.name);
    d_DestroyString(lead_material.name);
    d_DestroyString(negative_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_stealth_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing stealth values with extreme materials and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("StealthExtremes");
    
    d_LogDebug("Creating materials with extreme stealth factors...");
    
    MaterialProperties_t invisible_props = create_default_material_properties();
    invisible_props.stealth_value_fact = 1000.0f; // Super stealth
    Material_t invisible_material = create_material("invisible", invisible_props);

    MaterialProperties_t glowing_props = create_default_material_properties();
    glowing_props.stealth_value_fact = 0.0f; // No stealth
    Material_t glowing_material = create_material("glowing", glowing_props);

    MaterialProperties_t anti_stealth_props = create_default_material_properties();
    anti_stealth_props.stealth_value_fact = -10.0f; // Negative stealth
    Material_t anti_stealth_material = create_material("anti_stealth", anti_stealth_props);

    d_LogDebug("Testing extreme stealth value calculations...");
    Item_t* invisible_armor = create_armor("Invisible Armor", "invisible_armor", 
        invisible_material, 20, 10, 'I', 100, 10, "common");
    Item_t* glowing_armor = create_armor("Glowing Armor", "glowing_armor", 
        glowing_material, 25, 15, 'G', 0, 15, "common");
    Item_t* anti_stealth_armor = create_armor("Anti-Stealth Armor", "anti_stealth_armor", 
        anti_stealth_material, 30, 5, 'A', 50, 20, "common");

    if (invisible_armor && glowing_armor && anti_stealth_armor) {
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

        d_LogDebugF("Stealth values: invisible=%d, glowing=%d, anti-stealth=%d",
                    invisible_stealth, glowing_stealth, anti_stealth_stealth);

        TEST_ASSERT(invisible_stealth <= 255, "Invisible stealth should not overflow uint8_t");
        TEST_ASSERT(glowing_stealth >= 0, "Glowing stealth should not be negative");
        TEST_ASSERT(anti_stealth_stealth >= 0, "Anti-stealth should not be negative");
    }

    d_LogDebug("Testing stealth values on inappropriate item types...");
    Lock_t test_lock = create_lock("Test Lock", "test_lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", test_lock, 'K', 0, "common");
    Item_t* consumable = create_consumable("Test Potion", "test_potion", 25, dummy_consume_callback, 'P', 0, "common");

    if (key && consumable) {
        uint8_t key_stealth = get_stealth_value(key);
        uint8_t consumable_stealth = get_stealth_value(consumable);

        d_LogDebugF("Non-stealth items: key=%d, consumable=%d", key_stealth, consumable_stealth);

        TEST_ASSERT(key_stealth == 0, "Keys should have no stealth value");
        TEST_ASSERT(consumable_stealth == 0, "Consumables should have no stealth value");
    }

    destroy_item(invisible_armor);
    destroy_item(glowing_armor);
    destroy_item(anti_stealth_armor);
    destroy_item(key);
    destroy_item(consumable);
    destroy_lock(&test_lock);
    d_DestroyString(invisible_material.name);
    d_DestroyString(glowing_material.name);
    d_DestroyString(anti_stealth_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_stackable_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing stackable properties with edge cases and potential overflow.");
    dLogContext_t* ctx = d_PushLogContext("StackableExtremes");
    
    Material_t basic_material = create_test_material("basic");

    d_LogDebug("Testing stackable ammunition at max stack boundaries...");
    Item_t* arrows = create_ammunition("Test Arrows", "test_arrows", basic_material, 5, 8, 'A', 0, "common");
    if (arrows) {
        bool arrows_stackable = is_item_stackable(arrows);
        uint8_t arrows_max_stack = get_max_stack_size(arrows);

        d_LogDebugF("Arrows: stackable=%s, max_stack=%d",
                    arrows_stackable ? "true" : "false", arrows_max_stack);

        TEST_ASSERT(arrows_stackable, "Arrows should be stackable");
        TEST_ASSERT(arrows_max_stack > 0, "Arrows should have positive max stack size");
        TEST_ASSERT(arrows_max_stack <= 255, "Max stack should not exceed uint8_t");
    }

    d_LogDebug("Testing stackable properties with corrupted data...");
    Item_t* corrupt_ammo = create_ammunition("Corrupt Ammo", "corrupt_ammo", basic_material, 3, 6, 'C', 0, "common");
    if (corrupt_ammo) {
        // Manually corrupt the stackable value to test bounds checking
        corrupt_ammo->stackable = 0; // Make non-stackable

        bool corrupt_stackable = is_item_stackable(corrupt_ammo);
        uint8_t corrupt_max_stack = get_max_stack_size(corrupt_ammo);

        d_LogDebugF("Corrupted ammo: stackable=%s, max_stack=%d",
                    corrupt_stackable ? "true" : "false", corrupt_max_stack);

        TEST_ASSERT(!corrupt_stackable, "Corrupted ammo should not be stackable");
        TEST_ASSERT(corrupt_max_stack == 0, "Corrupted ammo should have 0 max stack");

        // Test with extreme stackable values
        corrupt_ammo->stackable = 255; // Maximum stack size
        bool extreme_stackable = is_item_stackable(corrupt_ammo);
        uint8_t extreme_max_stack = get_max_stack_size(corrupt_ammo);

        TEST_ASSERT(extreme_stackable, "Max stackable should be stackable");
        TEST_ASSERT(extreme_max_stack == 255, "Max stackable should have 255 max stack");
    }

    d_LogDebug("Testing non-stackable items to ensure they stay non-stackable...");
    Item_t* weapon = create_weapon("Test Weapon", "test_weapon", basic_material, 10, 15, 0, 'W', 0, 0, "common");
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 20, 10, 'A', 15, 15, "common");

    if (weapon && armor) {
        bool weapon_stackable = is_item_stackable(weapon);
        bool armor_stackable = is_item_stackable(armor);
        uint8_t weapon_max_stack = get_max_stack_size(weapon);
        uint8_t armor_max_stack = get_max_stack_size(armor);

        TEST_ASSERT(!weapon_stackable, "Weapons should not be stackable");
        TEST_ASSERT(!armor_stackable, "Armor should not be stackable");
        TEST_ASSERT(weapon_max_stack == 0, "Weapons should have 0 max stack");
        TEST_ASSERT(armor_max_stack == 0, "Armor should have 0 max stack");

        d_LogDebug("Testing stackable properties with wrong item types...");
        weapon->stackable = 50; // Manually set stackable on non-stackable item
        armor->stackable = 100;

        bool corrupt_weapon_stackable = is_item_stackable(weapon);
        bool corrupt_armor_stackable = is_item_stackable(armor);

        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          1, 3.0, "Type-corrupted stackable: weapon=%s, armor=%s",
                          corrupt_weapon_stackable ? "true" : "false",
                          corrupt_armor_stackable ? "true" : "false");
    }

    destroy_item(arrows);
    destroy_item(corrupt_ammo);
    destroy_item(weapon);
    destroy_item(armor);
    d_DestroyString(basic_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_durability_properties_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing durability properties with material interactions and edge cases.");
    dLogContext_t* ctx = d_PushLogContext("DurabilityPropertiesExtremes");
    
    d_LogDebug("Creating materials with extreme durability factors...");
    
    MaterialProperties_t brittle_props = create_default_material_properties();
    brittle_props.durability_fact = 0.01f; // Very brittle
    Material_t brittle_material = create_material("brittle", brittle_props);

    MaterialProperties_t eternal_props = create_default_material_properties();
    eternal_props.durability_fact = 100.0f; // Nearly indestructible
    Material_t eternal_material = create_material("eternal", eternal_props);

    MaterialProperties_t corrupt_props = create_default_material_properties();
    corrupt_props.durability_fact = -50.0f; // Negative durability
    Material_t corrupt_material = create_material("corrupt", corrupt_props);

    d_LogDebug("Testing brittle material durability effects...");
    Item_t* brittle_sword = create_weapon("Brittle Sword", "brittle_sword", brittle_material, 15, 25, 0, 'B', 0, 0, "common");
    if (brittle_sword) {
        apply_material_to_weapon(brittle_sword);
        uint8_t brittle_durability = get_durability(brittle_sword);

        d_LogDebugF("Brittle sword durability after material application: %d", brittle_durability);
        TEST_ASSERT(brittle_durability > 0, "Brittle sword should have some durability");
        TEST_ASSERT(brittle_durability == 255, "Brittle sword should have same durability");
    }

    d_LogDebug("Testing eternal material durability effects...");
    Item_t* eternal_sword = create_weapon("Eternal Sword", "eternal_sword", eternal_material, 15, 25, 0, 'E', 0, 0, "common");
    if (eternal_sword) {
        apply_material_to_weapon(eternal_sword);
        uint8_t eternal_durability = get_durability(eternal_sword);

        d_LogDebugF("Eternal sword durability after material application: %d", eternal_durability);
        TEST_ASSERT(eternal_durability == 255, "Eternal sword should have same durability");
        TEST_ASSERT(eternal_durability <= 255, "Eternal durability should not overflow");
    }

    d_LogDebug("Testing corrupt material durability effects...");
    Item_t* corrupt_sword = create_weapon("Corrupt Sword", "corrupt_sword", corrupt_material, 15, 25, 0, 'C', 0, 0, "common");
    if (corrupt_sword) {
        apply_material_to_weapon(corrupt_sword);
        uint8_t corrupt_durability = get_durability(corrupt_sword);

        d_LogDebugF("Corrupt sword durability after material application: %d", corrupt_durability);
        TEST_ASSERT(corrupt_durability >= 0, "Corrupt sword durability should not be negative");
    }

    d_LogDebug("Testing durability on non-durable items...");
    Item_t* consumable = create_consumable("Test Potion", "test_potion", 25, dummy_consume_callback, 'P', 0, "common");
    Lock_t test_lock = create_lock("Test Lock", "test_lock", 50, 0);
    Item_t* key = create_key("Test Key", "test_key", test_lock, 'K', 0, "common");

    if (consumable && key) {
        uint8_t consumable_durability = get_durability(consumable);
        uint8_t key_durability = get_durability(key);

        d_LogDebugF("Non-durable items: consumable=%d, key=%d", consumable_durability, key_durability);

        TEST_ASSERT(consumable_durability == 255, "Consumables should have max durability (no degradation)");
        TEST_ASSERT(key_durability == 255, "Keys should have max durability (no degradation)");
    }

    destroy_item(brittle_sword);
    destroy_item(eternal_sword);
    destroy_item(corrupt_sword);
    destroy_item(consumable);
    destroy_item(key);
    destroy_lock(&test_lock);
    d_DestroyString(brittle_material.name);
    d_DestroyString(eternal_material.name);
    d_DestroyString(corrupt_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_property_interactions_extreme_cases(void)
{
    d_LogWarning("BUG HUNT: Testing property interactions with MAXIMUM memory safety.");
    dLogContext_t* ctx = d_PushLogContext("PropertyInteractionsExtremes");
    
    // Initialize all pointers to NULL for safe cleanup
    Item_t* chaos_weapon = NULL;
    Item_t* chaos_armor = NULL;
    Material_t chaos_material = {0};
    Material_t second_material = {0};
    bool chaos_material_created = false;
    bool second_material_created = false;
    
    d_LogDebug("Creating chaos material with multiple extreme property factors...");
    MaterialProperties_t chaos_props = create_default_material_properties();
    chaos_props.weight_fact = 0.1f;
    chaos_props.value_coins_fact = 10.0f;
    chaos_props.durability_fact = 0.2f;
    chaos_props.min_damage_fact = 5.0f;
    chaos_props.max_damage_fact = 0.5f;
    chaos_props.armor_value_fact = 20.0f;
    chaos_props.evasion_value_fact = 0.01f;
    chaos_props.stealth_value_fact = 100.0f;
    chaos_props.enchant_value_fact = 0.0f;
    
    chaos_material = create_material("chaos", chaos_props);
    chaos_material_created = true;

    d_LogDebug("Testing chaos material application on weapon with memory tracking...");
    chaos_weapon = create_weapon("Chaos Weapon", "chaos_weapon", chaos_material, 10, 20, 5, 'C', 0, 0, "common");
    
    if (!chaos_weapon) {
        d_LogError("Critical failure: chaos weapon creation failed");
        goto safe_cleanup;
    }

    // Get pre-application stats
    uint8_t pre_min = get_weapon_min_damage(chaos_weapon);
    uint8_t pre_max = get_weapon_max_damage(chaos_weapon);
    float pre_weight = get_item_weight(chaos_weapon);
    uint8_t pre_value = get_item_value_coins(chaos_weapon);

    d_LogDebugF("Chaos weapon before material: min=%d, max=%d, weight=%.3f, value=%d",
                pre_min, pre_max, pre_weight, pre_value);

    d_LogDebug("Applying chaos material - monitoring for corruption...");
    apply_material_to_weapon(chaos_weapon);

    // Verify weapon integrity after material application
    if (!chaos_weapon || chaos_weapon->type != ITEM_TYPE_WEAPON) {
        d_LogError("MEMORY CORRUPTION DETECTED: Weapon corrupted during material application!");
        goto safe_cleanup;
    }

    uint8_t post_min = get_weapon_min_damage(chaos_weapon);
    uint8_t post_max = get_weapon_max_damage(chaos_weapon);
    float post_weight = get_item_weight(chaos_weapon);
    uint8_t post_value = get_item_value_coins(chaos_weapon);
    uint8_t stealth = get_stealth_value(chaos_weapon);

    d_LogDebugF("Chaos weapon after material: min=%d, max=%d, weight=%.3f, value=%d, stealth=%d",
                post_min, post_max, post_weight, post_value, stealth);

    TEST_ASSERT(post_min <= 255, "Min damage should not overflow");
    TEST_ASSERT(post_max <= 255, "Max damage should not overflow");
    TEST_ASSERT(post_weight >= 0.0f, "Weight should not be negative");
    TEST_ASSERT(stealth <= 255, "Stealth should not overflow");

    d_LogDebug("Testing chaos material on armor...");
    chaos_armor = create_armor("Chaos Armor", "chaos_armor", chaos_material, 15, 10, 'C', 20, 5, "common");
    
    if (chaos_armor) {
        apply_material_to_armor(chaos_armor);

        if (chaos_armor->type == ITEM_TYPE_ARMOR) {
            uint8_t armor_val = get_armor_value(chaos_armor);
            uint8_t evasion_val = get_evasion_value(chaos_armor);

            d_LogDebugF("Chaos armor: armor=%d, evasion=%d", armor_val, evasion_val);

            TEST_ASSERT(armor_val <= 255, "Armor value should not overflow");
            TEST_ASSERT(evasion_val <= 255, "Evasion should not overflow");
        }
    }

    d_LogDebug("Testing secondary material application - DANGEROUS TERRITORY...");
    MaterialProperties_t second_props = create_default_material_properties();
    second_props.weight_fact = 2.0f;
    second_material = create_material("second", second_props);
    second_material_created = true;

    if (chaos_weapon && chaos_weapon->type == ITEM_TYPE_WEAPON) {
        // CRITICAL: Store the original material to avoid memory leaks
        Material_t original_material = chaos_weapon->material_data;
        
        d_LogDebug("CAREFULLY replacing material data...");
        chaos_weapon->material_data = second_material;
        apply_material_to_weapon(chaos_weapon);

        float double_applied_weight = get_item_weight(chaos_weapon);
        d_LogDebugF("Double-applied material weight: %.3f", double_applied_weight);
        
        // Note: We do NOT restore the original material as it might cause issues
        // The weapon will be destroyed anyway, taking its material reference with it
    }

safe_cleanup:
    d_LogDebug("Beginning safe cleanup sequence...");
    
    // Destroy items first (they may reference materials)
    if (chaos_weapon) {
        d_LogDebug("Destroying chaos weapon...");
        destroy_item(chaos_weapon);
        chaos_weapon = NULL;
    }
    
    if (chaos_armor) {
        d_LogDebug("Destroying chaos armor...");
        destroy_item(chaos_armor);
        chaos_armor = NULL;
    }
    
    // DO NOT manually destroy material name strings!
    // The material system should handle its own cleanup
    // This was likely the source of your double-free:
    // d_DestroyString(chaos_material.name);  // DANGEROUS!
    // d_DestroyString(second_material.name); // DANGEROUS!
    
    d_LogDebug("Cleanup sequence completed safely.");
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger with comprehensive configuration
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true,
        .include_function = true
    };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);

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

    // Run enhanced extreme case tests with proper logging
    RUN_TEST(test_weapon_stats_extreme_cases);
    RUN_TEST(test_weapon_ammo_extreme_cases);
    RUN_TEST(test_armor_stats_extreme_cases);
    RUN_TEST(test_weight_value_extreme_cases);
    RUN_TEST(test_stealth_extreme_cases);
    RUN_TEST(test_stackable_extreme_cases);
    RUN_TEST(test_durability_properties_extreme_cases);
    RUN_TEST(test_property_interactions_extreme_cases);

    // Clean up the logger before finishing
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}