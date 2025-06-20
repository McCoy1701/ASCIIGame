#include "items.h"
#include "structs.h"
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/*
 * Helper function to create and assign a dString_t field from a const char*
 * Returns true on success, false on failure
 */
static bool _populate_string_field(dString_t* dest, const char* src) {
    if (src == NULL || dest == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, src, 0);
    *dest = *temp;  // Copy struct content
    free(temp);     // Free wrapper, keep string data
    return true;
}

/*
 * Helper function to populate item description for weapons
 */
static bool _populate_weapon_desc(dString_t* dest, const Material_t* material) {
    if (dest == NULL || material == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, "A weapon made of ", 0);
    d_AppendString(temp, material->name, 0);
    *dest = *temp;
    free(temp);
    return true;
}

/*
 * Helper function to populate item description for armor
 */
static bool _populate_armor_desc(dString_t* dest, const Material_t* material) {
    if (dest == NULL || material == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, "Armor made of ", 0);
    d_AppendString(temp, material->name, 0);
    *dest = *temp;
    free(temp);
    return true;
}

/*
 * Helper function to populate item rarity
 */
static bool _populate_rarity(dString_t* dest, const char* rarity) {
    if (dest == NULL || rarity == NULL) {
        return false;
    }

    return _populate_string_field(dest, rarity);
}
/*
 * Helper function to populate item description for keys
 */
static bool _populate_key_desc(dString_t* dest, const Lock_t* lock) {
    if (dest == NULL || lock == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, "A key that opens: ", 0);
    // Need to handle Lock_t's name field - assuming it's dString_t now
    d_AppendString(temp, d_PeekString(&lock->name), 0);
    *dest = *temp;
    free(temp);
    return true;
}

/*
 * Helper function to create a default neutral material for keys
 */
static Material_t _create_default_material(void) {
    Material_t material;

    // Set material name using helper
    if (!_populate_string_field(&material.name, "metal")) {
        // If this fails, we have bigger problems, but set empty name
        material.name.str = NULL;
        material.name.len = 0;
        material.name.alloced = 0;
    }

    // Initialize all properties to neutral (1.0f)
    material.properties.weight_fact = 1.0f;
    material.properties.value_coins_fact = 1.0f;
    material.properties.durability_fact = 1.0f;
    material.properties.min_damage_fact = 1.0f;
    material.properties.max_damage_fact = 1.0f;
    material.properties.armor_value_fact = 1.0f;
    material.properties.evasion_value_fact = 1.0f;
    material.properties.stealth_value_fact = 1.0f;
    material.properties.enchant_value_fact = 1.0f;

    return material;
}
/*
* Helper function to create a default organic material for consumables
*/
static Material_t _create_consumable_material(void) {
    Material_t material;

    // Set material name to "organic" using helper
    if (!_populate_string_field(&material.name, "organic")) {
        // If this fails, we have bigger problems, but set empty name
        material.name.str = NULL;
        material.name.len = 0;
        material.name.alloced = 0;
    }

    // Initialize all properties to neutral (1.0f)
    material.properties.weight_fact = 1.0f;
    material.properties.value_coins_fact = 1.0f;
    material.properties.durability_fact = 1.0f;
    material.properties.min_damage_fact = 1.0f;
    material.properties.max_damage_fact = 1.0f;
    material.properties.armor_value_fact = 1.0f;
    material.properties.evasion_value_fact = 1.0f;
    material.properties.stealth_value_fact = 1.0f;
    material.properties.enchant_value_fact = 1.0f;

    return material;
}

/*
 * Helper function to populate item description for consumables with value info
 */
static bool _populate_consumable_desc(dString_t* dest, uint8_t value) {
    if (dest == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, "A consumable item with magical properties (Potency: ", 0);
    d_AppendInt(temp, value);
    d_AppendChar(temp, ')');
    *dest = *temp;
    free(temp);
    return true;
}
/*
 * Helper function to populate item description for ammunition
 */
static bool _populate_ammunition_desc(dString_t* dest, const Material_t* material, uint8_t min_dmg, uint8_t max_dmg) {
    if (dest == NULL || material == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        return false;
    }

    d_AppendString(temp, "Ammunition made of ", 0);
    d_AppendString(temp, d_PeekString(&material->name), 0);
    d_AppendString(temp, " (Damage: ", 0);
    d_AppendInt(temp, min_dmg);
    d_AppendChar(temp, '-');
    d_AppendInt(temp, max_dmg);
    d_AppendChar(temp, ')');
    *dest = *temp;
    free(temp);
    return true;
}

