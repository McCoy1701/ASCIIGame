#include "structs.h"
#include "Daedalus.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef __ITEMS_H__
#define __ITEMS_H__

// =============================================================================
// ITEM CREATION & DESTRUCTION
// =============================================================================

/*
 * Create a new weapon item with specified properties and material
 *
 * `name` - Display name for the weapon (must be null-terminated)
 * `id` - Unique identifier for the weapon (must be null-terminated)
 * `material` - Material properties affecting weapon stats
 * `min_dmg` - Minimum damage value (0-255)
 * `max_dmg` - Maximum damage value (0-255, must be >= min_dmg)
 * `range` - Attack range in tiles (0 = melee, >0 = ranged weapon)
 * `glyph` - ASCII character representing the weapon
 *
 * `Item_t*` - Pointer to new weapon item, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_item() to free memory
 * -- Material factors are applied to base damage values
 * -- Ranged weapons (range > 0) require ammunition to use
 * -- Default durability is set to 255 (100% condition)
 * -- Returns NULL if name or id is NULL or memory allocation fails
 */
Item_t* create_weapon(const char* name, const char* id, Material_t material,
                     uint8_t min_dmg, uint8_t max_dmg, uint8_t range, char glyph);

/*
 * Create a new armor item with specified defensive properties
 *
 * `name` - Display name for the armor (must be null-terminated)
 * `id` - Unique identifier for the armor (must be null-terminated)
 * `material` - Material properties affecting armor stats
 * `armor_val` - Base armor value for damage reduction (0-255)
 * `evasion_val` - Base evasion value for dodge chance (0-255)
 * `glyph` - ASCII character representing the armor
 * `stealth_val` - Stealth bonus provided by this armor (0-255)
 * `enchant_val` - Magical enhancement value (0-255)
 *
 * `Item_t*` - Pointer to new armor item, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_item() to free memory
 * -- Material factors are applied to all defensive values
 * -- Default durability is set to 255 (100% condition)
 * -- Only one armor piece can be equipped at a time
 * -- Returns NULL if name or id is NULL or memory allocation fails
 */
Item_t* create_armor(const char* name, const char* id, Material_t material,
                    uint8_t armor_val, uint8_t evasion_val, char glyph,
                    uint8_t stealth_val, uint8_t enchant_val);

/*
 * Create a new key item that can open a specific lock
 *
 * `name` - Display name for the key (must be null-terminated)
 * `id` - Unique identifier for the key (must be null-terminated)
 * `lock` - Lock definition that this key can open (deep copied)
 * `glyph` - ASCII character representing the key
 *
 * `Item_t*` - Pointer to new key item, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_item() to free memory
 * -- Lock data is deep copied, original lock remains unchanged
 * -- Keys have minimal weight (0.1kg) and base value of 5 coins
 * -- Uses default neutral material (no stat modifications)
 * -- Keys do not degrade with use (infinite durability)
 * -- Returns NULL if name, id is NULL or memory allocation fails
 */
Item_t* create_key(const char* name, const char* id, Lock_t lock, char glyph);

/*
 * Create a new consumable item with effect callback functions
 *
 * `name` - Display name for the consumable (must be null-terminated)
 * `id` - Unique identifier for the consumable (must be null-terminated)
 * `value` - Potency/strength of the consumable effect (0-255)
 * `on_consume` - Function called when item is consumed (must not be NULL)
 * `glyph` - ASCII character representing the consumable
 *
 * `Item_t*` - Pointer to new consumable item, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_item() to free memory
 * -- on_consume callback receives the value parameter when triggered
 * -- Optional duration callbacks can be set after creation
 * -- Consumables stack up to 16 items per inventory slot
 * -- Uses organic material with neutral properties
 * -- Returns NULL if name, id, or on_consume is NULL or allocation fails
 */
Item_t* create_consumable(const char* name, const char* id, int value,
                         void (*on_consume)(uint8_t), char glyph);

