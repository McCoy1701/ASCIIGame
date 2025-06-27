// ASCIIGame/tests/items/test_items_integration_tests.c
// Integration tests for items system, refactored to the highest standard.

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Helper function for float comparison
static bool float_equals(float a, float b, float tolerance)
{
    return fabs(a - b) < tolerance;
}

// Dummy callback for creating consumables
static void dummy_consume_callback(uint8_t value)
{
    (void)value; // Suppress unused parameter warning
}

// =============================================================================
// ITEM LIFECYCLE INTEGRATION TESTS
// =============================================================================

int test_complete_item_lifecycle(void)
{
    d_LogInfo("Starting complete item lifecycle integration test...");

    // Create materials with different properties
    MaterialProperties_t steel_props = create_default_material_properties();
    steel_props.durability_fact = 1.2f;
    steel_props.weight_fact = 1.3f;
    Material_t steel_material = create_material("steel", steel_props);

    // Phase 1: Item Creation and Material Application
    d_LogDebug("Phase 1: Creating items and applying materials.");
    Item_t* sword = create_weapon("Iron Sword", "iron_sword", steel_material, 15, 25, 0, 'I');
    TEST_ASSERT(sword != NULL, "Item should be created successfully");
    if(!sword) return 0;

    uint8_t pre_mod_min_dmg = get_weapon_min_damage(sword);
    apply_material_to_weapon(sword);
    uint8_t post_mod_min_dmg = get_weapon_min_damage(sword);
    TEST_ASSERT(post_mod_min_dmg >= pre_mod_min_dmg, "Material should enhance sword damage");

    // Phase 2: Inventory Management Integration
    d_LogDebug("Phase 2: Adding item to inventory.");
    Inventory_t* player_inventory = create_inventory(20);
    TEST_ASSERT(player_inventory != NULL, "Player inventory should be created");
    bool sword_added = add_item_to_inventory(player_inventory, sword, 1);
    TEST_ASSERT(sword_added, "Item should be added to inventory successfully");
    TEST_ASSERT(get_inventory_free_slots(player_inventory) == 19, "Should have 19 free slots after adding 1 item");

    // Phase 3: Equipment System Integration
    d_LogDebug("Phase 3: Equipment and combat stat integration.");
    bool sword_equipped = equip_item(player_inventory, "iron_sword");
    TEST_ASSERT(sword_equipped, "Item should be equipped successfully");
    Inventory_slot_t* equipped_weapon = get_equipped_weapon(player_inventory);
    TEST_ASSERT(equipped_weapon != NULL, "Equipment should be accessible after equipping");
    TEST_ASSERT(get_weapon_min_damage(&equipped_weapon->item) == post_mod_min_dmg, "Equipped weapon damage should match item damage");

    // Phase 4: Durability and Usage Integration
    d_LogDebug("Phase 4: Durability system integration.");
    uint8_t initial_sword_durability = get_durability(sword);
    damage_item_durability(sword, 50);
    uint8_t damaged_sword_durability = get_durability(sword);
    TEST_ASSERT(damaged_sword_durability < initial_sword_durability, "Sword durability should decrease after damage");
    TEST_ASSERT(!is_item_broken(sword), "Damaged sword should not be broken");

    // Phase 5: Repair and Maintenance Integration
    d_LogDebug("Phase 5: Repair system integration.");
    repair_item(sword, 25);
    TEST_ASSERT(get_durability(sword) > damaged_sword_durability, "Repaired sword should have higher durability");

    // Phase 6: Item Removal and Cleanup
    d_LogDebug("Phase 6: Item removal and state consistency.");
    bool sword_removed = remove_item_from_inventory(player_inventory, "iron_sword", 1);
    TEST_ASSERT(sword_removed, "Sword should be removed from inventory");
    TEST_ASSERT(get_equipped_weapon(player_inventory) == NULL, "No weapon should be equipped after removal");

    d_LogInfo("...complete item lifecycle integration test finished.");
    destroy_item(sword);
    destroy_inventory(player_inventory);
    return 1;
}

// =============================================================================
// COMBAT SIMULATION INTEGRATION TESTS
// =============================================================================

