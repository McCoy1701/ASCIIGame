// ASCIIGame/tests/items/test_items_inventory.c
// Test file for inventory management system

#include "../../include/tests.h"
#include "../../include/items.h"
#include "../../include/structs.h"
#include "../../include/defs.h"
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
void dummy_consume_callback(uint8_t value)
{
    (void)value; // Suppress unused parameter warning
}

// Helper function for float comparison
bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// =============================================================================
// INVENTORY CREATION/DESTRUCTION TESTS
// =============================================================================

int test_create_destroy_inventory(void)
{
    // Test normal inventory creation
    Inventory_t* inventory = create_inventory(10);
    TEST_ASSERT(inventory != NULL, "Inventory should be created successfully");
    TEST_ASSERT(inventory->size == 10, "Inventory size should be 10");
    TEST_ASSERT(inventory->slots != NULL, "Inventory slots should be allocated");

    // Check that all slots are initially empty
    for (uint8_t i = 0; i < inventory->size; i++) {
        TEST_ASSERT(inventory->slots[i].quantity == 0, "Initial slots should be empty");
        TEST_ASSERT(inventory->slots[i].is_equipped == 0, "Initial slots should not be equipped");
    }

    // Test inventory destruction
    destroy_inventory(inventory);
    TEST_ASSERT(1, "Inventory destruction should complete without error");

    // Test edge cases
    Inventory_t* null_inventory = create_inventory(0);
    TEST_ASSERT(null_inventory == NULL, "Zero-size inventory should fail");

    destroy_inventory(NULL);
    TEST_ASSERT(1, "Destroying NULL inventory should be safe");

    return 1;
}

// =============================================================================
// ITEM ADDITION TESTS
// =============================================================================