/*
 * Create ammunition for ranged weapons with damage properties
 *
 * `name` - Display name for the ammunition (must be null-terminated)
 * `id` - Unique identifier for the ammunition (must be null-terminated)
 * `material` - Material properties affecting ammunition damage
 * `min_dmg` - Minimum damage bonus from this ammunition (0-255)
 * `max_dmg` - Maximum damage bonus from this ammunition (0-255)
 * `glyph` - ASCII character representing the ammunition
 *
 * `Item_t*` - Pointer to new ammunition item, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_item() to free memory
 * -- Material factors modify the base damage values
 * -- Ammunition stacks up to 255 items per inventory slot
 * -- Very light weight (0.05kg per piece) for realistic carrying capacity
 * -- Can be used with any ranged weapon (range > 0)
 * -- Returns NULL if name or id is NULL or memory allocation fails
 */
Item_t* create_ammunition(const char* name, const char* id, Material_t material,
                         uint8_t min_dmg, uint8_t max_dmg, char glyph);

/*
 * Create a lock definition with specified security properties
 *
 * `name` - Display name for the lock (must be null-terminated)
 * `description` - Detailed description of the lock (must be null-terminated)
 * `pick_difficulty` - Difficulty level for lock picking (0-255, higher = harder)
 * `jammed_seconds` - Time in seconds the lock remains jammed after failed attempts
 *
 * `Lock_t` - New lock structure with initialized properties
 *
 * -- Lock structure contains allocated dString_t fields that need cleanup
 * -- Must call destroy_lock() to free internal string memory
 * -- Jammed locks cannot be opened until jam time expires
 * -- Pick difficulty affects success rate of lock picking attempts
 * -- Returns empty lock structure if memory allocation fails
 */
Lock_t create_lock(const char* name, const char* description, uint8_t pick_difficulty, uint8_t jammed_seconds);

/*
 * Destroy a lock and free all associated memory
 *
 * `lock` - Pointer to lock structure to destroy
 *
 * -- Frees internal dString_t fields and resets numeric values to 0
 * -- Safe to call with NULL pointer (does nothing)
 * -- After calling, the lock structure should not be used
 * -- Does not free the lock structure itself (caller responsibility if allocated)
 */
void destroy_lock(Lock_t* lock);

/*
 * Destroy an item and free its memory
 *
 * `item` - Pointer to item to destroy
 *
 * -- Currently performs basic memory cleanup
 * -- Safe to call with NULL pointer (does nothing)
 * -- Does not handle complex cleanup of internal dString_t fields (potential memory leak)
 * -- TODO: Implement proper cleanup of all dynamically allocated fields
 * -- After calling, the item pointer becomes invalid
 */
void destroy_item(Item_t* item);

// =============================================================================
// ITEM TYPE CHECKING & ACCESS
// =============================================================================

/*
 * Check if an item is a weapon
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is a weapon, false otherwise
 *
 * -- Returns false if item pointer is NULL
 * -- Safe to call with any item type
 * -- Use before accessing weapon-specific data
 */
bool is_weapon(const Item_t* item);

/*
 * Check if an item is armor
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is armor, false otherwise
 *
 * -- Returns false if item pointer is NULL
 * -- Safe to call with any item type
 * -- Use before accessing armor-specific data
 */
bool is_armor(const Item_t* item);

/*
 * Check if an item is a key
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is a key, false otherwise
 *
 * -- Returns false if item pointer is NULL
 * -- Safe to call with any item type
 * -- Use before accessing key-specific data
 */
bool is_key(const Item_t* item);

/*
 * Check if an item is a consumable
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is a consumable, false otherwise
 *
 * -- Returns false if item pointer is NULL
 * -- Safe to call with any item type
 * -- Use before accessing consumable-specific data
 */
bool is_consumable(const Item_t* item);

/*
 * Check if an item is ammunition
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is ammunition, false otherwise
 *
 * -- Returns false if item pointer is NULL
 * -- Safe to call with any item type
 * -- Use before accessing ammunition-specific data
 */
bool is_ammunition(const Item_t* item);

/*
 * Get weapon-specific data from an item
 *
 * `item` - Pointer to item (must be a weapon)
 *
 * `Weapon__Item_t*` - Pointer to weapon data, or NULL if item is not a weapon
 *
 * -- Returns NULL if item is NULL or not a weapon type
 * -- Returned pointer is valid until item is destroyed
 * -- Use is_weapon() first to verify item type
 */
Weapon__Item_t* get_weapon_data(Item_t* item);

