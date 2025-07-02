// editor/items_editor.c - Items Editor Subsystem
// Core Items Editor framework and main application loop management

#include <stdio.h>
#include <stdlib.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "defs.h"
#include "structs.h"
#include "editor.h"
#include "items_editor.h"
#include "world_editor.h"
#include "entity_editor.h"
#include "color_editor.h"
#include "ui_editor.h"
#include "items.h"
// Chaos Flag for testing failure cases
static bool g_force_load_failure = false;
/*
 * Execute one frame of items editor logic processing and input handling
 *
 * @param dt: Delta time in seconds since last frame for timing calculations
 * @return: void
 * -- Processes all input events through Archimedes input system
 * -- Handles ESC key for returning to main editor interface
 * -- Updates widget states and interactions for items editor UI
 * -- Called once per frame by main loop with consistent delta time
 * -- Manages item database state and user interactions
 */
static void e_ItemEditorDoLoop( float dt );

/*
 * Execute one frame of items editor rendering and visual display
 *
 * @param dt: Delta time in seconds since last frame for animation timing
 * @return: void
 * -- Renders item database overview with counts and statistics
 * -- Draws all active widgets through Archimedes widget rendering system
 * -- Provides visual feedback for current items editor state
 * -- Shows item collection summary and navigation hints
 */
static void e_ItemEditorRenderLoop( float dt );

// Global state for items editor
Item_t* items_database = NULL;           // Main item collection
int items_count = 0;                     // Total number of items loaded
dString_t* items_status_text = NULL;     // Status display text

/*
 * Save the current items database to persistent storage (currently unimplemented)
 */
void ie_save( void )
{
	d_LogInfo("Items save functionality not yet implemented");
	// TODO: Implement items database serialization
}

/*
 * Load items database from persistent storage (currently unimplemented)
 */
void ie_load( void )
{
	d_LogInfo("Items load functionality not yet implemented");
	// TODO: Implement items database loading from file
}

/*
 * Initialize the Items Editor subsystem and set up the UI framework
 */
void e_InitItemEditor( void )
{
    aWidget_t* w;
    app.delegate.logic = e_ItemEditorDoLoop;
    app.delegate.draw  = e_ItemEditorRenderLoop;

    // CHANGED: Initialize status display using Daedalus functions
    if (items_status_text == NULL) {
        items_status_text = d_InitString();
    }
    d_FormatString(items_status_text, "Items: %d loaded", items_count);

    // Load items database if not already loaded
    if (items_database == NULL) {
        items_database = load_items_database(&items_count);
        d_LogInfoF("Loaded %d items for editing", items_count);
    }

    a_InitWidgets( "resources/widgets/editor/items.json" );
    app.active_widget = a_GetWidget( "tab_bar" );

	// DEFENSIVE: Handle missing widgets gracefully for testing
	if (app.active_widget != NULL) {
		// Set up main tab navigation
		aContainerWidget_t* tab_container = ( aContainerWidget_t* )app.active_widget->data;
		if (tab_container != NULL) {
			for ( int i = 0; i < tab_container->num_components; i++ ) {
				aWidget_t* current = &tab_container->components[i];

				if ( strcmp( current->name, "world" ) == 0 ) {
					current->action = e_InitWorldEditor;
				}
				
				if ( strcmp( current->name, "item" ) == 0 ) {
					current->action = e_InitItemEditor;
				}
				
				if ( strcmp( current->name, "entity" ) == 0 ) {
					current->action = e_InitEntityEditor;
				}
				
				if ( strcmp( current->name, "colors" ) == 0 ) {
					current->action = e_InitColorEditor;
				}
				
				if ( strcmp( current->name, "ui" ) == 0 ) {
					current->action = e_InitUIEditor;
				}
			}
		} else {
			d_LogWarning("Tab container is NULL - widget system not fully initialized");
		}
	} else {
		d_LogWarning("Tab bar widget not found - running in minimal mode");
	}

	// Set up items menu bar (like world_menu_bar in world editor)
	w = a_GetWidget( "items_menu_bar" );
	if (w != NULL && w->data != NULL) {
		aContainerWidget_t* items_menu_container = ( aContainerWidget_t* )w->data;
		for ( int i = 0; i < items_menu_container->num_components; i++ ) {
			aWidget_t* current = &items_menu_container->components[i];

			if ( strcmp( current->name, "creation" ) == 0 ) {
				current->action = ie_creation;
			}

			if ( strcmp( current->name, "edit" ) == 0 ) {
				current->action = ie_edit;
			}

			if ( strcmp( current->name, "save" ) == 0 ) {
				current->action = ie_save;
			}

			if ( strcmp( current->name, "load" ) == 0 ) {
				current->action = ie_load;
			}
		}
	} else {
		d_LogWarning("Items menu bar widget not found - menu callbacks not configured");
	}

	d_LogInfo("Items Editor initialized with menu system and database");
}

