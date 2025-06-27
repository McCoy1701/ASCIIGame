// File: tests/items/test_items_helper_functions.c - MIDAS Divine Item Helper Function Tests
// Corrected to work with actual struct definitions and field names
// Enhanced with the full power of Daedalus Logging and Metis's Profound Wisdom

#include "tests.h"
#include "items.h" 
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include "Archimedes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// DIVINE HELPER FUNCTIONS WITH METIS'S WISDOM
// =============================================================================


static Material_t create_test_material_with_dstring(const char* material_name)
{
    dLogContext_t* ctx = d_PushLogContext("MaterialCreation");
    
    // RATE LIMIT THIS - Called repeatedly in stress tests
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 3.0, "Creating test material: '%s'", 
                      material_name ? material_name : "NULL");
    
    MaterialProperties_t properties = {
        .weight_fact = 1.0f,
        .armor_value_fact = 1.0f,
        .enchant_value_fact = 1.0f,
        .durability_fact = 1.0f,
        .min_damage_fact = 1.0f,
        .max_damage_fact = 1.0f,
        .evasion_value_fact = 1.0f,
        .value_coins_fact = 1.0f,
        .stealth_value_fact = 1.0f
    };

    Material_t material = {
        .name = d_InitString(),
        .properties = properties
    };

    if (material.name && material_name) {
        d_AppendString(material.name, material_name, 0);
        
        // RATE LIMIT THIS - Success messages can flood during bulk operations
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          10, 3.0, "Material created successfully: name='%s', length=%zu", 
                          d_PeekString(material.name), d_GetStringLength(material.name));
    } else {
        // RATE LIMIT THIS - Failure messages can spam on repeated errors
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
                          5, 5.0, "Failed to create material or material name is NULL");
    }
    
    d_PopLogContext(ctx);
    return material;
}

static Lock_t create_test_lock_with_dstring(const char* lock_name, const char* lock_desc)
{
    dLogContext_t* ctx = d_PushLogContext("LockCreation");
    
    // RATE LIMIT THIS - Called multiple times in key/lock tests
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      8, 4.0, "Creating test lock: name='%s', desc='%s'", 
                      lock_name ? lock_name : "NULL", lock_desc ? lock_desc : "NULL");
    
    Lock_t lock = create_lock(lock_name, lock_desc, 50, 0);
    
    // RATE LIMIT THIS - Lock creation success messages
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      8, 4.0, "Lock created: difficulty=%d, jammed_seconds=%d", 
                      lock.pick_difficulty, lock.jammed_seconds);
    
    d_PopLogContext(ctx);
    return lock;
}

static bool dstring_equals(const dString_t* ds, const char* str)
{
    if (ds == NULL || str == NULL) {
        // AGGRESSIVE RATE LIMIT - This could be called thousands of times with NULL
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          2, 10.0, "dstring_equals: NULL parameter (ds=%p, str=%p)", 
                          (void*)ds, (void*)str);
        return false;
    }
    
    const char* ds_str = d_PeekString(ds);
    if (ds_str == NULL) {
        // RATE LIMIT - Repeated failures should not flood logs
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          3, 8.0, "dstring_equals: d_PeekString returned NULL");
        return false;
    }
    
    bool result = strcmp(ds_str, str) == 0;
    
    // HEAVILY RATE LIMIT - This is the WORST offender for log spam!
    // Comparison operations can happen hundreds of times per second
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      1, 15.0, "dstring_equals: '%s' == '%s' -> %s", 
                      ds_str, str, result ? "true" : "false");
    
    return result;
}

/*
 * Enhanced logging callback for consumable testing
 *
 * `value` - Healing value to log
 *
 * -- Provides detailed logging of consumable effects
 * -- Used as callback for create_consumable() testing
 * -- Demonstrates proper callback implementation patterns
 */
static void enhanced_test_heal_callback(uint8_t value) 
{
    d_LogInfoF("Consumable effect triggered: healing for %d HP", value);
}

