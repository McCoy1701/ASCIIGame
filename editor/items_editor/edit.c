// editor/items_editor/edit.c - Items Editor Edit Mode
// Allows user to create, modify, and manage item data with advanced property editing

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "editor.h"
#include "items_editor.h"
#include "items.h"
#include "structs.h"
#include "defs.h"

/*
 * Execute one frame of item editor logic processing and user interaction
 *
 * @param dt: Delta time in seconds since last frame for timing calculations
 * @return: void
 * -- Processes mouse clicks for item selection, property editing, and material assignment
 * -- Handles keyboard input for item type cycling and property value adjustment
 * -- Updates selected item properties based on user interaction
 * -- Manages UI state transitions between different editing panels
 * -- Called once per frame by items editor main loop
 * -- Coordinates all user input processing for comprehensive item editing
 */
static void ie_EditLogic( float dt );

/*
 * Render the complete item editing interface with all panels and visual feedback
 *
 * @param dt: Delta time in seconds since last frame for animation timing
 * @return: void
 * -- Draws main item grid with type-based color coding and selection highlighting
 * -- Renders detailed item properties panel with editable values
 * -- Displays material selection grid with quality indicators
 * -- Shows currently selected item preview with full property breakdown
 * -- Provides visual feedback for all interactive elements
 * -- Maintains consistent visual hierarchy for intuitive editing workflow
 */
static void ie_EditDraw( float dt );

// Global state for item editing session
static Item_t* items_database = NULL;           // Main item collection
static int items_count = 0;                     // Total number of items loaded
static int selected_item_index = -1;            // Currently selected item (-1 = none)
static int selected_property_index = 0;         // Which property is being edited
static int selected_material_index = 0;         // Currently selected material
static ItemType_t filter_type = ITEM_TYPE_WEAPON; // Current type filter
static Material_t* materials_database = NULL;   // Available materials
static int materials_count = 0;                 // Total materials available

// UI state tracking
static uint8_t item_grid_x = 0, item_grid_y = 0;     // Mouse position in item grid
static uint8_t material_grid_x = 0, material_grid_y = 0; // Mouse position in material grid
static bool editing_properties = false;               // Property editing mode active
static int property_adjustment = 0;                   // Current property value change

/*
 * Initialize item editing mode with UI setup and data loading
 */
void ie_edit( void )
{
	app.delegate.logic = ie_EditLogic;
	app.delegate.draw = ie_EditDraw;

	app.active_widget = a_GetWidget( "items_edit_menu" );
	aContainerWidget_t* container = a_GetContainerFromWidget( "items_edit_menu" );
	app.active_widget->hidden = 0;

	// Initialize editing state
	selected_item_index = -1;
	selected_property_index = 0;
	selected_material_index = 0;
	editing_properties = false;
	property_adjustment = 0;

	// Load items and materials databases
	if (items_database == NULL) {
		items_database = load_items_database(&items_count);
		d_LogInfoF("Loaded %d items for editing", items_count);
	}
	
	if (materials_database == NULL) {
		materials_database = load_materials_database(&materials_count);
		d_LogInfoF("Loaded %d materials for editing", materials_count);
	}

	// Configure UI components
	for ( int i = 0; i < container->num_components; i++ ) {
		aWidget_t* current = &container->components[i];
		current->hidden = 0;

		if ( strcmp( current->name, "save_items" ) == 0 ) {
			//current->action = ie_SaveItemsDatabase;
		}
		
		if ( strcmp( current->name, "create_item" ) == 0 ) {
			//current->action = ie_CreateNewItem;
		}
		
		if ( strcmp( current->name, "duplicate_item" ) == 0 ) {
			//current->action = ie_DuplicateSelectedItem;
		}
	}

	d_LogInfo("Item editing mode initialized successfully");
}

