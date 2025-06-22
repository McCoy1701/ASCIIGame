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
    Lock_t lock = create_lock("Test Lock", "test_lock", 50, 0);
    return lock;
}

// Dummy consumable effect function for testing
void test_heal_effect(uint8_t value)
{
    printf("Healing for %d points\n", value);
}

// =============================================================================
// TEST FUNCTIONS
// =============================================================================

int test_create_weapon(void)
{
    Material_t material = create_test_material();

    // Test normal weapon creation
    Item_t* weapon = create_weapon("Iron Sword", "iron_sword", material, 10, 15, 0, 's');

    TEST_ASSERT(weapon != NULL, "Weapon should be created successfully");
    TEST_ASSERT(weapon->type == ITEM_TYPE_WEAPON, "Item type should be WEAPON");
    TEST_ASSERT(strcmp(weapon->name->str, "Iron Sword") == 0, "Weapon name should match");
    TEST_ASSERT(strcmp(weapon->id->str, "iron_sword") == 0, "Weapon ID should match");
    TEST_ASSERT(weapon->glyph == 's', "Weapon glyph should be 's'");
    TEST_ASSERT(weapon->data.weapon.min_damage == 10, "Min damage should be 10");
    TEST_ASSERT(weapon->data.weapon.max_damage == 15, "Max damage should be 15");
    TEST_ASSERT(weapon->data.weapon.range_tiles == 0, "Range should be 0 (melee)");
    TEST_ASSERT(weapon->data.weapon.durability == 255, "Durability should be 100% (255)");
    TEST_ASSERT(weapon->stackable == 0, "Weapons should not be stackable");

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Weapon Material Name: ", 0);
    d_AppendString(log_message, weapon->material_data.name->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    TEST_ASSERT(strcmp(weapon->material_data.name->str, "Test Material") == 0, "Material name should match");

    destroy_item(weapon);

    // Test weapon creation with NULL parameters
    Item_t* null_weapon = create_weapon(NULL, "test", material, 5, 10, 1, 'w');
    TEST_ASSERT(null_weapon == NULL, "Creating weapon with NULL name should fail");

    null_weapon = create_weapon("Test", NULL, material, 5, 10, 1, 'w');
    TEST_ASSERT(null_weapon == NULL, "Creating weapon with NULL id should fail");

    return 1;
}

int test_create_armor(void)
{
    Material_t material = create_test_material();

    // Test normal armor creation
    Item_t* armor = create_armor("Iron Chestplate", "iron_chest", material, 20, 5, 'A');

    TEST_ASSERT(armor != NULL, "Armor should be created successfully");
    TEST_ASSERT(armor->type == ITEM_TYPE_ARMOR, "Item type should be ARMOR");
    TEST_ASSERT(strcmp(armor->name->str, "Iron Chestplate") == 0, "Armor name should match");
    TEST_ASSERT(strcmp(armor->id->str, "iron_chest") == 0, "Armor ID should match");
    TEST_ASSERT(armor->glyph == 'A', "Armor glyph should be 'A'");
    TEST_ASSERT(armor->data.armor.armor_value == 20, "Armor value should be 20");
    TEST_ASSERT(armor->data.armor.evasion_value == 5, "Evasion value should be 5");
    TEST_ASSERT(armor->data.armor.durability == 255, "Durability should be 100% (255)");
    TEST_ASSERT(armor->stackable == 0, "Armor should not be stackable");
    TEST_ASSERT(strcmp(armor->material_data.name->str, "Test Material") == 0, "Material name should match");

    destroy_item(armor);

    // Test armor creation with NULL parameters
    Item_t* null_armor = create_armor(NULL, "test", material, 10, 5, 'a');
    TEST_ASSERT(null_armor == NULL, "Creating armor with NULL name should fail");

    return 1;
}

int test_create_key(void)
{
    Lock_t lock = create_test_lock();

    // Test normal key creation
    Item_t* key = create_key("Iron Key", "iron_key", lock, 'k');

    TEST_ASSERT(key != NULL, "Key should be created successfully");
    TEST_ASSERT(key->type == ITEM_TYPE_KEY, "Item type should be KEY");
    TEST_ASSERT(strcmp(key->name->str, "Iron Key") == 0, "Key name should match");
    TEST_ASSERT(strcmp(key->id->str, "iron_key") == 0, "Key ID should match");
    TEST_ASSERT(key->glyph == 'k', "Key glyph should be 'k'");
    TEST_ASSERT(key->stackable == 1, "Keys should not be stackable");
    TEST_ASSERT(key->weight_kg == 0.1f, "Keys should be light (weight 0.1)");

    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Key Lock Name: ", 0);
    d_AppendString(log_message, key->data.key.lock.name->str, 0);
    LOG(log_message->str);
    d_DestroyString(log_message);


    TEST_ASSERT(strcmp(key->data.key.lock.name->str, "Test Lock") == 0, "Lock name should match");
    TEST_ASSERT(key->data.key.lock.pick_difficulty == 50, "Lock difficulty should match");

    destroy_item(key);

    // Test key creation with NULL parameters
    Item_t* null_key = create_key(NULL, "test", lock, 'k');
    TEST_ASSERT(null_key == NULL, "Creating key with NULL name should fail");

    return 1;
}

int test_create_consumable(void)
{
    // Test normal consumable creation
    Item_t* potion = create_consumable("Health Potion", "health_pot", 25, test_heal_effect, 'h');

    TEST_ASSERT(potion != NULL, "Consumable should be created successfully");
    TEST_ASSERT(potion->type == ITEM_TYPE_CONSUMABLE, "Item type should be CONSUMABLE");
    TEST_ASSERT(strcmp(potion->name->str, "Health Potion") == 0, "Consumable name should match");
    TEST_ASSERT(strcmp(potion->id->str, "health_pot") == 0, "Consumable ID should match");
    TEST_ASSERT(potion->glyph == 'h', "Consumable glyph should be 'h'");
    TEST_ASSERT(potion->data.consumable.value == 25, "Consumable value should be 25");
    TEST_ASSERT(potion->data.consumable.on_consume == test_heal_effect, "Callback should match");
    TEST_ASSERT(potion->stackable == 16, "Consumables should stack to 16");
    TEST_ASSERT(potion->weight_kg == 0.1f, "Consumables should be light (weight 0.1)");
    TEST_ASSERT(potion->value_coins == 25, "Consumables default value_coins should be based on its effect value");
    TEST_ASSERT(potion->data.consumable.duration_seconds == 0, "Default duration should be 0");

    destroy_item(potion);

    // Test consumable with NULL callback
    Item_t* null_consumable = create_consumable("Test", "test", 10, NULL, 'p');
    TEST_ASSERT(null_consumable == NULL, "Creating consumable with NULL callback should fail");

    return 1;
}

int test_create_ammunition(void)
{
    Material_t material = create_test_material();

    // Test normal ammunition creation
    Item_t* arrow = create_ammunition("Iron Arrow", "iron_arrow", material, 3, 7, 'i');

    TEST_ASSERT(arrow != NULL, "Ammunition should be created successfully");
    TEST_ASSERT(arrow->type == ITEM_TYPE_AMMUNITION, "Item type should be AMMUNITION");
    TEST_ASSERT(strcmp(arrow->name->str, "Iron Arrow") == 0, "Ammunition name should match");
    TEST_ASSERT(strcmp(arrow->id->str, "iron_arrow") == 0, "Ammunition ID should match");
    TEST_ASSERT(arrow->glyph == 'i', "Ammunition glyph should be 'i'");
    TEST_ASSERT(arrow->data.ammo.min_damage == 3, "Min damage should be 3");
    TEST_ASSERT(arrow->data.ammo.max_damage == 7, "Max damage should be 7");
    TEST_ASSERT(arrow->stackable == 255, "Ammunition should stack to 255");
    TEST_ASSERT(arrow->weight_kg == 0.05f, "Ammunition should be light (weight 0.05)");
    TEST_ASSERT(strcmp(arrow->material_data.name->str, "Test Material") == 0, "Material name should match");

    destroy_item(arrow);

    return 1;
}

int test_destroy_item(void)
{
    Material_t material = create_test_material();

    // Test destroying a valid item
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 5, 10, 0, 's');
    TEST_ASSERT(weapon != NULL, "Weapon should be created for destruction test");

    // This should not crash
    destroy_item(weapon);
    TEST_ASSERT(1, "destroy_item should handle valid item without crashing");

    // Test destroying NULL item (should not crash)
    destroy_item(NULL);
    TEST_ASSERT(1, "destroy_item should handle NULL without crashing");

    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Item Creation & Destruction Tests");

    // Run all tests
    RUN_TEST(test_create_weapon);
    RUN_TEST(test_create_armor);
    RUN_TEST(test_create_key);
    RUN_TEST(test_create_consumable);
    RUN_TEST(test_create_ammunition);
    RUN_TEST(test_destroy_item);

    TEST_SUITE_END();
}