int test_add_item_to_inventory(void)
{
    // Create test inventory and items
    Inventory_t* inventory = create_inventory(5);
    TEST_ASSERT(inventory != NULL, "Inventory should be created");

    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Iron Sword", "iron_sword", basic_material, 20, 30, 0, 'S');
    Item_t* arrows = create_ammunition("Steel Arrows", "steel_arrows", basic_material, 5, 8, 'a');

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(arrows != NULL, "Arrows should be created");

    // Test adding non-stackable item
    bool sword_added = add_item_to_inventory(inventory, sword, 1);
    TEST_ASSERT(sword_added, "Sword should be added successfully");
    TEST_ASSERT(inventory->slots[0].quantity == 1, "First slot should have 1 sword");
    TEST_ASSERT(strcmp(inventory->slots[0].item.id, "iron_sword") == 0, "First slot should contain iron sword");

    // Test adding stackable items
    bool arrows_added = add_item_to_inventory(inventory, arrows, 50);
    TEST_ASSERT(arrows_added, "Arrows should be added successfully");

    // Arrows should stack up to their max stack size
    uint8_t arrows_in_slot = inventory->slots[1].quantity;
    TEST_ASSERT(arrows_in_slot > 0, "Arrows should be in inventory");
    TEST_ASSERT(strcmp(inventory->slots[1].item.id, "steel_arrows") == 0, "Second slot should contain arrows");

    // Test adding more arrows (should stack or create new slot)
    bool more_arrows_added = add_item_to_inventory(inventory, arrows, 30);
    TEST_ASSERT(more_arrows_added, "More arrows should be added successfully");

    // Test NULL handling
    TEST_ASSERT(!add_item_to_inventory(NULL, sword, 1), "Adding to NULL inventory should fail");
    TEST_ASSERT(!add_item_to_inventory(inventory, NULL, 1), "Adding NULL item should fail");
    TEST_ASSERT(!add_item_to_inventory(inventory, sword, 0), "Adding 0 quantity should fail");

    destroy_item(sword);
    destroy_item(arrows);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// ITEM REMOVAL TESTS
// =============================================================================

int test_remove_item_from_inventory(void)
{
    // Create test inventory and items
    Inventory_t* inventory = create_inventory(3);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* potion = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H');

    TEST_ASSERT(inventory != NULL, "Inventory should be created");
    TEST_ASSERT(potion != NULL, "Potion should be created");

    // Add items to inventory
    add_item_to_inventory(inventory, potion, 10);

    // Verify items were added
    Inventory_slot_t* found_slot = find_item_in_inventory(inventory, "health_potion");
    TEST_ASSERT(found_slot != NULL, "Potion should be found in inventory");
    uint8_t initial_quantity = found_slot->quantity;
    TEST_ASSERT(initial_quantity > 0, "Should have potions in inventory");

    // Test partial removal
    bool partial_removed = remove_item_from_inventory(inventory, "health_potion", 3);
    TEST_ASSERT(partial_removed, "Partial removal should succeed");

    found_slot = find_item_in_inventory(inventory, "health_potion");
    TEST_ASSERT(found_slot != NULL, "Potions should still be in inventory");
    TEST_ASSERT(found_slot->quantity == initial_quantity - 3, "Should have 3 fewer potions");

    // Test complete removal
    bool complete_removed = remove_item_from_inventory(inventory, "health_potion", found_slot->quantity);
    TEST_ASSERT(complete_removed, "Complete removal should succeed");

    found_slot = find_item_in_inventory(inventory, "health_potion");
    TEST_ASSERT(found_slot == NULL, "Potions should be completely removed");

    // Test removing non-existent item
    bool nonexistent_removed = remove_item_from_inventory(inventory, "fake_item", 1);
    TEST_ASSERT(!nonexistent_removed, "Removing non-existent item should fail");

    // Test NULL handling
    TEST_ASSERT(!remove_item_from_inventory(NULL, "health_potion", 1), "Removing from NULL inventory should fail");
    TEST_ASSERT(!remove_item_from_inventory(inventory, NULL, 1), "Removing NULL item should fail");
    TEST_ASSERT(!remove_item_from_inventory(inventory, "health_potion", 0), "Removing 0 quantity should fail");

    destroy_item(potion);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// ITEM STACKING TESTS
// =============================================================================

int test_item_stacking(void)
{
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* arrow1 = create_ammunition("Arrow", "arrow", basic_material, 3, 5, 'a');
    Item_t* arrow2 = create_ammunition("Arrow", "arrow", basic_material, 3, 5, 'a');
    Item_t* bolt = create_ammunition("Bolt", "bolt", basic_material, 4, 6, 'b');
    Item_t* sword = create_weapon("Sword", "sword", basic_material, 15, 25, 0, 'S');

    TEST_ASSERT(arrow1 != NULL && arrow2 != NULL && bolt != NULL && sword != NULL, "All items should be created");

    // Test stacking identical items
    bool arrows_can_stack = can_stack_items(arrow1, arrow2);
    TEST_ASSERT(arrows_can_stack, "Identical arrows should be stackable");

    // Test non-stacking different items
    bool different_items_stack = can_stack_items(arrow1, bolt);
    TEST_ASSERT(!different_items_stack, "Different ammunition should not stack");

    // Test non-stackable items
    bool weapon_stacks = can_stack_items(sword, sword);
    TEST_ASSERT(!weapon_stacks, "Weapons should not stack with themselves");

    // Test NULL handling
    TEST_ASSERT(!can_stack_items(NULL, arrow1), "NULL item should not stack");
    TEST_ASSERT(!can_stack_items(arrow1, NULL), "Item with NULL should not stack");

    destroy_item(arrow1);
    destroy_item(arrow2);
    destroy_item(bolt);
    destroy_item(sword);
    return 1;
}

// =============================================================================
// EQUIPMENT MANAGEMENT TESTS
// =============================================================================

int test_equipment_management(void)
{
    // Create inventory and items
    Inventory_t* inventory = create_inventory(10);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Battle Sword", "battle_sword", basic_material, 25, 35, 0, 'S');
    Item_t* armor = create_armor("Chain Mail", "chain_mail", basic_material, 30, 10, 'A');
    Item_t* potion = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H');

    TEST_ASSERT(inventory != NULL && sword != NULL && armor != NULL && potion != NULL, "All items should be created");

    // Add items to inventory
    add_item_to_inventory(inventory, sword, 1);
    add_item_to_inventory(inventory, armor, 1);
    add_item_to_inventory(inventory, potion, 5);

    // Test equipping weapon
    bool sword_equipped = equip_item(inventory, "battle_sword");
    TEST_ASSERT(sword_equipped, "Sword should be equipped successfully");

    Inventory_slot_t* equipped_weapon = get_equipped_weapon(inventory);
    TEST_ASSERT(equipped_weapon != NULL, "Should have equipped weapon");
    TEST_ASSERT(strcmp(equipped_weapon->item.id, "battle_sword") == 0, "Equipped weapon should be battle sword");

    // Test equipping armor
    bool armor_equipped = equip_item(inventory, "chain_mail");
    TEST_ASSERT(armor_equipped, "Armor should be equipped successfully");

    Inventory_slot_t* equipped_armor = get_equipped_armor(inventory);
    TEST_ASSERT(equipped_armor != NULL, "Should have equipped armor");
    TEST_ASSERT(strcmp(equipped_armor->item.id, "chain_mail") == 0, "Equipped armor should be chain mail");

    // Test equipping non-equippable item
    bool potion_equipped = equip_item(inventory, "health_potion");
    TEST_ASSERT(!potion_equipped, "Potion should not be equippable");

    // Test unequipping
    bool sword_unequipped = unequip_item(inventory, "battle_sword");
    TEST_ASSERT(sword_unequipped, "Sword should be unequipped successfully");

    equipped_weapon = get_equipped_weapon(inventory);
    TEST_ASSERT(equipped_weapon == NULL, "Should have no equipped weapon after unequipping");

    // Test equipping non-existent item
    bool fake_equipped = equip_item(inventory, "fake_sword");
    TEST_ASSERT(!fake_equipped, "Non-existent item should not be equippable");

    // Test NULL handling
    TEST_ASSERT(!equip_item(NULL, "battle_sword"), "Equipping in NULL inventory should fail");
    TEST_ASSERT(!equip_item(inventory, NULL), "Equipping NULL item should fail");

    destroy_item(sword);
    destroy_item(armor);
    destroy_item(potion);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// INVENTORY QUERIES TESTS
// =============================================================================

int test_inventory_queries(void)
{
    // Create inventory and items
    Inventory_t* inventory = create_inventory(5);
    MaterialProperties_t heavy_props = create_default_material_properties();
    heavy_props.weight_fact = 3.0f; // Heavy items
    Material_t heavy_material = create_material("heavy", heavy_props);

    Item_t* heavy_armor = create_armor("Heavy Armor", "heavy_armor", heavy_material, 50, 5, 'H');
    apply_material_to_armor(heavy_armor); // Apply material effects

    TEST_ASSERT(inventory != NULL && heavy_armor != NULL, "Inventory and armor should be created");

    // Test empty inventory
    uint8_t initial_free_slots = get_inventory_free_slots(inventory);
    uint8_t initial_weight = get_total_inventory_weight(inventory);
    bool initially_full = is_inventory_full(inventory);

    TEST_ASSERT(initial_free_slots == 5, "Empty inventory should have 5 free slots");
    TEST_ASSERT(initial_weight == 0, "Empty inventory should have 0 weight");
    TEST_ASSERT(!initially_full, "Empty inventory should not be full");

    // Add some items
    add_item_to_inventory(inventory, heavy_armor, 2);

    uint8_t after_add_free_slots = get_inventory_free_slots(inventory);
    uint8_t after_add_weight = get_total_inventory_weight(inventory);
    bool after_add_full = is_inventory_full(inventory);

    TEST_ASSERT(after_add_free_slots == 3, "Should have 3 free slots after adding items");
    TEST_ASSERT(after_add_weight > 0, "Inventory should have weight after adding items");
    TEST_ASSERT(!after_add_full, "Inventory should not be full with 3 free slots");

    // Fill inventory completely
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);
    Item_t* filler = create_weapon("Filler", "filler", basic_material, 1, 1, 0, 'f');

    for (int i = 0; i < 3; i++) {
        add_item_to_inventory(inventory, filler, 1);
    }

    uint8_t final_free_slots = get_inventory_free_slots(inventory);
    bool finally_full = is_inventory_full(inventory);

    TEST_ASSERT(final_free_slots == 0, "Full inventory should have 0 free slots");
    TEST_ASSERT(finally_full, "Completely filled inventory should be full");

    // Test NULL handling
    TEST_ASSERT(get_inventory_free_slots(NULL) == 0, "NULL inventory should have 0 free slots");
    TEST_ASSERT(get_total_inventory_weight(NULL) == 0, "NULL inventory should have 0 weight");
    TEST_ASSERT(is_inventory_full(NULL), "NULL inventory should be considered full");

    destroy_item(heavy_armor);
    destroy_item(filler);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    TEST_SUITE_START("Inventory Management System Tests");

    // Run inventory system tests
    RUN_TEST(test_create_destroy_inventory);
    RUN_TEST(test_add_item_to_inventory);
    RUN_TEST(test_remove_item_from_inventory);
    RUN_TEST(test_item_stacking);
    RUN_TEST(test_equipment_management);
    RUN_TEST(test_inventory_queries);

    TEST_SUITE_END();
}
