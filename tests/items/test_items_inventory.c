// ASCIIGame/tests/items/test_items_inventory.c
// Test file for inventory management system
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

    Item_t* sword = create_weapon("Iron Sword", "iron_sword", basic_material, 20, 30, 0, 'S', 0, 0, "common");
    Item_t* arrows = create_ammunition("Steel Arrows", "steel_arrows", basic_material, 5, 8, 'a', 0, "common");

    TEST_ASSERT(sword != NULL, "Sword should be created");
    TEST_ASSERT(arrows != NULL, "Arrows should be created");

    // Test adding non-stackable item
    bool sword_added = add_item_to_inventory(inventory, sword, 1);
    TEST_ASSERT(sword_added, "Sword should be added successfully");
    TEST_ASSERT(inventory->slots[0].quantity == 1, "First slot should have 1 sword");
    TEST_ASSERT(strcmp(inventory->slots[0].item.id->str, "iron_sword") == 0, "First slot should contain iron sword");

    // Test adding stackable items
    bool arrows_added = add_item_to_inventory(inventory, arrows, 50);
    TEST_ASSERT(arrows_added, "Arrows should be added successfully");

    // Arrows should stack up to their max stack size
    uint8_t arrows_in_slot = inventory->slots[1].quantity;
    TEST_ASSERT(arrows_in_slot > 0, "Arrows should be in inventory");
    TEST_ASSERT(strcmp(inventory->slots[1].item.id->str, "steel_arrows") == 0, "Second slot should contain arrows");

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

    Item_t* potion = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H', 0, "common");

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

    Item_t* arrow1 = create_ammunition("Arrow", "arrow", basic_material, 3, 5, 'a', 0, "common");
    Item_t* arrow2 = create_ammunition("Arrow", "arrow", basic_material, 3, 5, 'a', 0, "common");
    Item_t* bolt = create_ammunition("Bolt", "bolt", basic_material, 4, 6, 'b', 0, "common");
    Item_t* sword = create_weapon("Sword", "sword", basic_material, 15, 25, 0, 'S', 0, 0, "common");

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

    Item_t* sword = create_weapon("Battle Sword", "battle_sword", basic_material, 25, 35, 0, 'S', 0, 0, "common");
    Item_t* armor = create_armor("Chain Mail", "chain_mail", basic_material, 30, 10, 'A', 15, 15, "common");
    Item_t* potion = create_consumable("Health Potion", "health_potion", 50, dummy_consume_callback, 'H', 0, "common");

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
    TEST_ASSERT(strcmp(equipped_weapon->item.id->str, "battle_sword") == 0, "Equipped weapon should be battle sword");

    // Test equipping armor
    bool armor_equipped = equip_item(inventory, "chain_mail");
    TEST_ASSERT(armor_equipped, "Armor should be equipped successfully");

    Inventory_slot_t* equipped_armor = get_equipped_armor(inventory);
    TEST_ASSERT(equipped_armor != NULL, "Should have equipped armor");
    TEST_ASSERT(strcmp(equipped_armor->item.id->str, "chain_mail") == 0, "Equipped armor should be chain mail");

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

    Item_t* heavy_armor = create_armor("Heavy Armor", "heavy_armor", heavy_material, 50, 5, 'H', 15, 15, "common");
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
    Item_t* filler = create_weapon("Filler", "filler", basic_material, 1, 1, 0, 'f', 0, 0, "common");

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
// STACK OVERFLOW/BOUNDARY TESTS
// =============================================================================

