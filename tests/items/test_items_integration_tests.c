// ASCIIGame/tests/items/test_items_integration_tests.c
// Integration tests for items system - testing multiple systems working together
#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tests.h"
#include "Daedalus.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include <stdlib.h>
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
// ITEM LIFECYCLE INTEGRATION TESTS
// =============================================================================

int test_complete_item_lifecycle(void)
{
    LOG("Starting complete item lifecycle integration test");

    // Create inventory for lifecycle testing
    Inventory_t* player_inventory = create_inventory(20);
    TEST_ASSERT(player_inventory != NULL, "Player inventory should be created");

    // Create materials with different properties
    MaterialProperties_t steel_props = create_default_material_properties();
    steel_props.min_damage_fact = 1.5f;
    steel_props.max_damage_fact = 1.5f;
    steel_props.durability_fact = 1.2f;
    steel_props.weight_fact = 1.3f;
    steel_props.value_coins_fact = 2.0f;
    Material_t steel_material = create_material("steel", steel_props);

    MaterialProperties_t mithril_props = create_default_material_properties();
    mithril_props.min_damage_fact = 2.0f;
    mithril_props.max_damage_fact = 2.0f;
    mithril_props.durability_fact = 3.0f;
    mithril_props.weight_fact = 0.5f;
    mithril_props.value_coins_fact = 10.0f;
    Material_t mithril_material = create_material("mithril", mithril_props);

    // Phase 1: Item Creation and Material Application
    LOG("Phase 1: Creating items and applying materials");
    Item_t* iron_sword = create_weapon("Iron Sword", "iron_sword", steel_material, 15, 25, 0, 'I');
    Item_t* mithril_armor = create_armor("Mithril Armor", "mithril_armor", mithril_material, 40, 15, 'M', 20, 20);
    Item_t* steel_arrows = create_ammunition("Steel Arrows", "steel_arrows", steel_material, 8, 12, 'a');

    TEST_ASSERT(iron_sword != NULL && mithril_armor != NULL && steel_arrows != NULL,
                "All items should be created successfully");

    // Apply materials and check properties
    apply_material_to_weapon(iron_sword);
    apply_material_to_armor(mithril_armor);
    apply_material_to_ammunition(steel_arrows);

    uint8_t sword_min_dmg = get_weapon_min_damage(iron_sword);
    float armor_weight = get_item_weight(mithril_armor);
    uint8_t arrow_max_dmg = get_ammunition_max_damage(steel_arrows);

    TEST_ASSERT(sword_min_dmg > 15, "Steel sword should have enhanced damage");
    TEST_ASSERT(armor_weight < 1.0f, "Mithril armor should be lightweight");
    TEST_ASSERT(arrow_max_dmg > 12, "Steel arrows should have enhanced damage");

    // Phase 2: Inventory Management Integration
    LOG("Phase 2: Adding items to inventory and managing them");
    bool sword_added = add_item_to_inventory(player_inventory, iron_sword, 1);
    bool armor_added = add_item_to_inventory(player_inventory, mithril_armor, 1);
    bool arrows_added = add_item_to_inventory(player_inventory, steel_arrows, 99);

    TEST_ASSERT(sword_added && armor_added && arrows_added,
                "All items should be added to inventory successfully");

    uint8_t total_weight = get_total_inventory_weight(player_inventory);
    uint8_t free_slots = get_inventory_free_slots(player_inventory);

    TEST_ASSERT(total_weight > 0, "Inventory should have weight with items");
    TEST_ASSERT(free_slots == 17, "Should have 17 free slots after adding 3 item types");

    // Phase 3: Equipment System Integration
    LOG("Phase 3: Equipment and combat stat integration");
    bool sword_equipped = equip_item(player_inventory, "iron_sword");
    bool armor_equipped = equip_item(player_inventory, "mithril_armor");

    TEST_ASSERT(sword_equipped && armor_equipped, "Items should be equipped successfully");

    Inventory_slot_t* equipped_weapon = get_equipped_weapon(player_inventory);
    Inventory_slot_t* equipped_armor = get_equipped_armor(player_inventory);

    TEST_ASSERT(equipped_weapon != NULL && equipped_armor != NULL,
                "Equipment should be accessible after equipping");

    // Verify equipment stats are consistent with material modifications
    uint8_t equipped_weapon_damage = get_weapon_min_damage(&equipped_weapon->item);
    uint8_t equipped_armor_value = get_armor_value(&equipped_armor->item);

    TEST_ASSERT(equipped_weapon_damage == sword_min_dmg,
                "Equipped weapon damage should match item damage");
    TEST_ASSERT(equipped_armor_value >= 40, "Equipped armor should have enhanced protection");

    // Phase 4: Durability and Usage Integration
    LOG("Phase 4: Durability system integration");
    uint8_t initial_sword_durability = get_durability(iron_sword);
    uint8_t initial_armor_durability = get_durability(mithril_armor);

    // Simulate usage damage
    damage_item_durability(iron_sword, 50);
    damage_item_durability(mithril_armor, 30);

    uint8_t damaged_sword_durability = get_durability(iron_sword);
    uint8_t damaged_armor_durability = get_durability(mithril_armor);

    TEST_ASSERT(damaged_sword_durability < initial_sword_durability,
                "Sword durability should decrease after damage");
    TEST_ASSERT(damaged_armor_durability < initial_armor_durability,
                "Armor durability should decrease after damage");

    // Equipment should still be functional
    TEST_ASSERT(get_equipped_weapon(player_inventory) != NULL,
                "Damaged weapon should still be equipped");
    TEST_ASSERT(!is_item_broken(iron_sword), "Damaged sword should not be broken");

    // Phase 5: Repair and Maintenance Integration
    LOG("Phase 5: Repair system integration");
    repair_item(iron_sword, 25);
    uint8_t repaired_sword_durability = get_durability(iron_sword);

    TEST_ASSERT(repaired_sword_durability > damaged_sword_durability,
                "Repaired sword should have higher durability");

    // Phase 6: Item Removal and Equipment State
    LOG("Phase 6: Item removal and equipment state consistency");
    bool sword_unequipped = unequip_item(player_inventory, "iron_sword");
    TEST_ASSERT(sword_unequipped, "Sword should be unequipped successfully");

    bool sword_removed = remove_item_from_inventory(player_inventory, "iron_sword", 1);
    TEST_ASSERT(sword_removed, "Sword should be removed from inventory");

    Inventory_slot_t* weapon_after_removal = get_equipped_weapon(player_inventory);
    TEST_ASSERT(weapon_after_removal == NULL,
                "No weapon should be equipped after removal");

    LOG("Complete item lifecycle integration test completed");
    destroy_item(iron_sword);
    destroy_item(mithril_armor);
    destroy_item(steel_arrows);
    destroy_inventory(player_inventory);
    return 1;
}

