#define LOG( msg ) printf( "%s | File: %s, Line: %d\n", msg, __FILE__, __LINE__ )
#include "Daedalus.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_ITEM_NAME_LENGTH 64
#define MAX_ITEM_ID_LENGTH 32

// Static Helper Functions to have at top of file
static bool _populate_consumable_desc(dString_t* dest, uint8_t value);
static Material_t _create_default_material(void);
static bool _populate_key_desc(dString_t* dest, const Lock_t* lock);
static bool _populate_ammunition_desc(dString_t* dest, const Material_t* material, uint8_t min_dmg, uint8_t max_dmg);
static Material_t _create_consumable_material(void);
static bool _populate_rarity(dString_t* dest, const char* rarity);
static bool _populate_armor_desc(dString_t* dest, const Material_t* material);
static bool item_resists_durability_loss(const Item_t* item);
static bool _populate_weapon_desc(dString_t* dest, const Material_t* material);
static bool _populate_string_field(dString_t* dest, const char* src);
static bool _validate_and_truncate_string(dString_t* dest, const char* src, size_t max_length, const char* field_name);
/*
 * Safely validates a material structure for basic sanity
 */
static bool _is_material_valid(const Material_t* material) {
    if (material == NULL) {
        return false;
    }
    
    if (material->name != NULL) {
        if ((void*)material->name <= (void*)0x1000) {
            // RATE LIMIT THIS:
            d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
                              1, 10.0, "Material has suspicious name pointer");
            return false;
        }
        if (material->name->str != NULL && (void*)material->name->str <= (void*)0x1000) {
            // RATE LIMIT THIS:
            d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
                              1, 10.0, "Material name has suspicious str pointer");
            return false;
        }
    }
    return true;
}

// =============================================================================
// ITEM CREATION & DESTRUCTION
// =============================================================================

/*
 * Creates a weapon with specified damage range and material properties
 */
Item_t* create_weapon(const char* name, const char* id, Material_t material,
                     uint8_t min_dmg, uint8_t max_dmg, uint8_t range, char glyph)
{   
    // Validate material before using it
    if (!_is_material_valid(&material)) {
        d_LogWarningF("Invalid material passed to create_%s, using default", "weapon");
        material = _create_default_material();
    }
    
    // Early parameter validation
    d_LogIf(name == NULL || id == NULL, D_LOG_LEVEL_ERROR, 
            "Invalid weapon parameters - name or ID is NULL");
    
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        d_LogErrorF("Memory allocation failed for weapon '%s'", name);
        return NULL;
    }

    // Initialize ALL pointers to NULL first - critical for safe cleanup
    item->name = NULL;
    item->id = NULL;
    item->description = NULL;
    item->rarity = NULL;
    item->material_data.name = NULL;

    // Set item type early
    item->type = ITEM_TYPE_WEAPON;

    // Populate name using helper FIRST
    item->name = d_InitString();
    if (item->name == NULL || !_validate_and_truncate_string(item->name, name, MAX_ITEM_NAME_LENGTH, "Item name")) {
        d_LogErrorF("Failed to populate name for weapon '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate id using helper
    item->id = d_InitString();
    if (item->id == NULL || !_validate_and_truncate_string(item->id, id, MAX_ITEM_ID_LENGTH, "Item ID")) {
        d_LogErrorF("Failed to populate ID for weapon '%s'", name);
        goto cleanup_and_fail;
    }

    // NOW log weapon creation attempt with validated names - no more spam!
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating weapon: %s (%s) [dmg:%d-%d, range:%d]", 
                      item->name->str, item->id->str, min_dmg, max_dmg, range);

    // Set basic properties
    item->glyph = glyph;
    item->material_data.properties = material.properties;
    
    // Create a COPY of the material name to avoid double-free
    item->material_data.name = d_InitString();
    if (item->material_data.name == NULL) {
        d_LogErrorF("Failed to allocate material name for weapon '%s'", name);
        goto cleanup_and_fail;
    }
    
    if (material.name && material.name->str) {
        d_AppendString(item->material_data.name, material.name->str, 0);
    } else {
        d_AppendString(item->material_data.name, "unknown", 0);
    }

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
    if (item->description == NULL || !_populate_weapon_desc(item->description, &item->material_data)) {
        d_LogErrorF("Failed to populate description for weapon '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (item->rarity == NULL || !_populate_rarity(item->rarity, "common")) {
        d_LogErrorF("Failed to populate rarity for weapon '%s'", name);
        goto cleanup_and_fail;
    }

    // Log successful creation with validated name
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 3, 5.0,
         "Weapon '%s' forged successfully [value:%d coins, weight:%.2f kg]", 
               item->name->str, item->value_coins, item->weight_kg);

    return item;

cleanup_and_fail:
    // Clean up any allocated resources - destroy_item handles NULL pointers safely
    if (item != NULL) {
        // Clean up dString_t fields properly
        if (item->name != NULL) {
            d_DestroyString(item->name);
        }
        if (item->id != NULL) {
            d_DestroyString(item->id);
        }
        if (item->description != NULL) {
            d_DestroyString(item->description);
        }
        if (item->rarity != NULL) {
            d_DestroyString(item->rarity);
        }
        if (item->material_data.name != NULL) {
            d_DestroyString(item->material_data.name);
        }
        
        free(item);
    }
    return NULL;
}
/*
 * Creates armor with specified protection and stealth values
 */
