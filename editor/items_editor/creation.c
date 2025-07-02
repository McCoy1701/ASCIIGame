// editor/items_editor/creation.c
// Allows user to select item generation parameters and create new items

#include <stdio.h>
#include <stdlib.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "init_editor.h"
#include "structs.h"
#include "items_editor.h"
#include "items.h"
#include "defs.h"

// --- Forward declarations for static functions ---
static void ie_CreationDoLoop(float dt);
static void ie_CreationRenderLoop(float dt);
static void iec_UpdateFromUI(void);
static void iec_DrawCreationStatus(void);

// --- Module-level state ---
// These static variables are the "single source of truth" for the selected parameters.
static int item_type_index     = 0;
static int material_index      = 0;
static int rarity_index        = 0;
static int enchantment_index   = 0;

// Pointers to the actual UI select widgets. We'll grab these on init.
static aSelectWidget_t* type_select_widget        = NULL;
static aSelectWidget_t* material_select_widget    = NULL;
static aSelectWidget_t* rarity_select_widget      = NULL;
static aSelectWidget_t* enchantment_select_widget = NULL;

// Chaos Flag for testing failure cases
static bool g_force_creation_failure = false;

// Rarity names for display and item creation
static const char* rarity_names[] = {
    "Common",
    "Uncommon",
    "Rare",
    "Epic",
    "Legendary"
};

/**
 * @brief Default callback for consumables created in the editor.
 */
static void _default_on_consume(uint8_t val) {
    (void)val; // Does nothing, just satisfy the function pointer requirement.
}


// =============================================================================
// INITIALIZATION
// =============================================================================

void ie_creation(void)
{
    app.delegate.logic = ie_CreationDoLoop;
    app.delegate.draw  = ie_CreationRenderLoop;

    // Reset state variables to default
    item_type_index   = 0;
    material_index    = 0;
    rarity_index      = 0;
    enchantment_index = 0;

    // Get the main container widget for the creation screen
    app.active_widget = a_GetWidget("item_generation_menu");
    if (app.active_widget == NULL) {
        d_LogWarning("Creation mode entered but 'item_generation_menu' widget not found!");
        return;
    }
    app.active_widget->hidden = 0;
    
    aContainerWidget_t* container = a_GetContainerFromWidget("item_generation_menu");
    if (container == NULL) {
        d_LogWarning("Creation mode running without 'item_generation_menu' container; actions not set.");
        return;
    }

    // Find and store pointers to our specific select widgets for direct manipulation
    for (int i = 0; i < container->num_components; i++) {
        aWidget_t* current = &container->components[i];
        current->hidden = 0;

        if (strcmp(current->name, "item_type") == 0)         type_select_widget        = (aSelectWidget_t*)current->data;
        if (strcmp(current->name, "material_type") == 0)     material_select_widget    = (aSelectWidget_t*)current->data;
        if (strcmp(current->name, "rarity_level") == 0)      rarity_select_widget      = (aSelectWidget_t*)current->data;
        if (strcmp(current->name, "enchantment_level") == 0) enchantment_select_widget = (aSelectWidget_t*)current->data;
        
        if (strcmp(current->name, "generate_item") == 0)     current->action = iec_GenerateItem;
        if (strcmp(current->name, "random_item") == 0)       current->action = iec_GenerateRandomItem;
        if (strcmp(current->name, "template_load") == 0)     current->action = iec_LoadItemTemplate;
    }
    d_LogInfo("Item creation mode initialized.");
}


// =============================================================================
// MAIN LOOPS
// =============================================================================

static void ie_CreationDoLoop(float dt)
{
    (void)dt;
    a_DoInput();

    // Handle exiting the mode
    if (app.keyboard[SDL_SCANCODE_ESCAPE] == 1) {
        app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
        e_InitEditor();
        return;
    }

    // --- Hotkey Logic ---
    // Cycle parameters with hotkeys and update the corresponding widget's value
    if (app.keyboard[SDL_SCANCODE_1] == 1) {
        app.keyboard[SDL_SCANCODE_1] = 0;
        item_type_index = (item_type_index + 1) % 5;
        if (type_select_widget) type_select_widget->value = item_type_index;
    }
    if (app.keyboard[SDL_SCANCODE_2] == 1) {
        app.keyboard[SDL_SCANCODE_2] = 0;
        rarity_index = (rarity_index + 1) % 5;
        if (rarity_select_widget) rarity_select_widget->value = rarity_index;
    }
    if (app.keyboard[SDL_SCANCODE_3] == 1) {
        app.keyboard[SDL_SCANCODE_3] = 0;
        material_index = (material_index + 1) % 10; // 10 material options in the JSON
        if (material_select_widget) material_select_widget->value = material_index;
    }

    // Process all widget interactions (button clicks, dropdown selections)
    a_DoWidget();
    
    // After widgets are processed, sync our internal state from the UI
    iec_UpdateFromUI();
}

static void ie_CreationRenderLoop(float dt)
{
    (void)dt;
    
    // Draw the main editor background elements
    ie_DrawDbStatus(ie_GetStatusText());
    ie_DrawDbStats(ie_GetDatabase(), ie_GetItemCount());
    
    // Draw creation-specific UI text
    iec_DrawCreationStatus();

    // The widget system handles drawing the dropdowns and buttons
    a_DrawWidgets();
}


// =============================================================================
// LOGIC & UI HELPERS
// =============================================================================

/**
 * @brief Reads the current values from the UI widgets and updates the internal state.
 */