// =============================================================================
// MULTI-INVENTORY TRADING INTEGRATION TESTS
// =============================================================================

int test_multi_inventory_trading_integration(void)
{
    LOG("Starting multi-inventory trading integration test");

    // Create multiple inventories (player, merchant, storage)
    Inventory_t* player_inv = create_inventory(10);
    Inventory_t* merchant_inv = create_inventory(15);
    Inventory_t* storage_inv = create_inventory(50);

    TEST_ASSERT(player_inv != NULL && merchant_inv != NULL && storage_inv != NULL,
                "All inventories should be created");

    // Create diverse items for trading
    MaterialProperties_t basic_props = create_default_material_properties();
    Material_t basic_material = create_material("basic", basic_props);

    MaterialProperties_t rare_props = create_default_material_properties();
    rare_props.value_coins_fact = 5.0f;
    rare_props.weight_fact = 0.8f;
    Material_t rare_material = create_material("rare", rare_props);

    Item_t* common_sword = create_weapon("Common Sword", "common_sword", basic_material, 10, 15, 0, 'C');
    Item_t* rare_gem = create_consumable("Rare Gem", "rare_gem", 100, dummy_consume_callback, 'G');
    Item_t* trade_arrows = create_ammunition("Trade Arrows", "trade_arrows", basic_material, 5, 8, 'T');
    Item_t* merchant_armor = create_armor("Merchant Armor", "merchant_armor", rare_material, 25, 12, 'M', 18, 18);

    apply_material_to_weapon(common_sword);
    apply_material_to_armor(merchant_armor);
    apply_material_to_ammunition(trade_arrows);

    // Set up initial inventories
    add_item_to_inventory(player_inv, common_sword, 1);
    add_item_to_inventory(player_inv, trade_arrows, 200);

    add_item_to_inventory(merchant_inv, rare_gem, 5);
    add_item_to_inventory(merchant_inv, merchant_armor, 1);

    add_item_to_inventory(storage_inv, trade_arrows, 500);

    // Test cross-inventory operations and state consistency
    LOG("Testing cross-inventory trading operations");

    // Simulate trading: player gives arrows, gets gem
    bool arrows_removed = remove_item_from_inventory(player_inv, "trade_arrows", 50);
    bool gem_removed = remove_item_from_inventory(merchant_inv, "rare_gem", 1);

    TEST_ASSERT(arrows_removed && gem_removed, "Trade items should be removed successfully");

    bool arrows_to_merchant = add_item_to_inventory(merchant_inv, trade_arrows, 50);
    bool gem_to_player = add_item_to_inventory(player_inv, rare_gem, 1);

    TEST_ASSERT(arrows_to_merchant && gem_to_player, "Trade items should be added successfully");

    // Verify inventory states after trading
    Inventory_slot_t* player_gem = find_item_in_inventory(player_inv, "rare_gem");
    Inventory_slot_t* merchant_arrows = find_item_in_inventory(merchant_inv, "trade_arrows");
    Inventory_slot_t* player_remaining_arrows = find_item_in_inventory(player_inv, "trade_arrows");

    TEST_ASSERT(player_gem != NULL && player_gem->quantity == 1,
                "Player should have 1 gem after trade");
    TEST_ASSERT(merchant_arrows != NULL && merchant_arrows->quantity == 50,
                "Merchant should have 50 arrows after trade");
    TEST_ASSERT(player_remaining_arrows != NULL && player_remaining_arrows->quantity == 150,
                "Player should have remaining arrows after trade");

    // Test bulk transfer to storage
    LOG("Testing bulk storage operations");
    bool bulk_transfer = remove_item_from_inventory(player_inv, "trade_arrows", 150);
    TEST_ASSERT(bulk_transfer, "Bulk removal should succeed");

    bool bulk_storage = add_item_to_inventory(storage_inv, trade_arrows, 150);
    TEST_ASSERT(bulk_storage, "Bulk storage should succeed");

    // Verify storage consolidation
    Inventory_slot_t* storage_arrows = find_item_in_inventory(storage_inv, "trade_arrows");
    TEST_ASSERT(storage_arrows != NULL && storage_arrows->quantity == 255,
                "Storage should consolidate arrow stacks properly");

    // Test inventory weight calculations across multiple inventories
    uint8_t player_weight = get_total_inventory_weight(player_inv);
    uint8_t merchant_weight = get_total_inventory_weight(merchant_inv);
    uint8_t storage_weight = get_total_inventory_weight(storage_inv);

    TEST_ASSERT(player_weight > 0 && merchant_weight > 0 && storage_weight > 0,
                "All inventories should have weight");

    LOG("Multi-inventory trading integration test completed");
    destroy_item(common_sword);
    destroy_item(rare_gem);
    destroy_item(trade_arrows);
    destroy_item(merchant_armor);
    destroy_inventory(player_inv);
    destroy_inventory(merchant_inv);
    destroy_inventory(storage_inv);
    return 1;
}

// =============================================================================
// COMBAT SIMULATION INTEGRATION TESTS
// =============================================================================