Item_t* create_armor(const char* name, const char* id, Material_t material,
                    uint8_t armor_val, uint8_t evasion_val, char glyph,
                    uint8_t stealth_val, uint8_t enchant_val)
{
    // Validate material before using it
    if (!_is_material_valid(&material)) {
        d_LogWarningF("Invalid material passed to create_%s, using default", "armor");
        material = _create_default_material();
    }
    
    d_LogIf(name == NULL || id == NULL, D_LOG_LEVEL_ERROR,
            "Invalid armor parameters - name or ID is NULL");
    
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        d_LogErrorF("Memory allocation failed for armor '%s'", name);
        return NULL;
    }

    // Initialize ALL pointers to NULL first - CRITICAL!
    item->name = NULL;
    item->id = NULL;
    item->description = NULL;
    item->rarity = NULL;
    item->material_data.name = NULL;

    // Set item type early
    item->type = ITEM_TYPE_ARMOR;

    // Populate name using helper FIRST
    item->name = d_InitString();
    if (item->name == NULL || !_validate_and_truncate_string(item->name, name, MAX_ITEM_NAME_LENGTH, "Item name")) {
        d_LogErrorF("Failed to populate name for armor '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate id using helper
    item->id = d_InitString();
    if (item->id == NULL || !_validate_and_truncate_string(item->id, id, MAX_ITEM_ID_LENGTH, "Item ID")) {
        d_LogErrorF("Failed to populate ID for armor '%s'", name);
        goto cleanup_and_fail;
    }

    // NOW log armor creation attempt with validated names - no more spam!
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating armor: %s (%s) [armor:%d, evasion:%d, stealth:%d]", 
                      item->name->str, item->id->str, armor_val, evasion_val, stealth_val);

    // Set basic properties
    item->glyph = glyph;
    item->material_data.properties = material.properties;
    
    // Create a COPY of the material name to avoid double-free
    item->material_data.name = d_InitString();
    if (item->material_data.name == NULL) {
        d_LogErrorF("Failed to allocate material name for armor '%s'", name);
        goto cleanup_and_fail;
    }
    
    if (material.name && material.name->str) {
        d_AppendString(item->material_data.name, material.name->str, 0);
    } else {
        d_AppendString(item->material_data.name, "unknown", 0);
    }

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
    if (item->description == NULL || !_populate_armor_desc(item->description, &item->material_data)) {
        d_LogErrorF("Failed to populate description for armor '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (item->rarity == NULL || !_populate_rarity(item->rarity, "common")) {
        d_LogErrorF("Failed to populate rarity for armor '%s'", name);
        goto cleanup_and_fail;
    }

    // Log successful creation with validated name
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 1, 6.0,
        "Armor '%s' crafted successfully [protection:%d, value:%d coins]", 
               item->name->str, armor_val, item->value_coins);

    return item;

cleanup_and_fail:
    // Clean up any allocated resources
    if (item != NULL) {
        if (item->name != NULL) {
            d_DestroyString(item->name);
        }
        if (item->id != NULL) {
            d_DestroyString(item->id);
        }
        if (item->description != NULL) {
            d_DestroyString(item->description);
        }
        if (item->rarity != NULL) {
            d_DestroyString(item->rarity);
        }
        if (item->material_data.name != NULL) {
            d_DestroyString(item->material_data.name);
        }
        
        free(item);
    }
    return NULL;
}
/*
 * Creates a key that can open a specific lock
 */
Item_t* create_key(const char* name, const char* id, Lock_t lock, char glyph)
{
    d_LogIf(name == NULL || id == NULL, D_LOG_LEVEL_ERROR,
            "Invalid key parameters - name or ID is NULL");
    
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        d_LogErrorF("Memory allocation failed for key '%s'", name);
        return NULL;
    }

    // Initialize ALL pointers to NULL first - CRITICAL!
    item->name = NULL;
    item->id = NULL;
    item->description = NULL;
    item->rarity = NULL;
    item->material_data.name = NULL;
    item->data.key.lock.name = NULL;
    item->data.key.lock.description = NULL;

    // Set item type early
    item->type = ITEM_TYPE_KEY;
    d_LogDebug("Item type set to ITEM_TYPE_KEY");

    // Populate name using helper
    item->name = d_InitString();
    if (item->name == NULL || !_validate_and_truncate_string(item->name, name, MAX_ITEM_NAME_LENGTH, "Item name")) {
        d_LogErrorF("Failed to populate name for key '%s'", name);
        goto cleanup_and_fail;
    }
    d_LogDebugF("Key name populated: %s", item->name->str);

    // Populate id using helper
    item->id = d_InitString();
    if (item->id == NULL || !_validate_and_truncate_string(item->id, id, MAX_ITEM_ID_LENGTH, "Item ID")) {
        d_LogErrorF("Failed to populate ID for key '%s'", name);
        goto cleanup_and_fail;
    }
    d_LogDebugF("Key ID populated: %s", item->id->str);

    // Set basic properties
    item->glyph = glyph;

    // Keys don't have materials, so create default neutral material
    item->material_data.properties = create_default_material_properties();
    d_LogDebug("Creating default material properties with neutral factors");
    d_LogDebug("Default material properties initialized");
    
    item->material_data.name = d_InitString();
    if (item->material_data.name == NULL) {
        d_LogErrorF("Failed to allocate material name for key '%s'", name);
        goto cleanup_and_fail;
    }
    d_AppendString(item->material_data.name, "default", 0);
    d_LogDebugF("Key material set to: %s", item->material_data.name->str);

    // Initialize key-specific data - DEEP COPY the lock with length limiting
    item->data.key.lock.name = d_InitString();
    if (item->data.key.lock.name == NULL) {
        d_LogErrorF("Failed to allocate lock name copy for key '%s'", name);
        goto cleanup_and_fail;
    }

    if (lock.name && lock.name->str) {
        // Apply length limiting to prevent absurdly long lock names in logs
        if (!_validate_and_truncate_string(item->data.key.lock.name, lock.name->str, MAX_ITEM_NAME_LENGTH, "Lock name")) {
            d_LogErrorF("Failed to populate lock name for key '%s'", name);
            goto cleanup_and_fail;
        }
    } else {
        d_AppendString(item->data.key.lock.name, "Unknown Lock", 0);
    }

    // NOW log the key creation with the truncated lock name - no more spam!
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      3, 2.0, "Creating key: %s (%s) for lock '%s'", 
                      item->name->str, item->id->str, 
                      item->data.key.lock.name->str);

    d_LogDebug("Entering key description population");
    
    item->data.key.lock.description = d_InitString();
    if (item->data.key.lock.description == NULL) {
        d_LogErrorF("Failed to allocate lock description copy for key '%s'", name);
        goto cleanup_and_fail;
    }
    
    if (lock.description && lock.description->str) {
        d_AppendString(item->data.key.lock.description, lock.description->str, 0);
    } else {
        d_AppendString(item->data.key.lock.description, "No description", 0);
    }

    item->data.key.lock.pick_difficulty = lock.pick_difficulty;
    item->data.key.lock.jammed_seconds = lock.jammed_seconds;

    d_LogDebugF("Key description generated for lock: %s", item->data.key.lock.name->str);

    // Set default values
    item->weight_kg = 0.1f; // Keys are light but not weightless
    item->value_coins = 5; // Base value
    item->stackable = 1; // Keys can stack

    // Populate description using helper
    item->description = d_InitString();
    if (item->description == NULL || !_populate_key_desc(item->description, &lock)) {
        d_LogErrorF("Failed to populate description for key '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (item->rarity == NULL || !_populate_rarity(item->rarity, "common")) {
        d_LogErrorF("Failed to populate rarity for key '%s'", name);
        goto cleanup_and_fail;
    }

    // Log successful key creation with truncated lock name
    d_LogInfoF("Key '%s' forged successfully [opens:%s, difficulty:%d]", 
               item->name->str, item->data.key.lock.name->str, lock.pick_difficulty);

    return item;

cleanup_and_fail:
    // Clean up any allocated resources - PROPER dString_t cleanup
    if (item != NULL) {
        if (item->name != NULL) {
            d_DestroyString(item->name);
        }
        if (item->id != NULL) {
            d_DestroyString(item->id);
        }
        if (item->description != NULL) {
            d_DestroyString(item->description);
        }
        if (item->rarity != NULL) {
            d_DestroyString(item->rarity);
        }
        if (item->material_data.name != NULL) {
            d_DestroyString(item->material_data.name);
        }
        if (item->data.key.lock.name != NULL) {
            d_DestroyString(item->data.key.lock.name);
        }
        if (item->data.key.lock.description != NULL) {
            d_DestroyString(item->data.key.lock.description);
        }
        
        free(item);
    }
    return NULL;
}

