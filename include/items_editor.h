// include/items_editor.h - Items Editor Subsystem
// Item creation, editing, and management interface

#ifndef __ITEMS_EDITOR_H__
#define __ITEMS_EDITOR_H__

#include "structs.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Saves the current item database to a file.
 * 
 * @param filename The path of the file to save.
 * @return 0 on success, -1 on failure.
 */
int ie_save(const char* filename);

/**
 * @brief Loads an item database from a file, replacing the current one.
 * 
 * @param filename The path of the file to load.
 * @return 0 on success, -1 on failure.
 */
int ie_load(const char* filename);

/*
 * Initialize the Items Editor subsystem and set up the UI framework
 *
 * @return: void
 * -- Sets up the delegate functions for items editor logic and rendering loops
 * -- Initializes widget system from "resources/widgets/editor/items.json"
 * -- Configures tab container with action callbacks for all editor modes
 * -- Establishes navigation between world, item, entity, colors, and ui editors
 * -- Must be called before any other items editor functions
 * -- Automatically switches application state to items editor mode
 * -- Safe to call multiple times (reinitializes UI components)
 * -- Prepares the complete item editing environment for user interaction
 */
void e_InitItemEditor( void );

/*
 * Clean up and destroy the Items Editor subsystem
 *
 * @return: void
 * -- Frees any allocated item editor resources and memory structures
 * -- Does not destroy the widget system (handled separately)
 * -- Should be called when exiting items editor mode
 * -- Safe to call even if no items are currently loaded
 * -- After calling, items editor functions become unsafe to use
 * -- Currently unimplemented but reserved for future cleanup logic
 * -- Prevents memory leaks during editor mode transitions
 */
void e_DestroyItemEditor( void );

/*
 * Render a single item in the items grid with icon, name, and basic properties
 *
 * @param index: Linear index of the item in the grid layout
 * @param item: Pointer to item structure containing display data
 * @param grid_x: X coordinate within the items grid
 * @param grid_y: Y coordinate within the items grid
 * @param is_selected: Boolean flag indicating if this item is currently selected
 * @return: void
 * -- Renders background color based on item type and selection state
 * -- Displays item glyph using game font texture at 2x scale
 * -- Shows truncated item name at bottom of cell
 * -- Uses type-specific color coding for visual categorization
 * -- Selection highlighting overrides type colors with yellow background
 * -- Handles NULL item pointers gracefully without crashing
 */
void ie_DrawItemCell(int index, Item_t* item, int panel_x, int panel_y, bool is_selected);

/*
 * Calculate screen position and dimensions for an item cell in the grid
 *
 * @param index: Linear index of the cell within the items grid
 * @param grid_width: Total width of the grid in cells
 * @param grid_height: Total height of the grid in cells
 * @param x: Output pointer for calculated X screen position
 * @param y: Output pointer for calculated Y screen position
 * @param w: Output pointer for calculated cell width in pixels
 * @param h: Output pointer for calculated cell height in pixels
 * @return: void
 * -- Converts linear grid index to 2D screen coordinates for rendering
 * -- Uses ITEMS_GRID_START_X/Y constants for grid origin positioning
 * -- Cell dimensions based on ITEMS_CELL_WIDTH/HEIGHT constants
 * -- Output pointers must be valid; no bounds checking performed
 * -- Essential for translating logical grid positions to visual display
 */
void ie_GetItemCellSize(int index, int grid_width, int grid_height, 
                        int* x, int* y, int* w, int* h, int panel_x, int panel_y);

/*
 * Determine which item cell contains the current mouse cursor position
 *
 * @param grid_width: Width of the items grid in cells for boundary calculations
 * @param grid_height: Height of the items grid in cells for boundary calculations
 * @param grid_x: Output pointer for calculated grid X coordinate
 * @param grid_y: Output pointer for calculated grid Y coordinate
 * @return: void
 * -- Performs mouse-to-grid coordinate translation for item selection
 * -- Only updates output coordinates if mouse is within grid boundaries
 * -- Uses ITEMS_GRID_START_X/Y and cell dimensions for area detection
 * -- Output coordinates remain unchanged if mouse is outside grid area
 * -- Critical for item clicking and selection interface functionality
 */
void ie_GetItemCellAtMouse(int grid_width, int grid_height, uint8_t* grid_x, uint8_t* grid_y, int panel_x, int panel_y);