/*
 * Get armor-specific data from an item
 *
 * `item` - Pointer to item (must be armor)
 *
 * `Armor__Item_t*` - Pointer to armor data, or NULL if item is not armor
 *
 * -- Returns NULL if item is NULL or not an armor type
 * -- Returned pointer is valid until item is destroyed
 * -- Use is_armor() first to verify item type
 */
Armor__Item_t* get_armor_data(Item_t* item);

/*
 * Get key-specific data from an item
 *
 * `item` - Pointer to item (must be a key)
 *
 * `Key__Item_t*` - Pointer to key data, or NULL if item is not a key
 *
 * -- Returns NULL if item is NULL or not a key type
 * -- Returned pointer is valid until item is destroyed
 * -- Use is_key() first to verify item type
 */
Key__Item_t* get_key_data(Item_t* item);

/*
 * Get consumable-specific data from an item
 *
 * `item` - Pointer to item (must be a consumable)
 *
 * `Consumable__Item_t*` - Pointer to consumable data, or NULL if item is not consumable
 *
 * -- Returns NULL if item is NULL or not a consumable type
 * -- Returned pointer is valid until item is destroyed
 * -- Use is_consumable() first to verify item type
 */
Consumable__Item_t* get_consumable_data(Item_t* item);

/*
 * Get ammunition-specific data from an item
 *
 * `item` - Pointer to item (must be ammunition)
 *
 * `Ammunition__Item_t*` - Pointer to ammunition data, or NULL if item is not ammunition
 *
 * -- Returns NULL if item is NULL or not ammunition type
 * -- Returned pointer is valid until item is destroyed
 * -- Use is_ammunition() first to verify item type
 */
Ammunition__Item_t* get_ammunition_data(Item_t* item);

// =============================================================================
// MATERIAL SYSTEM
// =============================================================================

/*
 * Create a material with specified name and property multipliers
 *
 * `name` - Name of the material (must be null-terminated)
 * `properties` - Structure containing all material property multipliers
 *
 * `Material_t` - New material structure with allocated name string
 *
 * -- Material name is dynamically allocated and copied
 * -- Property multipliers are applied to base item values
 * -- Values of 1.0f represent no change, >1.0f increases, <1.0f decreases
 * -- Returns default material with "Default Material" name if name is NULL
 * -- Material memory must be managed by caller
 */
Material_t create_material(const char* name, MaterialProperties_t properties);

/*
 * Create default material properties with neutral multipliers
 *
 * `MaterialProperties_t` - Property structure with all multipliers set to 1.0f
 *
 * -- All property factors are set to 1.0f (no modification)
 * -- Use as base for creating custom materials
 * -- Neutral materials don't affect item statistics
 */
MaterialProperties_t create_default_material_properties(void);

/*
 * Apply material property multipliers to a weapon item
 *
 * `item` - Pointer to weapon item to modify
 *
 * -- Does nothing if item is NULL or not a weapon
 * -- Modifies min/max damage, stealth value, and enchant value
 * -- Also affects base weight and value properties
 * -- Durability is not modified by material (always starts at 255)
 * -- Values are bounded to prevent overflow/underflow
 */
void apply_material_to_weapon(Item_t* item);

/*
 * Apply material property multipliers to an armor item
 *
 * `item` - Pointer to armor item to modify
 *
 * -- Does nothing if item is NULL or not armor
 * -- Modifies armor value, evasion, stealth, enchant, and durability
 * -- Also affects base weight and value properties
 * -- Values are bounded to prevent overflow/underflow
 */
void apply_material_to_armor(Item_t* item);

/*
 * Apply material property multipliers to ammunition
 *
 * `item` - Pointer to ammunition item to modify
 *
 * -- Does nothing if item is NULL or not ammunition
 * -- Modifies min/max damage values based on material
 * -- Also affects base weight and value properties
 * -- Values are bounded to prevent overflow/underflow
 */
void apply_material_to_ammunition(Item_t* item);

/*
 * Calculate the final weight of an item including material modifiers
 *
 * `item` - Pointer to item to calculate weight for
 *
 * `float` - Final weight in kilograms, or 0.0f if item is NULL
 *
 * -- Weight already includes material factor modifications
 * -- Returns the current weight_kg field value
 * -- Used for inventory weight calculations
 */
float calculate_final_weight(const Item_t* item);