// =============================================================================
// ITEM CREATION & DESTRUCTION
// =============================================================================
Item_t* create_weapon(const char* name, const char* id, Material_t material,
                     uint8_t min_dmg, uint8_t max_dmg, uint8_t range, char glyph)
{
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_WEAPON;

    // Populate name using helper
    if (!_populate_string_field(&item->name, name)) {
        free(item);
        return NULL;
    }

    // Populate id using helper
    if (!_populate_string_field(&item->id, id)) {
        free(item->name.str);
        free(item);
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;
    item->material_data = material;

    // Initialize weapon-specific data
    item->data.weapon.min_damage = min_dmg;
    item->data.weapon.max_damage = max_dmg;
    item->data.weapon.range_tiles = range;
    item->data.weapon.stealth_value = 0;
    item->data.weapon.enchant_value = 0;
    item->data.weapon.durability = 255; // 100% durability

    // Initialize empty ammo (no ammo required by default)
    memset(&item->data.weapon.ammo, 0, sizeof(Ammunition__Item_t));

    // Set default values (will be modified by material factors)
    item->weight_kg = 1.0f; // Base weight
    item->value_coins = min_dmg + max_dmg; // Base value
    item->stackable = 0; // Weapons don't stack

    // Populate description using helper
    if (!_populate_weapon_desc(&item->description, &material)) {
        free(item->name.str);
        free(item->id.str);
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    if (!_populate_rarity(&item->rarity, "common")) {
        free(item->name.str);
        free(item->id.str);
        free(item->description.str);
        free(item);
        return NULL;
    }

    return item;
}

Item_t* create_armor(const char* name, const char* id, Material_t material,
                    uint8_t armor_val, uint8_t evasion_val, char glyph)
{
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_ARMOR;

    // Populate name and id using helpers
    if (!_populate_string_field(&item->name, name)) {
        free(item);
        return NULL;
    }

    if (!_populate_string_field(&item->id, id)) {
        free(item->name.str);
        free(item);
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;
    item->material_data = material;

    // Initialize armor-specific data
    item->data.armor.armor_value = armor_val;
    item->data.armor.evasion_value = evasion_val;
    item->data.armor.stealth_value = 0;
    item->data.armor.enchant_value = 0;
    item->data.armor.durability = 255; // 100% durability

    // Set default values (will be modified by material factors)
    item->weight_kg = 1.0f; // Base weight
    item->value_coins = armor_val + evasion_val; // Base value
    item->stackable = 0; // Armor doesn't stack

    // Populate description using helper
    if (!_populate_armor_desc(&item->description, &material)) {
        free(item->name.str);
        free(item->id.str);
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    if (!_populate_rarity(&item->rarity, "common")) {
        free(item->name.str);
        free(item->id.str);
        free(item->description.str);
        free(item);
        return NULL;
    }

    return item;
}

Item_t* create_key(const char* name, const char* id, Lock_t lock, char glyph)
{
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_KEY;

    // Populate name and id using helpers
    if (!_populate_string_field(&item->name, name)) {
        free(item);
        return NULL;
    }

    if (!_populate_string_field(&item->id, id)) {
        free(item->name.str);
        free(item);
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;

    // Keys don't have materials, so use default neutral material
    item->material_data = _create_default_material();

    // Initialize key-specific data
    item->data.key.lock = lock;

    // Set default values
    item->weight_kg = 0.1f; // Keys are light but not weightless
    item->value_coins = 5; // Base value
    item->stackable = 1; // Keys cannot stack (though this seems like it should be 0?)

    // Populate description using helper
    if (!_populate_key_desc(&item->description, &lock)) {
        free(item->name.str);
        free(item->id.str);
        // Clean up material name if it was allocated
        if (item->material_data.name.str != NULL) {
            free(item->material_data.name.str);
        }
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    if (!_populate_rarity(&item->rarity, "common")) {
        free(item->name.str);
        free(item->id.str);
        free(item->description.str);
        if (item->material_data.name.str != NULL) {
            free(item->material_data.name.str);
        }
        free(item);
        return NULL;
    }

    return item;
}

Item_t* create_consumable(const char* name, const char* id, uint8_t value,
                         void (*on_consume)(uint8_t), char glyph)
{
    if (name == NULL || id == NULL || on_consume == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_CONSUMABLE;

    // Populate name and id using helpers
    if (!_populate_string_field(&item->name, name)) {
        free(item);
        return NULL;
    }

    if (!_populate_string_field(&item->id, id)) {
        free(item->name.str);
        free(item);
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;

    // Consumables don't have materials, so use default organic material
    item->material_data = _create_consumable_material();

    // Initialize consumable-specific data
    item->data.consumable.on_consume = on_consume;
    item->data.consumable.on_duration_end = NULL; // Optional
    item->data.consumable.on_duration_tick = NULL; // Optional
    item->data.consumable.value = value;
    item->data.consumable.duration_seconds = 0; // Instant effect by default

    // Set default values
    item->weight_kg = 0.1f; // Consumables are light but not weightless
    item->value_coins = value; // Default value based on effect strength
    item->stackable = 16; // Can stack up to 16

    // Populate description using helper
    if (!_populate_consumable_desc(&item->description, item->data.consumable.value)) {
        free(item->name.str);
        free(item->id.str);
        // Clean up material name if it was allocated
        if (item->material_data.name.str != NULL) {
            free(item->material_data.name.str);
        }
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    if (!_populate_rarity(&item->rarity, "common")) {
        free(item->name.str);
        free(item->id.str);
        free(item->description.str);
        if (item->material_data.name.str != NULL) {
            free(item->material_data.name.str);
        }
        free(item);
        return NULL;
    }

    return item;
}


Item_t* create_ammunition(const char* name, const char* id, Material_t material,
                         uint8_t min_dmg, uint8_t max_dmg, char glyph)
{
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_AMMUNITION;

    // Populate name and id using helpers
    if (!_populate_string_field(&item->name, name)) {
        free(item);
        return NULL;
    }

    if (!_populate_string_field(&item->id, id)) {
        free(item->name.str);
        free(item);
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;
    item->material_data = material;

    // Initialize ammunition-specific data
    item->data.ammo.min_damage = min_dmg;
    item->data.ammo.max_damage = max_dmg;

    // Set default values (will be modified by material factors)
    item->weight_kg = 0.05f; // Ammo is very light but not weightless
    item->value_coins = (min_dmg + max_dmg) / 2; // Base value
    item->stackable = 255; // Ammo stacks very well

    // Populate description using helper
    if (!_populate_ammunition_desc(&item->description, &material, min_dmg, max_dmg)) {
        free(item->name.str);
        free(item->id.str);
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    if (!_populate_rarity(&item->rarity, "common")) {
        free(item->name.str);
        free(item->id.str);
        free(item->description.str);
        free(item);
        return NULL;
    }

    return item;
}


void destroy_item(Item_t* item)
{
    if (item == NULL) {
        return;
    }

    free(item);
}

// =============================================================================
// ITEM TYPE CHECKING & ACCESS
// =============================================================================

bool is_weapon(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }
    return item->type == ITEM_TYPE_WEAPON;
}

bool is_armor(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }
    return item->type == ITEM_TYPE_ARMOR;
}

bool is_key(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }
    return item->type == ITEM_TYPE_KEY;
}

bool is_consumable(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }
    return item->type == ITEM_TYPE_CONSUMABLE;
}

bool is_ammunition(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }
    return item->type == ITEM_TYPE_AMMUNITION;
}

// Safe data access functions
Weapon__Item_t* get_weapon_data(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        return NULL;
    }
    return &item->data.weapon;
}

Armor__Item_t* get_armor_data(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        return NULL;
    }
    return &item->data.armor;
}

Key__Item_t* get_key_data(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_KEY) {
        return NULL;
    }
    return &item->data.key;
}

Consumable__Item_t* get_consumable_data(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        return NULL;
    }
    return &item->data.consumable;
}

Ammunition__Item_t* get_ammunition_data(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        return NULL;
    }
    return &item->data.ammo;
}

// =============================================================================
// MATERIAL SYSTEM
// =============================================================================

Material_t create_material(const char* name, MaterialProperties_t properties)
{
    Material_t material;

    if (name == NULL) {
        // Return a default material if name is NULL
        strncpy(material.name, "unknown", MAX_NAME_LENGTH - 1);
        material.name[MAX_NAME_LENGTH - 1] = '\0';
        material.properties = create_default_material_properties();
        return material;
    }

    // Copy name safely
    strncpy(material.name, name, MAX_NAME_LENGTH - 1);
    material.name[MAX_NAME_LENGTH - 1] = '\0';

    // Set the properties
    material.properties = properties;

    return material;
}

MaterialProperties_t create_default_material_properties(void)
{
    MaterialProperties_t props = {
        .weight_fact = 1.0f,
        .value_coins_fact = 1.0f,
        .durability_fact = 1.0f,
        .min_damage_fact = 1.0f,
        .max_damage_fact = 1.0f,
        .armor_value_fact = 1.0f,
        .evasion_value_fact = 1.0f,
        .stealth_value_fact = 1.0f,
        .enchant_value_fact = 1.0f
    };
    return props;
}



void apply_material_to_weapon(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        return;
    }

    Weapon__Item_t* weapon = &item->data.weapon;
    Material_t* material = &item->material_data;

    // Apply material factors to weapon stats
    weapon->min_damage = (uint8_t)(weapon->min_damage * material->properties.min_damage_fact);
    weapon->max_damage = (uint8_t)(weapon->max_damage * material->properties.max_damage_fact);
    weapon->durability = (uint8_t)(weapon->durability * material->properties.durability_fact);
    weapon->stealth_value = (uint8_t)(weapon->stealth_value * material->properties.stealth_value_fact);
    weapon->enchant_value = (uint8_t)(weapon->enchant_value * material->properties.enchant_value_fact);

    // Apply to base item properties
    item->weight_kg = (float)(item->weight_kg * material->properties.weight_fact);
    item->value_coins = (uint8_t)(item->value_coins * material->properties.value_coins_fact);
}

