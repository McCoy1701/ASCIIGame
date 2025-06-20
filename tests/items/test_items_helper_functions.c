/*
 * ASCIIGame/tests/items/test_items_helper_functions.c
 *
 * Tests for helper functions used in item creation and management.
 * These functions handle dString_t operations and provide reusable
 * functionality across the item creation system.
 */

#include "../../include/tests.h"
#include "../../include/items.h"
#include "../../include/structs.h"
#include "../../include/defs.h"
#include "../../include/Daedalus.h"
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
    Material_t material;

    // Create dString_t for material name
    dString_t* temp_name = d_InitString();
    d_AppendString(temp_name, material_name, 0);
    material.name = *temp_name;
    free(temp_name);

    // Set properties
    material.properties.weight_fact = 1.2f;
    material.properties.value_coins_fact = 1.1f;
    material.properties.durability_fact = 0.9f;
    material.properties.min_damage_fact = 1.1f;
    material.properties.max_damage_fact = 1.1f;
    material.properties.armor_value_fact = 1.0f;
    material.properties.evasion_value_fact = 1.0f;
    material.properties.stealth_value_fact = 0.8f;
    material.properties.enchant_value_fact = 1.0f;

    return material;
}

/*
 * Helper function to create a test lock with dString_t fields
 */
Lock_t create_test_lock_with_dstring(const char* lock_name, const char* lock_desc)
{
    Lock_t lock;

    // Create dString_t for lock name
    dString_t* temp_name = d_InitString();
    d_AppendString(temp_name, lock_name, 0);
    lock.name = *temp_name;
    free(temp_name);

    // Create dString_t for lock description
    dString_t* temp_desc = d_InitString();
    d_AppendString(temp_desc, lock_desc, 0);
    lock.description = *temp_desc;
    free(temp_desc);

    lock.pick_difficulty = 50;
    lock.jammed_seconds = 0;
    return lock;
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
    printf(TEST_COLOR_BLUE "Testing string field population..." TEST_COLOR_RESET "\n");

    Material_t material = create_test_material_with_dstring("steel");

    // Test weapon creation to verify _populate_string_field works
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 'S');

    TEST_ASSERT(weapon != NULL, "Weapon should be created successfully");
    TEST_ASSERT(dstring_equals(&weapon->name, "Test Sword"), "Weapon name should be properly populated");
    TEST_ASSERT(dstring_equals(&weapon->id, "test_sword"), "Weapon ID should be properly populated");
    TEST_ASSERT(weapon->name.len == strlen("Test Sword"), "Name length should be correct");
    TEST_ASSERT(weapon->id.len == strlen("test_sword"), "ID length should be correct");

    // Clean up
    destroy_item(weapon);
    free(material.name.str);

    return 1;
}

/*
 * Test weapon description generation
 */
int test_weapon_description_generation(void)
{
    printf(TEST_COLOR_BLUE "Testing weapon description generation..." TEST_COLOR_RESET "\n");

    Material_t iron_material = create_test_material_with_dstring("iron");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test with iron material
    Item_t* iron_sword = create_weapon("Iron Blade", "iron_blade", iron_material, 8, 12, 0, 'I');
    TEST_ASSERT(iron_sword != NULL, "Iron sword should be created");
    TEST_ASSERT(dstring_equals(&iron_sword->description, "A weapon made of iron"),
                "Iron sword description should be correct");

    // Test with steel material
    Item_t* steel_sword = create_weapon("Steel Blade", "steel_blade", steel_material, 12, 18, 1, 'S');
    TEST_ASSERT(steel_sword != NULL, "Steel sword should be created");
    TEST_ASSERT(dstring_equals(&steel_sword->description, "A weapon made of steel"),
                "Steel sword description should be correct");

    // Clean up
    destroy_item(iron_sword);
    destroy_item(steel_sword);
    free(iron_material.name.str);
    free(steel_material.name.str);

    return 1;
}

/*
 * Test armor description generation
 */
int test_armor_description_generation(void)
{
    printf(TEST_COLOR_BLUE "Testing armor description generation..." TEST_COLOR_RESET "\n");

    Material_t leather_material = create_test_material_with_dstring("leather");
    Material_t chainmail_material = create_test_material_with_dstring("chainmail");

    // Test with leather material
    Item_t* leather_armor = create_armor("Leather Vest", "leather_vest", leather_material, 15, 8, 'L');
    TEST_ASSERT(leather_armor != NULL, "Leather armor should be created");
    TEST_ASSERT(dstring_equals(&leather_armor->description, "Armor made of leather"),
                "Leather armor description should be correct");

    // Test with chainmail material
    Item_t* chain_armor = create_armor("Chain Mail", "chain_mail", chainmail_material, 25, 12, 'C');
    TEST_ASSERT(chain_armor != NULL, "Chain armor should be created");
    TEST_ASSERT(dstring_equals(&chain_armor->description, "Armor made of chainmail"),
                "Chain armor description should be correct");

    // Clean up
    destroy_item(leather_armor);
    destroy_item(chain_armor);
    free(leather_material.name.str);
    free(chainmail_material.name.str);

    return 1;
}

/*
 * Test key description generation
 */