/*
 * Calculate the final value of an item including material modifiers
 *
 * `item` - Pointer to item to calculate value for
 *
 * `uint8_t` - Final value in coins, or 0 if item is NULL
 *
 * -- Value already includes material factor modifications
 * -- Returns the current value_coins field value
 * -- Used for trading and economy calculations
 */
uint8_t calculate_final_value(const Item_t* item);

// =============================================================================
// ITEM PROPERTIES & STATS
// =============================================================================

/*
 * Get the minimum damage value of a weapon
 *
 * `item` - Pointer to weapon item
 *
 * `uint8_t` - Minimum damage value, or 0 if item is NULL or not a weapon
 *
 * -- Returns damage value after material modifications
 * -- Use is_weapon() first to verify item type
 */
uint8_t get_weapon_min_damage(const Item_t* item);

/*
 * Get the maximum damage value of a weapon
 *
 * `item` - Pointer to weapon item
 *
 * `uint8_t` - Maximum damage value, or 0 if item is NULL or not a weapon
 *
 * -- Returns damage value after material modifications
 * -- Use is_weapon() first to verify item type
 */
uint8_t get_weapon_max_damage(const Item_t* item);

/*
 * Get the minimum damage value of ammunition
 *
 * `item` - Pointer to ammunition item
 *
 * `uint8_t` - Minimum damage bonus, or 0 if item is NULL or not ammunition
 *
 * -- Returns damage value after material modifications
 * -- This damage is added to weapon damage when fired
 * -- Use is_ammunition() first to verify item type
 */
uint8_t get_ammunition_min_damage(const Item_t* item);

/*
 * Get the maximum damage value of ammunition
 *
 * `item` - Pointer to ammunition item
 *
 * `uint8_t` - Maximum damage bonus, or 0 if item is NULL or not ammunition
 *
 * -- Returns damage value after material modifications
 * -- This damage is added to weapon damage when fired
 * -- Use is_ammunition() first to verify item type
 */
uint8_t get_ammunition_max_damage(const Item_t* item);

/*
 * Get the attack range of a weapon in tiles
 *
 * `item` - Pointer to weapon item
 *
 * `uint8_t` - Range in tiles, or 0 if item is NULL or not a weapon
 *
 * -- Range of 0 indicates melee weapon (adjacent tiles only)
 * -- Range > 0 indicates ranged weapon requiring ammunition
 * -- Range determines maximum distance for attacks
 */
uint8_t get_weapon_range(const Item_t* item);

/*
 * Check if a weapon requires ammunition to use
 *
 * `item` - Pointer to weapon item
 *
 * `bool` - true if weapon needs ammo, false otherwise
 *
 * -- Returns false if item is NULL or not a weapon
 * -- Ranged weapons (range > 0) require ammunition
 * -- Melee weapons (range = 0) do not require ammunition
 */
bool weapon_needs_ammo(const Item_t* item);

/*
 * Check if specific ammunition can be used with a weapon
 *
 * `weapon` - Pointer to weapon item
 * `ammo` - Pointer to ammunition item
 *
 * `bool` - true if ammunition is compatible, false otherwise
 *
 * -- Returns false if either item is NULL or wrong type
 * -- Currently allows any ammunition with any ranged weapon
 * -- Future versions may implement ammo type restrictions
 * -- Melee weapons cannot use ammunition
 */
bool weapon_can_use_ammo(const Item_t* weapon, const Item_t* ammo);

/*
 * Get the armor value of an armor item
 *
 * `item` - Pointer to armor item
 *
 * `uint8_t` - Armor value for damage reduction, or 0 if not armor
 *
 * -- Returns value after material modifications
 * -- Higher values provide better protection
 * -- Use is_armor() first to verify item type
 */
uint8_t get_armor_value(const Item_t* item);

/*
 * Get the evasion value of an armor item
 *
 * `item` - Pointer to armor item
 *
 * `uint8_t` - Evasion value for dodge chance, or 0 if not armor
 *
 * -- Returns value after material modifications
 * -- Higher values increase chance to avoid attacks
 * -- Use is_armor() first to verify item type
 */
uint8_t get_evasion_value(const Item_t* item);