/*
 * Creates a lock with specified difficulty and jam properties
 */
Lock_t create_lock(const char* name, const char* description, uint8_t pick_difficulty, uint8_t jammed_seconds)
{
    Lock_t lock;
    
    // Initialize all pointers to NULL first for safe cleanup
    lock.name = NULL;
    lock.description = NULL;
    lock.pick_difficulty = 0;
    lock.jammed_seconds = 0;

    // Initialize dString_t fields
    lock.name = d_InitString();
    if (lock.name == NULL) {
        d_LogError("Lock name string initialization failed");
        goto cleanup_and_fail;
    }

    lock.description = d_InitString();
    if (lock.description == NULL) {
        d_LogError("Lock description string initialization failed");
        goto cleanup_and_fail;
    }

    // Validate and populate name field FIRST, then log with truncated name
    if (name != NULL) {
        if (!_validate_and_truncate_string(lock.name, name, MAX_ITEM_NAME_LENGTH, "Lock name")) {
            d_LogErrorF("Failed to populate lock name: %s", name);
            goto cleanup_and_fail;
        }
    } else {
        d_AppendString(lock.name, "Unnamed Lock", 0);
    }

    // NOW log lock creation with the validated/truncated name - no more spam!
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating lock: %s [difficulty:%d, jammed:%ds]", 
                      lock.name->str, pick_difficulty, jammed_seconds);

    // Populate description field
    if (description != NULL) {
        if (!_validate_and_truncate_string(lock.description, description, MAX_ITEM_NAME_LENGTH * 2, "Lock description")) {
            d_LogWarningF("Failed to populate lock description, using default");
            d_AppendString(lock.description, "No description", 0);
        }
    } else {
        d_AppendString(lock.description, "No description", 0);
    }

    // Set numeric properties
    lock.pick_difficulty = pick_difficulty;
    lock.jammed_seconds = jammed_seconds;

    // Log successful lock creation with validated name
    d_LogInfoF("Lock '%s' assembled successfully [security level:%d]", 
               lock.name->str, pick_difficulty);

    return lock;

cleanup_and_fail:
    // Clean up any allocated resources and return empty lock
    if (lock.name != NULL) {
        d_DestroyString(lock.name);
    }
    if (lock.description != NULL) {
        d_DestroyString(lock.description);
    }
    
    // Return zeroed lock structure
    Lock_t empty_lock = {0};
    return empty_lock;
}

/*
 * Destroys a lock and frees all associated memory
 */
void destroy_lock(Lock_t* lock)
{
    if (lock == NULL) {
        d_LogWarning("Cannot destroy NULL lock");
        return;
    }

    // Log destruction with lock name if available
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Destroying lock: %s", 
                      (lock->name && lock->name->str) ? lock->name->str : "Unknown");

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

    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                  1, 1.0, "Lock destruction completed");
}

/*
 * Creates a consumable item with effect callback and duration
 */
