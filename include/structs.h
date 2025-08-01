/*
 * structs.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "defs.h"
#include "Archimedes.h"
#include "Daedalus.h"

#include <stdint.h>
#include <stdint.h>

enum
{
  APOLLO_PALETE = 0
};

enum
{
  WORLD_LEVEL = 0,
  REGION_LEVEL,
  LOCAL_LEVEL

};

typedef enum
{
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_KEY,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_AMMUNITION

} ItemType_t;

typedef struct
{
  SDL_Rect rects[MAX_GLYPHS];
  SDL_Texture* texture;
  int count;

} GlyphArray_t;

// World Building Structs
typedef struct
{
  uint16_t glyph;
  uint8_t temperature;
  uint8_t elevation;
  uint8_t is_passable;
  uint8_t fg;
  uint8_t bg;

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

  uint8_t world_width;
  uint8_t world_height;
  uint8_t region_width;
  uint8_t region_height;
  uint8_t local_width;
  uint8_t local_height;
  uint8_t z_height;
  
  // floats (1 is 100%, 0.5 is 50%, etc.)
  float temperature_factor; // every region in this world cell
  float elevation_factor; // every region in this world cell
  
} World_t;

typedef struct
{
  char magic[8];
  uint16_t version;
  uint8_t world_width, world_height;
  uint8_t region_width, region_height;
  uint8_t local_width, local_height;
  uint8_t z_height;

} FileHeader_t;

// World Position 'world-index:region-index:local-index:z'
typedef struct // World_Position_t
{
  // refer to in game tiles as 'world-index:region-index:z'
  uint8_t world_index;
  uint8_t region_index;
  uint16_t local_index;
  uint8_t local_z;
  uint8_t x, y;
  uint8_t level;  //TODO: needs moved out of here and put into RenderWorldBuffer_t once created

} WorldPosition_t;

// World Objects
typedef struct // WorldObject_t
{
    dString_t* name;
    dString_t* description;
    WorldPosition_t pos;

} WorldObject_t;

typedef struct
{
  int* data;
  int count;
  uint8_t w, h;
  uint8_t world_index;
  uint8_t region_index;
  uint16_t local_index;
  uint8_t level;
} GameTileArray_t;

typedef struct // Lock_t
{
  dString_t* name;
  dString_t* description;

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
    dString_t* name;

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

    dString_t* description;
    dString_t* name;
    dString_t* id;
    dString_t* rarity;

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