void apply_material_to_armor(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        return;
    }

    Armor__Item_t* armor = &item->data.armor;
    Material_t* material = &item->material_data;

    // Apply material factors to armor stats
    armor->armor_value = (uint8_t)(armor->armor_value * material->properties.armor_value_fact);
    armor->evasion_value = (uint8_t)(armor->evasion_value * material->properties.evasion_value_fact);
    armor->durability = (uint8_t)(armor->durability * material->properties.durability_fact);
    armor->stealth_value = (uint8_t)(armor->stealth_value * material->properties.stealth_value_fact);
    armor->enchant_value = (uint8_t)(armor->enchant_value * material->properties.enchant_value_fact);

    // Apply to base item properties
    item->weight_kg = (float)(item->weight_kg * material->properties.weight_fact);
    item->value_coins = (uint8_t)(item->value_coins * material->properties.value_coins_fact);
}

float calculate_final_weight(const Item_t* item)
{
    if (item == NULL) {
        // TODO: Implement logging errors on NULL check
        return 0.0f;
    }

    // Weight is already calculated with material factors applied
    return item->weight_kg;
}

uint8_t calculate_final_value(const Item_t* item)
{
    if (item == NULL) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }

    // Value is already calculated with material factors applied
    return item->value_coins;
}

// =============================================================================
// ITEM PROPERTIES & STATS
// =============================================================================