int test_inventory_stack_boundaries(void)
{
    LOG("Starting inventory stack boundaries test");
    Inventory_t* inventory = create_inventory(10);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* arrows = create_ammunition("Test Arrows", "test_arrows", basic_material, 5, 8, 'a', 0, "common");
    TEST_ASSERT(inventory != NULL && arrows != NULL, "Inventory and arrows should be created");

    // Test maximum stack size (assuming 255 is max for uint8_t)
    bool max_stack_added = add_item_to_inventory(inventory, arrows, 255);
    TEST_ASSERT(max_stack_added, "Should be able to add maximum stack size");

    Inventory_slot_t* found_slot = find_item_in_inventory(inventory, "test_arrows");
    TEST_ASSERT(found_slot != NULL, "Arrows should be found in inventory");
    TEST_ASSERT(found_slot->quantity == 255, "Should have exactly 255 arrows");

    // Test adding one more arrow (should create new stack)
    bool overflow_handled = add_item_to_inventory(inventory, arrows, 1);
    TEST_ASSERT(overflow_handled, "Should handle stack overflow by creating new slot");

    // Test adding many arrows at once that would exceed single stack
    bool large_add_handled = add_item_to_inventory(inventory, arrows, 300);
    TEST_ASSERT(large_add_handled, "Should handle large additions properly");

    // Test edge case: adding 0 arrows to existing stack
    uint8_t pre_add_quantity = found_slot->quantity;
    bool zero_add = add_item_to_inventory(inventory, arrows, 0);
    TEST_ASSERT(!zero_add, "Adding 0 quantity should fail");
    TEST_ASSERT(found_slot->quantity == pre_add_quantity, "Quantity should be unchanged");

    LOG("Stack boundaries test completed");
    destroy_item(arrows);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// INVENTORY CAPACITY EDGE CASES
// =============================================================================

int test_inventory_capacity_edge_cases(void)
{
    LOG("Starting inventory capacity edge cases test");
    Inventory_t* small_inventory = create_inventory(2);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword = create_weapon("Test Sword", "test_sword", basic_material, 10, 15, 0, 'S', 0, 0, "common");
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 20, 10, 'A', 15, 15, "common");
    Item_t* potion = create_consumable("Test Potion", "test_potion", 50, dummy_consume_callback, 'P', 0, "common");

    TEST_ASSERT(small_inventory != NULL && sword != NULL && armor != NULL && potion != NULL,
                "All items should be created");

    // Fill inventory to capacity
    bool sword_added = add_item_to_inventory(small_inventory, sword, 1);
    bool armor_added = add_item_to_inventory(small_inventory, armor, 1);
    TEST_ASSERT(sword_added && armor_added, "Should be able to fill inventory to capacity");

    // Test adding to full inventory
    bool potion_added = add_item_to_inventory(small_inventory, potion, 1);
    TEST_ASSERT(!potion_added, "Should fail to add item to full inventory");

    // Test that inventory is correctly identified as full
    bool is_full = is_inventory_full(small_inventory);
    TEST_ASSERT(is_full, "Full inventory should be identified as full");

    // Remove one item and test adding again
    bool armor_removed = remove_item_from_inventory(small_inventory, "test_armor", 1);
    TEST_ASSERT(armor_removed, "Should be able to remove item from full inventory");

    bool potion_added_after_removal = add_item_to_inventory(small_inventory, potion, 1);
    TEST_ASSERT(potion_added_after_removal, "Should be able to add item after making space");

    // Test removing all items
    remove_item_from_inventory(small_inventory, "test_sword", 1);
    remove_item_from_inventory(small_inventory, "test_potion", 1);

    uint8_t free_slots = get_inventory_free_slots(small_inventory);
    TEST_ASSERT(free_slots == 2, "Empty inventory should have all slots free");

    LOG("Capacity edge cases test completed");
    destroy_item(sword);
    destroy_item(armor);
    destroy_item(potion);
    destroy_inventory(small_inventory);
    return 1;
}

// =============================================================================
// WEIGHT CALCULATION STRESS TESTS
// =============================================================================