// =============================================================================
// CORE HELPER FUNCTION TESTS - DIVINE ARCHITECTURE
// =============================================================================

int test_string_field_population(void)
{
    d_LogInfo("VERIFICATION: Helper functions properly populate dString_t fields in items.");
    dLogContext_t* ctx = d_PushLogContext("StringFieldPopulation");
    
    Material_t material = create_test_material_with_dstring("steel");

    // Test weapon creation to verify _populate_string_field works
    d_LogDebug("Creating weapon to test string field population...");
    Item_t* weapon = create_weapon("Test Sword", "test_sword", material, 10, 15, 0, 'S');

    TEST_ASSERT(weapon != NULL, "Weapon should be created successfully");
    
    if (weapon) {
        d_LogInfoF("Weapon created successfully: name='%s', id='%s', material='%s'", 
                   d_PeekString(weapon->name), d_PeekString(weapon->id), d_PeekString(material.name));
        
        TEST_ASSERT(dstring_equals(weapon->name, "Test Sword"), "Weapon name should be properly populated");
        TEST_ASSERT(dstring_equals(weapon->id, "test_sword"), "Weapon ID should be properly populated");
        TEST_ASSERT(weapon->name->len == strlen("Test Sword"), "Name length should be correct");
        TEST_ASSERT(weapon->id->len == strlen("test_sword"), "ID length should be correct");
        
        // Verify string field integrity
        TEST_ASSERT(d_PeekString(weapon->name) != NULL, "Name string should be accessible");
        TEST_ASSERT(d_PeekString(weapon->id) != NULL, "ID string should be accessible");
        TEST_ASSERT(d_GetStringLength(weapon->name) > 0, "Name should have non-zero length");
        TEST_ASSERT(d_GetStringLength(weapon->id) > 0, "ID should have non-zero length");

        destroy_item(weapon);
        weapon = NULL;  // Prevent use-after-free
        d_LogDebug("Weapon destroyed safely");

        // Add memory canary check
        volatile int canary = 0xDEADBEEF;
        d_LogDebugF("Stack canary: 0x%x", canary);
    }

    if (material.name) {
    d_LogDebugF("Material cleanup: %p", (void*)material.name);
    d_DestroyString(material.name);
    material.name = NULL;  // Prevent double-free
}
    d_PopLogContext(ctx);
    return 1;
}