Item_t* create_consumable(const char* name, const char* id, int value,
                         void (*on_consume)(uint8_t), char glyph)
{
    d_LogIf(name == NULL || id == NULL || on_consume == NULL, D_LOG_LEVEL_ERROR,
            "Invalid consumable parameters - name, ID, or callback is NULL");
    
    if (name == NULL || id == NULL || on_consume == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        d_LogErrorF("Memory allocation failed for consumable '%s'", name);
        return NULL;
    }

    // Initialize ALL pointers to NULL first - CRITICAL!
    item->name = NULL;
    item->id = NULL;
    item->description = NULL;
    item->rarity = NULL;
    item->material_data.name = NULL;

    // Set item type early
    item->type = ITEM_TYPE_CONSUMABLE;

    // Handle value clamping FIRST - this fixes test_consumable_extreme_values
    uint8_t clamped_value = value;
    if (value > 255) {
        clamped_value = 255;
        d_LogWarningF("Consumable value %d exceeds uint8_t maximum, clamping to 255", value);
    }

    // Populate name using helper FIRST
    item->name = d_InitString();
    if (item->name == NULL || !_validate_and_truncate_string(item->name, name, MAX_ITEM_NAME_LENGTH, "Item name")) {
        d_LogErrorF("Failed to populate name for consumable '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate id using helper
    item->id = d_InitString();
    if (item->id == NULL || !_validate_and_truncate_string(item->id, id, MAX_ITEM_ID_LENGTH, "Item ID")) {
        d_LogErrorF("Failed to populate ID for consumable '%s'", name);
        goto cleanup_and_fail;
    }

    // NOW log consumable creation attempt with validated names - use clamped_value
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating consumable: %s (%s) [value:%d]", 
                      item->name->str, item->id->str, clamped_value);

    // Set basic properties
    item->glyph = glyph;

    // Create consumable material - get the properties and copy the name
    Material_t consumable_material = _create_consumable_material();
    item->material_data.properties = consumable_material.properties;
    
    // Create a COPY of the consumable material name
    item->material_data.name = d_InitString();
    if (item->material_data.name == NULL) {
        d_LogErrorF("Failed to allocate material name for consumable '%s'", name);
        goto cleanup_and_fail;
    }
    
    if (consumable_material.name && consumable_material.name->str) {
        d_AppendString(item->material_data.name, consumable_material.name->str, 0);
    } else {
        d_AppendString(item->material_data.name, "organic", 0);
    }

    // Clean up the temporary material name to prevent memory leak
    if (consumable_material.name) {
        d_DestroyString(consumable_material.name);
    }

    // Initialize consumable-specific data - use clamped_value consistently
    item->data.consumable.on_consume = on_consume;
    item->data.consumable.on_duration_end = NULL; // Optional
    item->data.consumable.on_duration_tick = NULL; // Optional
    item->data.consumable.value = clamped_value;  // Use clamped value here!
    item->data.consumable.duration_seconds = 0; // Instant effect by default

    // Set default values - use clamped_value for consistency
    item->weight_kg = 0.1f; // Consumables are light but not weightless
    item->value_coins = clamped_value; // Use clamped value for item value too
    item->stackable = 16; // Can stack up to 16

    // Populate description using helper - use clamped_value
    item->description = d_InitString();
    if (item->description == NULL || !_populate_consumable_desc(item->description, clamped_value)) {
        d_LogErrorF("Failed to populate description for consumable '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (item->rarity == NULL || !_populate_rarity(item->rarity, "common")) {
        d_LogErrorF("Failed to populate rarity for consumable '%s'", name);
        goto cleanup_and_fail;
    }

    // Log successful consumable creation with clamped value
    d_LogInfoF("Consumable '%s' brewed successfully [potency:%d, stacks:%d]", 
               item->name->str, clamped_value, item->stackable);

    return item;

cleanup_and_fail:
    // Clean up any allocated resources
    if (item != NULL) {
        if (item->name != NULL) {
            d_DestroyString(item->name);
        }
        if (item->id != NULL) {
            d_DestroyString(item->id);
        }
        if (item->description != NULL) {
            d_DestroyString(item->description);
        }
        if (item->rarity != NULL) {
            d_DestroyString(item->rarity);
        }
        if (item->material_data.name != NULL) {
            d_DestroyString(item->material_data.name);
        }
        
        free(item);
    }
    return NULL;
}
/*
 * Creates ammunition with specified damage range and material
 */
Item_t* create_ammunition(const char* name, const char* id, Material_t material,
                         uint8_t min_dmg, uint8_t max_dmg, char glyph)
{
    // Validate material before using it
    if (!_is_material_valid(&material)) {
        d_LogWarningF("Invalid material passed to create_%s, using default", "ammunition");
        material = _create_default_material();
    }
    
    d_LogIf(name == NULL || id == NULL, D_LOG_LEVEL_ERROR,
            "Invalid ammunition parameters - name or ID is NULL");
    
    if (name == NULL || id == NULL) {
        return NULL;
    }

    Item_t* item = (Item_t*)malloc(sizeof(Item_t));
    if (item == NULL) {
        d_LogErrorF("Memory allocation failed for ammunition '%s'", name);
        return NULL;
    }

    // Initialize ALL pointers to NULL first - CRITICAL!
    item->name = NULL;
    item->id = NULL;
    item->description = NULL;
    item->rarity = NULL;
    item->material_data.name = NULL;

    // Set item type early
    item->type = ITEM_TYPE_AMMUNITION;

    // Populate name using helper FIRST
    item->name = d_InitString();
    if (item->name == NULL || !_validate_and_truncate_string(item->name, name, MAX_ITEM_NAME_LENGTH, "Item name")) {
        d_LogErrorF("Failed to populate name for ammunition '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate id using helper
    item->id = d_InitString();
    if (item->id == NULL || !_validate_and_truncate_string(item->id, id, MAX_ITEM_ID_LENGTH, "Item ID")) {
        d_LogErrorF("Failed to populate ID for ammunition '%s'", name);
        goto cleanup_and_fail;
    }

    // NOW log ammunition creation attempt with validated names - no more spam!
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating ammunition: %s (%s) [dmg:%d-%d]", 
                      item->name->str, item->id->str, min_dmg, max_dmg);

    // Set basic properties
    item->glyph = glyph;
    item->material_data.properties = material.properties;
    
    // Create a COPY of the material name to avoid double-free
    item->material_data.name = d_InitString();
    if (item->material_data.name == NULL) {
        d_LogErrorF("Failed to allocate material name for ammunition '%s'", name);
        goto cleanup_and_fail;
    }
    
    if (material.name && material.name->str) {
        d_AppendString(item->material_data.name, material.name->str, 0);
    } else {
        d_AppendString(item->material_data.name, "unknown", 0);
    }

    // Initialize ammunition-specific data
    item->data.ammo.min_damage = min_dmg;
    item->data.ammo.max_damage = max_dmg;

    // Set default values (will be modified by material factors)
    item->weight_kg = 0.05f; // Ammo is very light but not weightless
    item->value_coins = (min_dmg + max_dmg) / 2; // Base value
    item->stackable = 255; // Ammo stacks very well

    // Populate description using helper
    item->description = d_InitString();
    if (item->description == NULL || !_populate_ammunition_desc(item->description, &item->material_data, min_dmg, max_dmg)) {
        d_LogErrorF("Failed to populate description for ammunition '%s'", name);
        goto cleanup_and_fail;
    }

    // Populate rarity using helper
    item->rarity = d_InitString();
    if (item->rarity == NULL || !_populate_rarity(item->rarity, "common")) {
        d_LogErrorF("Failed to populate rarity for ammunition '%s'", name);
        goto cleanup_and_fail;
    }

   // RATE LIMIT THIS - Called during every ammunition creation in stress tests
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO,
                    5, 3.0, "Ammunition '%s' crafted successfully [damage:%d-%d, stacks:%d]", 
                    item->name->str, min_dmg, max_dmg, item->stackable);

    return item;

cleanup_and_fail:
    // Clean up any allocated resources
    if (item != NULL) {
        if (item->name != NULL) {
            d_DestroyString(item->name);
        }
        if (item->id != NULL) {
            d_DestroyString(item->id);
        }
        if (item->description != NULL) {
            d_DestroyString(item->description);
        }
        if (item->rarity != NULL) {
            d_DestroyString(item->rarity);
        }
        if (item->material_data.name != NULL) {
            d_DestroyString(item->material_data.name);
        }
        
        free(item);
    }
    return NULL;
}
/*
 * Destroys an item and frees all associated memory
 */
void destroy_item(Item_t* item)
{
    if (item == NULL) {
        d_LogWarning("Cannot destroy NULL item");
        return;
    }

    // Log item destruction with type information
    const char* type_names[] = {
        "WEAPON", "ARMOR", "CONSUMABLE", "KEY", "AMMUNITION", "UNKNOWN"
    };
    const char* type_name = (item->type < 5) ? type_names[item->type] : type_names[5];
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Destroying %s: %s", type_name,
                      (item->name && item->name->str) ? item->name->str : "Unknown");

    // Clean up all string fields - ONLY if they appear to be valid dString_t pointers
    if (item->name != NULL) {
        // Additional safety check - ensure it looks like a valid dString_t
        if ((void*)item->name > (void*)0x1000) {  // Basic sanity check for valid pointer
            d_DestroyString(item->name);
            item->name = NULL;
        }
    }
    
    if (item->id != NULL) {
        if ((void*)item->id > (void*)0x1000) {
            d_DestroyString(item->id);
            item->id = NULL;
        }
    }
    
    if (item->description != NULL) {
        if ((void*)item->description > (void*)0x1000) {
            d_DestroyString(item->description);
            item->description = NULL;
        }
    }
    
    if (item->rarity != NULL) {
        if ((void*)item->rarity > (void*)0x1000) {
            d_DestroyString(item->rarity);
            item->rarity = NULL;
        }
    }

    // Clean up material string if allocated - BE EXTRA CAREFUL
    if (item->material_data.name != NULL) {
        // Verify this looks like a valid dString_t pointer before destroying
        if ((void*)item->material_data.name > (void*)0x1000) {
            d_DestroyString(item->material_data.name);
            item->material_data.name = NULL;
        }
    }

    // For keys, clean up the embedded lock strings
    if (item->type == ITEM_TYPE_KEY) {
        if (item->data.key.lock.name != NULL) {
            if ((void*)item->data.key.lock.name > (void*)0x1000) {
                d_DestroyString(item->data.key.lock.name);
                item->data.key.lock.name = NULL;
            }
        }
        if (item->data.key.lock.description != NULL) {
            if ((void*)item->data.key.lock.description > (void*)0x1000) {
                d_DestroyString(item->data.key.lock.description);
                item->data.key.lock.description = NULL;
            }
        }
    }

    // Finally free the item itself
    free(item);
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                  1, 1.0, "Item destruction completed");
}