// Weapon stats
uint8_t get_weapon_min_damage(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->data.weapon.min_damage;
}

uint8_t get_weapon_max_damage(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->data.weapon.max_damage;
}

uint8_t get_weapon_range(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->data.weapon.range_tiles;
}

bool weapon_needs_ammo(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        // TODO: Implement logging errors on NULL check
        return false;
    }

    // Ranged weapons (range > 0) need ammunition
    return item->data.weapon.range_tiles > 0;
}

bool weapon_can_use_ammo(const Item_t* weapon, const Item_t* ammo)
{
    if (weapon == NULL || ammo == NULL) {
        // TODO: Implement logging errors on NULL check
        return false;
    }

    if (weapon->type != ITEM_TYPE_WEAPON || ammo->type != ITEM_TYPE_AMMUNITION) {
        // TODO: Implement logging errors on type mismatch
        return false;
    }

    // For now, assume any ammunition can be used with any ranged weapon
    // In a more complex system, we might check ammo types (arrows vs bolts)
    return weapon->data.weapon.range_tiles > 0;
}

// Armor stats
uint8_t get_armor_value(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->data.armor.armor_value;
}

uint8_t get_evasion_value(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->data.armor.evasion_value;
}

// General item properties
float get_item_weight(const Item_t* item)
{
    if (item == NULL) {
        // TODO: Implement logging errors on NULL check
        return 0.0f;
    }
    return item->weight_kg;
}

uint8_t get_item_value_coins(const Item_t* item)
{
    if (item == NULL) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }
    return item->value_coins;
}

