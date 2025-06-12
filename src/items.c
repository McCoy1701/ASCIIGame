#include "items.h"
#include "structs.h"
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

    // Copy strings safely
    strncpy(item->name, name, MAX_NAME_LENGTH - 1);
    item->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(item->id, id, MAX_ID_LENGTH - 1);
    item->id[MAX_ID_LENGTH - 1] = '\0';

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
    item->weight = 10; // Base weight
    item->value = min_dmg + max_dmg; // Base value
    item->stackable = 0; // Weapons don't stack

    // Initialize description and rarity
    strncpy(item->description, "A weapon made of ", MAX_DESCRIPTION_LENGTH - 1);
    strncat(item->description, material.name, MAX_DESCRIPTION_LENGTH - strlen(item->description) - 1);
    item->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

    strncpy(item->rarity, "common", MAX_ID_LENGTH - 1);
    item->rarity[MAX_ID_LENGTH - 1] = '\0';

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

    // Copy strings safely
    strncpy(item->name, name, MAX_NAME_LENGTH - 1);
    item->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(item->id, id, MAX_ID_LENGTH - 1);
    item->id[MAX_ID_LENGTH - 1] = '\0';

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
    item->weight = 15; // Base weight
    item->value = armor_val + evasion_val; // Base value
    item->stackable = 0; // Armor doesn't stack

    // Initialize description and rarity
    strncpy(item->description, "Armor made of ", MAX_DESCRIPTION_LENGTH - 1);
    strncat(item->description, material.name, MAX_DESCRIPTION_LENGTH - strlen(item->description) - 1);
    item->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

    strncpy(item->rarity, "common", MAX_ID_LENGTH - 1);
    item->rarity[MAX_ID_LENGTH - 1] = '\0';

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

    // Copy strings safely
    strncpy(item->name, name, MAX_NAME_LENGTH - 1);
    item->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(item->id, id, MAX_ID_LENGTH - 1);
    item->id[MAX_ID_LENGTH - 1] = '\0';

    // Set basic properties
    item->glyph = glyph;

    // Keys don't have materials, so initialize with neutral material
    strncpy(item->material_data.name, "metal", MAX_NAME_LENGTH - 1);
    item->material_data.name[MAX_NAME_LENGTH - 1] = '\0';
    item->material_data.weight_fact = 1.0f;
    item->material_data.min_damage_fact = 1.0f;
    item->material_data.max_damage_fact = 1.0f;
    item->material_data.armor_value_fact = 1.0f;
    item->material_data.evasion_value_fact = 1.0f;
    item->material_data.durability_fact = 1.0f;
    item->material_data.stealth_value_fact = 1.0f;
    item->material_data.enchant_value_fact = 1.0f;

    // Initialize key-specific data
    item->data.key.lock = lock;

    // Set default values
    item->weight = 1; // Keys are light
    item->value = 5; // Base value
    item->stackable = 1; // Keys can stack

    // Initialize description and rarity
    strncpy(item->description, "A key that opens: ", MAX_DESCRIPTION_LENGTH - 1);
    strncat(item->description, lock.name, MAX_DESCRIPTION_LENGTH - strlen(item->description) - 1);
    item->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

    strncpy(item->rarity, "common", MAX_ID_LENGTH - 1);
    item->rarity[MAX_ID_LENGTH - 1] = '\0';

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

    // Copy strings safely
    strncpy(item->name, name, MAX_NAME_LENGTH - 1);
    item->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(item->id, id, MAX_ID_LENGTH - 1);
    item->id[MAX_ID_LENGTH - 1] = '\0';

    // Set basic properties
    item->glyph = glyph;

    // Consumables don't have materials, so initialize with neutral material
    strncpy(item->material_data.name, "organic", MAX_NAME_LENGTH - 1);
    item->material_data.name[MAX_NAME_LENGTH - 1] = '\0';
    item->material_data.weight_fact = 1.0f;
    item->material_data.min_damage_fact = 1.0f;
    item->material_data.max_damage_fact = 1.0f;
    item->material_data.evasion_value_fact = 1.0f;
    item->material_data.armor_value_fact = 1.0f;
    item->material_data.durability_fact = 1.0f;
    item->material_data.stealth_value_fact = 1.0f;
    item->material_data.enchant_value_fact = 1.0f;

    // Initialize consumable-specific data
    item->data.consumable.on_consume = on_consume;
    item->data.consumable.on_duration_end = NULL; // Optional
    item->data.consumable.on_duration_tick = NULL; // Optional
    item->data.consumable.value = value;
    item->data.consumable.duration_seconds = 0; // Instant effect by default

    // Set default values
    item->weight = 1; // Consumables are light
    item->value = value; // Value based on effect strength
    item->stackable = 10; // Can stack up to 10

    // Initialize description and rarity
    strncpy(item->description, "A consumable item with magical properties", MAX_DESCRIPTION_LENGTH - 1);
    item->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

    strncpy(item->rarity, "common", MAX_ID_LENGTH - 1);
    item->rarity[MAX_ID_LENGTH - 1] = '\0';

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

    // Copy strings safely
    strncpy(item->name, name, MAX_NAME_LENGTH - 1);
    item->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(item->id, id, MAX_ID_LENGTH - 1);
    item->id[MAX_ID_LENGTH - 1] = '\0';

    // Set basic properties
    item->glyph = glyph;
    item->material_data = material;

    // Initialize ammunition-specific data
    item->data.ammo.min_damage = min_dmg;
    item->data.ammo.max_damage = max_dmg;

    // Set default values (will be modified by material factors)
    item->weight = 1; // Ammo is light
    item->value = (min_dmg + max_dmg) / 2; // Base value
    item->stackable = 50; // Ammo stacks well

    // Initialize description and rarity
    strncpy(item->description, "Ammunition made of ", MAX_DESCRIPTION_LENGTH - 1);
    strncat(item->description, material.name, MAX_DESCRIPTION_LENGTH - strlen(item->description) - 1);
    item->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

    strncpy(item->rarity, "common", MAX_ID_LENGTH - 1);
    item->rarity[MAX_ID_LENGTH - 1] = '\0';

    return item;
}

void destroy_item(Item_t* item)
{
    if (item == NULL) {
        return;
    }

    free(item);
}
