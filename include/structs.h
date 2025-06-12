#include "defs.h"
#include <stdint.h>

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

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
  char glyph;
  uint8_t temperature;
  uint8_t elevation;
  uint8_t is_passable;
} GameTile_t;

typedef struct
{
    GameTile_t* tiles; //Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE
    // There are too many local cells to warrant their own id/name/factors
    // Each local cell will take the factors as the region and world cell it is nested in
} LocalCell_t;

typedef struct
{
  LocalCell_t* cells; //REGION_SIZE * REGION_SIZE

  char name[MAX_NAME_LENGTH];
  char id[MAX_ID_LENGTH]; // refer to in game tiles as 'world-id:region-id:x/y:x/y'

  // float (1 is 100%, 0.5 is 50%, etc.)
  float temperature_factor; // every local cell in this region
  float elevation_factor; // every local cell in this region

} RegionCell_t;

typedef struct
{
    RegionCell_t* regions; //WORLD_HEIGHT * WORLD_WIDTH

    char name[MAX_NAME_LENGTH];
    char id[MAX_ID_LENGTH]; // refer to in game tiles as 'world-id:region-id:x/y:x/y'

    // floats (1 is 100%, 0.5 is 50%, etc.)
    float temperature_factor; // every region in this world cell
    float elevation_factor; // every region in this world cell

} World_t;

// World Position 'world-id:region-id:x/y:x/y'

typedef struct {
    char world_id[MAX_ID_LENGTH];
    char region_id[MAX_ID_LENGTH];
    // refer to in game tiles as 'world-id:region-id:x/y:x/y'
    int16_t local_x;
    int16_t local_y;
    // refer to in game tiles as 'world-id:region-id:x/y:x/y'
    int16_t game_x;
    int16_t game_y;
} World_Position_t;

// World Objects

typedef struct {

} WorldObject_t;

typedef struct
{
  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];

  uint8_t pick_difficulty; // if its 255 it is unpickable
  // TODO: make a timer system and integrate jammed
  uint8_t jammed_seconds; // 0 is unjammed
} Lock_t;

typedef struct {
    // Every Item in our game will have a Material
    // The material will effect properties like weight, damage, armor, stealth, and enchantment
    char name[MAX_NAME_LENGTH];

    // floats (1 is 100%, 0.5 is 50%, etc.)
    float weight_fact;
    float damage_fact;
    float armor_fact;
    float durability_fact;
    float stealth_fact;
    float enchant_fact;
} Material_t;

// Inventory

// -- Different Types of Items

typedef struct {
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

typedef struct
{
    uint8_t armor_value;
    uint8_t evasion_value;
    uint8_t stealth_value; // TODO: add a stealth system (armor check on move)
    uint8_t enchant_value; // TODO: add a enchant system
    uint8_t durability; // 255 is 100%
} Armor__Item_t;

typedef struct {
    Lock_t lock;
} Key__Item_t;

typedef struct {
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
    char id[MAX_ID_LENGTH];
    char rarity[MAX_ID_LENGTH];

    uint8_t weight;
    uint8_t value;
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