/*
 * Get the weight of any item in kilograms
 *
 * `item` - Pointer to item
 *
 * `float` - Weight in kilograms, or 0.0f if item is NULL
 *
 * -- Weight includes material factor modifications
 * -- Used for inventory capacity calculations
 * -- Weight affects character movement and stamina
 */
float get_item_weight(const Item_t* item);

/*
 * Get the value of any item in coins
 *
 * `item` - Pointer to item
 *
 * `uint8_t` - Value in coins, or 0 if item is NULL
 *
 * -- Value includes material factor modifications
 * -- Used for trading and economy systems
 * -- Determines buy/sell prices with merchants
 */
uint8_t get_item_value_coins(const Item_t* item);

/*
 * Get the stealth bonus provided by an item
 *
 * `item` - Pointer to item
 *
 * `uint8_t` - Stealth value, or 0 if item doesn't provide stealth bonus
 *
 * -- Only weapons and armor provide stealth bonuses
 * -- Higher values improve stealth capabilities
 * -- Keys, consumables, and ammunition don't affect stealth
 */
uint8_t get_stealth_value(const Item_t* item);

/*
 * Get the current durability of an item
 *
 * `item` - Pointer to item
 *
 * `uint8_t` - Durability value (0-255), or 0 if item is NULL
 *
 * -- Only weapons and armor have durability that degrades
 * -- Value of 0 indicates item is broken and unusable
 * -- Keys, consumables, and ammunition return 255 (don't degrade)
 * -- Use get_durability_percentage() for percentage representation
 */
uint8_t get_durability(const Item_t* item);

/*
 * Check if an item can be stacked in inventory
 *
 * `item` - Pointer to item
 *
 * `bool` - true if item can stack, false otherwise
 *
 * -- Returns false if item is NULL
 * -- Stackable items can be combined in single inventory slots
 * -- Weapons and armor typically don't stack (unique properties)
 * -- Consumables and ammunition usually stack for convenience
 */
bool is_item_stackable(const Item_t* item);

/*
 * Get the maximum stack size for an item
 *
 * `item` - Pointer to item
 *
 * `uint8_t` - Maximum stack size, or 0 if item is NULL
 *
 * -- Returns 0 for non-stackable items
 * -- Indicates how many items can fit in one inventory slot
 * -- Different item types have different stack limits
 */
uint8_t get_max_stack_size(const Item_t* item);

// =============================================================================
// DURABILITY SYSTEM
// =============================================================================

/*
 * Reduce an item's durability by specified damage amount
 *
 * `item` - Pointer to item to damage
 * `damage` - Amount of durability damage to apply (0-65535)
 *
 * -- Does nothing if item is NULL
 * -- Only affects weapons and armor (other items don't degrade)
 * -- Item may resist durability loss based on material properties
 * -- Durability cannot go below 0 (item becomes broken)
 * -- Broken items cannot be used until repaired
 */
void damage_item_durability(Item_t* item, uint16_t damage);

/*
 * Restore an item's durability by specified repair amount
 *
 * `item` - Pointer to item to repair
 * `repair_amount` - Amount of durability to restore (0-65535)
 *
 * -- Does nothing if item is NULL
 * -- Only affects weapons and armor (other items don't need repair)
 * -- Durability cannot exceed 255 (maximum condition)
 * -- Allows broken items to become usable again
 */
void repair_item(Item_t* item, uint16_t repair_amount);

/*
 * Check if an item is broken (durability = 0)
 *
 * `item` - Pointer to item to check
 *
 * `bool` - true if item is broken, false otherwise
 *
 * -- Returns true if item pointer is NULL (NULL items are "broken")
 * -- Only weapons and armor can be broken
 * -- Broken items cannot be equipped or used effectively
 * -- Keys, consumables, and ammunition cannot break
 */
bool is_item_broken(const Item_t* item);

/*
 * Get item durability as a percentage (0.0 to 1.0)
 *
 * `item` - Pointer to item
 *
 * `float` - Durability percentage, or 0.0f if item is NULL
 *
 * -- Returns value from 0.0f (broken) to 1.0f (perfect condition)
 * -- Items that don't degrade return 1.0f (always "perfect")
 * -- Useful for displaying condition bars and repair decisions
 */
float get_durability_percentage(const Item_t* item);

// =============================================================================
// INVENTORY MANAGEMENT
// =============================================================================