static void iec_UpdateFromUI(void)
{
    if (type_select_widget)        item_type_index   = type_select_widget->value;
    if (material_select_widget)    material_index    = material_select_widget->value;
    if (rarity_select_widget)      rarity_index      = rarity_select_widget->value;
    if (enchantment_select_widget) enchantment_index = enchantment_select_widget->value;
}

/**
 * @brief Draws the status text and hotkey hints for the creation screen.
 */
static void iec_DrawCreationStatus(void)
{
    dString_t* status_text = d_InitString();
    if (!status_text) return;

    // Use the options from the widgets if they exist, otherwise use fallback names
    const char* type_str     = type_select_widget ? type_select_widget->options[item_type_index] : "N/A";
    const char* rarity_str   = rarity_select_widget ? rarity_select_widget->options[rarity_index] : "N/A";
    const char* material_str = material_select_widget ? material_select_widget->options[material_index] : "N/A";

    d_FormatString(status_text, "Creating: %s | Rarity: %s | Material: %s", type_str, rarity_str, material_str);
    a_DrawText(d_PeekString(status_text), 15, 100, 100, 100, 200, app.font_type, TEXT_ALIGN_LEFT, 0);
    
    d_DestroyString(status_text);
    
    a_DrawText("1: Type | 2: Rarity | 3: Material | ESC: Back", 15, SCREEN_HEIGHT - 100, 200, 200, 180, app.font_type, TEXT_ALIGN_LEFT, 0);
}


// =============================================================================
// ITEM GENERATION
// =============================================================================

Item_t* ie_CreateItemWithParameters(ItemType_t type, int mat_id, int rare, int ench)
{
    if (g_force_creation_failure) {
        return NULL;
    }
    
    // Get the shared materials database
    Material_t* materials_db = ie_GetMaterialsDatabase();
    int material_count = ie_GetMaterialsCount();

    // Defensive check: ensure the material ID is valid
    if (!materials_db || mat_id >= material_count) {
        d_LogErrorF("Invalid material ID %d used for item creation.", mat_id);
        return NULL;
    }
    // Get a pointer to the selected material from the database
    Material_t* selected_material = &materials_db[mat_id];

    Item_t* new_item = NULL;
    const char* current_rarity_name = rarity_names[rare];

    switch (type) {
        case ITEM_TYPE_WEAPON:
            new_item = create_weapon("Crafted Sword", "crafted_weapon", *selected_material, 5, 10, 1, '/', 0, ench, current_rarity_name);
            break;
        case ITEM_TYPE_ARMOR:
            new_item = create_armor("Crafted Mail", "crafted_armor", *selected_material, 15, 2, '[', 0, ench, current_rarity_name);
            break;
        case ITEM_TYPE_KEY: {
            Lock_t lock = create_lock("Generic Door", "A generic lock.", 50, 10);
            new_item = create_key("Crafted Key", "crafted_key", lock, '~', ench, current_rarity_name);
            destroy_lock(&lock);
            break;
        }
        case ITEM_TYPE_CONSUMABLE:
            new_item = create_consumable("Crafted Potion", "crafted_consumable", 50, _default_on_consume, '!', ench, current_rarity_name);
            break;
        case ITEM_TYPE_AMMUNITION:
            new_item = create_ammunition("Crafted Arrow", "crafted_ammo", *selected_material, 3, 6, '|', ench, current_rarity_name);
            break;
        default:
            d_LogErrorF("Attempted to create item with unknown type: %d", type);
            break;
    }

    // We no longer need to free the material, as we're just using a pointer from the database.
    return new_item;
}


void iec_GenerateItem(void)
{
    d_LogInfo("Generate Item button pressed. Using current selections.");
    Item_t* new_item = ie_CreateItemWithParameters(
        (ItemType_t)item_type_index, 
        material_index, 
        rarity_index, 
        enchantment_index
    );

    if (new_item) {
        ie_AddItemToDatabase(new_item);
        d_LogInfoF("Generated new item: %s", d_PeekString(new_item->name));
        
        // FIX: Hide the creation menu before switching modes.
        if (app.active_widget && strcmp(app.active_widget->name, "item_generation_menu") == 0) {
            app.active_widget->hidden = 1;
        }
        
        // Now, switch to the edit mode
        ie_edit();
    } else {
        d_LogError("Failed to generate item with selected parameters.");
    }
}



void iec_GenerateRandomItem(void)
{
    ItemType_t random_type = rand() % 5;
    int random_material = rand() % 10;
    int rarity_roll = rand() % 100;
    int random_rarity;
    if (rarity_roll < 50) random_rarity = 0;
    else if (rarity_roll < 75) random_rarity = 1;
    else if (rarity_roll < 90) random_rarity = 2;
    else if (rarity_roll < 98) random_rarity = 3;
    else random_rarity = 4;
    
    int random_enchantment = (random_rarity > 1) ? (rand() % 4) : 0;

    Item_t* random_item = ie_CreateItemWithParameters(random_type, random_material,
                                                     random_rarity,
                                                     random_enchantment);

    if (random_item) {
        ie_AddItemToDatabase(random_item);
        d_LogInfoF("Generated random item: %s", d_PeekString(random_item->name));
        
        if (app.active_widget && strcmp(app.active_widget->name, "item_generation_menu") == 0) {
            app.active_widget->hidden = 1;
        }

        ie_edit();
    } else {
        d_LogError("Failed to generate random item.");
    }
}


void iec_LoadItemTemplate(void)
{
    d_LogInfo("Item template loading not yet implemented.");
}