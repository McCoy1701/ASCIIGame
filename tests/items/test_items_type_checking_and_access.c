#include "../../include/tests.h"
#include "../../include/items.h"
#include "../../include/structs.h"
#include "../../include/defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Global test counters
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper function to create a test material
Material_t create_test_material(void)
{
    Material_t material;

    material.name = d_InitString();
    d_AppendString(material.name, "Test Material", 0);

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

// Helper function to create a test lock
Lock_t create_test_lock(void)
{
    Lock_t lock = create_lock("Test", "Test Lock", 1, 5);
    return lock;
}

// Dummy consumable effect function for testing
void test_heal_effect(uint8_t value)
{
    printf("Healing for %d points\n", value);
}

// =============================================================================
// TYPE CHECKING TESTS
// =============================================================================

int test_is_weapon(void)
{
    Material_t material = create_test_material();

    // Create a weapon
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's');
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    // Test positive case
    TEST_ASSERT(is_weapon(weapon) == true, "is_weapon should return true for weapon");
    TEST_ASSERT(is_armor(weapon) == false, "is_armor should return false for weapon");
    TEST_ASSERT(is_key(weapon) == false, "is_key should return false for weapon");
    TEST_ASSERT(is_consumable(weapon) == false, "is_consumable should return false for weapon");
    TEST_ASSERT(is_ammunition(weapon) == false, "is_ammunition should return false for weapon");

    // Test NULL handling
    TEST_ASSERT(is_weapon(NULL) == false, "is_weapon should return false for NULL");

    destroy_item(weapon);
    return 1;
}

int test_is_armor(void)
{
    Material_t material = create_test_material();

    // Create armor
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15);
    TEST_ASSERT(armor != NULL, "Armor should be created for testing");

    // Test positive case
    TEST_ASSERT(is_armor(armor) == true, "is_armor should return true for armor");
    TEST_ASSERT(is_weapon(armor) == false, "is_weapon should return false for armor");
    TEST_ASSERT(is_key(armor) == false, "is_key should return false for armor");
    TEST_ASSERT(is_consumable(armor) == false, "is_consumable should return false for armor");
    TEST_ASSERT(is_ammunition(armor) == false, "is_ammunition should return false for armor");

    // Test NULL handling
    TEST_ASSERT(is_armor(NULL) == false, "is_armor should return false for NULL");

    destroy_item(armor);
    return 1;
}

int test_is_key(void)
{
    Lock_t lock = create_test_lock();

    // Create key
    Item_t* key = create_key("Test Key", "test_key", lock, 'k');
    TEST_ASSERT(key != NULL, "Key should be created for testing");

    // Test positive case
    TEST_ASSERT(is_key(key) == true, "is_key should return true for key");
    TEST_ASSERT(is_weapon(key) == false, "is_weapon should return false for key");
    TEST_ASSERT(is_armor(key) == false, "is_armor should return false for key");
    TEST_ASSERT(is_consumable(key) == false, "is_consumable should return false for key");
    TEST_ASSERT(is_ammunition(key) == false, "is_ammunition should return false for key");

    // Test NULL handling
    TEST_ASSERT(is_key(NULL) == false, "is_key should return false for NULL");

    destroy_item(key);
    return 1;
}

int test_is_consumable(void)
{
    // Create consumable
    Item_t* potion = create_consumable("Test Potion", "test_potion", 25, test_heal_effect, 'h');
    TEST_ASSERT(potion != NULL, "Consumable should be created for testing");

    // Test positive case
    TEST_ASSERT(is_consumable(potion) == true, "is_consumable should return true for consumable");
    TEST_ASSERT(is_weapon(potion) == false, "is_weapon should return false for consumable");
    TEST_ASSERT(is_armor(potion) == false, "is_armor should return false for consumable");
    TEST_ASSERT(is_key(potion) == false, "is_key should return false for consumable");
    TEST_ASSERT(is_ammunition(potion) == false, "is_ammunition should return false for consumable");

    // Test NULL handling
    TEST_ASSERT(is_consumable(NULL) == false, "is_consumable should return false for NULL");

    destroy_item(potion);
    return 1;
}

