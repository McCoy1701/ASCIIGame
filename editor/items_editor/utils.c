// editor/items_editor/utils.c - Items Editor Utility Functions
// Provides utility functions for items editor mode

#include "Archimedes.h"
#include "Daedalus.h"
#include "defs.h"
#include "editor.h"
#include "structs.h"
#include "items_editor.h"
#include "items.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Render a single item in the items grid with icon, name, and basic properties
 */
void ie_DrawItemCell(int index, Item_t* item, int grid_x, int grid_y, bool is_selected)
{
	int x, y, w, h;
	ie_GetItemCellSize(index, ITEMS_GRID_WIDTH, ITEMS_GRID_HEIGHT, &x, &y, &w, &h);
	
	// Background color based on selection and item type
	if (is_selected) {
		a_DrawFilledRect(x, y, w, h, 255, 255, 0, 255); // Yellow for selection
	} else {
		// Color by item type
		switch (item->type) {
			case ITEM_TYPE_WEAPON:
				a_DrawFilledRect(x, y, w, h, 150, 50, 50, 255); // Dark red
				break;
			case ITEM_TYPE_ARMOR:
				a_DrawFilledRect(x, y, w, h, 50, 50, 150, 255); // Dark blue
				break;
			case ITEM_TYPE_CONSUMABLE:
				a_DrawFilledRect(x, y, w, h, 50, 150, 50, 255); // Dark green
				break;
			default:
				a_DrawFilledRect(x, y, w, h, 100, 100, 100, 255); // Gray
				break;
		}
	}
	
	// Draw item glyph
	if (game_glyphs && item->glyph < game_glyphs->count) {
		a_BlitTextureRect(game_glyphs->texture, game_glyphs->rects[item->glyph],
		                  x + 4, y + 4, 2);
	}
	
	// Draw item name (truncated)
	if (item->name && item->name->str) {
		a_DrawText(item->name->str, x + 4, y + h - 16, 255, 255, 255, 
		           app.font_type, TEXT_ALIGN_LEFT, 0);
	}
}

/*
 * Calculate screen position and dimensions for an item cell in the grid
 */
void ie_GetItemCellSize(int index, int grid_width, int grid_height, 
                        int* x, int* y, int* w, int* h)
{
	int row = index / grid_width;
	int col = index % grid_width;
	*x = ITEMS_GRID_START_X + (col * ITEMS_CELL_WIDTH);
	*y = ITEMS_GRID_START_Y + (row * ITEMS_CELL_HEIGHT);
	*w = ITEMS_CELL_WIDTH;
	*h = ITEMS_CELL_HEIGHT;
}

/*
 * Determine which item cell contains the current mouse cursor position
 */
void ie_GetItemCellAtMouse(int grid_width, int grid_height, uint8_t* grid_x, uint8_t* grid_y)
{
	int start_x = ITEMS_GRID_START_X;
	int start_y = ITEMS_GRID_START_Y;
	int total_width = grid_width * ITEMS_CELL_WIDTH;
	int total_height = grid_height * ITEMS_CELL_HEIGHT;
	
	if (app.mouse.x >= start_x && app.mouse.x < start_x + total_width &&
	    app.mouse.y >= start_y && app.mouse.y < start_y + total_height) {
		int rel_x = app.mouse.x - start_x;
		int rel_y = app.mouse.y - start_y;
		*grid_x = rel_x / ITEMS_CELL_WIDTH;
		*grid_y = rel_y / ITEMS_CELL_HEIGHT;
	}
}

/*
 * Handle mouse interaction with the item properties panel
 */
void ie_ItemPropertiesMouseCheck(Item_t* selected_item, int* property_index)
{
	if (!selected_item) return;
	
	int panel_x = ITEM_PROPERTIES_PANEL_X;
	int panel_y = ITEM_PROPERTIES_PANEL_Y;
	int panel_width = ITEM_PROPERTIES_PANEL_WIDTH;
	int line_height = 20;
	
	if (app.mouse.x >= panel_x && app.mouse.x < panel_x + panel_width &&
	    app.mouse.y >= panel_y && app.mouse.y < panel_y + 200) {
		int rel_y = app.mouse.y - panel_y;
		*property_index = rel_y / line_height;
	}
}

/*
 * Handle mouse interaction with the material selection panel
 */
void ie_MaterialSelectionMouseCheck(int* material_index, uint8_t* grid_x, uint8_t* grid_y)
{
	ie_GetCellAtMouse(MATERIALS_GRID_WIDTH, MATERIALS_GRID_HEIGHT,
	                  MATERIALS_PANEL_X, MATERIALS_PANEL_Y,
	                  MATERIALS_CELL_WIDTH, MATERIALS_CELL_HEIGHT,
	                  grid_x, grid_y, 0);
	
	*material_index = (*grid_y * MATERIALS_GRID_WIDTH) + *grid_x;
}