// =============================================================================
// ITEM TYPE CHECKING & ACCESS
// =============================================================================

/*
 * Validates item as weapon type with null safety
 */
bool is_weapon(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_DEBUG, "Type check requested for NULL item - returning false");
    
    if (item == NULL) {
        return false;
    }
    
    bool result = item->type == ITEM_TYPE_WEAPON;
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item '%s' weapon check: %s", 
                      item->name ? item->name->str : "Unknown", 
                      result ? "TRUE" : "FALSE");
    
    return result;
}

/*
 * Validates item as armor type with null safety
 */
bool is_armor(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_DEBUG, "Armor type check requested for NULL item");
    
    if (item == NULL) {
        return false;
    }
    
    bool result = item->type == ITEM_TYPE_ARMOR;
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item '%s' armor check: %s", 
                      item->name ? item->name->str : "Unknown", 
                      result ? "TRUE" : "FALSE");
    
    return result;
}

/*
 * Validates item as key type with null safety
 */
bool is_key(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_DEBUG, "Key type check requested for NULL item");
    
    if (item == NULL) {
        return false;
    }
    
    bool result = item->type == ITEM_TYPE_KEY;
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item '%s' key check: %s", 
                      item->name ? item->name->str : "Unknown", 
                      result ? "TRUE" : "FALSE");
    
    return result;
}

/*
 * Validates item as consumable type with null safety
 */
bool is_consumable(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_DEBUG, "Consumable type check requested for NULL item");
    
    if (item == NULL) {
        return false;
    }
    
    bool result = item->type == ITEM_TYPE_CONSUMABLE;
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item '%s' consumable check: %s", 
                      item->name ? item->name->str : "Unknown", 
                      result ? "TRUE" : "FALSE");
    
    return result;
}

/*
 * Validates item as ammunition type with null safety
 */
bool is_ammunition(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_DEBUG, "Ammunition type check requested for NULL item");
    
    if (item == NULL) {
        return false;
    }
    
    bool result = item->type == ITEM_TYPE_AMMUNITION;
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item '%s' ammunition check: %s", 
                      item->name ? item->name->str : "Unknown", 
                      result ? "TRUE" : "FALSE");
    
    return result;
}

/*
 * Safely retrieves weapon data with type validation
 */
Weapon__Item_t* get_weapon_data(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot retrieve weapon data from NULL item");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_WEAPON, D_LOG_LEVEL_WARNING, 
            "Attempted to get weapon data from non-weapon item");
    
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        return NULL;
    }
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Weapon data accessed for '%s' [dmg:%d-%d]", 
                      item->name->str, item->data.weapon.min_damage, item->data.weapon.max_damage);
    
    return &item->data.weapon;
}

/*
 * Safely retrieves armor data with type validation
 */
Armor__Item_t* get_armor_data(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot retrieve armor data from NULL item");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_ARMOR, D_LOG_LEVEL_WARNING, 
            "Attempted to get armor data from non-armor item");
    
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        return NULL;
    }
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Armor data accessed for '%s' [armor:%d, evasion:%d]", 
                      item->name->str, item->data.armor.armor_value, item->data.armor.evasion_value);
    
    return &item->data.armor;
}

/*
 * Safely retrieves key data with type validation
 */
Key__Item_t* get_key_data(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot retrieve key data from NULL item");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_KEY, D_LOG_LEVEL_WARNING, 
            "Attempted to get key data from non-key item");
    
    if (item == NULL || item->type != ITEM_TYPE_KEY) {
        return NULL;
    }
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Key data accessed for '%s' [opens:%s]", 
                      item->name->str, 
                      item->data.key.lock.name ? item->data.key.lock.name->str : "Unknown");
    
    return &item->data.key;
}

/*
 * Safely retrieves consumable data with type validation
 */
Consumable__Item_t* get_consumable_data(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot retrieve consumable data from NULL item");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_CONSUMABLE, D_LOG_LEVEL_WARNING, 
            "Attempted to get consumable data from non-consumable item");
    
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        return NULL;
    }
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Consumable data accessed for '%s' [value:%d, duration:%ds]", 
                      item->name->str, item->data.consumable.value, item->data.consumable.duration_seconds);
    
    return &item->data.consumable;
}

/*
 * Safely retrieves ammunition data with type validation
 */