int test_is_ammunition(void)
{
    Material_t material = create_test_material();

    // Create ammunition
    Item_t* arrow = create_ammunition("Test Arrow", "test_arrow", material, 3, 7, 'i');
    TEST_ASSERT(arrow != NULL, "Ammunition should be created for testing");

    // Test positive case
    TEST_ASSERT(is_ammunition(arrow) == true, "is_ammunition should return true for ammunition");
    TEST_ASSERT(is_weapon(arrow) == false, "is_weapon should return false for ammunition");
    TEST_ASSERT(is_armor(arrow) == false, "is_armor should return false for ammunition");
    TEST_ASSERT(is_key(arrow) == false, "is_key should return false for ammunition");
    TEST_ASSERT(is_consumable(arrow) == false, "is_consumable should return false for ammunition");

    // Test NULL handling
    TEST_ASSERT(is_ammunition(NULL) == false, "is_ammunition should return false for NULL");

    destroy_item(arrow);
    return 1;
}

// =============================================================================
// DATA ACCESS TESTS
// =============================================================================

int test_get_weapon_data(void)
{
    Material_t material = create_test_material();

    // Create a weapon
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 2, 's');
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    // Test getting weapon data
    Weapon__Item_t* weapon_data = get_weapon_data(weapon);
    TEST_ASSERT(weapon_data != NULL, "get_weapon_data should return valid pointer for weapon");
    TEST_ASSERT(weapon_data->min_damage == 10, "Weapon data should have correct min_damage");
    TEST_ASSERT(weapon_data->max_damage == 15, "Weapon data should have correct max_damage");
    TEST_ASSERT(weapon_data->range_tiles == 2, "Weapon data should have correct range");

    // Test with wrong type
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15);
    TEST_ASSERT(armor != NULL, "Armor should be created for testing");

    Weapon__Item_t* wrong_data = get_weapon_data(armor);
    TEST_ASSERT(wrong_data == NULL, "get_weapon_data should return NULL for non-weapon");

    // Test with NULL
    TEST_ASSERT(get_weapon_data(NULL) == NULL, "get_weapon_data should return NULL for NULL input");

    destroy_item(weapon);
    destroy_item(armor);
    return 1;
}

int test_get_armor_data(void)
{
    Material_t material = create_test_material();

    // Create armor
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15);
    TEST_ASSERT(armor != NULL, "Armor should be created for testing");

    // Test getting armor data
    Armor__Item_t* armor_data = get_armor_data(armor);
    TEST_ASSERT(armor_data != NULL, "get_armor_data should return valid pointer for armor");
    TEST_ASSERT(armor_data->armor_value == 20, "Armor data should have correct armor_value");
    TEST_ASSERT(armor_data->evasion_value == 5, "Armor data should have correct evasion_value");

    // Test with wrong type
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's');
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    Armor__Item_t* wrong_data = get_armor_data(weapon);
    TEST_ASSERT(wrong_data == NULL, "get_armor_data should return NULL for non-armor");

    // Test with NULL
    TEST_ASSERT(get_armor_data(NULL) == NULL, "get_armor_data should return NULL for NULL input");

    destroy_item(armor);
    destroy_item(weapon);
    return 1;
}

int test_get_consumable_data(void)
{
    // Create consumable
    Item_t* potion = create_consumable("Test Potion", "test_potion", 25, test_heal_effect, 'h');
    TEST_ASSERT(potion != NULL, "Consumable should be created for testing");

    // Test getting consumable data
    Consumable__Item_t* consumable_data = get_consumable_data(potion);
    TEST_ASSERT(consumable_data != NULL, "get_consumable_data should return valid pointer for consumable");
    TEST_ASSERT(consumable_data->value == 25, "Consumable data should have correct value");
    TEST_ASSERT(consumable_data->on_consume == test_heal_effect, "Consumable data should have correct callback");

    // Test with wrong type
    Material_t material = create_test_material();
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's');
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    Consumable__Item_t* wrong_data = get_consumable_data(weapon);
    TEST_ASSERT(wrong_data == NULL, "get_consumable_data should return NULL for non-consumable");

    // Test with NULL
    TEST_ASSERT(get_consumable_data(NULL) == NULL, "get_consumable_data should return NULL for NULL input");

    destroy_item(potion);
    destroy_item(weapon);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Type Checking & Access Tests");

    // Run type checking tests
    RUN_TEST(test_is_weapon);
    RUN_TEST(test_is_armor);
    RUN_TEST(test_is_key);
    RUN_TEST(test_is_consumable);
    RUN_TEST(test_is_ammunition);

    // Run data access tests
    RUN_TEST(test_get_weapon_data);
    RUN_TEST(test_get_armor_data);
    RUN_TEST(test_get_consumable_data);

    TEST_SUITE_END();
}