static void e_ItemEditorDoLoop( float dt )
{
	a_DoInput();
    
    // CHANGED: Update the dString instead of using snprintf
    if (items_status_text != NULL) {
        d_ClearString(items_status_text);
        d_FormatString(items_status_text, "Items Database: %d items loaded", items_count);
    }
	
	if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 ) {
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		
		// Clean up before returning to main editor
		if (items_database != NULL) {
			e_DestroyItemEditor();
		}
		
		e_InitEditor();
	}

	a_DoWidget();
}

static void e_ItemEditorRenderLoop( float dt )
{
    (void)dt; // dt is not used in this loop yet, silence warning.

	// Draw database status and statistics
    ie_DrawDbStatus(items_status_text);
    ie_DrawDbStats(items_database, items_count);

    a_DrawFilledRect( 100, 100, 32, 32, 255, 165, 0, 255 );  // Orange for items
    a_DrawFilledRect( 300, 300, 32, 32, 255, 215, 0, 255 );  // Gold for items

    a_DrawWidgets();
}

/*
 * Clean up and destroy the Items Editor subsystem
 */
void e_DestroyItemEditor( void )
{
    if (items_database != NULL) {
        ie_FreeItemsDatabase(items_database, items_count);
        items_database = NULL;
        items_count = 0;
    }
    
    // CHANGED: Use d_DestroyString for proper cleanup
    if (items_status_text != NULL) {
        d_DestroyString(items_status_text);
        items_status_text = NULL;
    }
    
    d_LogInfo("Items Editor destroyed and memory cleaned up");
}

/*
 * Load items database from storage or create empty database for editing
 */
Item_t* load_items_database(int* items_count)
{	
	if (g_force_load_failure) {
        d_LogDebug("Chaos flag active: Forcing database load failure for test.");
        *items_count = 0;
        return NULL;
    }
	d_LogInfo("Loading items database...");
	
	// Allocate memory for items database
	Item_t* database = malloc(sizeof(Item_t) * MAX_ITEMS);
	if (database == NULL) {
		d_LogError("Failed to allocate memory for items database");
		*items_count = 0;
		return NULL;
	}
	
	// Initialize with some test items for development
	*items_count = 3; // Start with 3 test items
	
	// Test Item 1: Basic Sword
	database[0].type = ITEM_TYPE_WEAPON;
	database[0].name = d_InitString();
	d_SetString(database[0].name, "Iron Sword", 0);
	database[0].description = d_InitString();
	d_SetString(database[0].description, "A basic iron sword", 0);
	database[0].id = d_InitString();
	d_SetString(database[0].id, "iron_sword", 0);
	database[0].rarity = d_InitString();
	d_SetString(database[0].rarity, "common", 0);
	database[0].glyph = 47; // Assuming '/' character for sword
	database[0].weight_kg = 2.5f;
	database[0].value_coins = 50;
	database[0].data.weapon.min_damage = 8;
	database[0].data.weapon.max_damage = 12;
	database[0].data.weapon.range_tiles = 0; // Melee
	database[0].data.weapon.durability = 200;
	
	// Test Item 2: Basic Armor
	database[1].type = ITEM_TYPE_ARMOR;
	database[1].name = d_InitString();
	d_SetString(database[1].name, "Leather Armor", 0);
	database[1].description = d_InitString();
	d_SetString(database[1].description, "Basic leather protection", 0);
	database[1].id = d_InitString();
	d_SetString(database[1].id, "leather_armor", 0);
	database[1].rarity = d_InitString();
	d_SetString(database[1].rarity, "common", 0);
	database[1].glyph = 91; // Assuming '[' character for armor
	database[1].weight_kg = 5.0f;
	database[1].value_coins = 75;
	database[1].data.armor.armor_value = 15;
	database[1].data.armor.evasion_value = 5;
	database[1].data.armor.durability = 150;
	
	// Test Item 3: Health Potion
	database[2].type = ITEM_TYPE_CONSUMABLE;
	database[2].name = d_InitString();
	d_SetString(database[2].name, "Health Potion", 0);
	database[2].description = d_InitString();
	d_SetString(database[2].description, "Restores health when consumed", 0);
	database[2].id = d_InitString();
	d_SetString(database[2].id, "health_potion", 0);
	database[2].rarity = d_InitString();
	d_SetString(database[2].rarity, "common", 0);
	database[2].glyph = 33; // Assuming '!' character for potion
	database[2].weight_kg = 0.2f;
	database[2].value_coins = 25;
	database[2].data.consumable.value = 50; // Heal amount
	database[2].data.consumable.duration_seconds = 0; // Instant effect
	
	d_LogInfoF("Created items database with %d test items", *items_count);
	return database;
}

