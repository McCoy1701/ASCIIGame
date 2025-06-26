#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "Daedalus.h"
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

     // BUG FIX: Check if material->name is NULL
     if (material->name == NULL) {
         return false;  // or provide a default description
     }

     dString_t* temp = d_InitString();
     if (temp == NULL) {
         return false;
     }

     d_AppendString(temp, "A weapon made of ", 0);
     d_AppendString(temp, material->name->str, 0);  // Now safe!
     *dest = *temp;
     free(temp);
     return true;
 }
 // Helper function to determine if item resists durability loss
 // Returns true if item should NOT lose durability
 static bool item_resists_durability_loss(const Item_t* item)
 {
     if (item == NULL) {
         return false;
     }

     // Base chance is 5% to avoid durability loss
     float base_chance = 0.05f;
     float resistance_chance = base_chance * item->material_data.properties.durability_fact;

     // Cap at 95% maximum resistance (always have some chance of wear)
     if (resistance_chance > 0.95f) {
         resistance_chance = 0.95f;
     }

     // Generate random number between 0.0 and 1.0
     float random_roll = (float)rand() / (float)RAND_MAX;

     return random_roll < resistance_chance;
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
    d_AppendString(temp, material->name->str, 0);
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
     LOG("Entering _populate_key_desc");
     if (dest == NULL || lock == NULL) {
         LOG("Error: NULL pointer passed to _populate_key_desc");
         return false;
     }

     // Check if lock->name is valid
     if (lock->name == NULL || lock->name->str == NULL) {
         LOG("Error: Lock name is NULL");
         return false;
     }

     // Directly populate the destination string
     d_AppendString(dest, "A key that opens: ", 0);
     d_AppendString(dest, lock->name->str, 0);

     return true;
 }


/*
 * Helper function to create a default neutral material for keys
 */