/*
 * Create a new inventory with specified number of slots
 *
 * `size` - Number of inventory slots (1-255)
 *
 * `Inventory_t*` - Pointer to new inventory, or NULL on allocation failure
 *
 * -- Must be destroyed with destroy_inventory() to free memory
 * -- All slots are initially empty (quantity = 0)
 * -- Size cannot be 0 (invalid inventory)
 * -- Each slot can hold one item type up to its stack limit
 */
Inventory_t* create_inventory(uint8_t size);

/*
 * Destroy an inventory and free its memory
 *
 * `inventory` - Pointer to inventory to destroy
 *
 * -- Safe to call with NULL pointer (does nothing)
 * -- Does not destroy individual items (caller responsibility)
 * -- Frees slot array and inventory structure
 * -- Items may still be referenced elsewhere in the game
 */
void destroy_inventory(Inventory_t* inventory);

/*
 * Add items to inventory, stacking when possible
 *
 * `inventory` - Pointer to target inventory
 * `item` - Pointer to item type to add
 * `quantity` - Number of items to add (1-255)
 *
 * `bool` - true if all items were added successfully, false otherwise
 *
 * -- Returns false if inventory, item is NULL, or quantity is 0
 * -- Attempts to stack with existing identical items first
 * -- Creates new slots for remaining items if stacking insufficient
 * -- May partially succeed (some items added, others rejected)
 * -- Item data is copied into inventory slots
 */
bool add_item_to_inventory(Inventory_t* inventory, Item_t* item, uint8_t quantity);

/*
 * Remove items from inventory by item ID
 *
 * `inventory` - Pointer to source inventory
 * `item_id` - Unique ID of item type to remove
 * `quantity` - Number of items to remove (1-255)
 *
 * `bool` - true if all requested items were removed, false otherwise
 *
 * -- Returns false if inventory, item_id is NULL, or quantity is 0
 * -- Removes items from multiple stacks if necessary
 * -- May partially succeed if insufficient items available
 * -- Empty slots are left with quantity = 0
 */
bool remove_item_from_inventory(Inventory_t* inventory, const char* item_id, uint8_t quantity);

/*
 * Find the first inventory slot containing a specific item ID
 *
 * `inventory` - Pointer to inventory to search
 * `item_id` - Unique ID of item to find
 *
 * `Inventory_slot_t*` - Pointer to first matching slot, or NULL if not found
 *
 * -- Returns NULL if inventory, item_id is NULL, or item not found
 * -- Returned pointer is valid until inventory is modified
 * -- Only finds first occurrence (there may be multiple stacks)
 * -- Use for checking item existence or accessing item data
 */
Inventory_slot_t* find_item_in_inventory(Inventory_t* inventory, const char* item_id);

/*
 * Check if two items can be stacked together
 *
 * `item1` - Pointer to first item
 * `item2` - Pointer to second item
 *
 * `bool` - true if items can stack, false otherwise
 *
 * -- Returns false if either item pointer is NULL
 * -- Items must have identical IDs and be individually stackable
 * -- Different durability levels prevent stacking for weapons/armor
 * -- Used internally by inventory management functions
 */
bool can_stack_items(const Item_t* item1, const Item_t* item2);

/*
 * Equip an item from inventory for active use
 *
 * `inventory` - Pointer to inventory containing the item
 * `item_id` - Unique ID of item to equip
 *
 * `bool` - true if item was equipped successfully, false otherwise
 *
 * -- Returns false if inventory, item_id is NULL, or item not found
 * -- Only weapons and armor can be equipped
 * -- Broken items cannot be equipped
 * -- Automatically unequips any other item of the same type
 * -- Only one weapon and one armor can be equipped at a time
 */
bool equip_item(Inventory_t* inventory, const char* item_id);

/*
 * Unequip an item and return it to normal inventory status
 *
 * `inventory` - Pointer to inventory containing the item
 * `item_id` - Unique ID of item to unequip
 *
 * `bool` - true if item was unequipped successfully, false otherwise
 *
 * -- Returns false if inventory, item_id is NULL, or item not equipped
 * -- Item remains in inventory but is no longer active
 * -- Player loses benefits provided by the unequipped item
 */
bool unequip_item(Inventory_t* inventory, const char* item_id);