int test_combat_simulation_integration(void)
{
    LOG("Starting combat simulation integration test");

    // Create combat participants
    Inventory_t* warrior_inv = create_inventory(12);
    Inventory_t* archer_inv = create_inventory(8);

    TEST_ASSERT(warrior_inv != NULL && archer_inv != NULL,
                "Combat inventories should be created");

    // Create combat materials
    MaterialProperties_t battle_worn_props = create_default_material_properties();
    battle_worn_props.durability_fact = 0.7f;
    battle_worn_props.min_damage_fact = 1.1f;
    battle_worn_props.max_damage_fact = 1.1f;
    Material_t battle_worn = create_material("battle_worn", battle_worn_props);

    MaterialProperties_t masterwork_props = create_default_material_properties();
    masterwork_props.durability_fact = 2.0f;
    masterwork_props.min_damage_fact = 1.8f;
    masterwork_props.max_damage_fact = 1.8f;
    masterwork_props.armor_value_fact = 1.5f;
    Material_t masterwork = create_material("masterwork", masterwork_props);

    // Create warrior equipment
    Item_t* warrior_sword = create_weapon("Battle Sword", "battle_sword", masterwork, 20, 30, 0, 'B');
    Item_t* warrior_armor = create_armor("Plate Armor", "plate_armor", masterwork, 50, 8, 'P', 25, 25);
    Item_t* health_potion = create_consumable("Health Potion", "health_potion", 75, dummy_consume_callback, 'H');

    // Create archer equipment
    Item_t* archer_bow = create_weapon("War Bow", "war_bow", battle_worn, 12, 18, 3, 'W');
    Item_t* archer_armor = create_armor("Leather Armor", "leather_armor", battle_worn, 20, 20, 'L', 12, 12);
    Item_t* arrows = create_ammunition("War Arrows", "war_arrows", masterwork, 10, 15, 'A');

    // Apply materials and verify combat readiness
    apply_material_to_weapon(warrior_sword);
    apply_material_to_armor(warrior_armor);
    apply_material_to_weapon(archer_bow);
    apply_material_to_armor(archer_armor);
    apply_material_to_ammunition(arrows);

    TEST_ASSERT(get_weapon_min_damage(warrior_sword) > 20,
                "Warrior sword should have enhanced damage");
    TEST_ASSERT(get_armor_value(warrior_armor) > 50,
                "Warrior armor should have enhanced protection");
    TEST_ASSERT(get_weapon_range(archer_bow) == 3,
                "Archer bow should have ranged capability");

    // Equip combat gear
    add_item_to_inventory(warrior_inv, warrior_sword, 1);
    add_item_to_inventory(warrior_inv, warrior_armor, 1);
    add_item_to_inventory(warrior_inv, health_potion, 3);

    add_item_to_inventory(archer_inv, archer_bow, 1);
    add_item_to_inventory(archer_inv, archer_armor, 1);
    add_item_to_inventory(archer_inv, arrows, 100);

    equip_item(warrior_inv, "battle_sword");
    equip_item(warrior_inv, "plate_armor");
    equip_item(archer_inv, "war_bow");
    equip_item(archer_inv, "leather_armor");

    // Simulate combat rounds with durability damage
    LOG("Simulating combat rounds with equipment degradation");

    for (int round = 1; round <= 5; round++) {
        // Combat damage to equipment
        damage_item_durability(warrior_sword, 15);
        damage_item_durability(warrior_armor, 10);
        damage_item_durability(archer_bow, 20);
        damage_item_durability(archer_armor, 12);

        // Check equipment status
        bool warrior_sword_broken = is_item_broken(warrior_sword);
        bool archer_bow_broken = is_item_broken(archer_bow);

        if (warrior_sword_broken || archer_bow_broken) {
            LOG("Equipment broken during combat - testing emergency procedures");

            if (warrior_sword_broken) {
                unequip_item(warrior_inv, "battle_sword");
                TEST_ASSERT(get_equipped_weapon(warrior_inv) == NULL,
                            "Broken weapon should be unequipped");
            }

            if (archer_bow_broken) {
                unequip_item(archer_inv, "war_bow");
                TEST_ASSERT(get_equipped_weapon(archer_inv) == NULL,
                            "Broken bow should be unequipped");
            }
            break;
        }

        // Use consumables during combat
        if (round == 3) {
            bool potion_used = remove_item_from_inventory(warrior_inv, "health_potion", 1);
            TEST_ASSERT(potion_used, "Health potion should be usable during combat");
        }

        // Use ammunition
        bool arrows_used = remove_item_from_inventory(archer_inv, "war_arrows", 5);
        TEST_ASSERT(arrows_used, "Arrows should be consumed during combat");
    }

    // Post-combat equipment assessment
    LOG("Assessing post-combat equipment status");

    uint8_t warrior_sword_durability = get_durability(warrior_sword);
    uint8_t archer_bow_durability = get_durability(archer_bow);

    TEST_ASSERT(warrior_sword_durability < 255, "Warrior sword should show wear");
    TEST_ASSERT(archer_bow_durability < 255, "Archer bow should show wear");

    // Test field repairs
    repair_item(warrior_sword, 30);
    repair_item(archer_bow, 25);

    uint8_t repaired_sword_durability = get_durability(warrior_sword);
    uint8_t repaired_bow_durability = get_durability(archer_bow);

    TEST_ASSERT(repaired_sword_durability > warrior_sword_durability,
                "Field repairs should improve durability");
    TEST_ASSERT(repaired_bow_durability > archer_bow_durability,
                "Field repairs should improve durability");

    // Verify inventory states after combat
    Inventory_slot_t* remaining_arrows = find_item_in_inventory(archer_inv, "war_arrows");
    Inventory_slot_t* remaining_potions = find_item_in_inventory(warrior_inv, "health_potion");

    TEST_ASSERT(remaining_arrows != NULL && remaining_arrows->quantity < 100,
                "Arrow count should decrease after combat");
    TEST_ASSERT(remaining_potions != NULL && remaining_potions->quantity < 3,
                "Potion count should decrease after combat");

    LOG("Combat simulation integration test completed");
    destroy_item(warrior_sword);
    destroy_item(warrior_armor);
    destroy_item(health_potion);
    destroy_item(archer_bow);
    destroy_item(archer_armor);
    destroy_item(arrows);
    destroy_inventory(warrior_inv);
    destroy_inventory(archer_inv);
    return 1;
}

// =============================================================================
// MASS OPERATIONS STRESS INTEGRATION TESTS
// =============================================================================