static void ie_EditLogic( float dt )
{
	a_DoInput();

	// Handle mouse interactions
	if ( app.mouse.button == 1 ) {
		app.mouse.button = 0;
		
		// Check item grid selection
		ie_GetItemCellAtMouse(ITEMS_GRID_WIDTH, ITEMS_GRID_HEIGHT, 
		                      &item_grid_x, &item_grid_y);
		int clicked_index = (item_grid_y * ITEMS_GRID_WIDTH) + item_grid_x;
		
		if (clicked_index >= 0 && clicked_index < items_count) {
			selected_item_index = clicked_index;
			editing_properties = false; // Reset property editing
			d_LogInfoF("Selected item: %s", 
			         items_database[selected_item_index].name->str);
		}
		
		// Check property panel interaction
		if (selected_item_index >= 0) {
			int old_property_index = selected_property_index;
			ie_ItemPropertiesMouseCheck(&items_database[selected_item_index], 
			                           &selected_property_index);
			
			if (old_property_index != selected_property_index) {
				editing_properties = true;
				d_LogInfoF("Editing property index: %d", selected_property_index);
			}
		}
		
		// Check material selection
		ie_MaterialSelectionMouseCheck(&selected_material_index, 
		                              &material_grid_x, &material_grid_y);
	}
	
	// Handle right-click for applying changes
	if ( app.mouse.button == 3 ) {
		app.mouse.button = 0;
		
		if (selected_item_index >= 0 && selected_material_index >= 0) {
			// Apply selected material to current item
			Item_t* item = &items_database[selected_item_index];
			item->material_data = materials_database[selected_material_index];
			
			// Recalculate item properties based on new material
			ie_ApplyMaterialToItem(item, &materials_database[selected_material_index]);
			
			d_LogInfoF("Applied material '%s' to item '%s'",
			         materials_database[selected_material_index].name->str,
			         item->name->str);
		}
	}

	// Handle keyboard input for editing
	ie_ItemTypeKeyboardCheck(&filter_type, &property_adjustment);
	
	// Apply property adjustments
	if (property_adjustment != 0 && selected_item_index >= 0 && editing_properties) {
		ie_AdjustItemProperty(&items_database[selected_item_index], 
		                     selected_property_index, property_adjustment);
		property_adjustment = 0; // Reset after applying
	}
	
	// Handle type filter changes
	if (app.keyboard[SDL_SCANCODE_F] == 1) {
		app.keyboard[SDL_SCANCODE_F] = 0;
		filter_type = (filter_type + 1) % 5; // Cycle through all item types
		d_LogInfoF("Filter changed to: %d", filter_type);
	}
	
	// Handle escape to return to main items editor
	if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 ) {
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		e_InitItemsEditor();
	}

	a_DoWidget();
}

static void ie_EditDraw( float dt )
{
	// Draw main item grid with filtering
	if (items_database != NULL && items_count > 0) {
		Item_t* filtered_items[MAX_ITEMS];
		int filtered_count = ie_FilterItemsByType(&items_database, items_count, 
		                                         filter_type, filtered_items);
		
		for (int i = 0; i < filtered_count && i < ITEMS_GRID_MAX; i++) {
			bool is_selected = (i == selected_item_index);
			ie_DrawItemCell(i, filtered_items[i], item_grid_x, item_grid_y, is_selected);
		}
		
		// Draw filter status
		char filter_text[64];
		const char* type_names[] = {"WEAPON", "ARMOR", "KEY", "CONSUMABLE", "AMMUNITION"};
		d_LogInfoF("Filter: %s (%d items)", type_names[filter_type], filtered_count);
		a_DrawText(filter_text, 10, 10, 255, 255, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
	}
	
	// Draw item properties panel
	if (selected_item_index >= 0 && selected_item_index < items_count) {
		ie_DrawItemProperties(&items_database[selected_item_index],
		                     ITEM_PROPERTIES_PANEL_X, ITEM_PROPERTIES_PANEL_Y,
		                     selected_property_index);
		
		// Highlight editing mode
		if (editing_properties) {
			a_DrawRect(ITEM_PROPERTIES_PANEL_X - 2, ITEM_PROPERTIES_PANEL_Y - 2,
			          ITEM_PROPERTIES_PANEL_WIDTH + 4, 200 + 4,
			          255, 255, 0, 255); // Yellow border for editing mode
		}
	}
	
	// Draw material selection grid
	if (materials_database != NULL && materials_count > 0) {
		ie_DrawMaterialGrid(materials_database, materials_count, selected_material_index);
	}
	
	// Draw selected item preview (large)
	if (selected_item_index >= 0) {
		Item_t* item = &items_database[selected_item_index];
		
		// Large item icon
		if (game_glyphs && item->glyph < game_glyphs->count) {
			a_DrawFilledRect(ITEM_PREVIEW_X, ITEM_PREVIEW_Y, 
			                game_glyphs->rects[item->glyph].w * 4,
			                game_glyphs->rects[item->glyph].h * 4,
			                50, 50, 50, 255);
			
			a_BlitTextureRect(game_glyphs->texture, game_glyphs->rects[item->glyph],
			                 ITEM_PREVIEW_X, ITEM_PREVIEW_Y, 4);
		}
		
		// Item name display
		if (item->name && item->name->str) {
			a_DrawText(item->name->str, ITEM_PREVIEW_X, ITEM_PREVIEW_Y + 80,
			          255, 255, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
		}
	}
	
	// Draw UI hints
	a_DrawText("Left Click: Select | Right Click: Apply Material | F: Filter | TAB: Type | +/-: Adjust",
	          10, SCREEN_HEIGHT - 30, 200, 200, 200, app.font_type, TEXT_ALIGN_LEFT, 0);

	a_DrawWidgets();
}