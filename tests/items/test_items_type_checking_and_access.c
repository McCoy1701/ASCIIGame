// ASCIIGame/tests/items/test_items_type_checking_and_access.c
// Test file for item type checking and data access, enhanced with Daedalus Logging.

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "Daedalus.h" // The architect's toolkit, including the logging system.
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper function to create a test material
static Material_t create_test_material(void)
{
    MaterialProperties_t props = create_default_material_properties();
    return create_material("Test Material", props);
}

// Helper function to create a test lock
static Lock_t create_test_lock(void)
{
    return create_lock("Test Lock", "test_lock", 10, 5);
}

// Dummy consumable effect function for testing
static void test_heal_effect(uint8_t value)
{
    // In a real scenario, this would interact with a player state.
    // For a test, we can log the action.
    d_LogDebugF("Heal effect callback triggered with value: %d", value);
    (void)value;
}

// =============================================================================
// TYPE CHECKING TESTS
// =============================================================================

int test_is_weapon(void)
{
    d_LogInfo("Verifying is_weapon() identifies weapons and rejects other types.");
    Material_t material = create_test_material();
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's', 0, 0, "Common");
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    // Test positive case
    TEST_ASSERT(is_weapon(weapon) == true, "is_weapon should return true for weapon");

    // Test negative cases
    TEST_ASSERT(is_armor(weapon) == false, "is_armor should return false for weapon");
    TEST_ASSERT(is_consumable(weapon) == false, "is_consumable should return false for weapon");

    // Test NULL handling
    d_LogDebug("Verifying is_weapon() handles NULL input.");
    TEST_ASSERT(is_weapon(NULL) == false, "is_weapon should return false for NULL");

    destroy_item(weapon);
    return 1;
}

int test_is_armor(void)
{
    d_LogInfo("Verifying is_armor() identifies armor and rejects other types.");
    Material_t material = create_test_material();
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15, "Common");
    TEST_ASSERT(armor != NULL, "Armor should be created for testing");

    // Test positive case
    TEST_ASSERT(is_armor(armor) == true, "is_armor should return true for armor");

    // Test negative cases
    TEST_ASSERT(is_weapon(armor) == false, "is_weapon should return false for armor");
    TEST_ASSERT(is_key(armor) == false, "is_key should return false for armor");

    // Test NULL handling
    d_LogDebug("Verifying is_armor() handles NULL input.");
    TEST_ASSERT(is_armor(NULL) == false, "is_armor should return false for NULL");

    destroy_item(armor);
    return 1;
}

int test_is_key(void)
{
    d_LogInfo("Verifying is_key() identifies keys and rejects other types.");
    Lock_t lock = create_test_lock();
    Item_t* key = create_key("Test Key", "test_key", lock, 'k', 0, "Common");
    TEST_ASSERT(key != NULL, "Key should be created for testing");

    // Test positive case
    TEST_ASSERT(is_key(key) == true, "is_key should return true for key");

    // Test negative cases
    TEST_ASSERT(is_weapon(key) == false, "is_weapon should return false for key");
    TEST_ASSERT(is_ammunition(key) == false, "is_ammunition should return false for key");

    // Test NULL handling
    d_LogDebug("Verifying is_key() handles NULL input.");
    TEST_ASSERT(is_key(NULL) == false, "is_key should return false for NULL");

    destroy_item(key);
    return 1;
}

int test_is_consumable(void)
{
    d_LogInfo("Verifying is_consumable() identifies consumables and rejects other types.");
    Item_t* potion = create_consumable("Test Potion", "test_potion", 25, test_heal_effect, 'h', 0, "Common");
    TEST_ASSERT(potion != NULL, "Consumable should be created for testing");

    // Test positive case
    TEST_ASSERT(is_consumable(potion) == true, "is_consumable should return true for consumable");

    // Test negative cases
    TEST_ASSERT(is_armor(potion) == false, "is_armor should return false for consumable");

    // Test NULL handling
    d_LogDebug("Verifying is_consumable() handles NULL input.");
    TEST_ASSERT(is_consumable(NULL) == false, "is_consumable should return false for NULL");

    destroy_item(potion);
    return 1;
}

int test_is_ammunition(void)
{
    d_LogInfo("Verifying is_ammunition() identifies ammo and rejects other types.");
    Material_t material = create_test_material();
    Item_t* arrow = create_ammunition("Test Arrow", "test_arrow", material, 3, 7, 'i', 0, "Common");
    TEST_ASSERT(arrow != NULL, "Ammunition should be created for testing");

    // Test positive case
    TEST_ASSERT(is_ammunition(arrow) == true, "is_ammunition should return true for ammunition");

    // Test negative cases
    TEST_ASSERT(is_weapon(arrow) == false, "is_weapon should return false for ammunition");

    // Test NULL handling
    d_LogDebug("Verifying is_ammunition() handles NULL input.");
    TEST_ASSERT(is_ammunition(NULL) == false, "is_ammunition should return false for NULL");

    destroy_item(arrow);
    return 1;
}