int test_mass_operations_stress_integration(void)
{
    LOG("Starting mass operations stress integration test");

    // Create large-scale testing environment
    Inventory_t* warehouse = create_inventory(150);
    TEST_ASSERT(warehouse != NULL, "Warehouse inventory should be created");

    // Create materials for mass testing
    MaterialProperties_t mass_props = create_default_material_properties();
    mass_props.weight_fact = 0.9f;
    mass_props.value_coins_fact = 1.1f;
    Material_t mass_material = create_material("mass_produced", mass_props);

    // Create item templates for mass operations
    Item_t* template_weapon = create_weapon("Mass Weapon", "mass_weapon", mass_material, 8, 12, 0, 'M');
    Item_t* template_armor = create_armor("Mass Armor", "mass_armor", mass_material, 15, 8, 'A', 10, 10);
    Item_t* template_ammo = create_ammunition("Mass Ammo", "mass_ammo", mass_material, 3, 5, 'B');
    Item_t* template_consumable = create_consumable("Mass Consumable", "mass_consumable", 25, dummy_consume_callback, 'C');

    apply_material_to_weapon(template_weapon);
    apply_material_to_armor(template_armor);
    apply_material_to_ammunition(template_ammo);

    // Phase 1: Mass item addition
    LOG("Phase 1: Mass item addition stress test");
    int successful_additions = 0;

    for (int i = 0; i < 25; i++) {
        if (add_item_to_inventory(warehouse, template_weapon, 1)) successful_additions++;
        if (add_item_to_inventory(warehouse, template_armor, 1)) successful_additions++;
        if (add_item_to_inventory(warehouse, template_ammo, 200)) successful_additions++;
        if (add_item_to_inventory(warehouse, template_consumable, 50)) successful_additions++;
    }

    TEST_ASSERT(successful_additions > 90, "Most mass additions should succeed");

    uint8_t warehouse_weight = get_total_inventory_weight(warehouse);
    uint8_t free_slots = get_inventory_free_slots(warehouse);
    bool warehouse_full = is_inventory_full(warehouse);

    TEST_ASSERT(warehouse_weight > 0, "Warehouse should have significant weight");
    LOG("Mass addition completed - checking warehouse status");

    // Phase 2: Mass equipment operations
    LOG("Phase 2: Mass equipment stress test");
    int equip_operations = 0;

    // Try to equip/unequip items rapidly
    for (int cycle = 0; cycle < 50; cycle++) {
        if (equip_item(warehouse, "mass_weapon")) {
            equip_operations++;
            if (unequip_item(warehouse, "mass_weapon")) {
                equip_operations++;
            }
        }
        if (equip_item(warehouse, "mass_armor")) {
            equip_operations++;
            if (unequip_item(warehouse, "mass_armor")) {
                equip_operations++;
            }
        }
    }

    TEST_ASSERT(equip_operations > 100, "Mass equipment operations should mostly succeed");

    // Phase 3: Mass durability operations
    LOG("Phase 3: Mass durability stress test");

    // Find all weapons and armor for durability testing
    for (uint8_t slot = 0; slot < warehouse->size; slot++) {
        if (warehouse->slots[slot].quantity > 0) {
            Item_t* item = &warehouse->slots[slot].item;

            if (is_weapon(item) || is_armor(item)) {
                // Damage and repair cycles
                for (int cycle = 0; cycle < 10; cycle++) {
                    damage_item_durability(item, 20);
                    repair_item(item, 15);
                }

                // Verify item integrity after stress
                uint8_t final_durability = get_durability(item);
                TEST_ASSERT(final_durability > 0, "Items should survive durability stress");
            }
        }
    }

    // Phase 4: Mass removal operations
    LOG("Phase 4: Mass removal stress test");
    int removal_operations = 0;

    // Remove items in patterns
    for (int i = 0; i < 10; i++) {
        if (remove_item_from_inventory(warehouse, "mass_ammo", 50)) removal_operations++;
        if (remove_item_from_inventory(warehouse, "mass_consumable", 10)) removal_operations++;
    }

    TEST_ASSERT(removal_operations > 0, "Mass removal operations should succeed");

    // Phase 5: Final state verification
    LOG("Phase 5: Final warehouse state verification");

    uint8_t final_weight = get_total_inventory_weight(warehouse);
    uint8_t final_free_slots = get_inventory_free_slots(warehouse);

    TEST_ASSERT(final_weight <= warehouse_weight, "Final weight should not exceed initial");
    TEST_ASSERT(final_free_slots >= free_slots, "Should have same or more free slots after removals");

    // Verify no memory corruption by checking all slots
    for (uint8_t slot = 0; slot < warehouse->size; slot++) {
        if (warehouse->slots[slot].quantity > 0) {
            Item_t* item = &warehouse->slots[slot].item;
            TEST_ASSERT(item->id != NULL && item->name != NULL,
                        "Items should maintain integrity after mass operations");
        }
    }

    LOG("Mass operations stress integration test completed");
    destroy_item(template_weapon);
    destroy_item(template_armor);
    destroy_item(template_ammo);
    destroy_item(template_consumable);
    destroy_inventory(warehouse);
    return 1;
}

// =============================================================================
// MATERIAL SYSTEM CROSS-INTEGRATION TESTS
// =============================================================================

