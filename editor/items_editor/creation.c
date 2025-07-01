// editor/items_editor/creation.c - Items Editor Creation Mode
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

/*
 * Execute one frame of item creation logic processing and parameter selection
 *
 * @param dt: Delta time in seconds since last frame for timing calculations
 * @return: void
 * -- Processes user input for escaping back to main items editor
 * -- Handles widget interactions for parameter selection dropdowns
 * -- Updates global creation parameter indices based on UI selections
 * -- Called once per frame during item creation mode
 * -- Manages UI state and user interaction for item generation workflow
 */
static void ie_CreationDoLoop( float dt );

/*
 * Render the item creation interface with parameter selection controls
 *
 * @param dt: Delta time in seconds since last frame for animation timing
 * @return: void
 * -- Draws all creation parameter widgets and selection dropdowns
 * -- Renders preview of item being created with current selections
 * -- Shows generation progress and parameter validation feedback
 * -- Provides visual feedback for all interactive creation elements
 * -- Maintains consistent visual hierarchy for intuitive creation workflow
 */
static void ie_CreationRenderLoop( float dt );

// Global selection indices for item creation parameters
static int item_type_index     = 0;  // WEAPON/ARMOR/CONSUMABLE/KEY/AMMUNITION
static int material_index      = 0;  // Iron/Steel/Mithril/etc selection
static int rarity_index        = 0;  // Common/Uncommon/Rare/Epic/Legendary
static int quality_index       = 0;  // Poor/Average/Good/Excellent/Masterwork
static int enchantment_index   = 0;  // None/Minor/Major/Epic enchantment level

/*
 * Initialize item creation mode with parameter selection UI and preview system
 */
void ie_creation( void )
{
	app.delegate.logic = ie_CreationDoLoop;
	app.delegate.draw  = ie_CreationRenderLoop;

	app.active_widget = a_GetWidget( "item_generation_menu" );
	aContainerWidget_t* container = a_GetContainerFromWidget( "item_generation_menu" );

	app.active_widget->hidden = 0;

	// Reset creation parameters to defaults
	item_type_index = 0;
	material_index = 0;
	rarity_index = 0;
	quality_index = 0;
	enchantment_index = 0;

	for ( int i = 0; i < container->num_components; i++ ) {
		aWidget_t* current = &container->components[i];
		current->hidden = 0;

		if ( strcmp( current->name, "generate_item" ) == 0 ) {
			current->action = iec_GenerateItem;
		}
		
		if ( strcmp( current->name, "random_item" ) == 0 ) {
			current->action = iec_GenerateRandomItem;
		}
		
		if ( strcmp( current->name, "template_load" ) == 0 ) {
			current->action = iec_LoadItemTemplate;
		}
	}

	d_LogInfo("Item creation mode initialized with parameter selection UI");
}

static void ie_CreationDoLoop( float dt )
{
	a_DoInput();

	if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 ) {
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		e_InitItemsEditor();
	}

	// Handle hotkeys for quick parameter cycling
	if ( app.keyboard[ SDL_SCANCODE_1 ] == 1 ) {
		app.keyboard[SDL_SCANCODE_1] = 0;
		item_type_index = (item_type_index + 1) % 5; // Cycle through item types
		d_LogInfoF("Item type cycled to index: %d", item_type_index);
	}
	
	if ( app.keyboard[ SDL_SCANCODE_2 ] == 1 ) {
		app.keyboard[SDL_SCANCODE_2] = 0;
		rarity_index = (rarity_index + 1) % 5; // Cycle through rarity levels
		d_LogInfoF("Rarity cycled to index: %d", rarity_index);
	}

	a_DoWidget();
}

static void ie_CreationRenderLoop( float dt )
{
	// Draw parameter selection status
	char status_text[256];
	const char* type_names[] = {"WEAPON", "ARMOR", "KEY", "CONSUMABLE", "AMMUNITION"};
	const char* rarity_names[] = {"COMMON", "UNCOMMON", "RARE", "EPIC", "LEGENDARY"};
	const char* quality_names[] = {"POOR", "AVERAGE", "GOOD", "EXCELLENT", "MASTERWORK"};
	
	d_LogInfoF("Creating: %s | Rarity: %s | Quality: %s",
	         type_names[item_type_index], rarity_names[rarity_index], quality_names[quality_index]);
	a_DrawText(status_text, 50, 50, 255, 255, 255, app.font_type, TEXT_ALIGN_LEFT, 0);

	// Draw item preview based on current selections
	ie_DrawItemCreationPreview(item_type_index, material_index, rarity_index, 
	                          quality_index, enchantment_index);

	// Draw creation hints
	a_DrawText("1: Cycle Type | 2: Cycle Rarity | ESC: Back to Editor",
	          50, SCREEN_HEIGHT - 60, 200, 200, 200, app.font_type, TEXT_ALIGN_LEFT, 0);

	a_DrawWidgets();
}