/*
 * Handle mouse interaction with the item properties panel for editing
 *
 * @param selected_item: Pointer to currently selected item for property editing
 * @param property_index: Output pointer for which property line was clicked
 * @return: void
 * -- Detects mouse clicks within the item properties display panel
 * -- Calculates which property line was clicked based on mouse Y position
 * -- Only processes clicks when a valid item is selected
 * -- Property index corresponds to editable fields in the properties display
 * -- Used for enabling inline editing of item statistics and attributes
 * -- Enables direct manipulation of damage, armor, weight, and other values
 */
void ie_ItemPropertiesMouseCheck(Item_t* selected_item, int* property_index, int panel_x, int panel_y);

/*
 * Handle mouse interaction with the material selection panel
 *
 * @param material_index: Output pointer for selected material array index
 * @param grid_x: Output pointer for material grid X coordinate
 * @param grid_y: Output pointer for material grid Y coordinate
 * @return: void
 * -- Detects mouse clicks within the material selection grid
 * -- Converts grid coordinates to linear material array index
 * -- Uses MATERIALS_PANEL position and cell dimensions for hit detection
 * -- Essential for material assignment during item creation and modification
 * -- Enables visual material selection interface for crafting workflow
 * -- Material index can be used to lookup material properties and apply to items
 */
void ie_MaterialSelectionMouseCheck(int* material_index, uint8_t* grid_x, uint8_t* grid_y);

/*
 * Handle keyboard input for item type cycling and property adjustment
 *
 * @param current_type: Pointer to current ItemType_t for cycling through types
 * @param property_adjustment: Output pointer for property increment/decrement value
 * @return: void
 * -- TAB key cycles through available item types (weapon, armor, consumable, etc.)
 * -- EQUALS key (+) increases selected property value by 1
 * -- MINUS key (-) decreases selected property value by 1
 * -- Provides keyboard-driven item creation and editing workflow
 * -- Type cycling wraps around from ammunition back to weapon
 * -- Property adjustments respect item type constraints and value limits
 * -- Essential for rapid item prototyping and parameter tweaking
 */
void ie_ItemTypeKeyboardCheck(ItemType_t* current_type, int* property_adjustment);

/*
 * Render item properties in a formatted panel display with selection highlighting
 *
 * @param item: Pointer to item whose properties should be displayed
 * @param panel_x: Screen X coordinate for the properties panel origin
 * @param panel_y: Screen Y coordinate for the properties panel origin
 * @param selected_property: Index of currently selected property for editing highlight
 * @return: void
 * -- Displays item name, type, weight, value, and type-specific properties
 * -- Uses color coding to distinguish different property categories
 * -- Highlights selected property line for editing mode indication
 * -- Shows weapon damage/range or armor protection/evasion based on type
 * -- Renders dark background panel for improved text readability
 * -- Handles NULL item pointers gracefully by showing empty panel
 */
void ie_DrawItemProperties(Item_t* item, int panel_x, int panel_y, int selected_property);

/*
 * Render material selection grid with property previews and quality indicators
 *
 * @param materials: Array of available materials for selection
 * @param material_count: Number of valid materials in the array
 * @param selected_index: Index of currently selected material for highlighting
 * @return: void
 * -- Displays materials in grid layout with name and quality indicators
 * -- Uses color-coded quality indicators based on material property factors
 * -- Highlights selected material with yellow selection background
 * -- Shows material names truncated to fit within cell boundaries
 * -- Quality indicator reflects average of value and durability factors
 * -- Limited to MATERIALS_GRID_MAX materials for interface constraints
 */
void ie_DrawMaterialGrid(Material_t* materials, int material_count, int selected_index, int panel_x, int panel_y);

/*
 * Calculate screen position for material selection grid cells
 *
 * @param index: Linear index of the material within the selection grid
 * @param x: Output pointer for calculated X screen position
 * @param y: Output pointer for calculated Y screen position
 * @param w: Output pointer for calculated cell width in pixels
 * @param h: Output pointer for calculated cell height in pixels
 * @return: void
 * -- Converts linear material index to 2D screen coordinates
 * -- Uses MATERIALS_PANEL_X/Y and cell dimensions for positioning
 * -- Essential for consistent material grid rendering and mouse interaction
 * -- Output pointers must be valid; no validation performed
 * -- Supports grid layouts up to MATERIALS_GRID_WIDTH × MATERIALS_GRID_HEIGHT
 */
void ie_GetMaterialCellSize(int index, int* x, int* y, int* w, int* h, int panel_x, int panel_y);