uint8_t get_stealth_value(const Item_t* item)
{
    if (item == NULL) {
        // TODO: Implement logging errors on NULL check
        return 0;
    }

    switch (item->type) {
        case ITEM_TYPE_WEAPON:
        return item->data.weapon.stealth_value;
        case ITEM_TYPE_ARMOR:
        return item->data.armor.stealth_value;
        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        return 0; // These items don't have stealth values
        default:
        return 0;
    }
}

uint8_t get_durability(const Item_t* item)
{
    if (item == NULL) {
        return 0;
    }

    switch (item->type) {
        case ITEM_TYPE_WEAPON:
        return item->data.weapon.durability;
        case ITEM_TYPE_ARMOR:
        return item->data.armor.durability;
        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        return 255; // These items don't degrade
        default:
        return 0;
    }
}

bool is_item_stackable(const Item_t* item)
{
    if (item == NULL) {
        return false;
    }

    // stackable value: 0 = not stackable, 1+ = stackable
    return item->stackable > 0;
}

uint8_t get_max_stack_size(const Item_t* item)
{
    if (item == NULL) {
        return 0;
    }

    // Return the maximum stack size for this item
    return item->stackable;
}
// =============================================================================
// DURABILITY SYSTEM
// =============================================================================

void damage_item_durability(Item_t* item, uint16_t damage)
{
    if (item == NULL) {
        return;
    }

    // Only weapons and armor have durability that can be damaged
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            if (item->data.weapon.durability > damage) {
                item->data.weapon.durability -= damage;
            } else {
                item->data.weapon.durability = 0; // Item is broken
            }
            break;

        case ITEM_TYPE_ARMOR:
            if (item->data.armor.durability > damage) {
                item->data.armor.durability -= damage;
            } else {
                item->data.armor.durability = 0; // Item is broken
            }
            break;

        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        default:
            // These items don't have durability that degrades
            break;
    }
}

void repair_item(Item_t* item, uint16_t repair_amount)
{
    if (item == NULL) {
        return;
    }

    // Only weapons and armor can be repaired
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            if (item->data.weapon.durability + repair_amount > 255) {
                item->data.weapon.durability = 255; // Cap at max durability
            } else {
                item->data.weapon.durability += repair_amount;
            }
            break;

        case ITEM_TYPE_ARMOR:
            if (item->data.armor.durability + repair_amount > 255) {
                item->data.armor.durability = 255; // Cap at max durability
            } else {
                item->data.armor.durability += repair_amount;
            }
            break;

        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        default:
            // These items can't be repaired
            break;
    }
}

bool is_item_broken(const Item_t* item)
{
    if (item == NULL) {
        return true; // NULL items are considered "broken"
    }

    // Check durability based on item type
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            return item->data.weapon.durability == 0;

        case ITEM_TYPE_ARMOR:
            return item->data.armor.durability == 0;

        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        default:
            // These items can't break
            return false;
    }
}

float get_durability_percentage(const Item_t* item)
{
    if (item == NULL) {
        return 0.0f;
    }

    // Return durability as percentage (0.0 = 0%, 1.0 = 100%)
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            return (float)item->data.weapon.durability / 255.0f;

        case ITEM_TYPE_ARMOR:
            return (float)item->data.armor.durability / 255.0f;

        case ITEM_TYPE_KEY:
        case ITEM_TYPE_CONSUMABLE:
        case ITEM_TYPE_AMMUNITION:
        default:
            // These items don't degrade, so they're always at "full durability"
            return 1.0f;
    }
}

// =============================================================================
// INVENTORY MANAGEMENT
// =============================================================================

Inventory_t* create_inventory(uint8_t size)
{
    if (size == 0) {
        // TODO: Implement error handling for invalid inventory size
        return NULL; // Invalid inventory size
    }

    Inventory_t* inventory = (Inventory_t*)malloc(sizeof(Inventory_t));
    if (inventory == NULL) {
        // TODO: Implement error handling for memory allocation failure
        return NULL; // Memory allocation failed
    }

    inventory->slots = (Inventory_slot_t*)calloc(size, sizeof(Inventory_slot_t));
    if (inventory->slots == NULL) {
        free(inventory);
        // TODO: Implement error handling for memory allocation failure
        return NULL; // Memory allocation failed
    }

    inventory->size = size;

    // Initialize all slots as empty
    for (uint8_t i = 0; i < size; i++) {
        inventory->slots[i].quantity = 0;
        inventory->slots[i].is_equipped = 0;
        // Note: item data will be uninitialized but quantity=0 marks slot as empty
    }

    return inventory;
}