// =============================================================================
// DATA ACCESS TESTS
// =============================================================================

int test_get_weapon_data(void)
{
    d_LogInfo("Verifying get_weapon_data() returns correct data for weapons and NULL for others.");
    Material_t material = create_test_material();
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 2, 's', 0, 0, "Common");
    TEST_ASSERT(weapon != NULL, "Weapon should be created for testing");

    // Test getting weapon data
    Weapon__Item_t* weapon_data = get_weapon_data(weapon);
    TEST_ASSERT(weapon_data != NULL, "get_weapon_data should return valid pointer for weapon");

    d_LogDebugF("Checking weapon data: min_dmg=%d, max_dmg=%d, range=%d", weapon_data->min_damage, weapon_data->max_damage, weapon_data->range_tiles);
    TEST_ASSERT(weapon_data->min_damage == 10, "Weapon data should have correct min_damage");
    TEST_ASSERT(weapon_data->max_damage == 15, "Weapon data should have correct max_damage");
    TEST_ASSERT(weapon_data->range_tiles == 2, "Weapon data should have correct range");

    // Test with wrong type
    d_LogDebug("Verifying get_weapon_data returns NULL for a non-weapon item.");
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15, "Common");
    TEST_ASSERT(get_weapon_data(armor) == NULL, "get_weapon_data should return NULL for non-weapon");

    // Test with NULL
    d_LogDebug("Verifying get_weapon_data handles NULL input.");
    TEST_ASSERT(get_weapon_data(NULL) == NULL, "get_weapon_data should return NULL for NULL input");

    destroy_item(weapon);
    destroy_item(armor);
    return 1;
}

int test_get_armor_data(void)
{
    d_LogInfo("Verifying get_armor_data() returns correct data for armor and NULL for others.");
    Material_t material = create_test_material();
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 5, 'A', 15, 15, "Common");
    TEST_ASSERT(armor != NULL, "Armor should be created for testing");

    // Test getting armor data
    Armor__Item_t* armor_data = get_armor_data(armor);
    TEST_ASSERT(armor_data != NULL, "get_armor_data should return valid pointer for armor");

    d_LogDebugF("Checking armor data: armor_val=%d, evasion_val=%d", armor_data->armor_value, armor_data->evasion_value);
    TEST_ASSERT(armor_data->armor_value == 20, "Armor data should have correct armor_value");
    TEST_ASSERT(armor_data->evasion_value == 5, "Armor data should have correct evasion_value");

    // Test with wrong type
    d_LogDebug("Verifying get_armor_data returns NULL for a non-armor item.");
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's', 0, 0, "Common");
    TEST_ASSERT(get_armor_data(weapon) == NULL, "get_armor_data should return NULL for non-armor");

    // Test with NULL
    d_LogDebug("Verifying get_armor_data handles NULL input.");
    TEST_ASSERT(get_armor_data(NULL) == NULL, "get_armor_data should return NULL for NULL input");

    destroy_item(armor);
    destroy_item(weapon);
    return 1;
}

int test_get_consumable_data(void)
{
    d_LogInfo("Verifying get_consumable_data() returns correct data for consumables and NULL for others.");
    Item_t* potion = create_consumable("Test Potion", "test_potion", 25, test_heal_effect, 'h', 0, "Common");
    TEST_ASSERT(potion != NULL, "Consumable should be created for testing");

    // Test getting consumable data
    Consumable__Item_t* consumable_data = get_consumable_data(potion);
    TEST_ASSERT(consumable_data != NULL, "get_consumable_data should return valid pointer for consumable");

    d_LogDebugF("Checking consumable data: value=%d, callback_ptr=%p", consumable_data->value, consumable_data->on_consume);
    TEST_ASSERT(consumable_data->value == 25, "Consumable data should have correct value");
    TEST_ASSERT(consumable_data->on_consume == test_heal_effect, "Consumable data should have correct callback");

    // Test with wrong type
    d_LogDebug("Verifying get_consumable_data returns NULL for a non-consumable item.");
    Material_t material = create_test_material();
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 's', 0, 0, "Common");
    TEST_ASSERT(get_consumable_data(weapon) == NULL, "get_consumable_data should return NULL for non-consumable");

    // Test with NULL
    d_LogDebug("Verifying get_consumable_data handles NULL input.");
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
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);

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

    // This captures the return code from the test suite summary
    TEST_SUITE_END();

    // Clean up the logger
    d_DestroyLogger(d_GetGlobalLogger());

}
