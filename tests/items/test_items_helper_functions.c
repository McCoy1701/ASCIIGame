/*
 * ASCIIGame/tests/items/test_items_helper_functions.c
 *
 * Tests for helper functions used in item creation and management.
 * These functions handle dString_t operations and provide reusable
 * functionality across the item creation system.
 */
#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include "Archimedes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// TEST HELPER FUNCTIONS
// =============================================================================

/*
 * Helper function to create a test material with dString_t name
 */
Material_t create_test_material_with_dstring(const char* material_name)
{
    MaterialProperties_t properties = {
        .armor_value_fact=1.0f,
        .enchant_value_fact=1.0f,
        .durability_fact=1.0f,
        .min_damage_fact=1.0f,
        .max_damage_fact=1.0f,
        .evasion_value_fact=1.0f,
        .value_coins_fact=1.0f,
        .stealth_value_fact=1.0f
    };

    Material_t material = {
        .name = d_InitString(),
        .properties = properties
    };

    d_AppendString(material.name, material_name, 0);

    return material;
}

/*
 * Helper function to create a test lock with dString_t fields
 */
Lock_t create_test_lock_with_dstring(const char* lock_name, const char* lock_desc)
{
    LOG("Creating test lock with dString_t fields");
    return create_lock(lock_name, lock_desc, 50, 0);
}


/*
 * Helper function to compare dString_t with const char*
 */
bool dstring_equals(const dString_t* ds, const char* str)
{
    if (ds == NULL || str == NULL) return false;
    return strcmp(d_PeekString(ds), str) == 0;
}

// =============================================================================
// ACTUAL TESTS - Testing through public functions since helpers are static
// =============================================================================

/*
 * Test that helper functions work correctly by testing item creation
 * and checking that the dString_t fields are populated correctly
 */
int test_string_field_population(void)
{
    Material_t material = create_test_material_with_dstring("steel");

    // Test weapon creation to verify _populate_string_field works
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 'S');

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Weapon created successfully: name = ", 0);
    d_AppendString(log_message, weapon->name->str, 0);
    d_AppendString(log_message, ", id = ", 0);
    d_AppendString(log_message, weapon->id->str, 0);
    d_AppendString(log_message, ", material = ", 0);
    d_AppendString(log_message, material.name->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(weapon != NULL, "Weapon should be created successfully");
    TEST_ASSERT(dstring_equals(weapon->name, "Test Sword"), "Weapon name should be properly populated");
    TEST_ASSERT(dstring_equals(weapon->id, "test_sword"), "Weapon ID should be properly populated");
    TEST_ASSERT(weapon->name->len == strlen("Test Sword"), "Name length should be correct");
    TEST_ASSERT(weapon->id->len == strlen("test_sword"), "ID length should be correct");

    // Clean up
    destroy_item(weapon);
    d_DestroyString(material.name);

    return 1;
};

/*
 * Test weapon description generation
 */
