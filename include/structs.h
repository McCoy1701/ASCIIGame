#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <stdint.h>
#include "defs.h"

typedef enum {
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_KEY,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_AMMUNITION
} ItemType_t;

// World Building Structs
typedef struct
{
  uint16_t glyph;
  uint8_t temperature;
  uint8_t elevation;
  uint8_t is_passable;
} GameTile_t;

typedef struct
{
  GameTile_t* tiles;
  GameTile_t tile;
  
  float temperature_factor; // every local cell in this region
  float elevation_factor; // every local cell in this region
  // float (1 is 100%, 0.5 is 50%, etc.)
} RegionCell_t;

typedef struct
{
  RegionCell_t* regions;
  GameTile_t tile;

  uint16_t world_size;
  uint16_t region_size;
  uint16_t local_size;
  
  // floats (1 is 100%, 0.5 is 50%, etc.)
  float temperature_factor; // every region in this world cell
  float elevation_factor; // every region in this world cell
} World_t;

typedef struct
{
  char magic[8];
  uint32_t version;
  uint16_t world_index;
  uint16_t region_index;
  uint32_t local_index;
  int32_t  local_x, local_y, local_z;
  uint16_t world_size;
  uint16_t region_size;
  uint32_t local_size;
  uint16_t z_height;
} FileHeader_t;

// World Position 'world-id:region-id:x/y:x/y'
typedef struct // World_Position_t
{
  uint16_t world_index;
  uint16_t region_index;
  uint32_t local_index;
  // refer to in game tiles as 'world-id:region-id:x/y:x/y'
  int16_t local_x, local_y, local_z;
} World_Position_t;

// World Objects
typedef struct // WorldObject_t
{
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
} WorldObject_t;

typedef struct // Lock_t
{
  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];

  uint8_t pick_difficulty; // if its 255 it is unpickable
  // TODO: make a timer system and integrate jammed
  uint8_t jammed_seconds; // 0 is unjammed
} Lock_t;

typedef struct // MaterialProperties_t
{
    // floats (1 is 100%, 0.5 is 50%, etc.)
    float weight_fact;
    float value_coins_fact;
    float durability_fact;
    float min_damage_fact;
    float max_damage_fact;
    float armor_value_fact;
    float evasion_value_fact;
    float stealth_value_fact;
    float enchant_value_fact;
} MaterialProperties_t;

typedef struct // Material_t
{
    // Every Item in our game will have a Material
    // The material will effect properties like weight, damage, armor, stealth, and enchantment
    char name[MAX_NAME_LENGTH];

    MaterialProperties_t properties;
} Material_t;

// Inventory

// -- Different Types of Items

typedef struct // Ammunition__Item_t
{
    uint8_t min_damage; // will be added to bows/ranged weapons
    uint8_t max_damage;
} Ammunition__Item_t;

typedef struct
{
    Ammunition__Item_t ammo; // if none, no ammo needed

    uint8_t min_damage;
    uint8_t max_damage;
    uint8_t range_tiles; // how many tiles the weapon can reach (0 is melee)
    uint8_t stealth_value; // TODO: add a stealth system (weapon check on attack)
    uint8_t enchant_value; // TODO: add a enchant system
    uint8_t durability; // 255 is 100%
} Weapon__Item_t;

typedef struct // Armor__Item_t
{
    uint8_t armor_value;
    uint8_t evasion_value;
    uint8_t stealth_value; // TODO: add a stealth system (armor check on move)
    uint8_t enchant_value; // TODO: add a enchant system
    uint8_t durability; // 255 is 100%
} Armor__Item_t;

typedef struct // Key__Item_t
{
    Lock_t lock;
} Key__Item_t;

typedef struct // Consumable__Item_t
{
    // Callback functions
    void (*on_consume)(uint8_t value); // e.g HealPlayer(10)
    void (*on_duration_end)(uint8_t value); // e.g RemoveStrength(10)
    void (*on_duration_tick)(uint8_t value); // e.g DamagePlayer(1)
    // The above functions will take a value parameter
    uint8_t value;

    uint16_t duration_seconds; // TODO: implement timer logic

} Consumable__Item_t;

typedef struct
{
    ItemType_t type;
    union {
        Weapon__Item_t weapon;
        Armor__Item_t armor;
        Key__Item_t key;
        Consumable__Item_t consumable;
        Ammunition__Item_t ammo;
    } data;

    Material_t material_data; // materials affect the properties of the item

    char glyph;

    char description[MAX_DESCRIPTION_LENGTH]; // TODO: make new string in archmedes
    char name[MAX_NAME_LENGTH]; // TODO: make new string in archmedes
    char id[MAX_ID_LENGTH]; // get_item_by_id() will return the item in .dat or an error
    char rarity[MAX_ID_LENGTH];

    float weight_kg;

    uint8_t value_coins;
    uint8_t stackable; // 0 or 1 cannot stack , 255 is max stackable
} Item_t;

// -- Final Inventory and Slots

typedef struct
{
    Item_t item;

    uint8_t quantity;
    uint8_t is_equipped; // 0 or >= 1 bool
} Inventory_slot_t;

typedef struct
{
    Inventory_slot_t* slots;

    uint8_t size;
} Inventory_t;

#endif