int test_combat_simulation_integration(void)
{
    d_LogInfo("Starting combat simulation integration test...");

    // Create combat participants' inventories
    Inventory_t* warrior_inv = create_inventory(12);
    Inventory_t* archer_inv = create_inventory(8);
    TEST_ASSERT(warrior_inv && archer_inv, "Combat inventories should be created");

    // Create materials
    Material_t battle_worn = create_material("battle_worn", (MaterialProperties_t){.durability_fact=0.7f});
    Material_t masterwork = create_material("masterwork", (MaterialProperties_t){.durability_fact=2.0f, .armor_value_fact=1.5f});

    // Create and equip gear for warrior
    Item_t* warrior_armor = create_armor("Plate Armor", "plate_armor", masterwork, 50, 8, 'P', 25, 25);
    apply_material_to_armor(warrior_armor);
    add_item_to_inventory(warrior_inv, warrior_armor, 1);
    equip_item(warrior_inv, "plate_armor");

    // Create and equip gear for archer
    Item_t* archer_bow = create_weapon("War Bow", "war_bow", battle_worn, 12, 18, 3, 'W');
    apply_material_to_weapon(archer_bow);
    add_item_to_inventory(archer_inv, archer_bow, 1);
    equip_item(archer_inv, "war_bow");

    d_LogDebug("Simulating combat rounds with equipment degradation...");
    uint8_t initial_armor_dur = get_durability(warrior_armor);
    uint8_t initial_bow_dur = get_durability(archer_bow);

    for (int round = 1; round <= 5; round++) {
        d_LogInfoF("Combat Round %d", round);
        damage_item_durability(warrior_armor, 10);
        damage_item_durability(archer_bow, 20);
    }

    TEST_ASSERT(get_durability(warrior_armor) < initial_armor_dur, "Warrior armor should show wear");
    TEST_ASSERT(get_durability(archer_bow) < initial_bow_dur, "Archer bow should show wear");

    d_LogDebug("Testing field repairs post-combat...");
    uint8_t damaged_armor_dur = get_durability(warrior_armor);
    repair_item(warrior_armor, 50);
    TEST_ASSERT(get_durability(warrior_armor) > damaged_armor_dur, "Field repairs on armor should succeed");

    d_LogInfo("...combat simulation integration test completed.");
    destroy_item(warrior_armor);
    destroy_item(archer_bow);
    destroy_inventory(warrior_inv);
    destroy_inventory(archer_inv);
    return 1;
}

// =============================================================================
// MASS OPERATIONS STRESS INTEGRATION TESTS
// =============================================================================

int test_mass_operations_stress_integration(void)
{
    d_LogWarning("Starting mass operations stress integration test...");
    Inventory_t* warehouse = create_inventory(150);
    TEST_ASSERT(warehouse != NULL, "Warehouse inventory should be created");

    Material_t mass_material = create_material("mass_produced", create_default_material_properties());
    Item_t* template_ammo = create_ammunition("Mass Ammo", "mass_ammo", mass_material, 3, 5, 'B');

    // Phase 1: Mass item addition
    d_LogDebug("Phase 1: Mass item addition stress test.");
    LOOP_TEST_START();
    for (int i = 0; i < 100; i++) {
        bool added = add_item_to_inventory(warehouse, template_ammo, 200);
        if (i < 3) {
            TEST_ASSERT(added, "Initial mass additions should succeed");
        }
    }
    LOOP_TEST_END();
    TEST_ASSERT(get_inventory_free_slots(warehouse) < 150, "Warehouse should be filled with items");

    // Phase 2: Mass removal operations
    d_LogDebug("Phase 2: Mass removal stress test.");
    int initial_free_slots = get_inventory_free_slots(warehouse);
    LOOP_TEST_START();
    for (int i = 0; i < 50; i++) {
        bool removed = remove_item_from_inventory(warehouse, "mass_ammo", 50);
         if (i < 3) {
            TEST_ASSERT(removed, "Initial mass removals should succeed");
        }
    }
    LOOP_TEST_END();
    TEST_ASSERT(get_inventory_free_slots(warehouse) > initial_free_slots, "Should have more free slots after removals");

    // Phase 3: Final state verification
    d_LogInfo("Phase 3: Final warehouse state verification.");
    Inventory_slot_t* final_ammo = find_item_in_inventory(warehouse, "mass_ammo");
    TEST_ASSERT(final_ammo != NULL, "Some ammo should remain in warehouse");
    TEST_ASSERT(final_ammo->quantity > 0, "Remaining ammo should have a valid quantity");

    d_LogInfo("...mass operations stress integration test completed.");
    destroy_item(template_ammo);
    destroy_inventory(warehouse);
    return 1;
}