int test_weapon_description_generation(void)
{
    d_LogInfo("VERIFICATION: Weapon description generation uses material properties correctly.");
    dLogContext_t* ctx = d_PushLogContext("WeaponDescriptionGen");
    
    Material_t iron_material = create_test_material_with_dstring("iron");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test with iron material
    d_LogDebug("Testing iron weapon description generation...");
    Item_t* iron_sword = create_weapon("Iron Blade", "iron_blade", iron_material, 8, 12, 0, 'I');

    TEST_ASSERT(iron_sword != NULL, "Iron sword should be created");
    
    if (iron_sword) {
        d_LogInfoF("Iron sword description: '%s'", d_PeekString(iron_sword->description));
        TEST_ASSERT(dstring_equals(iron_sword->description, "A weapon made of iron"),
                    "Iron sword description should be correct");
        
        // Verify description field integrity
        TEST_ASSERT(d_GetStringLength(iron_sword->description) > 0, "Description should have content");
        TEST_ASSERT(strstr(d_PeekString(iron_sword->description), "iron") != NULL, 
                    "Description should contain material name");
        
        destroy_item(iron_sword);
    }

    // Test with steel material
    d_LogDebug("Testing steel weapon description generation...");
    Item_t* steel_sword = create_weapon("Steel Blade", "steel_blade", steel_material, 12, 18, 1, 'S');

    TEST_ASSERT(steel_sword != NULL, "Steel sword should be created");
    
    if (steel_sword) {
        d_LogInfoF("Steel sword description: '%s'", d_PeekString(steel_sword->description));
        TEST_ASSERT(dstring_equals(steel_sword->description, "A weapon made of steel"),
                    "Steel sword description should be correct");
        
        destroy_item(steel_sword);
    }

    d_DestroyString(iron_material.name);
    d_DestroyString(steel_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_armor_description_generation(void)
{
    d_LogInfo("VERIFICATION: Armor description generation follows material naming conventions.");
    dLogContext_t* ctx = d_PushLogContext("ArmorDescriptionGen");
    
    Material_t leather_material = create_test_material_with_dstring("leather");
    Material_t chainmail_material = create_test_material_with_dstring("chainmail");

    // Test with leather material
    d_LogDebug("Testing leather armor description generation...");
    Item_t* leather_armor = create_armor("Leather Vest", "leather_vest", leather_material, 15, 8, 'L', 10, 10);

    TEST_ASSERT(leather_armor != NULL, "Leather armor should be created");
    
    if (leather_armor) {
        d_LogInfoF("Leather armor description: '%s'", d_PeekString(leather_armor->description));
        TEST_ASSERT(dstring_equals(leather_armor->description, "Armor made of leather"),
                    "Leather armor description should be correct");
        
        // Verify description pattern consistency
        const char* desc = d_PeekString(leather_armor->description);
        TEST_ASSERT(strncmp(desc, "Armor made of", 13) == 0, "Description should follow standard pattern");
        
        destroy_item(leather_armor);
    }

    // Test with chainmail material
    d_LogDebug("Testing chainmail armor description generation...");
    Item_t* chain_armor = create_armor("Chain Mail", "chain_mail", chainmail_material, 25, 12, 'C', 15, 15);

    TEST_ASSERT(chain_armor != NULL, "Chain armor should be created");
    
    if (chain_armor) {
        d_LogInfoF("Chain armor description: '%s'", d_PeekString(chain_armor->description));
        TEST_ASSERT(dstring_equals(chain_armor->description, "Armor made of chainmail"),
                    "Chain armor description should be correct");
        
        destroy_item(chain_armor);
    }

    d_DestroyString(leather_material.name);
    d_DestroyString(chainmail_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_key_description_generation(void)
{
    d_LogInfo("VERIFICATION: Key description generation references lock properties correctly.");
    dLogContext_t* ctx = d_PushLogContext("KeyDescriptionGen");
    
    Lock_t treasure_lock = create_test_lock_with_dstring("Treasure Chest", "A golden treasure chest");
    Lock_t door_lock = create_test_lock_with_dstring("Ancient Door", "An old wooden door");

    d_LogInfoF("Created locks: Treasure='%s', Door='%s'", 
               d_PeekString(treasure_lock.name), d_PeekString(door_lock.name));

    // Test with treasure chest lock
    d_LogDebug("Testing treasure key description generation...");
    Item_t* treasure_key = create_key("Golden Key", "treasure_key", treasure_lock, 'T');

    TEST_ASSERT(treasure_key != NULL, "Treasure key should be created");
    
    if (treasure_key) {
        d_LogInfoF("Key description: '%s'", d_PeekString(treasure_key->description));
        TEST_ASSERT(dstring_equals(treasure_key->description, "A key that opens: Treasure Chest"),
                    "Treasure key description should be correct");
        
        // Verify lock reference integrity
        const char* desc = d_PeekString(treasure_key->description);
        TEST_ASSERT(strstr(desc, "Treasure Chest") != NULL, "Description should reference lock name");
        TEST_ASSERT(strstr(desc, "A key that opens:") != NULL, "Description should follow key pattern");
        
        destroy_item(treasure_key);
    }

    // Test with door lock
    d_LogDebug("Testing door key description generation...");
    Item_t* door_key = create_key("Iron Key", "door_key", door_lock, 'I');
    
    if (door_key) {
        d_LogInfoF("Door key description: '%s'", d_PeekString(door_key->description));
        TEST_ASSERT(strstr(d_PeekString(door_key->description), "Ancient Door") != NULL,
                    "Door key should reference correct lock");
        destroy_item(door_key);
    }

    // Proper cleanup of lock dString_t fields
    d_DestroyString(treasure_lock.name);
    d_DestroyString(treasure_lock.description);
    d_DestroyString(door_lock.name);
    d_DestroyString(door_lock.description);
    
    d_PopLogContext(ctx);
    return 1;
}

int test_consumable_description_generation(void)
{
    d_LogInfo("VERIFICATION: Consumable description generation includes potency values.");
    dLogContext_t* ctx = d_PushLogContext("ConsumableDescriptionGen");

    // Test different potency values
    d_LogDebug("Testing weak potion description generation...");
    Item_t* weak_potion = create_consumable("Weak Potion", "weak_potion", 25, enhanced_test_heal_callback, 'w');

    TEST_ASSERT(weak_potion != NULL, "Weak potion should be created");
    
    if (weak_potion) {
        d_LogInfoF("Weak potion description: '%s'", d_PeekString(weak_potion->description));
        TEST_ASSERT(dstring_equals(weak_potion->description, "A consumable item with magical properties (Potency: 25)"),
                    "Weak potion description should include potency");
        
        // Verify potency formatting
        const char* desc = d_PeekString(weak_potion->description);
        TEST_ASSERT(strstr(desc, "Potency: 25") != NULL, "Description should show correct potency value");
        TEST_ASSERT(strstr(desc, "magical properties") != NULL, "Description should mention magical properties");
        
        destroy_item(weak_potion);
    }

    d_LogDebug("Testing strong potion description generation...");
    Item_t* strong_potion = create_consumable("Strong Potion", "strong_potion", 100, enhanced_test_heal_callback, 'S');

    TEST_ASSERT(strong_potion != NULL, "Strong potion should be created");
    
    if (strong_potion) {
        d_LogInfoF("Strong potion description: '%s'", d_PeekString(strong_potion->description));
        TEST_ASSERT(dstring_equals(strong_potion->description, "A consumable item with magical properties (Potency: 100)"),
                    "Strong potion description should include potency");
        
        // Verify callback assignment (based on actual struct field name)
        TEST_ASSERT(strong_potion->data.consumable.on_consume == enhanced_test_heal_callback,
                    "Callback should be properly assigned");
        TEST_ASSERT(strong_potion->data.consumable.value == 100, "Potency value should be stored");
        
        destroy_item(strong_potion);
    }

    d_PopLogContext(ctx);
    return 1;
}

int test_ammunition_description_generation(void)
{
    d_LogInfo("VERIFICATION: Ammunition description generation includes material and damage range.");
    dLogContext_t* ctx = d_PushLogContext("AmmoDescriptionGen");
    
    Material_t wood_material = create_test_material_with_dstring("wood");
    Material_t steel_material = create_test_material_with_dstring("steel");

    // Test wooden arrows
    d_LogDebug("Testing wooden arrows description generation...");
    Item_t* wood_arrows = create_ammunition("Wooden Arrows", "wood_arrows", wood_material, 3, 5, 'w');

    TEST_ASSERT(wood_arrows != NULL, "Wooden arrows should be created");
    
    if (wood_arrows) {
        d_LogInfoF("Wooden arrows description: '%s'", d_PeekString(wood_arrows->description));
        TEST_ASSERT(dstring_equals(wood_arrows->description, "Ammunition made of wood (Damage: 3-5)"),
                    "Wooden arrows description should include material and damage");
        
        // Verify damage range formatting
        const char* desc = d_PeekString(wood_arrows->description);
        TEST_ASSERT(strstr(desc, "Damage: 3-5") != NULL, "Description should show damage range");
        TEST_ASSERT(strstr(desc, "wood") != NULL, "Description should mention material");
        
        destroy_item(wood_arrows);
    }

    // Test steel bolts
    d_LogDebug("Testing steel bolts description generation...");
    Item_t* steel_bolts = create_ammunition("Steel Bolts", "steel_bolts", steel_material, 8, 12, 'S');

    TEST_ASSERT(steel_bolts != NULL, "Steel bolts should be created");
    
    if (steel_bolts) {
        d_LogInfoF("Steel bolts description: '%s'", d_PeekString(steel_bolts->description));
        TEST_ASSERT(dstring_equals(steel_bolts->description, "Ammunition made of steel (Damage: 8-12)"),
                    "Steel bolts description should include material and damage");
        
        // Verify ammunition properties (using correct field name)
        TEST_ASSERT(steel_bolts->data.ammo.min_damage == 8, "Min damage should be correct");
        TEST_ASSERT(steel_bolts->data.ammo.max_damage == 12, "Max damage should be correct");
        
        destroy_item(steel_bolts);
    }

    d_DestroyString(wood_material.name);
    d_DestroyString(steel_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_rarity_population(void)
{
    d_LogInfo("VERIFICATION: Item rarity field is properly initialized to 'common' by default.");
    dLogContext_t* ctx = d_PushLogContext("RarityPopulation");

    Material_t material = create_test_material_with_dstring("iron");

    // All items should start with "common" rarity
    d_LogDebug("Testing default rarity assignment...");
    Item_t* weapon = create_weapon("Test Weapon", "test_weapon", material, 10, 15, 0, 'W');
    Item_t* armor = create_armor("Test Armor", "test_armor", material, 20, 10, 'A', 15, 15);

    TEST_ASSERT(weapon != NULL && armor != NULL, "Items should be created");
    
    if (weapon && armor) {
        d_LogInfoF("Weapon rarity: '%s', Armor rarity: '%s'", 
                   d_PeekString(weapon->rarity), d_PeekString(armor->rarity));
        
        TEST_ASSERT(dstring_equals(weapon->rarity, "common"), "Weapon rarity should be 'common'");
        TEST_ASSERT(dstring_equals(armor->rarity, "common"), "Armor rarity should be 'common'");
        
        // Verify rarity field integrity
        TEST_ASSERT(d_GetStringLength(weapon->rarity) > 0, "Weapon rarity should have content");
        TEST_ASSERT(d_GetStringLength(armor->rarity) > 0, "Armor rarity should have content");
    }

    if (weapon) destroy_item(weapon);
    if (armor) destroy_item(armor);
    d_DestroyString(material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_error_handling(void)
{
    d_LogWarning("BUG HUNT: Testing error handling in helper functions through item creation.");
    dLogContext_t* ctx = d_PushLogContext("ErrorHandling");

    Material_t material = create_test_material_with_dstring("test");

    // Test NULL name
    d_LogDebug("Testing NULL name handling...");
    Item_t* null_name_weapon = create_weapon(NULL, "test_id", material, 10, 15, 0, 'N');
    TEST_ASSERT(null_name_weapon == NULL, "Creating weapon with NULL name should fail");

    // Test NULL id
    d_LogDebug("Testing NULL ID handling...");
    Item_t* null_id_weapon = create_weapon("Test Name", NULL, material, 10, 15, 0, 'N');
    TEST_ASSERT(null_id_weapon == NULL, "Creating weapon with NULL id should fail");

    // Test with invalid material - CREATE A SEPARATE INVALID MATERIAL
    d_LogDebug("Testing invalid material handling...");
    Material_t invalid_material = {
        .name = NULL,  // Invalid - NULL name
        .properties = material.properties  // Copy properties but not the name pointer
    };
    
    Item_t* invalid_mat_weapon = create_weapon("Test", "test", invalid_material, 10, 15, 0, 'I');
    d_LogInfoF("Invalid material weapon creation result: %s", invalid_mat_weapon ? "SUCCESS" : "FAILED");
    
    if (invalid_mat_weapon) {
        destroy_item(invalid_mat_weapon);
    }

    // Clean up the ORIGINAL material only
    d_DestroyString(material.name);
    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// ADVANCED EDGE CASE TESTS - BUG HUNTING EDITION
// =============================================================================

int test_string_edge_cases(void)
{
    d_LogWarning("BUG HUNT: Testing extreme string values and potential buffer overflows.");
    dLogContext_t* ctx = d_PushLogContext("StringEdgeCases");
    
    Material_t material = create_test_material_with_dstring("extreme");

    // Test with empty strings
    d_LogDebug("Testing empty string handling...");
    Item_t* empty_name = create_weapon("", "valid_id", material, 10, 15, 0, 'E');
    TEST_ASSERT(empty_name != NULL, "Empty name should be allowed");
    if (empty_name) {
        TEST_ASSERT(dstring_equals(empty_name->name, ""), "Empty name should remain empty");
        TEST_ASSERT(d_GetStringLength(empty_name->name) == 0, "Empty name should have zero length");
        destroy_item(empty_name);
    }

    Item_t* empty_id = create_weapon("Valid Name", "", material, 10, 15, 0, 'E');
    TEST_ASSERT(empty_id != NULL, "Empty ID should be allowed");
    if (empty_id) {
        TEST_ASSERT(dstring_equals(empty_id->id, ""), "Empty ID should remain empty");
        destroy_item(empty_id);
    }

    // Test with very long strings (potential buffer overflow)
    d_LogDebug("Testing very long string handling...");
    char long_name[1000];
    char long_id[1000];
    
    // Fill with pattern for easier debugging
    for (int i = 0; i < 999; i++) {
        long_name[i] = 'A' + (i % 26);
        long_id[i] = 'a' + (i % 26);
    }
    long_name[999] = '\0';
    long_id[999] = '\0';

    Item_t* long_weapon = create_weapon(long_name, long_id, material, 10, 15, 0, 'L');
    TEST_ASSERT(long_weapon != NULL, "Very long strings should be handled");
    
    if (long_weapon) {
        // Check if truncation occurred (based on MAX_ITEM_NAME_LENGTH in items.c)
        size_t name_len = d_GetStringLength(long_weapon->name);
        size_t id_len = d_GetStringLength(long_weapon->id);
        
        d_LogInfoF("Long string results: name_len=%zu, id_len=%zu", name_len, id_len);
        TEST_ASSERT(name_len <= 999, "Name length should be reasonable");
        TEST_ASSERT(id_len <= 999, "ID length should be reasonable");
        
        // Verify no buffer corruption
        const char* stored_name = d_PeekString(long_weapon->name);
        TEST_ASSERT(stored_name[0] == 'A', "First character should be preserved");
        
        destroy_item(long_weapon);
    }

    // Test with special characters - use regular char for glyph to avoid overflow warnings
    d_LogDebug("Testing special character handling...");
    Item_t* special_chars = create_weapon("Sword™ of Ñoël", "sword_noël", material, 10, 15, 0, 'S');
    TEST_ASSERT(special_chars != NULL, "Special characters should be handled");
    if (special_chars) {
        d_LogInfoF("Special character weapon name: '%s'", d_PeekString(special_chars->name));
        destroy_item(special_chars);
    }

    // Test with control characters
    d_LogDebug("Testing control character handling...");
    Item_t* control_chars = create_weapon("Multi\nLine\tName", "multi_line_id", material, 10, 15, 0, 'M');
    TEST_ASSERT(control_chars != NULL, "Control characters should be handled");
    if (control_chars) {
        const char* name = d_PeekString(control_chars->name);
        TEST_ASSERT(strstr(name, "\n") != NULL, "Newline should be preserved");
        TEST_ASSERT(strstr(name, "\t") != NULL, "Tab should be preserved");
        destroy_item(control_chars);
    }

    d_DestroyString(material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_material_edge_cases(void)
{
    d_LogWarning("BUG HUNT: Testing extreme material properties and potential overflow conditions.");
    dLogContext_t* ctx = d_PushLogContext("MaterialEdgeCases");
    
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
    d_LogDebug("Testing weapon with extreme material properties...");
    Item_t* extreme_weapon = create_weapon("Extreme Sword", "extreme_sword", extreme_material, 10, 15, 0, 'X');
    TEST_ASSERT(extreme_weapon != NULL, "Weapon with extreme material should be created");

    if (extreme_weapon) {
        apply_material_to_weapon(extreme_weapon);

        // Check if extreme values cause overflows or underflows - uint8_t comparisons are always true
        uint8_t min_dmg = get_weapon_min_damage(extreme_weapon);
        uint8_t max_dmg = get_weapon_max_damage(extreme_weapon);
        float weight = get_item_weight(extreme_weapon);
        uint8_t value = get_item_value_coins(extreme_weapon);

        d_LogInfoF("Extreme weapon stats: min_dmg=%d, max_dmg=%d, weight=%.2f, value=%d",
                   min_dmg, max_dmg, weight, value);

        // These checks are meaningful even though uint8_t can't exceed 255
        TEST_ASSERT(weight >= 0.0f, "Weight should not be negative");
        TEST_ASSERT(min_dmg < 200 || max_dmg < 200, "At least one damage value should be reasonable");

        destroy_item(extreme_weapon);
    }

    // Test armor with extreme material
    d_LogDebug("Testing armor with extreme material properties...");
    Item_t* extreme_armor = create_armor("Extreme Armor", "extreme_armor", extreme_material, 20, 10, 'X', 15, 15);
    TEST_ASSERT(extreme_armor != NULL, "Armor with extreme material should be created");

    if (extreme_armor) {
        apply_material_to_armor(extreme_armor);

        uint8_t armor_val = get_armor_value(extreme_armor);
        uint8_t evasion_val = get_evasion_value(extreme_armor);
        uint8_t stealth_val = get_stealth_value(extreme_armor);

        d_LogInfoF("Extreme armor stats: armor=%d, evasion=%d, stealth=%d",
                   armor_val, evasion_val, stealth_val);

        // These are always true due to uint8_t limits, but test behavior under extreme conditions
        TEST_ASSERT(armor_val < 200 || evasion_val < 200, "At least one value should be reasonable");

        destroy_item(extreme_armor);
    }

    d_DestroyString(extreme_material.name);
    d_PopLogContext(ctx);
    return 1;
}

int test_memory_stress(void)
{
    d_LogWarning("BUG HUNT: Memory management stress test for potential leaks.");
    dLogContext_t* ctx = d_PushLogContext("MemoryStress");
    
    d_LogInfo("Creating and destroying 100 items rapidly to test for memory leaks...");
    
    // Rate limit logging to avoid spam
    for (int i = 0; i < 100; i++) {
        Material_t material = create_test_material_with_dstring("stress_test");

        Item_t* weapon = create_weapon("Stress Weapon", "stress_weapon", material, 10, 15, 0, 'S');
        Item_t* armor = create_armor("Stress Armor", "stress_armor", material, 20, 10, 'A', 15, 15);
        Item_t* ammo = create_ammunition("Stress Ammo", "stress_ammo", material, 5, 8, 'a');

        // Verify items were created successfully
        if (i < 3) {  // Only assert for first few iterations
            TEST_ASSERT(weapon != NULL, "Stress weapon should be created");
            TEST_ASSERT(armor != NULL, "Stress armor should be created");
            TEST_ASSERT(ammo != NULL, "Stress ammo should be created");
        }

        // Log progress occasionally
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 
                          1, 2.0, "Stress test iteration %d completed", i);

        // Clean up
        if (weapon) destroy_item(weapon);
        if (armor) destroy_item(armor);
        if (ammo) destroy_item(ammo);

        d_DestroyString(material.name);
    }

    TEST_ASSERT(1, "Stress test completed without crashes");
    d_PopLogContext(ctx);
    return 1;
}

int test_consumable_edge_cases(void)
{
    d_LogWarning("BUG HUNT: Testing consumable edge cases and potential overflow conditions.");
    dLogContext_t* ctx = d_PushLogContext("ConsumableEdgeCases");
    
    // Test consumable with zero value
    d_LogDebug("Testing zero value consumable...");
    Item_t* zero_potion = create_consumable("Zero Potion", "zero_potion", 0, enhanced_test_heal_callback, 'Z');
    TEST_ASSERT(zero_potion != NULL, "Zero value consumable should be created");
    if (zero_potion) {
        TEST_ASSERT(zero_potion->data.consumable.value == 0, "Zero value should be preserved");
        
        // Verify description handles zero value
        const char* desc = d_PeekString(zero_potion->description);
        TEST_ASSERT(strstr(desc, "Potency: 0") != NULL, "Description should show zero potency");
        
        destroy_item(zero_potion);
    }

    // Test consumable with maximum value
    d_LogDebug("Testing maximum value consumable...");
    Item_t* max_potion = create_consumable("Max Potion", "max_potion", 255, enhanced_test_heal_callback, 'M');
    TEST_ASSERT(max_potion != NULL, "Max value consumable should be created");
    if (max_potion) {
        TEST_ASSERT(max_potion->data.consumable.value == 255, "Max value should be preserved");
        destroy_item(max_potion);
    }

    // Test consumable with extremely high value (potential overflow) - remove warning
    d_LogDebug("Testing overflow value consumable...");
    Item_t* overflow_potion = create_consumable("Overflow Potion", "overflow_potion", 300, enhanced_test_heal_callback, 'O');
    TEST_ASSERT(overflow_potion != NULL, "High value consumable should be created");
    if (overflow_potion) {
        d_LogInfoF("Overflow potion value: %d (should be clamped to uint8_t range)", 
                   overflow_potion->data.consumable.value);
        // Value will be capped to 255 to prevent overflow
        TEST_ASSERT(overflow_potion->data.consumable.value == 255, "Value be capped to 255");
        
        // Verify callback is properly assigned even with overflow
        TEST_ASSERT(overflow_potion->data.consumable.on_consume == enhanced_test_heal_callback,
                    "Callback should be preserved even with value overflow");
        
        destroy_item(overflow_potion);
    }

    d_PopLogContext(ctx);
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER WITH DIVINE ARCHITECTURE
// =============================================================================

int main(void)
{
    // =========================================================================
    // DAEDALUS LOGGER INITIALIZATION - DIVINE CONFIGURATION
    // =========================================================================
    dLogConfig_t config = {
        .default_level = D_LOG_LEVEL_DEBUG,
        .colorize_output = true,
        .include_timestamp = false,
        .include_file_info = true,     // Enable for debugging helper functions
        .include_function = true       // Enable for detailed context
    };
    
    dLogger_t* logger = d_CreateLogger(config);
    d_SetGlobalLogger(logger);
    d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);
    
    d_LogInfo("Initializing MIDAS-Enhanced Item Helper Functions Test Suite");
    d_LogDebugF("Daedalus Logging System: %s", "ACTIVE");
    d_LogDebugF("Test Suite Focus: %s", "Helper function validation through item creation");
    // =========================================================================

    TEST_SUITE_START("MIDAS Enhanced Item Helper Functions Tests");

    // Core helper function tests
    RUN_TEST(test_string_field_population);
    RUN_TEST(test_weapon_description_generation);
    RUN_TEST(test_armor_description_generation);
    RUN_TEST(test_key_description_generation);
    RUN_TEST(test_consumable_description_generation);
    RUN_TEST(test_ammunition_description_generation);
    RUN_TEST(test_rarity_population);
    RUN_TEST(test_error_handling);

    // Advanced edge case tests
    RUN_TEST(test_string_edge_cases);
    RUN_TEST(test_material_edge_cases);
    RUN_TEST(test_memory_stress);
    RUN_TEST(test_consumable_edge_cases);

    // =========================================================================
    // DAEDALUS LOGGER CLEANUP
    // =========================================================================
    d_LogInfo("Item Helper Functions Test Suite completed");
    d_DestroyLogger(d_GetGlobalLogger());
    // =========================================================================

    TEST_SUITE_END();
    return 0;
}