void destroy_inventory(Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return;
    }

    // Note: We don't destroy individual items here because they might be
    // referenced elsewhere. The caller is responsible for item cleanup.

    if (inventory->slots != NULL) {
        free(inventory->slots);
    }
    free(inventory);
}

bool add_item_to_inventory(Inventory_t* inventory, Item_t* item, uint8_t quantity)
{
    if (inventory == NULL || item == NULL || quantity == 0) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    // First, try to stack with existing items if the item is stackable
    if (is_item_stackable(item)) {
        for (uint8_t i = 0; i < inventory->size; i++) {
            if (inventory->slots[i].quantity > 0 &&
                can_stack_items(&inventory->slots[i].item, item)) {

                uint8_t max_stack = get_max_stack_size(item);
                uint8_t available_space = max_stack - inventory->slots[i].quantity;

                if (available_space > 0) {
                    uint8_t to_add = (quantity <= available_space) ? quantity : available_space;
                    inventory->slots[i].quantity += to_add;
                    quantity -= to_add;

                    if (quantity == 0) {
                        return true; // All items added successfully
                    }
                }
            }
        }
    }

    // If we still have items to add, find empty slots
    while (quantity > 0) {
        // Find first empty slot
        uint8_t empty_slot = inventory->size; // Invalid index indicates no empty slot
        for (uint8_t i = 0; i < inventory->size; i++) {
            if (inventory->slots[i].quantity == 0) {
                empty_slot = i;
                break;
            }
        }

        if (empty_slot >= inventory->size) {
            // TODO: Implement error handling for no empty slots
            return false; // No empty slots, couldn't add all items
        }

        // Add items to empty slot
        inventory->slots[empty_slot].item = *item; // Copy item data
        inventory->slots[empty_slot].is_equipped = 0;

        if (is_item_stackable(item)) {
            uint8_t max_stack = get_max_stack_size(item);
            uint8_t to_add = (quantity <= max_stack) ? quantity : max_stack;
            inventory->slots[empty_slot].quantity = to_add;
            quantity -= to_add;
        } else {
            inventory->slots[empty_slot].quantity = 1;
            quantity -= 1;
        }
    }

    return true; // Successfully added all items
}

bool remove_item_from_inventory(Inventory_t* inventory, const char* item_id, uint8_t quantity)
{
    if (inventory == NULL || item_id == NULL || quantity == 0) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    uint8_t remaining_to_remove = quantity;

    // Find and remove items with matching ID
    for (uint8_t i = 0; i < inventory->size && remaining_to_remove > 0; i++) {
        if (inventory->slots[i].quantity > 0 &&
            strcmp(inventory->slots[i].item.id, item_id) == 0) {

            if (inventory->slots[i].quantity <= remaining_to_remove) {
                // Remove entire stack
                remaining_to_remove -= inventory->slots[i].quantity;
                inventory->slots[i].quantity = 0;
                inventory->slots[i].is_equipped = 0;
            } else {
                // Partially remove from stack
                inventory->slots[i].quantity -= remaining_to_remove;
                remaining_to_remove = 0;
            }
        }
    }

    return remaining_to_remove == 0; // Return true if we removed all requested items
}

Inventory_slot_t* find_item_in_inventory(Inventory_t* inventory, const char* item_id)
{
    if (inventory == NULL || item_id == NULL) {
        // TODO: Implement error handling for invalid input
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            strcmp(inventory->slots[i].item.id, item_id) == 0) {
            return &inventory->slots[i];
        }
    }
    // TODO: Implement error handling for item not found
    return NULL; // Item not found
}

bool can_stack_items(const Item_t* item1, const Item_t* item2)
{
    if (item1 == NULL || item2 == NULL) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    // Items can stack if they have the same ID and are stackable
    return (strcmp(item1->id, item2->id) == 0) && is_item_stackable(item1);
}