/*
 * Generate a new item based on current parameter selections and add to database
 */
void iec_GenerateItem( void )
{
	ItemType_t new_item_type = ITEM_TYPE_WEAPON;
	int new_material_id = 0;
	int new_rarity_level = 0;
	int new_quality_level = 0;
	int new_enchantment_level = 0;

	aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;

	// Read values from UI widgets
	for ( int i = 0; i < container->num_components; i++ ) {
		aWidget_t* current = &container->components[i];

		if ( strcmp( current->name, "item_type" ) == 0 ) {
			aSelectWidget_t* type_select = ( aSelectWidget_t* )current->data;
			item_type_index = type_select->value;
		}

		if ( strcmp( current->name, "material_type" ) == 0 ) {
			aSelectWidget_t* material_select = ( aSelectWidget_t* )current->data;
			material_index = material_select->value;
		}

		if ( strcmp( current->name, "rarity_level" ) == 0 ) {
			aSelectWidget_t* rarity_select = ( aSelectWidget_t* )current->data;
			rarity_index = rarity_select->value;
		}

		if ( strcmp( current->name, "quality_level" ) == 0 ) {
			aSelectWidget_t* quality_select = ( aSelectWidget_t* )current->data;
			quality_index = quality_select->value;
		}

		if ( strcmp( current->name, "enchantment_level" ) == 0 ) {
			aSelectWidget_t* enchant_select = ( aSelectWidget_t* )current->data;
			enchantment_index = enchant_select->value;
		}
	}

	// Map indices to actual values
	new_item_type = (ItemType_t)item_type_index;
	new_material_id = material_index;
	new_rarity_level = rarity_index;
	new_quality_level = quality_index;
	new_enchantment_level = enchantment_index;

	// Create the item with selected parameters
	Item_t* new_item = ie_CreateItemWithParameters(new_item_type, new_material_id,
	                                              new_rarity_level, new_quality_level,
	                                              new_enchantment_level);

	if (new_item != NULL) {
		// Add to items database
		ie_AddItemToDatabase(new_item);
		
		d_LogInfoF("Generated new item: %s (Type: %d, Material: %d, Rarity: %d)",
		         new_item->name->str, new_item_type, new_material_id, new_rarity_level);
		
		// Switch to edit mode to show the new item
		ie_edit();
	} else {
		d_LogError("Failed to generate item with selected parameters");
	}
}

/*
 * Generate a completely random item with random parameters for quick testing
 */
void iec_GenerateRandomItem( void )
{
	// Generate random parameters with weighted distribution
	ItemType_t random_type = rand() % 5;
	int random_material = rand() % 10; // Assuming 10 materials available
	
	// Weighted rarity (common items more likely)
	int rarity_roll = rand() % 100;
	int random_rarity;
	if (rarity_roll < 50) random_rarity = 0;      // Common (50%)
	else if (rarity_roll < 75) random_rarity = 1; // Uncommon (25%)
	else if (rarity_roll < 90) random_rarity = 2; // Rare (15%)
	else if (rarity_roll < 98) random_rarity = 3; // Epic (8%)
	else random_rarity = 4;                       // Legendary (2%)
	
	int random_quality = rand() % 5;
	int random_enchantment = (random_rarity > 1) ? (rand() % 4) : 0; // Higher rarity = more enchantment chance

	// Create the random item
	Item_t* random_item = ie_CreateItemWithParameters(random_type, random_material,
	                                                 random_rarity, random_quality,
	                                                 random_enchantment);

	if (random_item != NULL) {
		ie_AddItemToDatabase(random_item);
		
		const char* type_names[] = {"WEAPON", "ARMOR", "KEY", "CONSUMABLE", "AMMUNITION"};
		const char* rarity_names[] = {"COMMON", "UNCOMMON", "RARE", "EPIC", "LEGENDARY"};
		
		d_LogInfoF("Generated random item: %s (%s %s)",
		         random_item->name->str, rarity_names[random_rarity], type_names[random_type]);
		
		ie_edit();
	} else {
		d_LogError("Failed to generate random item");
	}
}

/*
 * Load an item template from file for modification and customization
 */
void iec_LoadItemTemplate( void )
{
	// Implementation pending - requires file browser system
	d_LogInfo("Item template loading not yet implemented");
	// TODO: Implement template loading from file system
}