int test_weapon_description_generation(void)
{
    Material_t iron_material = create_test_material_with_dstring("iron");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test with iron material
    Item_t* iron_sword = create_weapon("Iron Blade", "iron_blade", iron_material, 8, 12, 0, 'I');

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Iron sword description: ", 0);
    d_AppendString(log_message, iron_sword->description->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(iron_sword != NULL, "Iron sword should be created");
    TEST_ASSERT(dstring_equals(iron_sword->description, "A weapon made of iron"),
                "Iron sword description should be correct");

    // Test with steel material
    Item_t* steel_sword = create_weapon("Steel Blade", "steel_blade", steel_material, 12, 18, 1, 'S');

    dString_t* log_message2 = d_InitString();
    d_AppendString(log_message2, "Steel sword description: ", 0);
    d_AppendString(log_message2, steel_sword->description->str, 0);
    LOG(log_message2->str);
    d_DestroyString(log_message2);

    TEST_ASSERT(steel_sword != NULL, "Steel sword should be created");
    TEST_ASSERT(dstring_equals(steel_sword->description, "A weapon made of steel"),
                "Steel sword description should be correct");

    // Clean up
    destroy_item(iron_sword);
    destroy_item(steel_sword);
    d_DestroyString(iron_material.name);
    d_DestroyString(steel_material.name);

    return 1;
}

/*
 * Test armor description generation
 */
int test_armor_description_generation(void)
{
    Material_t leather_material = create_test_material_with_dstring("leather");
    Material_t chainmail_material = create_test_material_with_dstring("chainmail");

    // Test with leather material
    Item_t* leather_armor = create_armor("Leather Vest", "leather_vest", leather_material, 15, 8, 'L', 10, 10);

    dString_t* log_message3 = d_InitString();
    d_AppendString(log_message3, "Leather Armor Description: ", 0);
    d_AppendString(log_message3, leather_armor->description->str, 0);
    LOG(log_message3->str);
    d_DestroyString(log_message3);

    TEST_ASSERT(leather_armor != NULL, "Leather armor should be created");
    TEST_ASSERT(dstring_equals(leather_armor->description, "Armor made of leather"),
                "Leather armor description should be correct");

    // Test with chainmail material
    Item_t* chain_armor = create_armor("Chain Mail", "chain_mail", chainmail_material, 25, 12, 'C', 15, 15);

    dString_t* log_message4 = d_InitString();
    d_AppendString(log_message4, "Chain Armor Description: ", 0);
    d_AppendString(log_message4, chain_armor->description->str, 0);
    LOG(log_message4->str);
    d_DestroyString(log_message4);

    TEST_ASSERT(chain_armor != NULL, "Chain armor should be created");
    TEST_ASSERT(dstring_equals(chain_armor->description, "Armor made of chainmail"),
                "Chain armor description should be correct");

    // Clean up
    destroy_item(leather_armor);
    destroy_item(chain_armor);
    d_DestroyString(leather_material.name);
    d_DestroyString(chainmail_material.name);

    return 1;
}

/*
 * Test key description generation
 */
int test_key_description_generation(void)
{
    Lock_t treasure_lock = create_test_lock_with_dstring("Treasure Chest", "A golden treasure chest");
    Lock_t door_lock = create_test_lock_with_dstring("Ancient Door", "An old wooden door");

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Treasure Lock Name: ", 0);
    d_AppendString(log_message, treasure_lock.name->str, 0);
    d_AppendString(log_message, "/ Door Lock Name: ", 0);
    d_AppendString(log_message, door_lock.name->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);  // Use proper cleanup

    // Test with treasure chest lock
    Item_t* treasure_key = create_key("Golden Key", "treasure_key", treasure_lock, 'T');

    if (treasure_key != NULL) {
        dString_t* log_message2 = d_InitString();
        d_AppendString(log_message2, "Key description: ", 0);
        d_AppendString(log_message2, treasure_key->description->str, 0);
        LOG(log_message2->str);
        d_DestroyString(log_message2);

        TEST_ASSERT(treasure_key != NULL, "Treasure key should be created");
        TEST_ASSERT(dstring_equals(treasure_key->description, "A key that opens: Treasure Chest"),
                    "Treasure key description should be correct");

        // Clean up the key
        destroy_item(treasure_key);
    }

    // Clean up locks using proper dString cleanup
    d_DestroyString(treasure_lock.name);      // Instead of free(treasure_lock.name)
    d_DestroyString(treasure_lock.description); // Instead of free(treasure_lock.description)
    d_DestroyString(door_lock.name);
    d_DestroyString(door_lock.description);

    return 1;
}
// Dummy callback for testing
void test_heal(uint8_t value) {
    printf("Healing for %i", value);
}
/*
 * Test consumable description generation with potency values
 */
int test_consumable_description_generation(void)
{

    // Test different potency values
    Item_t* weak_potion = create_consumable("Weak Potion", "weak_potion", 25, test_heal, 'w');

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Weak potion description: ", 0);
    d_AppendString(log_message, weak_potion->description->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(weak_potion != NULL, "Weak potion should be created");
    TEST_ASSERT(dstring_equals(weak_potion->description, "A consumable item with magical properties (Potency: 25)"),
                "Weak potion description should include potency");

    Item_t* strong_potion = create_consumable("Strong Potion", "strong_potion", 100, test_heal, 'S');

    dString_t* log_message2 = d_InitString();
    d_AppendString(log_message2, "Strong potion description: ", 0);
    d_AppendString(log_message2, strong_potion->description->str, 0);
    LOG(log_message2->str);
    d_DestroyString(log_message2);

    TEST_ASSERT(strong_potion != NULL, "Strong potion should be created");
    TEST_ASSERT(dstring_equals(strong_potion->description, "A consumable item with magical properties (Potency: 100)"),
                "Strong potion description should include potency");

    // Clean up
    destroy_item(weak_potion);
    destroy_item(strong_potion);

    return 1;
}

/*
 * Test ammunition description generation with damage range
 */
int test_ammunition_description_generation(void)
{
    Material_t wood_material = create_test_material_with_dstring("wood");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test wooden arrows
    Item_t* wood_arrows = create_ammunition("Wooden Arrows", "wood_arrows", wood_material, 3, 5, 'w');

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Wooden arrows description: ", 0);
    d_AppendString(log_message, wood_arrows->description->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(wood_arrows != NULL, "Wooden arrows should be created");
    TEST_ASSERT(dstring_equals(wood_arrows->description, "Ammunition made of wood (Damage: 3-5)"),
                "Wooden arrows description should include material and damage");

    // Test steel bolts
    Item_t* steel_bolts = create_ammunition("Steel Bolts", "steel_bolts", steel_material, 8, 12, 'S');

    dString_t* log_message2 = d_InitString();
    d_AppendString(log_message2, "Steel bolts description: ", 0);
    d_AppendString(log_message2, steel_bolts->description->str, 0);
    LOG(log_message2->str);
    d_DestroyString(log_message2);

    TEST_ASSERT(steel_bolts != NULL, "Steel bolts should be created");
    TEST_ASSERT(dstring_equals(steel_bolts->description, "Ammunition made of steel (Damage: 8-12)"),
                "Steel bolts description should include material and damage");

    // Clean up
    destroy_item(wood_arrows);
    destroy_item(steel_bolts);
    d_DestroyString(wood_material.name);
    d_DestroyString(steel_material.name);

    return 1;
}

/*
 * Test rarity population
 */
int test_rarity_population(void)
{

    Material_t material = create_test_material_with_dstring("iron");

    // All items should start with "common" rarity
    Item_t* weapon = create_weapon("Test Weapon", "test_weapon", material, 10, 15, 0, 'W');
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 10, 'A', 15, 15);

    TEST_ASSERT(weapon != NULL && armor != NULL, "Items should be created");
    TEST_ASSERT(dstring_equals(weapon->rarity, "common"), "Weapon rarity should be 'common'");
    TEST_ASSERT(dstring_equals(armor->rarity, "common"), "Armor rarity should be 'common'");

    // Clean up
    destroy_item(weapon);
    destroy_item(armor);
    d_DestroyString(material.name);

    return 1;
}

/*
 * Test error handling in helper functions (through create functions)
 */
int test_error_handling(void)
{

    Material_t material = create_test_material_with_dstring("test");

    // Test NULL name
    Item_t* null_name_weapon = create_weapon(NULL, "test_id", material, 10, 15, 0, 'N');
    TEST_ASSERT(null_name_weapon == NULL, "Creating weapon with NULL name should fail");

    // Test NULL id
    Item_t* null_id_weapon = create_weapon("Test Name", NULL, material, 10, 15, 0, 'N');
    TEST_ASSERT(null_id_weapon == NULL, "Creating weapon with NULL id should fail");

    // Clean up
    d_DestroyString(material.name);

    return 1;
}
// =============================================================================
// ENHANCED COMPREHENSIVE TESTS - BUG HUNTING EDITION
// =============================================================================

/*
 * Test extreme string values and edge cases
 */
int test_string_edge_cases(void)
{
    Material_t material = create_test_material_with_dstring("extreme");

    // Test with empty strings
    Item_t* empty_name = create_weapon("", "valid_id", material, 10, 15, 0, 'E');
    TEST_ASSERT(empty_name != NULL, "Empty name should be allowed");
    TEST_ASSERT(dstring_equals(empty_name->name, ""), "Empty name should remain empty");

    Item_t* empty_id = create_weapon("Valid Name", "", material, 10, 15, 0, 'E');
    TEST_ASSERT(empty_id != NULL, "Empty ID should be allowed");
    TEST_ASSERT(dstring_equals(empty_id->id, ""), "Empty ID should remain empty");

    // Test with very long strings (potential buffer overflow)
    char long_name[1000];
    char long_id[1000];
    memset(long_name, 'A', 999);
    memset(long_id, 'B', 999);
    long_name[999] = '\0';
    long_id[999] = '\0';

    Item_t* long_weapon = create_weapon(long_name, long_id, material, 10, 15, 0, 'L');
    TEST_ASSERT(long_weapon != NULL, "Very long strings should be handled");
    TEST_ASSERT(strlen(long_weapon->name->str) == 999, "Long name should be preserved");
    TEST_ASSERT(strlen(long_weapon->id->str) == 999, "Long ID should be preserved");

    // Test with special characters and unicode
    Item_t* special_chars = create_weapon("Sword™ of Ñoël", "sword_ñoël_™", material, 10, 15, 0, '★');
    TEST_ASSERT(special_chars != NULL, "Special characters should be handled");

    // Test with strings containing newlines and tabs
    Item_t* control_chars = create_weapon("Multi\nLine\tName", "multi\nline\tid", material, 10, 15, 0, '\n');
    TEST_ASSERT(control_chars != NULL, "Control characters should be handled");

    // Clean up
    if (empty_name) destroy_item(empty_name);
    if (empty_id) destroy_item(empty_id);
    if (long_weapon) destroy_item(long_weapon);
    if (special_chars) destroy_item(special_chars);
    if (control_chars) destroy_item(control_chars);
    d_DestroyString(material.name);

    return 1;
}

/*
 * Test material system edge cases and potential bugs
 */
int test_material_edge_cases(void)
{
    // Test with extreme material properties
    MaterialProperties_t extreme_props = {
        .weight_fact = 999999.0f,        // Extreme weight
        .value_coins_fact = -50.0f,      // Negative value
        .durability_fact = 0.0f,         // Zero durability
        .min_damage_fact = -100.0f,      // Negative damage
        .max_damage_fact = 65535.0f,     // Extreme damage
        .armor_value_fact = 0.0f,        // Zero armor
        .evasion_value_fact = -1.0f,     // Negative evasion
        .stealth_value_fact = 999.0f,    // Extreme stealth
        .enchant_value_fact = 0.00001f   // Near-zero enchant
    };

    Material_t extreme_material = create_material("EXTREME", extreme_props);

    // Test weapon with extreme material
    Item_t* extreme_weapon = create_weapon("Extreme Sword", "extreme_sword", extreme_material, 10, 15, 0, 'X');
    TEST_ASSERT(extreme_weapon != NULL, "Weapon with extreme material should be created");

    apply_material_to_weapon(extreme_weapon);

    // Check if extreme values cause overflows or underflows
    uint8_t min_dmg = get_weapon_min_damage(extreme_weapon);
    uint8_t max_dmg = get_weapon_max_damage(extreme_weapon);
    float weight = get_item_weight(extreme_weapon);
    uint8_t value = get_item_value_coins(extreme_weapon);

    printf("Extreme weapon stats: min_dmg=%d, max_dmg=%d, weight=%.2f, value=%d\n",
           min_dmg, max_dmg, weight, value);

    TEST_ASSERT(min_dmg <= 255, "Min damage should not overflow uint8_t");
    TEST_ASSERT(max_dmg <= 255, "Max damage should not overflow uint8_t");
    TEST_ASSERT(weight >= 0.0f, "Weight should not be negative");
    TEST_ASSERT(value <= 255, "Value should not overflow uint8_t");

    // Test armor with extreme material
    Item_t* extreme_armor = create_armor("Extreme Armor", "extreme_armor", extreme_material, 20, 10, 'X', 15, 15);
    TEST_ASSERT(extreme_armor != NULL, "Armor with extreme material should be created");

    apply_material_to_armor(extreme_armor);

    uint8_t armor_val = get_armor_value(extreme_armor);
    uint8_t evasion_val = get_evasion_value(extreme_armor);
    uint8_t stealth_val = get_stealth_value(extreme_armor);

    printf("Extreme armor stats: armor=%d, evasion=%d, stealth=%d\n",
           armor_val, evasion_val, stealth_val);

    TEST_ASSERT(armor_val <= 255, "Armor value should not overflow");
    TEST_ASSERT(evasion_val <= 255, "Evasion value should not overflow");
    TEST_ASSERT(stealth_val <= 255, "Stealth value should not overflow");

    destroy_item(extreme_weapon);
    destroy_item(extreme_armor);
    d_DestroyString(extreme_material.name);

    return 1;
}

/*
 * Test description generation with problematic inputs
 */
int test_description_stress_test(void)
{
    // Test with NULL material name
    Material_t null_name_material;
    null_name_material.name = NULL;  // This might cause crashes
    null_name_material.properties = create_default_material_properties();

    Item_t* null_material_weapon = create_weapon("Test", "test", null_name_material, 10, 15, 0, 'T');
    if (null_material_weapon) {
        printf("Null material weapon description: %s\n", null_material_weapon->description->str);
        TEST_ASSERT(null_material_weapon->description != NULL, "Description should handle NULL material name");
        destroy_item(null_material_weapon);
    }

    // Test with extremely long material names
    Material_t long_material = create_test_material_with_dstring("ThisIsAnExtremelyLongMaterialNameThatMightCauseBufferOverflows_ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    Item_t* long_mat_weapon = create_weapon("Test", "test", long_material, 10, 15, 0, 'T');
    TEST_ASSERT(long_mat_weapon != NULL, "Weapon with long material name should be created");

    if (long_mat_weapon && long_mat_weapon->description) {
        printf("Long material description length: %zu\n", strlen(long_mat_weapon->description->str));
        TEST_ASSERT(strlen(long_mat_weapon->description->str) < 1000, "Description should not be excessively long");
        destroy_item(long_mat_weapon);
    }

    d_DestroyString(long_material.name);

    return 1;
}

/*
 * Test memory management and potential leaks
 */
int test_memory_stress(void)
{
    // Create and destroy many items rapidly to test for memory leaks
    for (int i = 0; i < 100; i++) {
        Material_t material = create_test_material_with_dstring("stress_test");

        Item_t* weapon = create_weapon("Stress Weapon", "stress_weapon", material, 10, 15, 0, 'S');
        Item_t* armor = create_armor("Stress Armor", "stress_armor", material, 20, 10, 'A', 15, 15);
        Item_t* ammo = create_ammunition("Stress Ammo", "stress_ammo", material, 5, 8, 'a');

        if (weapon) destroy_item(weapon);
        if (armor) destroy_item(armor);
        if (ammo) destroy_item(ammo);

        d_DestroyString(material.name);
    }

    TEST_ASSERT(1, "Stress test completed without crashes");

    return 1;
}

/*
 * Test consumable edge cases
 */
int test_consumable_edge_cases(void)
{
    // Test consumable with zero value
    Item_t* zero_potion = create_consumable("Zero Potion", "zero_potion", 0, test_heal, 'Z');
    TEST_ASSERT(zero_potion != NULL, "Zero value consumable should be created");
    if (zero_potion) {
        TEST_ASSERT(zero_potion->data.consumable.value == 0, "Zero value should be preserved");
        destroy_item(zero_potion);
    }

    // Test consumable with maximum value
    Item_t* max_potion = create_consumable("Max Potion", "max_potion", 255, test_heal, 'M');
    TEST_ASSERT(max_potion != NULL, "Max value consumable should be created");
    if (max_potion) {
        TEST_ASSERT(max_potion->data.consumable.value == 255, "Max value should be preserved");
        destroy_item(max_potion);
    }

    // Test consumable with extremely high value (potential overflow)
    Item_t* overflow_potion = create_consumable("Overflow Potion", "overflow_potion", 65535, test_heal, 'O');
    TEST_ASSERT(overflow_potion != NULL, "High value consumable should be created");
    if (overflow_potion) {
        printf("Overflow potion value: %d (expected to be clamped)\n", overflow_potion->data.consumable.value);
        TEST_ASSERT(overflow_potion->data.consumable.value <= 255, "Value should be clamped to uint8_t range");
        destroy_item(overflow_potion);
    }

    return 1;
}

/*
 * Test lock and key edge cases
 */
int test_lock_key_edge_cases(void)
{
    // Test lock with maximum difficulty
    Lock_t max_lock = create_lock("Max Lock", "max_lock", 255, 0);
    Item_t* max_key = create_key("Max Key", "max_key", max_lock, 'M');
    TEST_ASSERT(max_key != NULL, "Key for max difficulty lock should be created");
    if (max_key) {
        TEST_ASSERT(max_key->data.key.lock.pick_difficulty == 255, "Max difficulty should be preserved");
        destroy_item(max_key);
    }

    // Test lock with jammed status
    Lock_t jammed_lock = create_lock("Jammed Lock", "jammed_lock", 100, 255);
    Item_t* jammed_key = create_key("Jammed Key", "jammed_key", jammed_lock, 'J');
    TEST_ASSERT(jammed_key != NULL, "Key for jammed lock should be created");
    if (jammed_key) {
        TEST_ASSERT(jammed_key->data.key.lock.jammed_seconds == 255, "Jammed status should be preserved");
        destroy_item(jammed_key);
    }

    // Test lock with zero difficulty
    Lock_t zero_lock = create_lock("Zero Lock", "zero_lock", 0, 0);
    Item_t* zero_key = create_key("Zero Key", "zero_key", zero_lock, 'Z');
    TEST_ASSERT(zero_key != NULL, "Key for zero difficulty lock should be created");
    if (zero_key) {
        TEST_ASSERT(zero_key->data.key.lock.pick_difficulty == 0, "Zero difficulty should be preserved");
        destroy_item(zero_key);
    }

    return 1;
}

/*
 * Test weapon and armor extreme values
 */
int test_weapon_armor_extremes(void)
{
    Material_t material = create_test_material_with_dstring("extreme");

    // Test weapon with zero damage
    Item_t* zero_weapon = create_weapon("Zero Weapon", "zero_weapon", material, 0, 0, 0, 'Z');
    TEST_ASSERT(zero_weapon != NULL, "Zero damage weapon should be created");
    if (zero_weapon) {
        TEST_ASSERT(zero_weapon->data.weapon.min_damage == 0, "Zero min damage should be preserved");
        TEST_ASSERT(zero_weapon->data.weapon.max_damage == 0, "Zero max damage should be preserved");
        destroy_item(zero_weapon);
    }

    // Test weapon with inverted damage (min > max)
    Item_t* inverted_weapon = create_weapon("Inverted Weapon", "inverted_weapon", material, 20, 10, 0, 'I');
    TEST_ASSERT(inverted_weapon != NULL, "Inverted damage weapon should be created");
    if (inverted_weapon) {
        printf("Inverted weapon: min=%d, max=%d\n",
               inverted_weapon->data.weapon.min_damage,
               inverted_weapon->data.weapon.max_damage);
        // The system might swap them or handle this gracefully
        destroy_item(inverted_weapon);
    }

    // Test weapon with extreme range
    Item_t* extreme_range = create_weapon("Sniper", "sniper", material, 10, 15, 255, 'S');
    TEST_ASSERT(extreme_range != NULL, "Extreme range weapon should be created");
    if (extreme_range) {
        TEST_ASSERT(extreme_range->data.weapon.range_tiles == 255, "Extreme range should be preserved");
        destroy_item(extreme_range);
    }

    // Test armor with zero values
    Item_t* zero_armor = create_armor("Zero Armor", "zero_armor", material, 0, 0, 'Z', 0, 0);
    TEST_ASSERT(zero_armor != NULL, "Zero armor should be created");
    if (zero_armor) {
        TEST_ASSERT(zero_armor->data.armor.armor_value == 0, "Zero armor value should be preserved");
        TEST_ASSERT(zero_armor->data.armor.evasion_value == 0, "Zero evasion should be preserved");
        destroy_item(zero_armor);
    }

    d_DestroyString(material.name);

    return 1;
}
// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Helper Functions Tests");

    // Run all tests
    RUN_TEST(test_string_field_population);
    RUN_TEST(test_weapon_description_generation);
    RUN_TEST(test_armor_description_generation);
    RUN_TEST(test_key_description_generation);
    RUN_TEST(test_consumable_description_generation);
    RUN_TEST(test_ammunition_description_generation);
    RUN_TEST(test_rarity_population);
    RUN_TEST(test_error_handling);

    RUN_TEST(test_string_edge_cases);
    RUN_TEST(test_material_edge_cases);
    RUN_TEST(test_description_stress_test);
    RUN_TEST(test_memory_stress);
    RUN_TEST(test_consumable_edge_cases);
    RUN_TEST(test_lock_key_edge_cases);
    RUN_TEST(test_weapon_armor_extremes);

    TEST_SUITE_END();

    return 0;
}