// =============================================================================
// BUG HUNTING INTEGRATION TESTS
// =============================================================================

int test_material_system_cross_integration(void)
{
    d_LogWarning("BUG HUNT: Testing extreme material property interactions.");

    MaterialProperties_t chaos_props = create_default_material_properties();
    chaos_props.min_damage_fact = 3.0f;
    chaos_props.max_damage_fact = 0.5f; // Intentionally inverted
    chaos_props.weight_fact = -0.1f;    // Negative weight
    Material_t chaos_material = create_material("chaos", chaos_props);
    Item_t* chaos_weapon = create_weapon("Chaos Blade", "chaos_blade", chaos_material, 30, 50, 1, 'C');

    apply_material_to_weapon(chaos_weapon);

    TEST_ASSERT(get_weapon_min_damage(chaos_weapon) >= get_weapon_max_damage(chaos_weapon), "System should gracefully handle inverted damage factors");
    TEST_ASSERT(get_item_weight(chaos_weapon) >= 0.0f, "Item weight should be clamped to non-negative");

    destroy_item(chaos_weapon);
    return 1;
}

int test_inventory_corruption_recovery_integration(void)
{
    d_LogWarning("BUG HUNT: Testing recovery from simulated inventory state corruption.");
    Inventory_t* p_inv = create_inventory(10);
    Material_t mat = create_material("standard", create_default_material_properties());
    Item_t* weapon = create_weapon("Critical Weapon", "crit_wep", mat, 25, 40, 0, 'C');

    add_item_to_inventory(p_inv, weapon, 1);
    equip_item(p_inv, "crit_wep");
    TEST_ASSERT(get_equipped_weapon(p_inv) != NULL, "Weapon should be equipped initially");

    d_LogDebug("Simulating corruption by manually emptying the equipped slot...");
    Inventory_slot_t* weapon_slot = find_item_in_inventory(p_inv, "crit_wep");
    if(weapon_slot) {
        weapon_slot->quantity = 0; // Manually empty the slot without unequipping
    }

    TEST_ASSERT(get_equipped_weapon(p_inv) == NULL, "get_equipped_weapon should not return an empty slot");

    destroy_item(weapon);
    destroy_inventory(p_inv);
    return 1;
}

int test_extreme_durability_stress(void)
{
    d_LogWarning("BUG HUNT: Stress testing an item with extreme durability.");
    MaterialProperties_t ultra_props = create_default_material_properties();
    ultra_props.durability_fact = 10.0f; // Very high durability factor
    Material_t ultra_durable = create_material("ultra_durable", ultra_props);
    Item_t* legendary_weapon = create_weapon("Legendary Blade", "leg_blade", ultra_durable, 25, 35, 0, 'L');

    int hits_resisted = 0;
    int total_hits = 100;

    d_LogDebugF("Applying %d hits to legendary weapon...", total_hits);
    for (int hit = 0; hit < total_hits; hit++) {
        uint8_t before_damage = get_durability(legendary_weapon);
        damage_item_durability(legendary_weapon, 5); // Apply small damage each time
        if (get_durability(legendary_weapon) == before_damage) {
            hits_resisted++;
        }
    }

    d_LogInfoF("Durability resistance results: %d hits resisted out of %d.", hits_resisted, total_hits);
    TEST_ASSERT(hits_resisted > 0, "Ultra-durable weapon should resist some hits");
    TEST_ASSERT(!is_item_broken(legendary_weapon), "Ultra-durable weapon should not break from minor stress");

    destroy_item(legendary_weapon);
    return 1;
}


// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger
    dLogConfig_t config = { .default_level = D_LOG_LEVEL_INFO };
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);

    srand(1233445);
    TEST_SUITE_START("Items System Integration Tests");

    // Run comprehensive integration tests
    RUN_TEST(test_complete_item_lifecycle);
    RUN_TEST(test_combat_simulation_integration);
    RUN_TEST(test_mass_operations_stress_integration);

    // Run bug-hunting tests
    RUN_TEST(test_material_system_cross_integration);
    RUN_TEST(test_inventory_corruption_recovery_integration);
    RUN_TEST(test_extreme_durability_stress);

    // Clean up the logger before finishing
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}