/*
 * Generic grid mouse detection utility for various UI panels
 *
 * @param width: Grid width in cells for boundary calculations
 * @param height: Grid height in cells for boundary calculations
 * @param origin_x: Screen X coordinate of grid origin point
 * @param origin_y: Screen Y coordinate of grid origin point
 * @param cell_width: Width of each cell in pixels
 * @param cell_height: Height of each cell in pixels
 * @param grid_x: Output pointer for calculated grid X coordinate
 * @param grid_y: Output pointer for calculated grid Y coordinate
 * @param centered: Boolean flag - 1 for center-aligned grid, 0 for origin-aligned
 * @return: void
 * -- Reusable mouse-to-grid coordinate conversion for any grid-based UI
 * -- Supports both centered and origin-aligned grid positioning modes
 * -- Only updates output coordinates when mouse is within grid boundaries
 * -- Used by material selection, glyph palette, and other grid interfaces
 * -- Centered mode calculates grid position relative to center point
 */
void ie_GetCellAtMouse(int width, int height, int origin_x, int origin_y,
                       int cell_width, int cell_height, uint8_t* grid_x, 
                       uint8_t* grid_y, int centered);

/*
 * Filter and organize items based on type for categorized display
 *
 * @param items: Array of item pointers to filter from
 * @param item_count: Number of items in the source array
 * @param filter_type: ItemType_t specifying which type to include in results
 * @param filtered_items: Output array to store filtered item pointers
 * @return: `int` - Number of items that matched the filter criteria
 * -- Creates filtered view of items matching specified type criteria
 * -- Preserves original item pointers without copying item data
 * -- Supports filtering by weapon, armor, consumable, key, or ammunition types
 * -- Filtered array must be pre-allocated with sufficient capacity
 * -- Return value indicates how many items were added to filtered array
 * -- Essential for type-specific item editing and organization interfaces
 */
int ie_FilterItemsByType(Item_t* items, int item_count, ItemType_t filter_type, Item_t** filtered_items);

/*
 * Initialize item editing mode with UI setup and data loading
 *
 * @return: void
 * -- Sets up delegate functions for item editor logic and rendering loops
 * -- Initializes item editing UI widgets from "resources/widgets/editor/items_edit.json"
 * -- Loads items database and materials database for editing session
 * -- Configures UI panels for item grid, properties, and material selection
 * -- Resets all editing state variables to default values
 * -- Switches application to item editing mode with proper widget visibility
 * -- Safe to call multiple times (reinitializes editing session)
 */
void ie_edit( void );

/*
 * Load an item template from file for modification and customization
 *
 * @return: void
 * -- Opens file browser for template selection
 * -- Loads item template data from JSON or binary format
 * -- Populates creation parameters with template values
 * -- Allows user to modify template before final generation
 * -- Handles file format validation and error reporting
 * -- Provides base for creating item variations and series
 */
void iec_LoadItemTemplate( void );

/*
 * Generate a completely random item with random parameters for quick testing
 *
 * @return: void
 * -- Randomly selects item type, material, rarity, and quality parameters
 * -- Uses weighted randomization for realistic distribution of rare items
 * -- Creates item using the same generation system as manual selection
 * -- Useful for populating test databases and rapid prototyping
 * -- Provides feedback on randomly generated parameters
 * -- Switches to edit mode to immediately show randomly created item
 */
void iec_GenerateRandomItem( void );

/*
 * Generate a new item based on current parameter selections and add to database
 *
 * @return: void
 * -- Reads current selection values from creation parameter dropdown widgets
 * -- Maps UI indices to actual item type, material, rarity, and quality enums
 * -- Creates new Item_t structure with selected parameters and calculated properties
 * -- Applies material property modifiers to base item statistics
 * -- Adds generated item to active items database for editing
 * -- Provides user feedback on successful item creation
 * -- Handles memory allocation and error cases gracefully
 * -- Switches back to edit mode to immediately show created item
 */
void iec_GenerateItem( void );

/*
 * Initialize item creation mode with parameter selection UI and preview system
 *
 * @return: void
 * -- Sets up delegate functions for item creation logic and rendering loops
 * -- Initializes creation UI widgets from "resources/widgets/editor/items_creation.json"
 * -- Configures parameter selection dropdowns for type, material, rarity, quality
 * -- Sets up item generation callback for create button activation
 * -- Resets all creation parameter indices to default values
 * -- Switches application to item creation mode with proper widget visibility
 * -- Safe to call multiple times (reinitializes creation session)
 */
void ie_creation( void );

