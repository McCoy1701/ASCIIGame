#include "structs.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef __ITEMS_H__
#define __ITEMS_H__

// =============================================================================
// ITEM CREATION & DESTRUCTION
// =============================================================================

// Item constructors
Item_t* create_weapon(const char* name, const char* id, Material_t material,
                     uint8_t min_dmg, uint8_t max_dmg, uint8_t range, char glyph);
Item_t* create_armor(const char* name, const char* id, Material_t material,
                    uint8_t armor_val, uint8_t evasion_val, char glyph);
Item_t* create_key(const char* name, const char* id, Lock_t lock, char glyph);
Item_t* create_consumable(const char* name, const char* id, uint8_t value,
                         void (*on_consume)(uint8_t), char glyph);
Item_t* create_ammunition(const char* name, const char* id, Material_t material,
                         uint8_t min_dmg, uint8_t max_dmg, char glyph);

// Item destruction
void destroy_item(Item_t* item);

// =============================================================================
// ITEM TYPE CHECKING & ACCESS
// =============================================================================

// Type checking functions
bool is_weapon(const Item_t* item);
bool is_armor(const Item_t* item);
bool is_key(const Item_t* item);
bool is_consumable(const Item_t* item);
bool is_ammunition(const Item_t* item);

// Safe data access functions
Weapon__Item_t* get_weapon_data(Item_t* item);
Armor__Item_t* get_armor_data(Item_t* item);
Key__Item_t* get_key_data(Item_t* item);
Consumable__Item_t* get_consumable_data(Item_t* item);
Ammunition__Item_t* get_ammunition_data(Item_t* item);

// =============================================================================
// MATERIAL SYSTEM
// =============================================================================

// Material creation
Material_t create_material(const char* name, MaterialProperties_t properties);
MaterialProperties_t create_default_material_properties(void);

// Apply material effects to items
void apply_material_to_weapon(Item_t* item);
void apply_material_to_armor(Item_t* item);
float calculate_final_weight(const Item_t* item);
uint8_t calculate_final_value(const Item_t* item);

// =============================================================================
// ITEM PROPERTIES & STATS
// =============================================================================

// Weapon stats
uint8_t get_weapon_min_damage(const Item_t* item);
uint8_t get_weapon_max_damage(const Item_t* item);
uint8_t get_weapon_range(const Item_t* item);
bool weapon_needs_ammo(const Item_t* item);
bool weapon_can_use_ammo(const Item_t* weapon, const Item_t* ammo);

// Armor stats
uint8_t get_armor_value(const Item_t* item);
uint8_t get_evasion_value(const Item_t* item);

// General item properties
float get_item_weight(const Item_t* item);
uint8_t get_item_value_coins(const Item_t* item);
uint8_t get_stealth_value(const Item_t* item);
uint8_t get_durability(const Item_t* item);
bool is_item_stackable(const Item_t* item);
uint8_t get_max_stack_size(const Item_t* item);

// =============================================================================
// DURABILITY SYSTEM
// =============================================================================

void damage_item_durability(Item_t* item, uint16_t damage);
void repair_item(Item_t* item, uint16_t repair_amount);
bool is_item_broken(const Item_t* item);
float get_durability_percentage(const Item_t* item);

// =============================================================================
// INVENTORY MANAGEMENT
// =============================================================================

// Inventory creation/destruction
Inventory_t* create_inventory(uint8_t size);
void destroy_inventory(Inventory_t* inventory);

// Inventory operations
bool add_item_to_inventory(Inventory_t* inventory, Item_t* item, uint8_t quantity);
bool remove_item_from_inventory(Inventory_t* inventory, const char* item_id, uint8_t quantity);
Inventory_slot_t* find_item_in_inventory(Inventory_t* inventory, const char* item_id);
bool can_stack_items(const Item_t* item1, const Item_t* item2);

// Equipment management
bool equip_item(Inventory_t* inventory, const char* item_id);
bool unequip_item(Inventory_t* inventory, const char* item_id);
Inventory_slot_t* get_equipped_weapon(Inventory_t* inventory);
Inventory_slot_t* get_equipped_armor(Inventory_t* inventory);

// Inventory queries
uint8_t get_inventory_free_slots(const Inventory_t* inventory);
uint8_t get_total_inventory_weight(const Inventory_t* inventory);
bool is_inventory_full(const Inventory_t* inventory);

// =============================================================================
// ITEM USAGE & EFFECTS
// =============================================================================

// Consumable usage
bool use_consumable(Item_t* item);
void trigger_consumable_duration_tick(Item_t* item);
void trigger_consumable_duration_end(Item_t* item);

// Key/Lock interaction
bool can_key_open_lock(const Item_t* key, const Lock_t* lock);

#endif
