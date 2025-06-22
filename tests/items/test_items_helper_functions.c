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
    free(log_message);

    TEST_ASSERT(weapon != NULL, "Weapon should be created successfully");
    TEST_ASSERT(dstring_equals(weapon->name, "Test Sword"), "Weapon name should be properly populated");
    TEST_ASSERT(dstring_equals(weapon->id, "test_sword"), "Weapon ID should be properly populated");
    TEST_ASSERT(weapon->name->len == strlen("Test Sword"), "Name length should be correct");
    TEST_ASSERT(weapon->id->len == strlen("test_sword"), "ID length should be correct");

    // Clean up
    destroy_item(weapon);
    free(material.name);

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
    free(log_message);

    TEST_ASSERT(iron_sword != NULL, "Iron sword should be created");
    TEST_ASSERT(dstring_equals(iron_sword->description, "A weapon made of iron"),
                "Iron sword description should be correct");

    // Test with steel material
    Item_t* steel_sword = create_weapon("Steel Blade", "steel_blade", steel_material, 12, 18, 1, 'S');

    dString_t* log_message2 = d_InitString();
    d_AppendString(log_message2, "Steel sword description: ", 0);
    d_AppendString(log_message2, steel_sword->description->str, 0);
    LOG(log_message2->str);
    free(log_message2);

    TEST_ASSERT(steel_sword != NULL, "Steel sword should be created");
    TEST_ASSERT(dstring_equals(steel_sword->description, "A weapon made of steel"),
                "Steel sword description should be correct");

    // Clean up
    destroy_item(iron_sword);
    destroy_item(steel_sword);
    free(iron_material.name->str);
    free(steel_material.name->str);

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
    Item_t* leather_armor = create_armor("Leather Vest", "leather_vest", leather_material, 15, 8, 'L');

    dString_t* log_message3 = d_InitString();
    d_AppendString(log_message3, "Leather Armor Description: ", 0);
    d_AppendString(log_message3, leather_armor->description->str, 0);
    LOG(log_message3->str);
    free(log_message3);

    TEST_ASSERT(leather_armor != NULL, "Leather armor should be created");
    TEST_ASSERT(dstring_equals(leather_armor->description, "Armor made of leather"),
                "Leather armor description should be correct");

    // Test with chainmail material
    Item_t* chain_armor = create_armor("Chain Mail", "chain_mail", chainmail_material, 25, 12, 'C');

    dString_t* log_message4 = d_InitString();
    d_AppendString(log_message4, "Chain Armor Description: ", 0);
    d_AppendString(log_message4, chain_armor->description->str, 0);
    LOG(log_message4->str);
    free(log_message4);

    TEST_ASSERT(chain_armor != NULL, "Chain armor should be created");
    TEST_ASSERT(dstring_equals(chain_armor->description, "Armor made of chainmail"),
                "Chain armor description should be correct");

    // Clean up
    destroy_item(leather_armor);
    destroy_item(chain_armor);
    free(leather_material.name->str);
    free(chainmail_material.name->str);

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
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 10, 'A');

    TEST_ASSERT(weapon != NULL && armor != NULL, "Items should be created");
    TEST_ASSERT(dstring_equals(weapon->rarity, "common"), "Weapon rarity should be 'common'");
    TEST_ASSERT(dstring_equals(armor->rarity, "common"), "Armor rarity should be 'common'");

    // Clean up
    destroy_item(weapon);
    destroy_item(armor);
    free(material.name->str);

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
    free(material.name->str);

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
