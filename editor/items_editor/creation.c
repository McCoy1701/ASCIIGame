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
// Chaos Flag for testing failure cases
static bool g_force_creation_failure = false;
/**
 * Default callback for consumables.
 */
static void _default_on_consume(uint8_t val) {
    (void)val; // Do nothing, just satisfy the function pointer requirement.
}
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

    if (app.active_widget != NULL) {
        app.active_widget->hidden = 0;
    } else {
        d_LogWarning("Creation mode entered but 'item_generation_menu' widget not found!");
    }

    // Reset creation parameters to defaults
    item_type_index = 0;
    material_index = 0;
    rarity_index = 0;
    quality_index = 0;
    enchantment_index = 0;

    // FIX: Also add a NULL check for the container before looping through its components.
    if (container != NULL) {
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
    } else {
        d_LogWarning("Creation mode running without 'item_generation_menu' container; actions not set.");
    }


    d_LogInfo("Item creation mode initialized");
}

static void ie_CreationDoLoop( float dt )
{
	a_DoInput();

	if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 ) {
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		e_InitItemEditor();
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
    const char* type_names[] = {"WEAPON", "ARMOR", "KEY", "CONSUMABLE", "AMMUNITION", "INVALID"};
    const char* rarity_names[] = {"COMMON", "UNCOMMON", "RARE", "EPIC", "LEGENDARY", "INVALID"};
    const char* quality_names[] = {"POOR", "AVERAGE", "GOOD", "EXCELLENT", "MASTERWORK", "INVALID"};
    
    // Add safety checks to prevent reading out of bounds if an index is invalid.
    const char* type_str = (item_type_index < 5) ? type_names[item_type_index] : type_names[5];
    const char* rarity_str = (rarity_index < 5) ? rarity_names[rarity_index] : rarity_names[5];
    const char* quality_str = (quality_index < 5) ? quality_names[quality_index] : quality_names[5];

    // Draw item preview based on current selections
    ie_DrawItemCreationPreview(item_type_index, material_index, rarity_index, 
                              quality_index, enchantment_index);


    a_DrawWidgets();
}
/**
 * @brief Creates an item using the specified parameters
 */
Item_t* ie_CreateItemWithParameters(ItemType_t type, int mat_id, int rare, int qual, int ench)
{
	if (g_force_creation_failure) {
        d_LogDebug("Chaos flag active: Forcing item creation failure for test.");
        return NULL;
    }
    // This function creates temporary resources (like materials) to build the final item.
    // We must be careful to clean them up before the function returns.
    MaterialProperties_t props = create_default_material_properties();
    Material_t material = create_material("Iron", props);

    Item_t* new_item = NULL;
    
    // Silence unused parameter warnings for now, as these will be used later.
    (void)mat_id; (void)rare; (void)qual; (void)ench;

    // Call the correct low-level creator based on the selected type
    switch (type) {
        case ITEM_TYPE_WEAPON:
            new_item = create_weapon("Crafted Sword", "crafted_weapon", material, 5, 10, 1, '/');
            break;
        case ITEM_TYPE_ARMOR:
            new_item = create_armor("Crafted Mail", "crafted_armor", material, 15, 2, '[', 0, 0);
            break;
        case ITEM_TYPE_KEY: {
            // Create a temporary lock on the stack.
            Lock_t lock = create_lock("Generic Door", "A generic lock.", 50, 10);
            
            // create_key performs a deep copy of the lock's internal strings.
            new_item = create_key("Crafted Key", "crafted_key", lock, '~');
            
            // FIX: We must now destroy the temporary lock's strings to prevent a memory leak.
            destroy_lock(&lock);
            break;
        }
        case ITEM_TYPE_CONSUMABLE:
            // Consumables don't use the standard material, so no need to pass it.
            // The create_consumable function handles its own 'organic' material.
            new_item = create_consumable("Crafted Potion", "crafted_consumable", 50, _default_on_consume, '!');
			break;
        case ITEM_TYPE_AMMUNITION:
            new_item = create_ammunition("Crafted Arrow", "crafted_ammo", material, 3, 6, '|');
            break;
        default:
            d_LogErrorF("Attempted to create item with unknown type: %d", type);
            break;
    }
    
    // FIX: Clean up the name string from the temporary material struct.
    // This was the other source of a memory leak.
    d_DestroyString(material.name);

    return new_item;
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

    if (app.active_widget == NULL) {
        d_LogError("Cannot generate item: No active widget is set.");
        return;
    }

    aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;
    if (container == NULL) {
        d_LogError("Cannot generate item: Active widget has no UI container.");
        return;
    }
    
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