/*
 * Get the currently equipped weapon from inventory
 *
 * `inventory` - Pointer to inventory to search
 *
 * `Inventory_slot_t*` - Pointer to equipped weapon slot, or NULL if none equipped
 *
 * -- Returns NULL if inventory is NULL or no weapon is equipped
 * -- Automatically unequips broken weapons and returns NULL
 * -- Returned pointer is valid until inventory is modified
 * -- Use get_weapon_data() to access weapon-specific properties
 */
Inventory_slot_t* get_equipped_weapon(Inventory_t* inventory);

/*
 * Get the currently equipped armor from inventory
 *
 * `inventory` - Pointer to inventory to search
 *
 * `Inventory_slot_t*` - Pointer to equipped armor slot, or NULL if none equipped
 *
 * -- Returns NULL if inventory is NULL or no armor is equipped
 * -- Automatically unequips broken armor and returns NULL
 * -- Returned pointer is valid until inventory is modified
 * -- Use get_armor_data() to access armor-specific properties
 */
Inventory_slot_t* get_equipped_armor(Inventory_t* inventory);

/*
 * Count the number of empty slots in an inventory
 *
 * `inventory` - Pointer to inventory to analyze
 *
 * `uint8_t` - Number of empty slots, or 0 if inventory is NULL
 *
 * -- Empty slots have quantity = 0
 * -- Does not count partially filled stackable slots
 * -- Useful for determining if new items can be added
 */
uint8_t get_inventory_free_slots(const Inventory_t* inventory);

/*
 * Calculate the total weight of all items in inventory
 *
 * `inventory` - Pointer to inventory to analyze
 *
 * `uint8_t` - Total weight in kilograms (capped at 255), or 0 if inventory is NULL
 *
 * -- Accounts for quantity of each item stack
 * -- Uses final weight including material modifications
 * -- Result is capped at 255 to fit in uint8_t
 * -- Used for encumbrance and movement speed calculations
 */
uint8_t get_total_inventory_weight(const Inventory_t* inventory);

/*
 * Check if inventory has no free slots available
 *
 * `inventory` - Pointer to inventory to check
 *
 * `bool` - true if inventory is full, false if slots available
 *
 * -- Returns true if inventory is NULL (NULL inventory is "full")
 * -- Full inventory cannot accept new non-stackable items
 * -- Stackable items may still be added to existing stacks
 */
bool is_inventory_full(const Inventory_t* inventory);

// =============================================================================
// ITEM USAGE & EFFECTS
// =============================================================================

/*
 * Consume a consumable item and trigger its effect
 *
 * `item` - Pointer to consumable item to use
 *
 * `bool` - true if item was consumed successfully, false otherwise
 *
 * -- Returns false if item is NULL, wrong type, or has no effect callback
 * -- Calls the item's on_consume function with the item's value parameter
 * -- Duration effects are handled separately by the game's timer system
 * -- Item should be removed from inventory after successful consumption
 */
bool use_consumable(Item_t* item);

/*
 * Trigger duration tick effect for consumable items
 *
 * `item` - Pointer to consumable item with active duration effect
 *
 * -- Does nothing if item is NULL, wrong type, or has no duration callback
 * -- Called periodically by game timer system during effect duration
 * -- Decrements remaining duration and triggers end effect when expired
 * -- Should be called once per second for proper timing
 */
void trigger_consumable_duration_tick(Item_t* item);

/*
 * Trigger end-of-duration effect for consumable items
 *
 * `item` - Pointer to consumable item whose duration has expired
 *
 * -- Does nothing if item is NULL, wrong type, or has no end callback
 * -- Called automatically when duration reaches 0
 * -- Resets duration to 0 to mark effect as completed
 * -- Used for effects that need cleanup or reversal
 */
void trigger_consumable_duration_end(Item_t* item);

/*
 * Check if a key can open a specific lock
 *
 * `key` - Pointer to key item
 * `lock` - Pointer to lock to test against
 *
 * `bool` - true if key can open the lock, false otherwise
 *
 * -- Returns false if key, lock is NULL, or key is wrong item type
 * -- Jammed locks cannot be opened regardless of key
 * -- Currently uses simple name matching for key-lock compatibility
 * -- Future versions may implement master keys and lock hierarchies
 */
bool can_key_open_lock(const Item_t* key, const Lock_t* lock);

#endif