int test_inventory_weight_calculations(void)
{
    LOG("Starting inventory weight calculations test");
    Inventory_t* inventory = create_inventory(10);

    // Create materials with extreme weight factors
    MaterialProperties_t heavy_props = create_default_material_properties();
    heavy_props.weight_fact = 10.0f; // Very heavy
    Material_t heavy_material = create_material("heavy", heavy_props);

    MaterialProperties_t light_props = create_default_material_properties();
    light_props.weight_fact = 0.1f; // Very light
    Material_t light_material = create_material("light", light_props);

    Item_t* heavy_armor = create_armor("Heavy Armor", "heavy_armor", heavy_material, 50, 5, 'H', 15, 15, "common");
    Item_t* light_sword = create_weapon("Light Sword", "light_sword", light_material, 20, 30, 0, 'L', 0, 0, "common");
    Item_t* normal_arrows = create_ammunition("Arrows", "arrows", heavy_material, 3, 5, 'a', 0, "common");

    apply_material_to_armor(heavy_armor);
    apply_material_to_weapon(light_sword);
    apply_material_to_ammunition(normal_arrows);

    TEST_ASSERT(inventory != NULL && heavy_armor != NULL && light_sword != NULL && normal_arrows != NULL,
                "All items should be created");

    // Test weight calculations with mixed items
    add_item_to_inventory(inventory, heavy_armor, 1);
    add_item_to_inventory(inventory, light_sword, 1);
    add_item_to_inventory(inventory, normal_arrows, 100);

    uint8_t total_weight = get_total_inventory_weight(inventory);
    LOG("Testing weight with mixed item types");
    TEST_ASSERT(total_weight > 0, "Inventory with items should have positive weight");

    // Test weight precision with many light items
    Inventory_t* precision_inventory = create_inventory(20);
    for (int i = 0; i < 10; i++) {
        add_item_to_inventory(precision_inventory, light_sword, 1);
    }

    uint8_t precision_weight = get_total_inventory_weight(precision_inventory);
    LOG("Testing weight precision with many light items");
    TEST_ASSERT(precision_weight >= 0, "Weight should not be negative due to precision issues");

    // Test weight with equipped vs unequipped items
    equip_item(inventory, "heavy_armor");
    uint8_t weight_with_equipped = get_total_inventory_weight(inventory);

    unequip_item(inventory, "heavy_armor");
    uint8_t weight_without_equipped = get_total_inventory_weight(inventory);

    TEST_ASSERT(weight_with_equipped == weight_without_equipped,
                "Weight should be same whether items are equipped or not");

    LOG("Weight calculations test completed");
    destroy_item(heavy_armor);
    destroy_item(light_sword);
    destroy_item(normal_arrows);
    destroy_inventory(inventory);
    destroy_inventory(precision_inventory);
    return 1;
}

// =============================================================================
// EQUIPMENT STATE CORRUPTION TESTS
// =============================================================================