bool equip_item(Inventory_t* inventory, const char* item_id)
{
    if (inventory == NULL || item_id == NULL) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    Inventory_slot_t* slot = find_item_in_inventory(inventory, item_id);
    if (slot == NULL) {
        // TODO: Implement error handling for item not found
        return false;
    }

    // Only weapons and armor can be equipped
    if (slot->item.type != ITEM_TYPE_WEAPON && slot->item.type != ITEM_TYPE_ARMOR) {
        // TODO: Implement error handling for invalid item type
        return false;
    }

    // Check if item is broken
    if (is_item_broken(&slot->item)) {
        //
        return false; // Can't equip broken items
    }

    // Unequip any existing item of the same type
    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            inventory->slots[i].item.type == slot->item.type &&
            inventory->slots[i].is_equipped) {
            inventory->slots[i].is_equipped = 0;
        }
    }

    // Equip the item
    slot->is_equipped = 1;
    return true;
}

bool unequip_item(Inventory_t* inventory, const char* item_id)
{
    if (inventory == NULL || item_id == NULL) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    Inventory_slot_t* slot = find_item_in_inventory(inventory, item_id);
    if (slot == NULL || !slot->is_equipped) {
        // TODO: Implement error handling for item not found or not equipped
        return false;
    }

    slot->is_equipped = 0;
    return true;
}

Inventory_slot_t* get_equipped_weapon(Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            inventory->slots[i].item.type == ITEM_TYPE_WEAPON &&
            inventory->slots[i].is_equipped) {
            return &inventory->slots[i];
        }
    }

    // TODO: Implement error handling for no weapon equipped
    return NULL;
}

Inventory_slot_t* get_equipped_armor(Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            inventory->slots[i].item.type == ITEM_TYPE_ARMOR &&
            inventory->slots[i].is_equipped) {
            return &inventory->slots[i];
        }
    }

    // TODO: Implement error handling for no armor equipped
    return NULL;
}

uint8_t get_inventory_free_slots(const Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return 0;
    }

    uint8_t free_slots = 0;
    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity == 0) {
            free_slots++;
        }
    }

    return free_slots;
}

uint8_t get_total_inventory_weight(const Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return 0;
    }

    float total_weight = 0.0f;
    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0) {
            float item_weight = get_item_weight(&inventory->slots[i].item);
            total_weight += item_weight * inventory->slots[i].quantity;
        }
    }

    // Convert to uint8_t, capping at 255
    return (total_weight > 255.0f) ? 255 : (uint8_t)total_weight;
}

bool is_inventory_full(const Inventory_t* inventory)
{
    if (inventory == NULL) {
        // TODO: Implement error handling for invalid input
        return true; // NULL inventory is considered "full"
    }

    return get_inventory_free_slots(inventory) == 0;
}

// =============================================================================
// ITEM USAGE & EFFECTS
// =============================================================================

bool use_consumable(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Check if the consumable has a valid on_consume callback
    if (consumable->on_consume == NULL) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    // Execute the consumable effect
    consumable->on_consume(consumable->value);

    // If the consumable has duration effects, those will be handled separately
    // by the game's timer system calling trigger_consumable_duration_tick()

    return true;
}

void trigger_consumable_duration_tick(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        // TODO: Implement error handling for invalid input
        return;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Only trigger if there's a duration tick callback and duration remaining
    if (consumable->on_duration_tick != NULL && consumable->duration_seconds > 0) {
        consumable->on_duration_tick(consumable->value);

        // Decrement duration (this assumes the function is called once per second)
        consumable->duration_seconds--;

        // If duration has ended, trigger the end effect
        if (consumable->duration_seconds == 0) {
            trigger_consumable_duration_end(item);
        }
    }
}

void trigger_consumable_duration_end(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        // TODO: Implement error handling for invalid input
        return;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Trigger the end effect if it exists
    if (consumable->on_duration_end != NULL) {
        consumable->on_duration_end(consumable->value);
    }

    // Reset duration to 0 to mark effect as ended
    consumable->duration_seconds = 0;
}

bool can_key_open_lock(const Item_t* key, const Lock_t* lock)
{
    if (key == NULL || lock == NULL) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    // Only keys can open locks
    if (key->type != ITEM_TYPE_KEY) {
        // TODO: Implement error handling for invalid input
        return false;
    }

    // Check if the lock is jammed
    if (lock->jammed_seconds > 0) {
        // TODO: Implement error handling for invalid input
        return false; // Can't open jammed locks
    }

    // For now, use simple string comparison of lock names/IDs
    // In a more complex system, you might have master keys, lock levels, etc.
    const Lock_t* key_lock = &key->data.key.lock;

    // Keys can open locks with matching names
    return strcmp(key_lock->name, lock->name) == 0;
}