/*
 * Load items database from storage or create empty database for editing
 *
 * @param items_count: Output pointer for number of items loaded
 * @return: Pointer to allocated Item_t array or NULL on failure
 * -- Creates empty database if no saved data exists
 * -- Allocates memory for MAX_ITEMS capacity for future expansion
 * -- Initializes basic test items for development and testing
 * -- Sets items_count to actual number of items loaded/created
 * -- Returns NULL on memory allocation failure
 * -- Caller responsible for freeing returned memory with ie_FreeItemsDatabase
 */
Item_t* load_items_database(int* items_count);

/*
 * Free items database and all associated memory
 *
 * @param database: Pointer to Item_t array to free
 * @param count: Number of items in the database
 * @return: void
 * -- Frees all dString_t fields in each item (name, description, id, rarity)
 * -- Properly cleans up nested data structures
 * -- Frees the main database array memory
 * -- Safe to call with NULL database pointer
 * -- Essential for preventing memory leaks during editor shutdown
 */
void ie_FreeItemsDatabase(Item_t* database, int count);

/**
 * @brief Finds an item in a database by its unique string ID.
 * 
 * @param database A pointer to the array of items.
 * @param count The number of items in the database.
 * @param id The string ID to search for.
 * @return A pointer to the found item, or NULL if not found.
 */
Item_t* ie_FindItemByID(Item_t* database, int count, const char* id);

/**
 * @brief Validates the core data of a single item.
 *
 * @param item A pointer to the item to validate.
 * @return `true` if the item is valid, `false` otherwise.
 */
bool ie_ValidateItem(const Item_t* item);

/**
 * @brief Finds an item in a database by its unique string ID.
 * 
 * @param database A pointer to the array of items.
 * @param count The number of items in the database.
 * @param id The string ID to search for.
 * @return A pointer to the found item, or NULL if not found.
 */
Item_t* ie_FindItemByID(Item_t* database, int count, const char* id);

/**
 * @brief Creates an item using the specified parameters.
 * 
 * @param type The type of item to create.
 * @param mat_id The index of the material to use.
 * @param rare The index of the rarity level.
 * @param qual The index of the quality level.
 * @param ench The index of the enchantment level.
 * @return A pointer to the newly created Item_t, or NULL on failure.
 */
Item_t* ie_CreateItemWithParameters(ItemType_t type, int mat_id, int rare, int ench);

/**
 * @brief Counts items by type and draws the statistics to the screen.
 * 
 * @param database A pointer to the item database.
 * @param count The number of items in the database.
 */
void ie_DrawDbStats(const Item_t* database, int count);

/**
 * @brief Draws the simple "Items: X loaded" status text.
 * 
 * @param status_text The dString containing the status message.
 */
void ie_DrawDbStatus(const dString_t* status_text);

/**
 * @brief Provides safe, read-only access to the items database.
 * 
 * @return A pointer to the global item database.
 */
Item_t* ie_GetDatabase(void);

/**
 * @brief Provides safe, read-only access to the item count.
 * 
 * @return The current number of items in the database.
 */
int ie_GetItemCount(void);

/**
 * @brief Provides safe, read-only access to the status text string.
 * 
 * @return A pointer to the dString for the status text.
 */
dString_t* ie_GetStatusText(void);

/**
 * @brief Adds a newly created item to the main items database.
 * 
 * @param item A pointer to the item to add. The function takes ownership of this pointer.
 * @return void
 */
void ie_AddItemToDatabase(Item_t* item);

/**
 * @brief Creates a temporary item and draws its properties to show a preview.
 * 
 * @param t The item type.
 * @param m The material index.
 * @param r The weapon range.
 * @param q The weapon quality.
 * @param e The weapon enchantment.
 * @return void
 */
void ie_DrawItemCreationPreview(int t, int m, int r, int q, int e);

/**
 * @brief Creates a hardcoded list of materials for the editor.
 * 
 * @param count Output pointer for the number of materials created.
 * @return A pointer to the array of materials.
 */
Material_t* load_materials_database(int* count);

/**
 * @brief Applies a new material to an item and recalculates its stats.
 * 
 * @param item The item to modify.
 * @param material The new material to apply.
 * @return void
 */
void ie_ApplyMaterialToItem(Item_t* item, Material_t* material);

/**
 * @brief Adjusts a specific property of an item up or down.
 * 
 * @param item The item to modify.
 * @param property_index The index of the property to change (based on draw order).
 * @param adjustment The amount to change by (+1 or -1).
 * @return void
 */
void ie_AdjustItemProperty(Item_t* item, int prop_idx, int adj);

Material_t* ie_GetMaterialsDatabase(void);
int         ie_GetMaterialsCount(void);
#endif