Ammunition__Item_t* get_ammunition_data(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot retrieve ammunition data from NULL item");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_AMMUNITION, D_LOG_LEVEL_WARNING, 
            "Attempted to get ammunition data from non-ammunition item");
    
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        return NULL;
    }
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Ammunition data accessed for '%s' [dmg:%d-%d]", 
                      item->name->str, item->data.ammo.min_damage, item->data.ammo.max_damage);
    
    return &item->data.ammo;
}

// =============================================================================
// MATERIAL SYSTEM
// =============================================================================

/*
 * Creates material with specified properties and validates input
 */
Material_t create_material(const char* name, MaterialProperties_t properties)
{
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Creating material: %s", name ? name : "NULL");
    
    Material_t material;
    material.name = d_InitString();
    
    if (material.name == NULL) {
        d_LogError("Failed to initialize material name string");
        memset(&material, 0, sizeof(Material_t));
        return material;
    }
    
    if (name == NULL) {
        d_LogWarning("Material name is NULL - using default name");
        _populate_string_field(material.name, "Default Material");
        material.properties = create_default_material_properties();
        d_LogInfo("Default material created due to NULL name");
        return material;
    }

    // Copy name safely
    if (!_populate_string_field(material.name, name)) {
        d_LogErrorF("Failed to populate material name: %s", name);
        _populate_string_field(material.name, "Failed Material");
        material.properties = create_default_material_properties();
        return material;
    }

    // Set the properties
    material.properties = properties;
    
    d_LogInfoF("Material '%s' created successfully [weight:%.2f, value:%.2f, durability:%.2f]", 
               name, properties.weight_fact, properties.value_coins_fact, properties.durability_fact);

    return material;
}

/*
 * Initializes neutral material properties for standard items
 */
MaterialProperties_t create_default_material_properties(void)
{
    d_LogDebug("Creating default material properties with neutral factors");
    
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
    
    d_LogDebug("Default material properties initialized");
    return props;
}

/*
 * Applies material modifiers to weapon statistics
 */
void apply_material_to_weapon(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_ERROR, "Cannot apply material to NULL weapon");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_WEAPON, D_LOG_LEVEL_ERROR, 
            "Cannot apply weapon material to non-weapon item");
    
    if (item == NULL || item->type != ITEM_TYPE_WEAPON) {
        return;
    }

    Weapon__Item_t* weapon = &item->data.weapon;
    Material_t* material = &item->material_data;
    
    // Store original values for logging
    uint8_t orig_min = weapon->min_damage;
    uint8_t orig_max = weapon->max_damage;
    float orig_weight = item->weight_kg;
    uint8_t orig_value = item->value_coins;

    // Apply material factors to weapon stats
    weapon->min_damage = (uint8_t)(weapon->min_damage * material->properties.min_damage_fact);
    weapon->max_damage = (uint8_t)(weapon->max_damage * material->properties.max_damage_fact);
    weapon->stealth_value = (uint8_t)(weapon->stealth_value * material->properties.stealth_value_fact);
    weapon->enchant_value = (uint8_t)(weapon->enchant_value * material->properties.enchant_value_fact);

    // Apply to base item properties with bounds checking
    float new_weight = item->weight_kg * material->properties.weight_fact;
    item->weight_kg = (new_weight < 0.0f) ? 0.0f : new_weight;

    float new_value = item->value_coins * material->properties.value_coins_fact;
    item->value_coins = (new_value < 0.0f) ? 0 : (uint8_t)new_value;
    
    d_LogInfoF("Material applied to weapon '%s': dmg[%d-%d→%d-%d], weight[%.2f→%.2f], value[%d→%d]", 
               item->name->str, orig_min, orig_max, weapon->min_damage, weapon->max_damage,
               orig_weight, item->weight_kg, orig_value, item->value_coins);
}

/*
 * Applies material modifiers to armor statistics
 */
void apply_material_to_armor(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_ERROR, "Cannot apply material to NULL armor");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_ARMOR, D_LOG_LEVEL_ERROR, 
            "Cannot apply armor material to non-armor item");
    
    if (item == NULL || item->type != ITEM_TYPE_ARMOR) {
        return;
    }

    Armor__Item_t* armor = &item->data.armor;
    Material_t* material = &item->material_data;
    
    // Store original values for logging
    uint8_t orig_armor = armor->armor_value;
    uint8_t orig_evasion = armor->evasion_value;
    uint8_t orig_durability = armor->durability;
    float orig_weight = item->weight_kg;
    uint8_t orig_value = item->value_coins;

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
    
    d_LogInfoF("Material applied to armor '%s': armor[%d→%d], evasion[%d→%d], durability[%d→%d], weight[%.2f→%.2f]", 
               item->name->str, orig_armor, armor->armor_value, orig_evasion, armor->evasion_value,
               orig_durability, armor->durability, orig_weight, item->weight_kg);
}

/*
 * Applies material modifiers to ammunition statistics
 */
void apply_material_to_ammunition(Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_ERROR, "Cannot apply material to NULL ammunition");
    d_LogIf(item != NULL && item->type != ITEM_TYPE_AMMUNITION, D_LOG_LEVEL_ERROR, 
            "Cannot apply ammunition material to non-ammunition item");
    
    if (item == NULL || item->type != ITEM_TYPE_AMMUNITION) {
        return;
    }

    Ammunition__Item_t* ammo = &item->data.ammo;
    Material_t* material = &item->material_data;
    
    // Store original values for logging
    uint8_t orig_min = ammo->min_damage;
    uint8_t orig_max = ammo->max_damage;
    float orig_weight = item->weight_kg;
    uint8_t orig_value = item->value_coins;

    // Apply material factors to ammunition stats
    ammo->min_damage = (uint8_t)(ammo->min_damage * material->properties.min_damage_fact);
    ammo->max_damage = (uint8_t)(ammo->max_damage * material->properties.max_damage_fact);

    // Apply to base item properties with bounds checking
    float new_weight = item->weight_kg * material->properties.weight_fact;
    item->weight_kg = (new_weight < 0.0f) ? 0.0f : new_weight;

    float new_value = item->value_coins * material->properties.value_coins_fact;
    item->value_coins = (new_value < 0.0f) ? 0 : (uint8_t)new_value;
    
    d_LogInfoF("Material applied to ammunition '%s': dmg[%d-%d→%d-%d], weight[%.2f→%.2f], value[%d→%d]", 
               item->name->str, orig_min, orig_max, ammo->min_damage, ammo->max_damage,
               orig_weight, item->weight_kg, orig_value, item->value_coins);
}

/*
 * Calculates item's final weight including material modifiers
 */