/*
 * Handle keyboard input for item type cycling and property adjustment
 */
void ie_ItemTypeKeyboardCheck(ItemType_t* current_type, int* property_adjustment)
{
	// Cycle through item types with TAB
	if (app.keyboard[SDL_SCANCODE_TAB] == 1) {
		app.keyboard[SDL_SCANCODE_TAB] = 0;
		*current_type = (*current_type + 1) % 5; // Wrap around item types
	}
	
	// Adjust selected property with +/- keys
	if (app.keyboard[SDL_SCANCODE_EQUALS] == 1) {
		app.keyboard[SDL_SCANCODE_EQUALS] = 0;
		*property_adjustment = 1;
	}
	
	if (app.keyboard[SDL_SCANCODE_MINUS] == 1) {
		app.keyboard[SDL_SCANCODE_MINUS] = 0;
		*property_adjustment = -1;
	}
}

/*
 * Render item properties in a formatted panel display
 */
void ie_DrawItemProperties(Item_t* item, int panel_x, int panel_y, int selected_property)
{
	if (!item) return;
	
	char property_text[256];
	int line_y = panel_y;
	int line_height = 20;
	
	// Draw panel background
	a_DrawFilledRect(panel_x, panel_y, ITEM_PROPERTIES_PANEL_WIDTH, 200, 40, 40, 40, 255);
	
	// Item name
	if (item->name && item->name->str) {
		d_LogInfoF("Name: %s", item->name->str);
		a_DrawText(property_text, panel_x + 5, line_y, 255, 255, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
		line_y += line_height;
	}
	
	// Item type
	const char* type_names[] = {"WEAPON", "ARMOR", "KEY", "CONSUMABLE", "AMMUNITION"};
	d_LogInfoF("Type: %s",
	         (item->type < 5) ? type_names[item->type] : "UNKNOWN");
	a_DrawText(property_text, panel_x + 5, line_y, 200, 200, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
	line_y += line_height;
	
	// Weight and value
	d_LogInfoF("Weight: %.2f kg", item->weight_kg); 
	a_DrawText(property_text, panel_x + 5, line_y, 180, 180, 180, app.font_type, TEXT_ALIGN_LEFT, 0);
	line_y += line_height;
	
	d_LogInfoF("Value: %d coins", item->value_coins);
	a_DrawText(property_text, panel_x + 5, line_y, 255, 215, 0, app.font_type, TEXT_ALIGN_LEFT, 0);
	line_y += line_height;
	
	// Type-specific properties
	switch (item->type) {
		case ITEM_TYPE_WEAPON:
			d_LogInfoF("Damage: %d-%d",
			         item->data.weapon.min_damage, item->data.weapon.max_damage);
			a_DrawText(property_text, panel_x + 5, line_y, 255, 100, 100, app.font_type, TEXT_ALIGN_LEFT, 0);
			line_y += line_height;
			
			d_LogInfoF("Range: %d tiles", item->data.weapon.range_tiles);
			a_DrawText(property_text, panel_x + 5, line_y, 150, 150, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
			break;
			
		case ITEM_TYPE_ARMOR:
			d_LogInfoF("Armor: %d", item->data.armor.armor_value);
			a_DrawText(property_text, panel_x + 5, line_y, 100, 100, 255, app.font_type, TEXT_ALIGN_LEFT, 0);
			line_y += line_height;
			
			d_LogInfoF("Evasion: %d", item->data.armor.evasion_value);
			a_DrawText(property_text, panel_x + 5, line_y, 100, 255, 100, app.font_type, TEXT_ALIGN_LEFT, 0);
			break;
	}
}

/*
 * Render material selection grid with property previews
 */
void ie_DrawMaterialGrid(Material_t* materials, int material_count, int selected_index)
{
	for (int i = 0; i < material_count && i < MATERIALS_GRID_MAX; i++) {
		int x, y, w, h;
		ie_GetMaterialCellSize(i, &x, &y, &w, &h);
		
		// Background color
		if (i == selected_index) {
			a_DrawFilledRect(x, y, w, h, 255, 255, 0, 255); // Yellow selection
		} else {
			a_DrawFilledRect(x, y, w, h, 60, 60, 60, 255); // Dark gray
		}
		
		// Material name
		if (materials[i].name && materials[i].name->str) {
			a_DrawText(materials[i].name->str, x + 2, y + 2, 255, 255, 255, 
			           app.font_type, TEXT_ALIGN_LEFT, 0);
		}
		
		// Quality indicator based on property factors
		float avg_factor = (materials[i].properties.value_coins_fact + 
		                   materials[i].properties.durability_fact) / 2.0f;
		uint8_t quality_color = (uint8_t)(avg_factor * 255);
		a_DrawFilledRect(x + w - 10, y + 2, 8, 8, quality_color, quality_color, 0, 255);
	}
}

/*
 * Calculate screen position for material selection cells
 */
void ie_GetMaterialCellSize(int index, int* x, int* y, int* w, int* h)
{
	int row = index / MATERIALS_GRID_WIDTH;
	int col = index % MATERIALS_GRID_WIDTH;
	*x = MATERIALS_PANEL_X + (col * MATERIALS_CELL_WIDTH);
	*y = MATERIALS_PANEL_Y + (row * MATERIALS_CELL_HEIGHT);
	*w = MATERIALS_CELL_WIDTH;
	*h = MATERIALS_CELL_HEIGHT;
}

/*
 * Generic grid mouse detection for various UI panels
 */
void ie_GetCellAtMouse(int width, int height, int origin_x, int origin_y,
                       int cell_width, int cell_height, uint8_t* grid_x, 
                       uint8_t* grid_y, int centered)
{
	int start_x, start_y;
	
	if (centered) {
		start_x = origin_x - (width * cell_width) / 2;
		start_y = origin_y - (height * cell_height) / 2;
	} else {
		start_x = origin_x;
		start_y = origin_y;
	}
	
	int total_width = width * cell_width;
	int total_height = height * cell_height;
	
	if (app.mouse.x >= start_x && app.mouse.x < start_x + total_width &&
	    app.mouse.y >= start_y && app.mouse.y < start_y + total_height) {
		*grid_x = (app.mouse.x - start_x) / cell_width;
		*grid_y = (app.mouse.y - start_y) / cell_height;
	}
}

/*
 * Filter and sort items based on type and properties 
 */
int ie_FilterItemsByType(Item_t* items, int item_count, ItemType_t filter_type, Item_t** filtered_items)
{
    int filtered_count = 0;
    
    // Check for valid inputs to prevent crashes
    if (!items || !filtered_items) {
        return 0;
    }
    
    for (int i = 0; i < item_count; i++) {
        // Directly check the type of the item in the array
        if (items[i].type == filter_type) {
            // Store a pointer TO that item in the filtered list
            filtered_items[filtered_count++] = &items[i];
        }
    }
    
    return filtered_count;
}

/**
 * @brief Validates the core data of a single item.
 */
bool ie_ValidateItem(const Item_t* item)
{
    // A NULL item pointer is inherently invalid.
    if (item == NULL) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_ERROR,
		                  1, 1.0, "Validation failed: item pointer is NULL.");
        return false;
    }

    // Essential string fields must not be NULL.
    if (item->name == NULL || item->name->str == NULL || strlen(item->name->str) == 0) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
						  5, 1.0, "Validation failed: item name is NULL.");
        return false;
    }

    if (item->id == NULL || item->id->str == NULL || strlen(item->id->str) == 0) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
						  5, 1.0, "Validation failed: item id is NULL.");
        return false;
    }
    
	if (item->description == NULL || item->description->str == NULL || strlen(item->description->str) == 0) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
						  5, 1.0, "Validation failed: item description is NULL.");
        return false;
    }

	if (item->rarity == NULL || item->rarity->str == NULL || strlen(item->rarity->str) == 0) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_WARNING,
						  5, 1.0, "Validation failed: item rarity is NULL.");
        return false;
    }

    // Item type must be within the defined enum range.
    if (item->type < ITEM_TYPE_WEAPON || item->type > ITEM_TYPE_AMMUNITION) {
		d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_ERROR,
						  5, 1.0, "Validation failed: item '%s' has invalid type: %d", item->name->str, item->type);
        return false;
    }

    // Weight should not be negative.
    if (item->weight_kg < 0.0f) {
        d_LogErrorF("Validation failed: item '%s' has negative weight: %.2f", item->name->str, item->weight_kg);
        return false;
    }

    // If all checks pass, the item is considered valid.
    return true;
}

/**
 * @brief Finds an item in a database by its unique string ID.
 */
Item_t* ie_FindItemByID(Item_t* database, int count, const char* id)
{
    if (database == NULL || id == NULL) {
		d_LogWarning("Cannot search for item by ID with NULL parameters.");
        return NULL; // Cannot search with invalid inputs.
    }

    for (int i = 0; i < count; i++) {
        // Ensure the item's ID and string are not NULL before comparing.
        if (database[i].id != NULL && database[i].id->str != NULL) {
            if (strcmp(database[i].id->str, id) == 0) {
                return &database[i]; // Found it!
            }
        }
    }
	d_LogWarningF("Item with ID '%s' not found in database.", id);
    return NULL; // Item not found.
}