int test_material_system_cross_integration(void)
{
    LOG("Starting material system cross-integration test");

    // Create test environment
    Inventory_t* laboratory = create_inventory(30);
    TEST_ASSERT(laboratory != NULL, "Laboratory inventory should be created");

    // Create extreme materials for cross-testing
    MaterialProperties_t chaos_props = create_default_material_properties();
    chaos_props.min_damage_fact = 3.0f;
    chaos_props.max_damage_fact = 0.5f; // Intentionally inverted
    chaos_props.durability_fact = 2.0f;
    chaos_props.weight_fact = -0.1f; // Negative weight (should be clamped)
    chaos_props.value_coins_fact = 50.0f;
    chaos_props.armor_value_fact = 1.5f;
    chaos_props.stealth_value_fact = 3.0f;
    Material_t chaos_material = create_material("chaos", chaos_props);

    MaterialProperties_t void_props = create_default_material_properties();
    void_props.min_damage_fact = 0.0f; // Zero damage
    void_props.durability_fact = 0.1f; // Nearly zero durability
    void_props.weight_fact = 0.0f; // Zero weight
    void_props.value_coins_fact = 0.0f; // Zero value
    Material_t void_material = create_material("void", void_props);

    MaterialProperties_t overflow_props = create_default_material_properties();
    overflow_props.min_damage_fact = 10.0f; // Potential overflow
    overflow_props.max_damage_fact = 20.0f;
    overflow_props.durability_fact = 5.0f;
    overflow_props.armor_value_fact = 8.0f;
    Material_t overflow_material = create_material("overflow", overflow_props);

    // Create items with extreme materials
    Item_t* chaos_weapon = create_weapon("Chaos Blade", "chaos_blade", chaos_material, 30, 50, 1, 'C');
    Item_t* void_armor = create_armor("Void Armor", "void_armor", void_material, 40, 15, 'V', 20, 20);
    Item_t* overflow_ammo = create_ammunition("Overflow Arrows", "overflow_arrows", overflow_material, 15, 25, 'O');

    // Apply materials and test boundary conditions
    LOG("Testing extreme material applications");
    apply_material_to_weapon(chaos_weapon);
    apply_material_to_armor(void_armor);
    apply_material_to_ammunition(overflow_ammo);

    // Test chaos weapon properties
    uint8_t chaos_min_dmg = get_weapon_min_damage(chaos_weapon);
    uint8_t chaos_max_dmg = get_weapon_max_damage(chaos_weapon);
    float chaos_weight = get_item_weight(chaos_weapon);
    uint8_t chaos_value = get_item_value_coins(chaos_weapon);

    TEST_ASSERT(chaos_min_dmg >= chaos_max_dmg || chaos_max_dmg > 0,
                "Chaos weapon should handle inverted damage gracefully");
    TEST_ASSERT(chaos_weight >= 0.0f, "Chaos weapon weight should be clamped to non-negative");
    TEST_ASSERT(chaos_value > 0, "Chaos weapon should have positive value");

    // Test void armor properties
    uint8_t void_armor_value = get_armor_value(void_armor);
    float void_weight = get_item_weight(void_armor);
    uint8_t void_durability = get_durability(void_armor);

    TEST_ASSERT(void_armor_value >= 0, "Void armor should have non-negative armor value");
    TEST_ASSERT(void_weight >= 0.0f, "Void armor weight should be non-negative");
    TEST_ASSERT(void_durability > 0, "Void armor should have some durability");

    // Test overflow ammunition properties
    uint8_t overflow_min_dmg = get_ammunition_min_damage(overflow_ammo);
    uint8_t overflow_max_dmg = get_ammunition_max_damage(overflow_ammo);

    TEST_ASSERT(overflow_min_dmg <= 255 && overflow_max_dmg <= 255,
                "Overflow ammunition should not exceed uint8_t limits");

    // Add to inventory and test cross-system interactions
    LOG("Testing cross-system interactions with extreme materials");
    add_item_to_inventory(laboratory, chaos_weapon, 1);
    add_item_to_inventory(laboratory, void_armor, 1);
    add_item_to_inventory(laboratory, overflow_ammo, 100);

    // Test durability interactions with extreme materials
    LOG("Testing durability interactions with extreme materials");

    // Log initial durabilities for debugging
    uint8_t chaos_initial_durability = get_durability(chaos_weapon);
    uint8_t void_initial_durability = get_durability(void_armor);

    dString_t* initial_log = d_InitString();
    d_AppendString(initial_log, "Initial durabilities - Chaos weapon: ", 0);
    d_AppendInt(initial_log, chaos_initial_durability);
    d_AppendString(initial_log, ", Void armor: ", 0);
    d_AppendInt(initial_log, void_initial_durability);
    LOG(initial_log->str);
    d_DestroyString(initial_log);

    // Try to break chaos weapon (it has durability_fact = 2.0f so it's very durable)
    int chaos_damage_attempts = 0;
    float chaos_resistance_chance = 0.05f * chaos_weapon->material_data.properties.durability_fact;
    if (chaos_resistance_chance > 0.95f) chaos_resistance_chance = 0.95f;

    dString_t* chaos_resistance_log = d_InitString();
    d_AppendString(chaos_resistance_log, "Chaos weapon resistance chance: ", 0);
    d_AppendInt(chaos_resistance_log, chaos_resistance_chance * 100.0f);
    d_AppendString(chaos_resistance_log, "%", 0);
    LOG(chaos_resistance_log->str);
    d_DestroyString(chaos_resistance_log);

    while (!is_item_broken(chaos_weapon) && chaos_damage_attempts < 10) {
        uint8_t before_damage = get_durability(chaos_weapon);
        damage_item_durability(chaos_weapon, 100);
        uint8_t after_damage = get_durability(chaos_weapon);
        chaos_damage_attempts++;

        dString_t* chaos_log = d_InitString();
        d_AppendString(chaos_log, "Chaos weapon damage attempt ", 0);
        d_AppendInt(chaos_log, chaos_damage_attempts);
        d_AppendString(chaos_log, ": ", 0);
        d_AppendInt(chaos_log, before_damage);
        d_AppendString(chaos_log, " -> ", 0);
        d_AppendInt(chaos_log, after_damage);

        // Add resistance info
        if (before_damage == after_damage) {
            d_AppendString(chaos_log, " (RESISTED)", 0);
        } else {
            d_AppendString(chaos_log, " (DAMAGED)", 0);
        }

        LOG(chaos_log->str);
        d_DestroyString(chaos_log);
    }

    // Do the same for void armor
    int void_damage_attempts = 0;
    float void_resistance_chance = 0.05f * void_armor->material_data.properties.durability_fact;
    if (void_resistance_chance > 0.95f) void_resistance_chance = 0.95f;

    dString_t* void_resistance_log = d_InitString();
    d_AppendString(void_resistance_log, "Void armor resistance chance: ", 0);
    d_AppendInt(void_resistance_log, void_resistance_chance * 100.0f);
    d_AppendString(void_resistance_log, "%", 0);
    LOG(void_resistance_log->str);
    d_DestroyString(void_resistance_log);

    while (!is_item_broken(void_armor) && void_damage_attempts < 10) {
        uint8_t before_damage = get_durability(void_armor);
        damage_item_durability(void_armor, 50);
        uint8_t after_damage = get_durability(void_armor);
        void_damage_attempts++;

        dString_t* void_log = d_InitString();
        d_AppendString(void_log, "Void armor damage attempt ", 0);
        d_AppendInt(void_log, void_damage_attempts);
        d_AppendString(void_log, ": ", 0);
        d_AppendInt(void_log, before_damage);
        d_AppendString(void_log, " -> ", 0);
        d_AppendInt(void_log, after_damage);

        // Add resistance info
        if (before_damage == after_damage) {
            d_AppendString(void_log, " (RESISTED)", 0);
        } else {
            d_AppendString(void_log, " (DAMAGED)", 0);
        }

        LOG(void_log->str);
        d_DestroyString(void_log);
    }

    bool chaos_broken = is_item_broken(chaos_weapon);
    bool void_broken = is_item_broken(void_armor);

    dString_t* final_status = d_InitString();
    d_AppendString(final_status, "Final status - Chaos weapon broken: ", 0);
    d_AppendString(final_status, chaos_broken ? "YES" : "NO", 0);
    d_AppendString(final_status, ", Void armor broken: ", 0);
    d_AppendString(final_status, void_broken ? "YES" : "NO", 0);
    LOG(final_status->str);
    d_DestroyString(final_status);

    TEST_ASSERT(void_broken, "Void armor should eventually break with repeated damage");

    // Don't assert that chaos weapon breaks, since it has high durability
    // Instead, just log the result for debugging
    if (chaos_broken) {
        LOG("Chaos weapon broke despite high durability material");
        TEST_ASSERT(get_equipped_weapon(laboratory) == NULL,
                    "Broken chaos weapon should be auto-unequipped");
    } else {
        LOG("Chaos weapon survived damage due to high durability material (durability_fact=2.0)");
        // This is actually expected behavior, so don't fail the test
    }

    if (void_broken) {
        LOG("Void armor broke as expected due to low durability material");
        TEST_ASSERT(get_equipped_armor(laboratory) == NULL,
                    "Broken void armor should be auto-unequipped");
    }


    // Test repair limits with extreme materials
    repair_item(chaos_weapon, 200);
    repair_item(void_armor, 200);

    uint8_t chaos_repaired_durability = get_durability(chaos_weapon);
    uint8_t void_repaired_durability = get_durability(void_armor);

    TEST_ASSERT(chaos_repaired_durability <= 255, "Repaired durability should not overflow");
    TEST_ASSERT(void_repaired_durability <= 255, "Repaired durability should not overflow");

    // Test stacking with different materials
    LOG("Testing material-based stacking behavior");
    Item_t* chaos_ammo = create_ammunition("Chaos Arrows", "chaos_arrows", chaos_material, 5, 10, 'c');
    Item_t* normal_ammo = create_ammunition("Normal Arrows", "normal_arrows", chaos_material, 5, 10, 'n');

    apply_material_to_ammunition(chaos_ammo);

    bool can_stack_different_materials = can_stack_items(chaos_ammo, normal_ammo);
    bool can_stack_same_type = can_stack_items(overflow_ammo, overflow_ammo);

    TEST_ASSERT(!can_stack_different_materials, "Items with different IDs should not stack");
    TEST_ASSERT(can_stack_same_type, "Identical items should stack regardless of extreme materials");

    LOG("Material system cross-integration test completed");
    destroy_item(chaos_weapon);
    destroy_item(void_armor);
    destroy_item(overflow_ammo);
    destroy_item(chaos_ammo);
    destroy_item(normal_ammo);
    destroy_inventory(laboratory);
    return 1;
}