float calculate_final_weight(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot calculate weight for NULL item");
    
    if (item == NULL) {
        return 0.0f;
    }

    // Weight is already calculated with material factors applied
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Final weight for '%s': %.2f kg", 
                      item->name->str, item->weight_kg);
    
    return item->weight_kg;
}

/*
 * Calculates item's final value including material modifiers
 */
uint8_t calculate_final_value(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot calculate value for NULL item");
    
    if (item == NULL) {
        return 0;
    }

    // Value is already calculated with material factors applied
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Final value for '%s': %d coins", 
                      item->name->str, item->value_coins);
    
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
        d_LogError("Invalid input for use_consumable - item is NULL or not consumable");
        return false;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Check if the consumable has a valid on_consume callback
    if (consumable->on_consume == NULL) {
        d_LogWarningF("Consumable '%s' has no effect callback - cannot be used", 
                      item->name ? item->name->str : "Unknown");
        return false;
    }

    // Log successful usage with rate limiting to prevent spam during testing
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_INFO,
                      3, 2.0, "Using consumable '%s' with value %d", 
                      item->name->str, consumable->value);

    // Execute the consumable effect
    consumable->on_consume(consumable->value);

    return true;
}

void trigger_consumable_duration_tick(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        d_LogError("Invalid input for trigger_consumable_duration_tick");
        return;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Only proceed if there's duration remaining
    if (consumable->duration_seconds > 0) {
        // Execute tick callback if it exists
        if (consumable->on_duration_tick != NULL) {
            d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                              1, 5.0, "Duration tick for '%s': %d seconds remaining", 
                              item->name->str, consumable->duration_seconds);
            consumable->on_duration_tick(consumable->value);
        }

        // ALWAYS decrement duration, regardless of callback presence
        consumable->duration_seconds--;

        // Trigger end effect when duration reaches 0
        if (consumable->duration_seconds == 0) {
            d_LogInfoF("Duration effect ended for consumable '%s'", item->name->str);
            trigger_consumable_duration_end(item);
        }
    }
}

void trigger_consumable_duration_end(Item_t* item)
{
    if (item == NULL || item->type != ITEM_TYPE_CONSUMABLE) {
        d_LogError("Invalid input for trigger_consumable_duration_end");
        return;
    }

    Consumable__Item_t* consumable = &item->data.consumable;

    // Trigger the end effect if it exists
    if (consumable->on_duration_end != NULL) {
        d_LogDebugF("Triggering end effect for consumable '%s'", item->name->str);
        consumable->on_duration_end(consumable->value);
    }

    // Reset duration to 0 to mark effect as ended
    consumable->duration_seconds = 0;
}

bool can_key_open_lock(const Item_t* key, const Lock_t* lock)
{
    // Use LogIf for efficient conditional logging
    d_LogIf(key == NULL, D_LOG_LEVEL_WARNING, "Key/Lock check failed: key is NULL");
    d_LogIf(lock == NULL, D_LOG_LEVEL_WARNING, "Key/Lock check failed: lock is NULL");
    
    if (key == NULL || lock == NULL) {
        return false;
    }

    // Only keys can open locks
    if (key->type != ITEM_TYPE_KEY) {
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                          2, 3.0, "Item '%s' is not a key (type: %d)", 
                          key->name->str, key->type);
        return false;
    }

    // Check if the lock is jammed - THIS WAS THE SPAM SOURCE!
    if (lock->jammed_seconds > 0) {
        // Aggressive rate limiting for jammed lock messages - only 1 per 10 seconds
        d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
                          1, 10.0, "Lock '%s' is jammed (%d seconds remaining)", 
                          lock->name->str, lock->jammed_seconds);
        return false;
    }

    const Lock_t* key_lock = &key->data.key.lock;
    bool can_open = strcmp(key_lock->name->str, lock->name->str) == 0;

    // Log successful/failed key attempts with moderate rate limiting
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      2, 5.0, "Key '%s' %s open lock '%s'", 
                      key->name->str, can_open ? "CAN" : "CANNOT", lock->name->str);

    return can_open;
}
// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/*
 * Safely copies string data into dString_t structure
 */
static bool _populate_string_field(dString_t* dest, const char* src) 
{
    d_LogIf(src == NULL || dest == NULL, D_LOG_LEVEL_ERROR,
            "Cannot populate string field with NULL parameters");
    
    if (src == NULL || dest == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        d_LogError("Failed to initialize temporary string for field population");
        return false;
    }

    d_AppendString(temp, src, 0);
    *dest = *temp;  // Copy struct content
    free(temp);     // Free wrapper, keep string data
    
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "String field populated: %s", src);
    
    return true;
}

/*
 * Generates descriptive text for weapon items based on material
 */
static bool _populate_weapon_desc(dString_t* dest, const Material_t* material) 
{
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate weapon description - destination NULL");
    d_LogIf(material == NULL, D_LOG_LEVEL_ERROR, "Cannot populate weapon description - material NULL");
    
    if (dest == NULL || material == NULL) {
        return false;
    }

    // Check if material->name is valid
    if (material->name == NULL || material->name->str == NULL) {
        d_LogError("Material name is NULL - cannot generate weapon description");
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        d_LogError("Failed to allocate temporary string for weapon description");
        return false;
    }

    d_AppendString(temp, "A weapon made of ", 0);
    d_AppendString(temp, material->name->str, 0);
    *dest = *temp;
    free(temp);
    
    // RATE LIMIT THIS - Called during every weapon creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Weapon description generated for material: %s", 
                      material->name->str);
    return true;
}

/*
 * Calculates durability resistance based on material properties
 */
static bool item_resists_durability_loss(const Item_t* item)
{
    d_LogIf(item == NULL, D_LOG_LEVEL_WARNING, "Cannot check durability resistance for NULL item");
    
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
    bool resists = random_roll < resistance_chance;

    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 10.0, "Durability check for '%s': %.1f%% resistance, %s", 
                      item->name->str, resistance_chance * 100.0f, 
                      resists ? "RESISTED" : "damaged");

    return resists;
}

/*
 * Generates descriptive text for armor items based on material
 */
static bool _populate_armor_desc(dString_t* dest, const Material_t* material) 
{
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate armor description - destination NULL");
    d_LogIf(material == NULL, D_LOG_LEVEL_ERROR, "Cannot populate armor description - material NULL");
    
    if (dest == NULL || material == NULL) {
        return false;
    }

    // Validate material name
    if (material->name == NULL || material->name->str == NULL) {
        d_LogError("Material name is NULL - cannot generate armor description");
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        d_LogError("Failed to allocate temporary string for armor description");
        return false;
    }

    d_AppendString(temp, "Armor made of ", 0);
    d_AppendString(temp, material->name->str, 0);
    *dest = *temp;
    free(temp);
    
    // RATE LIMIT THIS - Called during every armor creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Armor description generated for material: %s", 
                      material->name->str);
    return true;
}