static Material_t _create_default_material(void) {
    Material_t material;

    material.name = d_InitString();
    d_AppendString(material.name, "default", 0);

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
    material.name = d_InitString();
    if (!material.name) {
        LOG("Consumable creation failed; Failed to allocate memory for material name");
        return material;
    }
    if (!_populate_string_field(material.name, "organic")) {
        LOG("Consumable creation failed; Failed to populate material name");
        free(material.name->str);
        free(material.name);
        return material;
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
    d_AppendString(temp, material->name->str, 0);
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
        LOG("Invalid Weapon name or ID provided");
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        LOG("Failed to allocate memory for Weapon");
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_WEAPON;

    // Populate name using helper
    item->name = d_InitString();
    if (!_populate_string_field(item->name, name)) {
        free(item);
        LOG("Failed to allocate memory for Weapon name");
        return NULL;
    }

    // Populate id using helper
    item->id = d_InitString();
    if (!_populate_string_field(item->id, id)) {
        LOG("Failed to allocate memory for Weapon ID");
        free(item->name->str);
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
    item->description = d_InitString();
    if (!_populate_weapon_desc(item->description, &material)) {
        LOG("Failed to allocate memory for Weapon Description");
        free(item->name->str);
        free(item->id->str);
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (!_populate_rarity(item->rarity, "common")) {
        LOG("Failed to allocate memory for Rarity");
        free(item->name->str);
        free(item->id->str);
        free(item->description->str);
        free(item);
        return NULL;
    }

    return item;
}

Item_t* create_armor(const char* name, const char* id, Material_t material,
                    uint8_t armor_val, uint8_t evasion_val, char glyph,
                    uint8_t stealth_val, uint8_t enchant_val)
{
    if (name == NULL || id == NULL) {
        LOG("Invalid name or id provided");
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        LOG("Failed to allocate memory for Armor");
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_ARMOR;

    // Populate name and id using helpers
    item->name = d_InitString();
    if (!_populate_string_field(item->name, name)) {
        free(item);
        LOG("Failed to allocate memory for name");
        return NULL;
    }

    item->id = d_InitString();
    if (!_populate_string_field(item->id, id)) {
        free(item->name->str);
        free(item);
        LOG("Failed to allocate memory for id");
        return NULL;
    }

    // Set basic properties
    item->glyph = glyph;
    item->material_data = material;

    // Initialize armor-specific data
    item->data.armor.armor_value = armor_val;
    item->data.armor.evasion_value = evasion_val;
    item->data.armor.stealth_value = stealth_val;
    item->data.armor.enchant_value = enchant_val;
    item->data.armor.durability = 255; // 100% durability

    // Set default values (will be modified by material factors)
    item->weight_kg = 1.0f; // Base weight
    item->value_coins = armor_val + evasion_val; // Base value
    item->stackable = 0; // Armor doesn't stack

    // Populate description using helper
    item->description = d_InitString();
    if (!_populate_armor_desc(item->description, &material)) {
        free(item->name->str);
        free(item->id->str);
        free(item);
        LOG("Failed to allocate memory for description");
        return NULL;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (!_populate_rarity(item->rarity, "common")) {
        free(item->name->str);
        free(item->id->str);
        free(item->description->str);
        free(item);
        LOG("Failed to allocate memory for rarity");
        return NULL;
    }

    return item;
}

Item_t* create_key(const char* name, const char* id, Lock_t lock, char glyph)
{
    dString_t* log_message = d_InitString();
    d_AppendString(log_message, "Creating key: ", 0);
    d_AppendString(log_message, name, 0);
    d_AppendString(log_message, " (", 0);
    d_AppendString(log_message, id, 0);
    d_AppendString(log_message, ")", 0);
    LOG(log_message->str);
    d_DestroyString(log_message);

    if (name == NULL || id == NULL) {
        LOG("Invalid name or id provided for key");
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        LOG("Failed to allocate memory for key");
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_KEY;

    LOG("Item->type = ITEM_TYPE_KEY");

    // Populate name and id using helpers
    item->name = d_InitString();
    if (!_populate_string_field(item->name, name)) {
        LOG("Failed to allocate memory for key name");
        free(item);
        return NULL;
    }
    dString_t* log_message2 = d_InitString();
    d_AppendString(log_message2, "Item->name = ", 0);
    d_AppendString(log_message2, item->name->str, 0);
    LOG(log_message2->str);
    d_DestroyString(log_message2);

    item->id = d_InitString();
    if (!_populate_string_field(item->id, id)) {
        LOG("Failed to allocate memory for key id");
        free(item->name->str);
        free(item);
        return NULL;
    }
    dString_t* log_message3 = d_InitString();
    d_AppendString(log_message3, "Item->id = ", 0);
    d_AppendString(log_message3, item->id->str, 0);
    LOG(log_message3->str);
    d_DestroyString(log_message3);

    // Set basic properties
    item->glyph = glyph;

    // Keys don't have materials, so use default neutral material
    item->material_data = _create_default_material();
    dString_t* log_message4 = d_InitString();
    d_AppendString(log_message4, "Item->material_data = ", 0);
    d_AppendString(log_message4, item->material_data.name->str, 0);
    LOG(log_message4->str);
    d_DestroyString(log_message4);

    // Initialize key-specific data - DEEP COPY the lock
    item->data.key.lock.name = d_InitString();
    d_AppendString(item->data.key.lock.name, lock.name->str, 0);

    item->data.key.lock.description = d_InitString();
    d_AppendString(item->data.key.lock.description, lock.description->str, 0);

    item->data.key.lock.pick_difficulty = lock.pick_difficulty;
    item->data.key.lock.jammed_seconds = lock.jammed_seconds;

    // Set default values
    item->weight_kg = 0.1f; // Keys are light but not weightless
    item->value_coins = 5; // Base value
    item->stackable = 1; // Keys cannot stack (though this seems like it should be 0?)

    // Populate description using helper
    item->description = d_InitString();
    if (!_populate_key_desc(item->description, &lock)) {
        LOG("Failed to allocate memory for key description");
        free(item->name->str);
        free(item->id->str);
        // Clean up material name if it was allocated
        if (item->material_data.name->str != NULL) {
            free(item->material_data.name->str);
        }
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (!_populate_rarity(item->rarity, "common")) {
        LOG("Failed to allocate memory for key rarity");
        free(item->name->str);
        free(item->id->str);
        free(item->description->str);
        if (item->material_data.name->str != NULL) {
            free(item->material_data.name->str);
        }
        free(item);
        return NULL;
    }

    return item;
}

Lock_t create_lock(const char* name, const char* description, uint8_t pick_difficulty, uint8_t jammed_seconds)
{
    Lock_t lock;

    // Initialize dString_t fields
    lock.name = d_InitString();
    if (lock.name == NULL) {
        LOG("Lock name string initialization failed");
        // Return empty lock on allocation failure
        Lock_t empty_lock = {0};
        return empty_lock;
    }

    lock.description = d_InitString();
    if (lock.description == NULL) {
        LOG("Lock description string initialization failed");
        // Clean up and return empty lock
        d_DestroyString(lock.name);
        Lock_t empty_lock = {0};
        return empty_lock;
    }

    // Populate string fields
    if (name != NULL) {
        d_AppendString(lock.name, name, 0);
    }

    if (description != NULL) {
        d_AppendString(lock.description, description, 0);
    }

    // Set numeric properties
    lock.pick_difficulty = pick_difficulty;
    lock.jammed_seconds = jammed_seconds;

    return lock;
}

/*
 * Destroys a lock and frees its memory
 */
void destroy_lock(Lock_t* lock)
{
    if (lock == NULL) {
        LOG("Destruction Failed; Lock is NULL");
        return;
    }

    if (lock->name != NULL) {
        d_DestroyString(lock->name);
        lock->name = NULL;
    }

    if (lock->description != NULL) {
        d_DestroyString(lock->description);
        lock->description = NULL;
    }

    lock->pick_difficulty = 0;
    lock->jammed_seconds = 0;
}

Item_t* create_consumable(const char* name, const char* id, uint8_t value,
                         void (*on_consume)(uint8_t), char glyph)
{
    if (name == NULL || id == NULL || on_consume == NULL) {
        LOG("Consumable creation failed; Invalid parameters");
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        LOG("Consumable creation failed; Memory allocation failed");
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_CONSUMABLE;

    // Populate name and id using helpers
    item->name = d_InitString();
    if (!_populate_string_field(item->name, name)) {
        LOG("Consumable creation failed; Failed to populate name");
        free(item);
        return NULL;
    }

    item->id = d_InitString();
    if (!_populate_string_field(item->id, id)) {
        LOG("Consumable creation failed; Failed to populate id");
        free(item->name->str);
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
    item->description = d_InitString();
    if (!_populate_consumable_desc(item->description, item->data.consumable.value)) {
        LOG("Consumable creation failed; Failed to populate description");
        free(item->name->str);
        free(item->id->str);
        // Clean up material name if it was allocated
        if (item->material_data.name->str != NULL) {
            free(item->material_data.name->str);
        }
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (!_populate_rarity(item->rarity, "common")) {
        LOG("Consumable creation failed; Failed to populate rarity");
        free(item->name->str);
        free(item->id->str);
        free(item->description->str);
        if (item->material_data.name->str != NULL) {
            free(item->material_data.name->str);
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
        LOG("Create ammunition failed; Invalid name or id");
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        LOG("Create ammunition failed; Failed to allocate memory");
        return NULL;
    }

    // Set item type
    item->type = ITEM_TYPE_AMMUNITION;

    // Populate name and id using helpers
    item->name = d_InitString();
    if (!_populate_string_field(item->name, name)) {
        LOG("Create ammunition failed; Failed to allocate memory for name");
        free(item);
        return NULL;
    }

    item->id = d_InitString();
    if (!_populate_string_field(item->id, id)) {
        LOG("Create ammunition failed; Failed to allocate memory for id");
        free(item->name->str);
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
    item->description = d_InitString();
    if (!_populate_ammunition_desc(item->description, &material, min_dmg, max_dmg)) {
        LOG("Create ammunition failed; Failed to allocate memory for description");
        free(item->name->str);
        free(item->id->str);
        free(item);
        return NULL;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (!_populate_rarity(item->rarity, "common")) {
        LOG("Create ammunition failed; Failed to allocate memory for rarity");
        free(item->name->str);
        free(item->id->str);
        free(item->description->str);
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
    material.name = d_InitString();
    if (name == NULL) {
        LOG("Material name is NULL");
        // Return a default material if name is NULL
        _populate_string_field(material.name, "Default Material");
        material.properties = create_default_material_properties();
        return material;
    }

    // Copy name safely
    _populate_string_field(material.name, name);

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
    // NOTE: Durability is no longer modified by material - stays at 255
    weapon->stealth_value = (uint8_t)(weapon->stealth_value * material->properties.stealth_value_fact);
    weapon->enchant_value = (uint8_t)(weapon->enchant_value * material->properties.enchant_value_fact);

    // Apply to base item properties with bounds checking
    float new_weight = item->weight_kg * material->properties.weight_fact;
    item->weight_kg = (new_weight < 0.0f) ? 0.0f : new_weight;

    float new_value = item->value_coins * material->properties.value_coins_fact;
    item->value_coins = (new_value < 0.0f) ? 0 : (uint8_t)new_value;
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

    // Apply to base item properties with bounds checking
    float new_weight = item->weight_kg * material->properties.weight_fact;
    item->weight_kg = (new_weight < 0.0f) ? 0.0f : new_weight;

    float new_value = item->value_coins * material->properties.value_coins_fact;
    item->value_coins = (new_value < 0.0f) ? 0 : (uint8_t)new_value;
}

void apply_material_to_ammunition(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        return;
    }

    Ammunition__Item_t* ammo = &item->data.ammo;
    Material_t* material = &item->material_data;

    // Apply material factors to ammunition stats
    ammo->min_damage = (uint8_t)(ammo->min_damage * material->properties.min_damage_fact);
    ammo->max_damage = (uint8_t)(ammo->max_damage * material->properties.max_damage_fact);

    // Apply to base item properties with bounds checking
    float new_weight = item->weight_kg * material->properties.weight_fact;
    item->weight_kg = (new_weight < 0.0f) ? 0.0f : new_weight;

    float new_value = item->value_coins * material->properties.value_coins_fact;
    item->value_coins = (new_value < 0.0f) ? 0 : (uint8_t)new_value;
}

float calculate_final_weight(const Item_t* item)
{
    if (item == NULL) {
        LOG("calculate_final_weight: Item is NULL");
        return 0.0f;
    }

    // Weight is already calculated with material factors applied
    return item->weight_kg;
}

uint8_t calculate_final_value(const Item_t* item)
{
    if (item == NULL) {
        LOG("calculate_final_value: Item is NULL");
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
        LOG("get_weapon_min_damage: Item is NULL or not a weapon");
        return 0;
    }
    return item->data.weapon.min_damage;
}

uint8_t get_weapon_max_damage(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        LOG("get_weapon_max_damage: Item is NULL or not a weapon");
        return 0;
    }
    return item->data.weapon.max_damage;
}
uint8_t get_ammunition_min_damage(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        LOG("get_ammunition_min_damage: Item is NULL or not ammunition");
        return 0;
    }
    return item->data.ammo.min_damage;
}

uint8_t get_ammunition_max_damage(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        LOG("get_ammunition_max_damage: Item is NULL or not ammunition");
        return 0;
    }
    return item->data.ammo.max_damage;
}

uint8_t get_weapon_range(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        LOG("get_weapon_range: Item is NULL or not a weapon");
        return 0;
    }
    return item->data.weapon.range_tiles;
}

bool weapon_needs_ammo(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        LOG("weapon_needs_ammo: Item is NULL or not a weapon");
        return false;
    }

    // Ranged weapons (range > 0) need ammunition
    return item->data.weapon.range_tiles > 0;
}

bool weapon_can_use_ammo(const Item_t* weapon, const Item_t* ammo)
{
    if (weapon == NULL || ammo == NULL) {
        LOG("weapon_can_use_ammo: Item is NULL");
        return false;
    }

    if (weapon->type != ITEM_TYPE_WEAPON || ammo->type != ITEM_TYPE_AMMUNITION) {
        LOG("weapon_can_use_ammo: Type mismatch");
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
        LOG("get_armor_value: Item is NULL or not armor");
        return 0;
    }
    return item->data.armor.armor_value;
}

uint8_t get_evasion_value(const Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        LOG("get_evasion_value: Item is NULL or not armor");
        return 0;
    }
    return item->data.armor.evasion_value;
}

// General item properties
float get_item_weight(const Item_t* item)
{
    if (item == NULL) {
        LOG("get_item_weight: Item is NULL");
        return 0.0f;
    }
    return item->weight_kg;
}

uint8_t get_item_value_coins(const Item_t* item)
{
    if (item == NULL) {
        LOG("get_item_value_coins: Item is NULL");
        return 0;
    }
    return item->value_coins;
}

uint8_t get_stealth_value(const Item_t* item)
{
    if (item == NULL) {
        LOG("get_stealth_value: Item is NULL");
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
        LOG("get_durability: Item is NULL");
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
        LOG("is_item_stackable: Item is NULL");
        return false;
    }

    // stackable value: 0 = not stackable, 1+ = stackable
    return item->stackable > 0;
}

uint8_t get_max_stack_size(const Item_t* item)
{
    if (item == NULL) {
        LOG("get_max_stack_size: Item is NULL");
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
        LOG("damage_item_durability: Item is NULL");
        return;
    }

    // Check if item resists durability loss due to material properties
    if (item_resists_durability_loss(item)) {
        LOG("Item resisted durability loss due to material properties");
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
        LOG("repair_item: Item is NULL");
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
        LOG("is_item_broken: Item is NULL");
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
        LOG("get_durability_percentage: Item is NULL");
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
        LOG("create_inventory: Invalid inventory size");
        return NULL; // Invalid inventory size
    }

    Inventory_t* inventory = (Inventory_t*)malloc(sizeof(Inventory_t));
    if (inventory == NULL) {
        LOG("create_inventory: Memory allocation failed");
        return NULL; // Memory allocation failed
    }

    inventory->slots = (Inventory_slot_t*)calloc(size, sizeof(Inventory_slot_t));
    if (inventory->slots == NULL) {
        free(inventory);
        LOG("create_inventory: Memory allocation failed");
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
        LOG("destroy_inventory: Invalid inventory");
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
        LOG("add_item_to_inventory: Invalid input");
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
            LOG("Adding items to inventory failed: No empty slots");
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
        LOG("Invalid input for remove_item_from_inventory");
        return false;
    }

    uint8_t remaining_to_remove = quantity;

    // Find and remove items with matching ID
    for (uint8_t i = 0; i < inventory->size && remaining_to_remove > 0; i++) {
        if (inventory->slots[i].quantity > 0 &&
            strcmp(inventory->slots[i].item.id->str, item_id) == 0) {

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
        LOG("Invalid input for find_item_in_inventory");
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            strcmp(inventory->slots[i].item.id->str, item_id) == 0) {
            return &inventory->slots[i];
        }
    }
    LOG("Item not found in inventory");
    return NULL; // Item not found
}

bool can_stack_items(const Item_t* item1, const Item_t* item2)
{
    if (item1 == NULL || item2 == NULL) {
        LOG("Invalid input for can_stack_items");
        return false;
    }

    // Items can stack if they have the same ID and are stackable
    return (strcmp(item1->id->str, item2->id->str) == 0) && is_item_stackable(item1);
}

bool equip_item(Inventory_t* inventory, const char* item_id)
{
    if (inventory == NULL || item_id == NULL) {
        LOG("Invalid input for equip_item");
        return false;
    }

    Inventory_slot_t* slot = find_item_in_inventory(inventory, item_id);
    if (slot == NULL) {
        LOG("Item not found in inventory for equip_item");
        return false;
    }

    // Only weapons and armor can be equipped
    if (slot->item.type != ITEM_TYPE_WEAPON && slot->item.type != ITEM_TYPE_ARMOR) {
        LOG("Invalid item type for equip_item");
        return false;
    }

    // Check if item is broken
    if (is_item_broken(&slot->item)) {
        LOG("Item is broken for equip_item");
        return false;
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
        LOG("Invalid input for unequip_item");
        return false;
    }

    Inventory_slot_t* slot = find_item_in_inventory(inventory, item_id);
    if (slot == NULL || !slot->is_equipped) {
        LOG("Item not found or not equipped for unequip_item");
        return false;
    }

    slot->is_equipped = 0;
    return true;
}
Inventory_slot_t* get_equipped_weapon(Inventory_t* inventory)
{
    if (inventory == NULL) {
        LOG("Invalid input for get_equipped_weapon");
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            inventory->slots[i].item.type == ITEM_TYPE_WEAPON &&
            inventory->slots[i].is_equipped) {

            // Check if the equipped weapon is broken
            if (is_item_broken(&inventory->slots[i].item)) {
                LOG("Auto-unequipping broken weapon");
                inventory->slots[i].is_equipped = false;
                return NULL;
            }

            return &inventory->slots[i];
        }
    }

    return NULL;
}


Inventory_slot_t* get_equipped_armor(Inventory_t* inventory)
{
    if (inventory == NULL) {
        return NULL;
    }

    for (uint8_t i = 0; i < inventory->size; i++) {
        if (inventory->slots[i].quantity > 0 &&
            inventory->slots[i].item.type == ITEM_TYPE_ARMOR &&
            inventory->slots[i].is_equipped) {

            // Check if the equipped armor is broken
            if (is_item_broken(&inventory->slots[i].item)) {
                LOG("Auto-unequipping broken armor");
                inventory->slots[i].is_equipped = false;
                return NULL;
            }

            return &inventory->slots[i];
        }
    }
    return NULL;
}


uint8_t get_inventory_free_slots(const Inventory_t* inventory)
{
    if (inventory == NULL) {
        LOG("Invalid input for get_inventory_free_slots");
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
        LOG("Invalid input for get_total_inventory_weight");
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
        LOG("Invalid input for is_inventory_full");
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
        LOG("Invalid input for use_consumable");
        return false;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Check if the consumable has a valid on_consume callback
    if (consumable->on_consume == NULL) {
        LOG("Invalid input for use_consumable");
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
        LOG("Invalid input for trigger_consumable_duration_tick");
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
        LOG("Invalid input for trigger_consumable_duration_end");
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
        LOG("Can Key Open Lock? No, because key or lock is NULL");
        return false;
    }

    // Only keys can open locks
    if (key->type != ITEM_TYPE_KEY) {
        LOG("Can Key Open Lock? No, because key is not a key");
        return false;
    }

    // Check if the lock is jammed
    if (lock->jammed_seconds > 0) {
        LOG("Can Key Open Lock? No, because lock is jammed");
        return false; // Can't open jammed locks
    }

    // For now, use simple string comparison of lock names/IDs
    // In a more complex system, you might have master keys, lock levels, etc.
    const Lock_t* key_lock = &key->data.key.lock;

    // Keys can open locks with matching names
    return strcmp(key_lock->name->str, lock->name->str) == 0;
}