int test_key_description_generation(void)
{
    printf(TEST_COLOR_BLUE "Testing key description generation..." TEST_COLOR_RESET "\n");

    Lock_t treasure_lock = create_test_lock_with_dstring("Treasure Chest", "A golden treasure chest");
    Lock_t door_lock = create_test_lock_with_dstring("Ancient Door", "An old wooden door");

    // Test with treasure chest lock
    Item_t* treasure_key = create_key("Golden Key", "treasure_key", treasure_lock, 'T');
    TEST_ASSERT(treasure_key != NULL, "Treasure key should be created");
    TEST_ASSERT(dstring_equals(&treasure_key->description, "A key that opens: Treasure Chest"),
                "Treasure key description should be correct");

    // Test with door lock
    Item_t* door_key = create_key("Old Key", "door_key", door_lock, 'D');
    TEST_ASSERT(door_key != NULL, "Door key should be created");
    TEST_ASSERT(dstring_equals(&door_key->description, "A key that opens: Ancient Door"),
                "Door key description should be correct");

    // Clean up
    destroy_item(treasure_key);
    destroy_item(door_key);
    free(treasure_lock.name.str);
    free(treasure_lock.description.str);
    free(door_lock.name.str);
    free(door_lock.description.str);

    return 1;
}

/*
 * Test consumable description generation with potency values
 */
int test_consumable_description_generation(void)
{
    printf(TEST_COLOR_BLUE "Testing consumable description generation..." TEST_COLOR_RESET "\n");

    // Dummy callback for testing
    void test_heal(uint8_t value) { /* Test function */ }

    // Test different potency values
    Item_t* weak_potion = create_consumable("Weak Potion", "weak_potion", 25, test_heal, 'w');
    TEST_ASSERT(weak_potion != NULL, "Weak potion should be created");
    TEST_ASSERT(dstring_equals(&weak_potion->description, "A consumable item with magical properties (Potency: 25)"),
                "Weak potion description should include potency");

    Item_t* strong_potion = create_consumable("Strong Potion", "strong_potion", 100, test_heal, 'S');
    TEST_ASSERT(strong_potion != NULL, "Strong potion should be created");
    TEST_ASSERT(dstring_equals(&strong_potion->description, "A consumable item with magical properties (Potency: 100)"),
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
    printf(TEST_COLOR_BLUE "Testing ammunition description generation..." TEST_COLOR_RESET "\n");

    Material_t wood_material = create_test_material_with_dstring("wood");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test wooden arrows
    Item_t* wood_arrows = create_ammunition("Wooden Arrows", "wood_arrows", wood_material, 3, 5, 'w');
    TEST_ASSERT(wood_arrows != NULL, "Wooden arrows should be created");
    TEST_ASSERT(dstring_equals(&wood_arrows->description, "Ammunition made of wood (Damage: 3-5)"),
                "Wooden arrows description should include material and damage");

    // Test steel bolts
    Item_t* steel_bolts = create_ammunition("Steel Bolts", "steel_bolts", steel_material, 8, 12, 'S');
    TEST_ASSERT(steel_bolts != NULL, "Steel bolts should be created");
    TEST_ASSERT(dstring_equals(&steel_bolts->description, "Ammunition made of steel (Damage: 8-12)"),
                "Steel bolts description should include material and damage");

    // Clean up
    destroy_item(wood_arrows);
    destroy_item(steel_bolts);
    free(wood_material.name.str);
    free(steel_material.name.str);

    return 1;
}

/*
 * Test rarity population
 */
int test_rarity_population(void)
{
    printf(TEST_COLOR_BLUE "Testing rarity population..." TEST_COLOR_RESET "\n");

    Material_t material = create_test_material_with_dstring("iron");

    // All items should start with "common" rarity
    Item_t* weapon = create_weapon("Test Weapon", "test_weapon", material, 10, 15, 0, 'W');
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 10, 'A');

    TEST_ASSERT(weapon != NULL && armor != NULL, "Items should be created");
    TEST_ASSERT(dstring_equals(&weapon->rarity, "common"), "Weapon rarity should be 'common'");
    TEST_ASSERT(dstring_equals(&armor->rarity, "common"), "Armor rarity should be 'common'");

    // Clean up
    destroy_item(weapon);
    destroy_item(armor);
    free(material.name.str);

    return 1;
}

/*
 * Test error handling in helper functions (through create functions)
 */
int test_error_handling(void)
{
    printf(TEST_COLOR_BLUE "Testing error handling..." TEST_COLOR_RESET "\n");

    Material_t material = create_test_material_with_dstring("test");

    // Test NULL name
    Item_t* null_name_weapon = create_weapon(NULL, "test_id", material, 10, 15, 0, 'N');
    TEST_ASSERT(null_name_weapon == NULL, "Creating weapon with NULL name should fail");

    // Test NULL id
    Item_t* null_id_weapon = create_weapon("Test Name", NULL, material, 10, 15, 0, 'N');
    TEST_ASSERT(null_id_weapon == NULL, "Creating weapon with NULL id should fail");

    // Clean up
    free(material.name.str);

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

    TEST_SUITE_END();

    return 0;
}