/*
 * Free items database and all associated memory defensively
 */
void ie_FreeItemsDatabase(Item_t* database, int count)
{
	// First, check if the database pointer itself is NULL.
	if (database == NULL) {
		d_LogWarning("Attempted to free a NULL items database pointer.");
		return;
	}
	
	d_LogInfoF("Freeing items database with %d items...", count);
	
	// Loop through each item in the database.
	for (int i = 0; i < count; i++) {
		// Log which item index we are processing. This is always safe.

		// Defensively check and free the 'name' string.
		if (database[i].name != NULL) {
			d_DestroyString(database[i].name);
		} else {
			d_LogWarningF("  - Item %d has a NULL name pointer. Cannot free.", i);
		}
		
		// Defensively check and free the 'description' string.
		if (database[i].description != NULL) {
			d_DestroyString(database[i].description);
		} else {
			d_LogWarningF("  - Item %d has a NULL description pointer. Cannot free.", i);
		}
		
		// Defensively check and free the 'id' string.
		if (database[i].id != NULL) {
			d_DestroyString(database[i].id);
		} else {
			d_LogWarningF("  - Item %d has a NULL id pointer. Cannot free.", i);
		}
		
		// Defensively check and free the 'rarity' string.
		if (database[i].rarity != NULL) {
			d_DestroyString(database[i].rarity);
		} else {
			d_LogWarningF("  - Item %d has a NULL rarity pointer. Cannot free.", i);
		}
	}
	
	// Finally, free the main database array itself.
	free(database);
	d_LogInfo("Items database memory freed successfully.");
}

void ie_AddItemToDatabase(Item_t* item) {
    d_LogInfo("STUB: ie_AddItemToDatabase called");
    // This will eventually add the item to the main items_database.
    // We destroy the item here for now to prevent memory leaks during testing.
    if (item) {
        destroy_item(item);
    }
}

void ie_DrawItemCreationPreview(int t, int m, int r, int q, int e) {
    (void)t;(void)m;(void)r;(void)q;(void)e; // Silence warnings
}

// Stubs for edit mode functions
Material_t* load_materials_database(int* count) {
    d_LogInfo("STUB: load_materials_database called");
    if (count) *count = 0;
    return NULL;
}

void ie_ApplyMaterialToItem(Item_t* item, Material_t* material) {
    (void)item; (void)material;
}

void ie_AdjustItemProperty(Item_t* item, int prop_idx, int adj) {
    (void)item; (void)prop_idx; (void)adj;
}

// ============================================================================
// PUBLIC GETTERS for accessing module state
// ============================================================================

/**
 * @brief Provides safe, read-only access to the items database.
 * @return A pointer to the global item database.
 */
Item_t* ie_GetDatabase(void) {
    return items_database;
}

/**
 * @brief Provides safe, read-only access to the item count.
 * @return The current number of items in the database.
 */
int ie_GetItemCount(void) {
    return items_count;
}

/**
 * @brief Provides safe, read-only access to the status text string.
 * @return A pointer to the dString for the status text.
 */
dString_t* ie_GetStatusText(void) {
    return items_status_text;
}