/*
 * Sets item rarity string using standard rarity levels
 */
static bool _populate_rarity(dString_t* dest, const char* rarity) 
{
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate rarity - destination NULL");
    d_LogIf(rarity == NULL, D_LOG_LEVEL_ERROR, "Cannot populate rarity - rarity string NULL");
    
    if (dest == NULL || rarity == NULL) {
        return false;
    }

    bool success = _populate_string_field(dest, rarity);
    
    d_LogIf(!success, D_LOG_LEVEL_ERROR, "Failed to populate rarity field");
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Item rarity set to: %s", rarity);
    
    return success;
}

static bool _populate_key_desc(dString_t* dest, const Lock_t* lock) 
{
    // RATE LIMIT THIS - Entry logging can spam during bulk key creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      8, 4.0, "Entering key description population");
    
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate key description - destination NULL");
    d_LogIf(lock == NULL, D_LOG_LEVEL_ERROR, "Cannot populate key description - lock NULL");
    
    if (dest == NULL || lock == NULL) {
        return false;
    }

    // Check if lock->name is valid
    if (lock->name == NULL || lock->name->str == NULL) {
        d_LogError("Lock name is NULL - cannot generate key description");
        return false;
    }

    // Directly populate the destination string
    d_AppendString(dest, "A key that opens: ", 0);
    d_AppendString(dest, lock->name->str, 0);

    // RATE LIMIT THIS - Success logging can flood during key generation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Key description generated for lock: %s", 
                      lock->name->str);
    return true;
}
/*
 * Creates neutral material properties for basic items
 */
static Material_t _create_default_material(void) 
{
    // RATE LIMIT THIS - Called frequently as fallback material
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 5.0, "Creating default neutral material");
    
    Material_t material;

    material.name = d_InitString();
    if (material.name == NULL) {
        d_LogError("Failed to initialize default material name");
        // Return zeroed material on failure
        memset(&material, 0, sizeof(Material_t));
        return material;
    }
    
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

    // RATE LIMIT THIS - Success logging during bulk operations
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 5.0, "Default material created with neutral properties");
    return material;
}

/*
 * Creates organic material properties for consumable items
 */
static Material_t _create_consumable_material(void) 
{
    // RATE LIMIT THIS - Called during every consumable creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      10, 3.0, "Creating organic material for consumables");
    
    Material_t material;

    // Set material name to "organic" using helper
    material.name = d_InitString();
    if (material.name == NULL) {
        d_LogError("Failed to initialize consumable material name");
        memset(&material, 0, sizeof(Material_t));
        return material;
    }
    
    if (!_populate_string_field(material.name, "organic")) {
        d_LogError("Failed to populate organic material name");
        d_DestroyString(material.name);
        memset(&material, 0, sizeof(Material_t));
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

    // RATE LIMIT THIS - Success logging for consumable material creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 5.0, "Organic material created for consumables");
    return material;
}

/*
 * Generates descriptive text for consumable items with potency value
 */
static bool _populate_consumable_desc(dString_t* dest, uint8_t value) 
{
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate consumable description - destination NULL");
    
    if (dest == NULL) {
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        d_LogError("Failed to allocate temporary string for consumable description");
        return false;
    }

    d_AppendString(temp, "A consumable item with magical properties (Potency: ", 0);
    d_AppendInt(temp, value);
    d_AppendChar(temp, ')');
    *dest = *temp;
    free(temp);
    
    // RATE LIMIT THIS - Called during every consumable creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      8, 4.0, "Consumable description generated with potency: %d", value);
    return true;
}
/*
 * Generates descriptive text for ammunition items with damage range
 */
static bool _populate_ammunition_desc(dString_t* dest, const Material_t* material, uint8_t min_dmg, uint8_t max_dmg) 
{
    d_LogIf(dest == NULL, D_LOG_LEVEL_ERROR, "Cannot populate ammunition description - destination NULL");
    d_LogIf(material == NULL, D_LOG_LEVEL_ERROR, "Cannot populate ammunition description - material NULL");
    
    if (dest == NULL || material == NULL) {
        return false;
    }

    // Validate material name
    if (material->name == NULL || material->name->str == NULL) {
        d_LogError("Material name is NULL - cannot generate ammunition description");
        return false;
    }

    dString_t* temp = d_InitString();
    if (temp == NULL) {
        d_LogError("Failed to allocate temporary string for ammunition description");
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
    
    // RATE LIMIT THIS - Called during every ammunition creation
    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG,
                      5, 3.0, "Ammunition description generated: %s [%d-%d dmg]", 
                      material->name->str, min_dmg, max_dmg);
    return true;
}
static bool _validate_and_truncate_string(dString_t* dest, const char* src, size_t max_length, const char* field_name)
{
    // Extremely aggressive NULL checks with explicit logging
    if (dest == NULL) {
        fprintf(stderr, "FATAL: Destination string is NULL for field: %s\n", field_name);
        abort(); // Immediately terminate the program
    }

    if (src == NULL) {
        fprintf(stderr, "FATAL: Source string is NULL for field: %s\n", field_name);
        abort(); // Immediately terminate the program
    }

    // Verify d_AppendStringN function pointer is available
    typedef void (*AppendStringNFunc)(dString_t*, const char*, size_t);
    AppendStringNFunc appendFunc = d_AppendStringN;

    if (appendFunc == NULL) {
        fprintf(stderr, "CRITICAL FAILURE: d_AppendStringN function is NOT LINKED!\n");
        fprintf(stderr, "Field attempting to use function: %s\n", field_name);
        fprintf(stderr, "Source string: %s\n", src);
        fprintf(stderr, "MAX LENGTH: %zu\n", max_length);
        
        // Diagnostic crash with maximum information
        // raise(SIGABRT);
    }

    size_t src_len = strlen(src);
    
    // Aggressive length checking
    if (src_len > max_length) {
        fprintf(stderr, "WARNING: Truncating %s from %zu to %zu characters\n", 
                field_name, src_len, max_length);
        
        // Use the function with EXPLICIT function pointer call
        appendFunc(dest, src, max_length);
    } else {
        // Use standard append for full-length strings
        d_AppendString(dest, src, 0);
    }
    
    return true;
}