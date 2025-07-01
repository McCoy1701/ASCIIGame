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
char* items_status_text = NULL;          // Status display text

/*
 * Enter item creation mode (placeholder implementation)
 */
void ie_creation(void)
{
	d_LogInfo("Item creation mode requested - not yet fully implemented");
	// TODO: Implement full item creation mode
	// This will eventually call the ie_creation() function from items_editor/creation.c
}

/*
 * Enter item editing mode (placeholder implementation)
 */
void ie_edit(void)
{
	d_LogInfo("Item editing mode requested - not yet fully implemented");
	// TODO: Implement full item editing mode
	// This will eventually call the ie_edit() function from items_editor/edit.c
}

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

	// Initialize status display
	items_status_text = malloc( sizeof(char) * 100 );
	snprintf(items_status_text, 100, "Items: %d loaded", items_count);

	// Load items database if not already loaded
	if (items_database == NULL) {
		items_database = load_items_database(&items_count);
		d_LogInfoF("Loaded %d items for editing", items_count);
	}

	a_InitWidgets( "resources/widgets/editor/items.json" );
	app.active_widget = a_GetWidget( "tab_bar" );

	// Set up main tab navigation
	aContainerWidget_t* tab_container = ( aContainerWidget_t* )app.active_widget->data;
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

	// Set up items menu bar (like world_menu_bar in world editor)
	w = a_GetWidget( "items_menu_bar" );
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

	d_LogInfo("Items Editor initialized with menu system and database");
}

static void e_ItemEditorDoLoop( float dt )
{
	a_DoInput();
	
	// Update status text
	if (items_status_text != NULL) {
		snprintf(items_status_text, 100, "Items Database: %d items loaded", items_count);
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
	// Draw items database status
	if (items_status_text != NULL) {
		a_DrawText( items_status_text, 750, 10, 255, 255, 255, app.font_type,
		           TEXT_ALIGN_CENTER, 0 );
	}

	// Draw database statistics
	if (items_database != NULL) {
		char stats_text[256];
		int weapon_count = 0, armor_count = 0, consumable_count = 0;
		
		// Count items by type
		for (int i = 0; i < items_count; i++) {
			switch (items_database[i].type) {
				case ITEM_TYPE_WEAPON: weapon_count++; break;
				case ITEM_TYPE_ARMOR: armor_count++; break;
				case ITEM_TYPE_CONSUMABLE: consumable_count++; break;
				default: break;
			}
		}
		
		snprintf(stats_text, 256, "Weapons: %d | Armor: %d | Consumables: %d",
		        weapon_count, armor_count, consumable_count);
		a_DrawText( stats_text, 50, 50, 200, 200, 200, app.font_type,
		           TEXT_ALIGN_LEFT, 0 );
	}

	// Visual indicators (different colors from world editor)
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
	
	if (items_status_text != NULL) {
		free(items_status_text);
		items_status_text = NULL;
	}
	
	d_LogInfo("Items Editor destroyed and memory cleaned up");
}

/*
 * Load items database from storage or create empty database for editing
 */
Item_t* load_items_database(int* items_count)
{
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
	database[2].glyph = 33; // Assuming '!' character for potion
	database[2].weight_kg = 0.2f;
	database[2].value_coins = 25;
	database[2].data.consumable.value = 50; // Heal amount
	database[2].data.consumable.duration_seconds = 0; // Instant effect
	
	d_LogInfoF("Created items database with %d test items", *items_count);
	return database;
}

/*
 * Free items database and all associated memory
 */
void ie_FreeItemsDatabase(Item_t* database, int count)
{
	if (database == NULL) {
		d_LogWarning("Attempted to free NULL items database");
		return;
	}
	
	d_LogInfoF("Freeing items database with %d items", count);
	
	// Free individual item data
	for (int i = 0; i < count; i++) {
		if (database[i].name != NULL) {
			d_DestroyString(database[i].name);
		}
		if (database[i].description != NULL) {
			d_DestroyString(database[i].description);
		}
		if (database[i].id != NULL) {
			d_DestroyString(database[i].id);
		}
		if (database[i].rarity != NULL) {
			d_DestroyString(database[i].rarity);
		}
	}
	
	// Free the main database array
	free(database);
	d_LogInfo("Items database memory freed successfully");
}