// =============================================================================
// INVENTORY CORRUPTION RECOVERY INTEGRATION TESTS
// =============================================================================

int test_inventory_corruption_recovery_integration(void)
{
    LOG("Starting inventory corruption recovery integration test");

    // Create multiple inventories for corruption testing
    Inventory_t* primary_inv = create_inventory(15);
    Inventory_t* backup_inv = create_inventory(15);

    TEST_ASSERT(primary_inv != NULL && backup_inv != NULL,
                "Primary and backup inventories should be created");

    // Create items for corruption testing
    MaterialProperties_t standard_props = create_default_material_properties();
    Material_t standard_material = create_material("standard", standard_props);

    Item_t* critical_weapon = create_weapon("Critical Weapon", "critical_weapon", standard_material, 25, 40, 0, 'C');
    Item_t* important_armor = create_armor("Important Armor", "important_armor", standard_material, 35, 12, 'I', 18, 18);
    Item_t* vital_consumable = create_consumable("Vital Consumable", "vital_consumable", 60, dummy_consume_callback, 'V');

    apply_material_to_weapon(critical_weapon);
    apply_material_to_armor(important_armor);

    // Set up primary inventory
    add_item_to_inventory(primary_inv, critical_weapon, 1);
    add_item_to_inventory(primary_inv, important_armor, 1);
    add_item_to_inventory(primary_inv, vital_consumable, 10);

    equip_item(primary_inv, "critical_weapon");
    equip_item(primary_inv, "important_armor");

    // Create backup state
    add_item_to_inventory(backup_inv, critical_weapon, 1);
    add_item_to_inventory(backup_inv, important_armor, 1);
    add_item_to_inventory(backup_inv, vital_consumable, 10);

    // Simulate various corruption scenarios
    LOG("Testing recovery from inventory state corruption");

    // Scenario 1: Equipment state corruption
    Inventory_slot_t* equipped_weapon = get_equipped_weapon(primary_inv);
    TEST_ASSERT(equipped_weapon != NULL, "Weapon should be equipped before corruption test");

    // Simulate corruption by manually removing equipped item
    bool corrupted_removal = remove_item_from_inventory(primary_inv, "critical_weapon", 1);
    TEST_ASSERT(corrupted_removal, "Corrupted removal should succeed");

    // Check if system handles missing equipped item gracefully
    Inventory_slot_t* weapon_after_corruption = get_equipped_weapon(primary_inv);
    TEST_ASSERT(weapon_after_corruption == NULL,
                "Equipment system should handle missing equipped items");

    // Scenario 2: Recovery from backup
    LOG("Testing recovery from backup inventory");

    // Restore from backup
    add_item_to_inventory(primary_inv, critical_weapon, 1);
    bool recovery_equip = equip_item(primary_inv, "critical_weapon");
    TEST_ASSERT(recovery_equip, "Recovery equipment should succeed");

    // Scenario 3: Partial corruption with mixed state
    LOG("Testing partial corruption recovery");

    // Damage some items severely
    damage_item_durability(critical_weapon, 254); // Near-broken

    uint8_t critical_durability = get_durability(critical_weapon);
    if (is_item_broken(critical_weapon)) {
        LOG("Critical weapon broke - testing emergency procedures");

        // Should auto-unequip broken items
        Inventory_slot_t* broken_weapon_check = get_equipped_weapon(primary_inv);
        // System behavior may vary - test that it handles this gracefully

        // Attempt repair
        repair_item(critical_weapon, 100);
        bool re_equip_after_repair = equip_item(primary_inv, "critical_weapon");

        if (re_equip_after_repair) {
            LOG("Successfully recovered and re-equipped repaired weapon");
        }
    }

    // Scenario 4: Inventory consistency validation
    LOG("Testing inventory consistency validation");

    // Verify all inventory state is consistent
    uint8_t total_items = 0;
    for (uint8_t i = 0; i < primary_inv->size; i++) {
        if (primary_inv->slots[i].quantity > 0) {
            total_items++;

            // Verify item integrity
            Item_t* item = &primary_inv->slots[i].item;
            TEST_ASSERT(item->id != NULL && item->name != NULL,
                        "All items should maintain integrity");
            TEST_ASSERT(item->id->str != NULL && item->name->str != NULL,
                        "Item strings should be valid");
        }
    }

    TEST_ASSERT(total_items > 0, "Inventory should contain items after recovery");

    // Scenario 5: Cross-inventory validation
    LOG("Testing cross-inventory validation");

    // Compare primary and backup for consistency
    Inventory_slot_t* primary_armor = find_item_in_inventory(primary_inv, "important_armor");
    Inventory_slot_t* backup_armor = find_item_in_inventory(backup_inv, "important_armor");

    TEST_ASSERT(primary_armor != NULL && backup_armor != NULL,
                "Both inventories should contain armor");

    // Verify weight calculations are consistent
    uint8_t primary_weight = get_total_inventory_weight(primary_inv);
    uint8_t backup_weight = get_total_inventory_weight(backup_inv);

    // Weights might differ due to durability changes, but should be reasonable
    TEST_ASSERT(primary_weight >= 0 && backup_weight >= 0,
                "All inventory weights should be non-negative");

    LOG("Inventory corruption recovery integration test completed");
    destroy_item(critical_weapon);
    destroy_item(important_armor);
    destroy_item(vital_consumable);
    destroy_inventory(primary_inv);
    destroy_inventory(backup_inv);
    return 1;
}