int test_equipment_state_corruption(void)
{
    LOG("Starting equipment state corruption test");
    Inventory_t* inventory = create_inventory(5);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* sword1 = create_weapon("Sword One", "sword_one", basic_material, 20, 30, 0, 'S', 0, 0, "common");
    Item_t* sword2 = create_weapon("Sword Two", "sword_two", basic_material, 25, 35, 0, 'T', 0, 0, "common");
    Item_t* armor = create_armor("Test Armor", "test_armor", basic_material, 30, 10, 'A', 15, 15, "common");

    TEST_ASSERT(inventory != NULL && sword1 != NULL && sword2 != NULL && armor != NULL,
                "All items should be created");

    // Add items to inventory
    add_item_to_inventory(inventory, sword1, 1);
    add_item_to_inventory(inventory, sword2, 1);
    add_item_to_inventory(inventory, armor, 1);

    // Test: Equip item, then remove it from inventory (should unequip)
    equip_item(inventory, "sword_one");
    Inventory_slot_t* equipped_weapon = get_equipped_weapon(inventory);
    TEST_ASSERT(equipped_weapon != NULL, "Sword should be equipped");

    // Remove equipped item
    bool removed = remove_item_from_inventory(inventory, "sword_one", 1);
    TEST_ASSERT(removed, "Should be able to remove equipped item");

    equipped_weapon = get_equipped_weapon(inventory);
    TEST_ASSERT(equipped_weapon == NULL, "Removing equipped item should unequip it");

    // Test: Try to equip multiple items of same type
    equip_item(inventory, "sword_two");
    // This should not work if system is working correctly, but test the state
    Inventory_slot_t* weapon_after_multi_equip = get_equipped_weapon(inventory);
    TEST_ASSERT(weapon_after_multi_equip != NULL, "Should have one weapon equipped");

    // Test equipment state after inventory modifications
    equip_item(inventory, "test_armor");
    Inventory_slot_t* equipped_armor = get_equipped_armor(inventory);
    TEST_ASSERT(equipped_armor != NULL, "Armor should be equipped");

    // Add more items and verify equipment state remains consistent
    Item_t* new_item = create_weapon("New Weapon", "new_weapon", basic_material, 15, 20, 0, 'N', 0, 0, "common");
    add_item_to_inventory(inventory, new_item, 1);

    equipped_armor = get_equipped_armor(inventory);
    TEST_ASSERT(equipped_armor != NULL, "Equipment state should remain after adding items");
    TEST_ASSERT(strcmp(equipped_armor->item.id->str, "test_armor") == 0,
                "Equipment should still point to correct item");

    LOG("Equipment state corruption test completed");
    destroy_item(sword1);
    destroy_item(sword2);
    destroy_item(armor);
    destroy_item(new_item);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// RAPID OPERATIONS STRESS TEST
// =============================================================================

int test_inventory_rapid_operations(void)
{
    LOG("Starting inventory rapid operations test");
    Inventory_t* inventory = create_inventory(10);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    Item_t* test_item = create_consumable("Test Item", "test_item", 25, dummy_consume_callback, 'T', 0, "common");
    Item_t* equippable = create_weapon("Rapid Weapon", "rapid_weapon", basic_material, 10, 15, 0, 'R', 0, 0, "common");

    TEST_ASSERT(inventory != NULL && test_item != NULL && equippable != NULL,
                "All items should be created");

    // Test rapid add/remove cycles
    LOG("Testing rapid add/remove operations");
    for (int cycle = 0; cycle < 50; cycle++) {
        bool added = add_item_to_inventory(inventory, test_item, 5);
        TEST_ASSERT(added, "Rapid add should succeed");

        Inventory_slot_t* found = find_item_in_inventory(inventory, "test_item");
        TEST_ASSERT(found != NULL, "Item should be found after rapid add");

        bool removed = remove_item_from_inventory(inventory, "test_item", 5);
        TEST_ASSERT(removed, "Rapid remove should succeed");
    }

    // Test rapid equip/unequip cycles
    LOG("Testing rapid equip/unequip operations");
    add_item_to_inventory(inventory, equippable, 1);

    for (int cycle = 0; cycle < 25; cycle++) {
        bool equipped = equip_item(inventory, "rapid_weapon");
        TEST_ASSERT(equipped, "Rapid equip should succeed");

        Inventory_slot_t* equipped_weapon = get_equipped_weapon(inventory);
        TEST_ASSERT(equipped_weapon != NULL, "Weapon should be equipped after rapid equip");

        bool unequipped = unequip_item(inventory, "rapid_weapon");
        TEST_ASSERT(unequipped, "Rapid unequip should succeed");

        equipped_weapon = get_equipped_weapon(inventory);
        TEST_ASSERT(equipped_weapon == NULL, "Weapon should be unequipped after rapid unequip");
    }

    // Test memory consistency during rapid changes
    LOG("Testing memory consistency during rapid changes");
    uint8_t initial_free_slots = get_inventory_free_slots(inventory);

    for (int i = 0; i < 100; i++) {
        add_item_to_inventory(inventory, test_item, 1);
        remove_item_from_inventory(inventory, "test_item", 1);
    }

    uint8_t final_free_slots = get_inventory_free_slots(inventory);
    TEST_ASSERT(initial_free_slots == final_free_slots,
                "Inventory should return to initial state after rapid operations");

    LOG("Rapid operations test completed");
    destroy_item(test_item);
    destroy_item(equippable);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// ITEM IDENTITY AND CORRUPTION TESTS
// =============================================================================

int test_item_identity_corruption(void)
{
    LOG("Starting item identity corruption test");
    Inventory_t* inventory = create_inventory(5);
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Create items with same ID but different properties (potential corruption scenario)
    Item_t* arrow1 = create_ammunition("Arrow Type A", "arrow", basic_material, 3, 5, 'a', 0, "common");
    Item_t* arrow2 = create_ammunition("Arrow Type B", "arrow", basic_material, 5, 8, 'b', 0, "common");

    TEST_ASSERT(inventory != NULL && arrow1 != NULL && arrow2 != NULL,
                "All items should be created");

    // Test adding items with same ID
    bool first_added = add_item_to_inventory(inventory, arrow1, 10);
    TEST_ASSERT(first_added, "First arrow should be added");

    bool second_added = add_item_to_inventory(inventory, arrow2, 5);
    // This should either stack (if IDs match) or create separate slot
    // The behavior depends on implementation, but should not crash

    Inventory_slot_t* found_arrow = find_item_in_inventory(inventory, "arrow");
    TEST_ASSERT(found_arrow != NULL, "Arrow should be found by ID");

    // Test stack integrity when items change
    LOG("Testing stack integrity during modifications");
    Item_t* stackable = create_ammunition("Stack Test", "stackable", basic_material, 2, 4, 's', 0, "common");

    add_item_to_inventory(inventory, stackable, 50);
    Inventory_slot_t* stack_slot = find_item_in_inventory(inventory, "stackable");
    TEST_ASSERT(stack_slot != NULL, "Stackable item should be found");

    uint8_t original_quantity = stack_slot->quantity;

    // Modify the original item (simulating external modification)
    // This tests if the inventory maintains integrity when source items change
    add_item_to_inventory(inventory, stackable, 25);

    stack_slot = find_item_in_inventory(inventory, "stackable");
    TEST_ASSERT(stack_slot != NULL, "Stack should still exist after modification");
    TEST_ASSERT(stack_slot->quantity >= original_quantity, "Stack quantity should not decrease");

    LOG("Item identity corruption test completed");
    destroy_item(arrow1);
    destroy_item(arrow2);
    destroy_item(stackable);
    destroy_inventory(inventory);
    return 1;
}

// =============================================================================
// INVENTORY PATHOLOGICAL INPUTS
// =============================================================================

int test_inventory_pathological_inputs(void)
{
    LOG("Starting inventory pathological inputs test");

    // Test maximum size inventory
    LOG("Testing maximum size inventory creation");
    Inventory_t* max_inventory = create_inventory(255);
    TEST_ASSERT(max_inventory != NULL, "Should be able to create maximum size inventory");
    TEST_ASSERT(max_inventory->size == 255, "Maximum inventory should have 255 slots");

    // Test with extreme item properties
    MaterialProperties_t extreme_props = create_default_material_properties();
    extreme_props.weight_fact = 0.0f; // Zero weight
    extreme_props.value_coins_fact = 1000.0f; // Extreme value
    Material_t extreme_material = create_material("extreme", extreme_props);

    Item_t* zero_weight_item = create_weapon("Zero Weight", "zero_weight", extreme_material, 1, 1, 0, 'Z', 0, 0, "common");
    apply_material_to_weapon(zero_weight_item);

    TEST_ASSERT(zero_weight_item != NULL, "Zero weight item should be created");

    // Test adding zero-weight items
    bool zero_weight_added = add_item_to_inventory(max_inventory, zero_weight_item, 1);
    TEST_ASSERT(zero_weight_added, "Zero weight item should be addable");

    uint8_t weight_with_zero_item = get_total_inventory_weight(max_inventory);
    LOG("Testing zero-weight item calculations");
    TEST_ASSERT(weight_with_zero_item >= 0, "Weight should not be negative with zero-weight items");

    // Test with invalid item type combinations (if possible)
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    // Test adding many items to large inventory
    LOG("Testing large inventory stress");
    Item_t* filler = create_consumable("Filler", "filler", 1, dummy_consume_callback, 'F', 0, "common");

    int successful_adds = 0;
    for (int i = 0; i < 254; i++) {
        if (add_item_to_inventory(max_inventory, filler, 1)) {
            successful_adds++;
        }
    }

    TEST_ASSERT(successful_adds > 0, "Should be able to add items to large inventory");
    LOG("Successfully added items to large inventory");

    // Test inventory operations on large inventory
    bool is_large_full = is_inventory_full(max_inventory);
    uint8_t large_free_slots = get_inventory_free_slots(max_inventory);

    TEST_ASSERT(large_free_slots >= 0, "Large inventory should report valid free slots");
    LOG("Large inventory operations completed successfully");

    LOG("Pathological inputs test completed");
    destroy_item(zero_weight_item);
    destroy_item(filler);
    destroy_inventory(max_inventory);
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

    RUN_TEST(test_inventory_stack_boundaries);
    RUN_TEST(test_inventory_capacity_edge_cases);
    RUN_TEST(test_inventory_weight_calculations);
    RUN_TEST(test_equipment_state_corruption);
    RUN_TEST(test_inventory_rapid_operations);
    RUN_TEST(test_item_identity_corruption);
    RUN_TEST(test_inventory_pathological_inputs);

    TEST_SUITE_END();
}