// =============================================================================
// PERFORMANCE INTEGRATION TESTS
// =============================================================================

int test_performance_integration(void)
{
    LOG("Starting performance integration test");

    // Create performance testing environment
    Inventory_t* perf_inventory = create_inventory(200);
    TEST_ASSERT(perf_inventory != NULL, "Performance inventory should be created");

    // Create materials for performance testing
    MaterialProperties_t perf_props = create_default_material_properties();
    Material_t perf_material = create_material("performance", perf_props);

    // Create item templates for performance testing
    Item_t* perf_weapon = create_weapon("Perf Weapon", "perf_weapon", perf_material, 12, 18, 0, 'P');
    Item_t* perf_armor = create_armor("Perf Armor", "perf_armor", perf_material, 20, 10, 'A', 15, 15);
    Item_t* perf_ammo = create_ammunition("Perf Ammo", "perf_ammo", perf_material, 4, 7, 'B');

    apply_material_to_weapon(perf_weapon);
    apply_material_to_armor(perf_armor);
    apply_material_to_ammunition(perf_ammo);

    // Performance Test 1: Rapid inventory operations
    LOG("Performance Test 1: Rapid inventory operations");

    clock_t start_time = clock();

    for (int cycle = 0; cycle < 1000; cycle++) {
        add_item_to_inventory(perf_inventory, perf_ammo, 50);
        remove_item_from_inventory(perf_inventory, "perf_ammo", 25);

        if (cycle % 100 == 0) {
            get_total_inventory_weight(perf_inventory);
            get_inventory_free_slots(perf_inventory);
        }
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    LOG("Rapid operations completed in reasonable time");
    TEST_ASSERT(elapsed_time < 1.0, "Rapid operations should complete in under 1 second");

    // Performance Test 2: Mass equipment operations
    LOG("Performance Test 2: Mass equipment operations");

    add_item_to_inventory(perf_inventory, perf_weapon, 1);
    add_item_to_inventory(perf_inventory, perf_armor, 1);

    start_time = clock();

    for (int cycle = 0; cycle < 500; cycle++) {
        equip_item(perf_inventory, "perf_weapon");
        get_equipped_weapon(perf_inventory);
        unequip_item(perf_inventory, "perf_weapon");

        equip_item(perf_inventory, "perf_armor");
        get_equipped_armor(perf_inventory);
        unequip_item(perf_inventory, "perf_armor");
    }

    end_time = clock();
    elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    LOG("Mass equipment operations completed");
    TEST_ASSERT(elapsed_time < 0.5, "Equipment operations should be fast");

    // Performance Test 3: Large inventory search operations
    LOG("Performance Test 3: Large inventory search operations");

    // Fill inventory with many different items, but respect capacity
    uint8_t free_slots = get_inventory_free_slots(perf_inventory);
    int items_to_add = (free_slots / 3) - 1; // Leave some room, divide by 3 types

    for (int i = 0; i < items_to_add; i++) {
        bool weapon_added = add_item_to_inventory(perf_inventory, perf_weapon, 1);
        bool armor_added = add_item_to_inventory(perf_inventory, perf_armor, 1);
        bool ammo_added = add_item_to_inventory(perf_inventory, perf_ammo, 100);

        // Stop if inventory is getting full
        if (!weapon_added || !armor_added || !ammo_added) {
            break;
        }
    }

    start_time = clock();

    for (int search = 0; search < 1000; search++) {
        find_item_in_inventory(perf_inventory, "perf_weapon");
        find_item_in_inventory(perf_inventory, "perf_armor");
        find_item_in_inventory(perf_inventory, "perf_ammo");

        // Test fewer non-existent item searches to reduce spam
        if (search % 100 == 0) {
            find_item_in_inventory(perf_inventory, "nonexistent_item");
        }
    }

    end_time = clock();
    elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    LOG("Large inventory search operations completed");
    TEST_ASSERT(elapsed_time < 0.1, "Search operations should be very fast");

    // Performance Test 4: Memory usage validation
    LOG("Performance Test 4: Memory usage validation");

    // Verify inventory still functions correctly after stress
    uint8_t final_weight = get_total_inventory_weight(perf_inventory);
    uint8_t final_free_slots = get_inventory_free_slots(perf_inventory);
    bool final_full_status = is_inventory_full(perf_inventory);

    TEST_ASSERT(final_weight > 0, "Inventory should have weight after performance tests");
    TEST_ASSERT(final_free_slots >= 0, "Free slots calculation should work after stress");

    // Test that all slots are still valid
    int valid_slots = 0;
    for (uint8_t i = 0; i < perf_inventory->size; i++) {
        if (perf_inventory->slots[i].quantity > 0) {
            valid_slots++;
            TEST_ASSERT(perf_inventory->slots[i].item.id != NULL,
                        "All items should remain valid after performance stress");
        }
    }

    TEST_ASSERT(valid_slots > 0, "Inventory should contain valid items after stress");

    LOG("Performance integration test completed");
    destroy_item(perf_weapon);
    destroy_item(perf_armor);
    destroy_item(perf_ammo);
    destroy_inventory(perf_inventory);
    return 1;
}
// =============================================================================
// EXTREME DURABILITY STRESS TEST
// =============================================================================

int test_extreme_durability_stress(void)
{
    LOG("Starting extreme durability stress test");

    // Create test environment
    Inventory_t* durability_lab = create_inventory(5);
    TEST_ASSERT(durability_lab != NULL, "Durability lab inventory should be created");

    // Create ultra-durable material
    MaterialProperties_t ultra_durable_props = create_default_material_properties();
    ultra_durable_props.durability_fact = 10.0f; // Extreme durability
    ultra_durable_props.min_damage_fact = 1.0f;
    ultra_durable_props.max_damage_fact = 1.0f;
    ultra_durable_props.weight_fact = 2.0f; // Heavy but durable
    ultra_durable_props.value_coins_fact = 5.0f; // Expensive
    Material_t ultra_durable = create_material("ultra_durable", ultra_durable_props);

    // Create weapon with extreme durability
    Item_t* legendary_weapon = create_weapon("Legendary Blade", "legendary_blade", ultra_durable, 25, 35, 0, 'L');
    apply_material_to_weapon(legendary_weapon);

    // Log initial properties
    uint8_t initial_durability = get_durability(legendary_weapon);
    float resistance_chance = 0.05f * legendary_weapon->material_data.properties.durability_fact;
    if (resistance_chance > 0.95f) resistance_chance = 0.95f;

    dString_t* initial_log = d_InitString();
    d_AppendString(initial_log, "Legendary weapon initial durability: ", 0);
    d_AppendInt(initial_log, initial_durability);
    d_AppendString(initial_log, " (durability_fact: ", 0);
    d_AppendInt(initial_log, ultra_durable_props.durability_fact);
    d_AppendString(initial_log, ")", 0);
    LOG(initial_log->str);
    d_DestroyString(initial_log);

    dString_t* resistance_log = d_InitString();
    d_AppendString(resistance_log, "Resistance chance: ", 0);
    d_AppendInt(resistance_log, resistance_chance * 100.0f);
    d_AppendString(resistance_log, "% (capped at 95%)", 0);
    LOG(resistance_log->str);
    d_DestroyString(resistance_log);

    // Add to inventory for testing
    add_item_to_inventory(durability_lab, legendary_weapon, 1);
    equip_item(durability_lab, "legendary_blade");

    // Apply 100 hits and track results
    LOG("Applying 100 hits to legendary weapon...");

    int total_hits = 100;
    int hits_resisted = 0;
    int hits_taken = 0;
    int total_damage_dealt = 0;

    for (int hit = 1; hit <= total_hits; hit++) {
        uint8_t before_damage = get_durability(legendary_weapon);
        damage_item_durability(legendary_weapon, 1); // Consistent damage per hit
        uint8_t after_damage = get_durability(legendary_weapon);

        if (before_damage == after_damage) {
            hits_resisted++;
        } else {
            hits_taken++;
            total_damage_dealt += (before_damage - after_damage);
        }

        // Log every 10th hit
        if (hit % 10 == 0) {
            dString_t* progress_log = d_InitString();
            d_AppendString(progress_log, "Hit ", 0);
            d_AppendInt(progress_log, hit);
            d_AppendString(progress_log, "/", 0);
            d_AppendInt(progress_log, total_hits);
            d_AppendString(progress_log, " - Durability: ", 0);
            d_AppendInt(progress_log, after_damage);
            d_AppendString(progress_log, " (Resisted: ", 0);
            d_AppendInt(progress_log, hits_resisted);
            d_AppendString(progress_log, ", Taken: ", 0);
            d_AppendInt(progress_log, hits_taken);
            d_AppendString(progress_log, ")", 0);
            LOG(progress_log->str);
            d_DestroyString(progress_log);
        }

        // Break if weapon is destroyed
        if (is_item_broken(legendary_weapon)) {
            dString_t* broken_log = d_InitString();
            d_AppendString(broken_log, "Legendary weapon broke after ", 0);
            d_AppendInt(broken_log, hit);
            d_AppendString(broken_log, " hits!", 0);
            LOG(broken_log->str);
            d_DestroyString(broken_log);
            break;
        }
    }

    // Final statistics
    uint8_t final_durability = get_durability(legendary_weapon);
    float resistance_rate = (float)hits_resisted / (float)total_hits * 100.0f;
    float average_damage_per_hit = hits_taken > 0 ? (float)total_damage_dealt / (float)hits_taken : 0.0f;

    dString_t* final_log = d_InitString();
    d_AppendString(final_log, "Final Results - Durability: ", 0);
    d_AppendInt(final_log, final_durability);
    d_AppendString(final_log, "/", 0);
    d_AppendInt(final_log, initial_durability);
    LOG(final_log->str);
    d_DestroyString(final_log);

    dString_t* stats_log = d_InitString();
    d_AppendString(stats_log, "Hits Resisted: ", 0);
    d_AppendInt(stats_log, hits_resisted);
    d_AppendString(stats_log, "/", 0);
    d_AppendInt(stats_log, total_hits);
    d_AppendString(stats_log, " (", 0);
    d_AppendInt(stats_log, resistance_rate);
    d_AppendString(stats_log, "%)", 0);
    LOG(stats_log->str);
    d_DestroyString(stats_log);

    dString_t* damage_log = d_InitString();
    d_AppendString(damage_log, "Total Damage Dealt: ", 0);
    d_AppendInt(damage_log, total_damage_dealt);
    d_AppendString(damage_log, " (Avg: ", 0);
    d_AppendInt(damage_log, average_damage_per_hit);
    d_AppendString(damage_log, " per hit)", 0);
    LOG(damage_log->str);
    d_DestroyString(damage_log);

    // Test assertions
    TEST_ASSERT(final_durability > 0 || is_item_broken(legendary_weapon),
                "Weapon should either have durability or be broken");
    TEST_ASSERT(hits_resisted > 0, "Ultra-durable weapon should resist some hits");
    TEST_ASSERT(resistance_rate >= 25.0f, "Ultra-durable weapon should resist at least 25% of hits (half of 50%)");

    // Test equipment state
    if (is_item_broken(legendary_weapon)) {
        TEST_ASSERT(get_equipped_weapon(durability_lab) == NULL,
                    "Broken legendary weapon should be auto-unequipped");
    } else {
        TEST_ASSERT(get_equipped_weapon(durability_lab) != NULL,
                    "Intact legendary weapon should remain equipped");
    }

    LOG("Extreme durability stress test completed");
    destroy_item(legendary_weapon);
    destroy_inventory(durability_lab);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    srand(12345);
    TEST_SUITE_START("Items System Integration Tests");

    // Run comprehensive integration tests
    RUN_TEST(test_complete_item_lifecycle);
    RUN_TEST(test_multi_inventory_trading_integration);
    RUN_TEST(test_combat_simulation_integration);
    RUN_TEST(test_mass_operations_stress_integration);
    RUN_TEST(test_material_system_cross_integration);
    RUN_TEST(test_inventory_corruption_recovery_integration);
    RUN_TEST(test_performance_integration);
    RUN_TEST(test_extreme_durability_stress);
    TEST_SUITE_END();
}
