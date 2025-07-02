// editor/items_editor/edit.c
// "The forge of Daedalus, where items are given new form."

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

// --- Forward Declarations for Static Functions ---
static void ie_EditLogic(float dt);
static void ie_EditDraw(float dt);
static void iee_InitWidgets(void);
static void iee_ResetState(void);

// Input Handling Helpers
static void iee_HandleMouseInput(void);
static void iee_HandleKeyboardInput(void);
static void iee_HandleItemGridSelection(void);
static void iee_HandlePropertySelection(void);
static void iee_HandleMaterialApplication(void);

// Drawing Helpers
static void iee_DrawItemGrid(void);
static void iee_DrawPropertiesPanel(void);
static void iee_DrawMaterialGrid(void);
static void iee_DrawItemPreview(void);
static void iee_DrawUIHints(void);

// --- Module-level State ---
static int        selected_item_index = -1;
static int        selected_property_index = 0;
static int        selected_material_index = 0;
static ItemType_t filter_type = ITEM_TYPE_WEAPON;
static bool       editing_properties = false;
static int        property_adjustment = 0;

static aWidget_t* item_grid_panel_widget = NULL;
static aWidget_t* properties_panel_widget = NULL;
static aWidget_t* material_grid_panel_widget = NULL;


// =============================================================================
// INITIALIZATION
// =============================================================================

void ie_edit(void)
{
    app.delegate.logic = ie_EditLogic;
    app.delegate.draw  = ie_EditDraw;

    iee_ResetState();
    iee_InitWidgets();
    
    d_LogInfo("Item editing mode initialized successfully.");
}

static void iee_ResetState(void)
{
    selected_item_index = -1;
    selected_property_index = 0;
    selected_material_index = 0;
    editing_properties = false;
    property_adjustment = 0;
    filter_type = ITEM_TYPE_WEAPON;
}

static void iee_InitWidgets(void)
{
    // This function now only looks for one container, making it much safer.
    app.active_widget = a_GetWidget("item_editor_workspace");
    if (app.active_widget) {
        app.active_widget->hidden = 0;

        aContainerWidget_t* workspace_container = (aContainerWidget_t*)app.active_widget->data;
        if (workspace_container) {
            for (int i = 0; i < workspace_container->num_components; i++) {
                aWidget_t* current = &workspace_container->components[i];
                if (strcmp(current->name, "item_grid_panel") == 0) {
                    item_grid_panel_widget = current;
                } else if (strcmp(current->name, "properties_panel") == 0) {
                    properties_panel_widget = current;
                } else if (strcmp(current->name, "material_grid_panel") == 0) {
                    material_grid_panel_widget = current;
                }
            }
        }
    }
}


// =============================================================================
// MAIN LOOPS (Now clean and readable "director" functions)
// =============================================================================

static void ie_EditLogic(float dt)
{
    (void)dt;
    a_DoInput();

    iee_HandleMouseInput();
    iee_HandleKeyboardInput();
    
    if (app.keyboard[SDL_SCANCODE_ESCAPE] == 1) {
        app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
        
        // When we exit, hide the edit menu.
        if (app.active_widget && strcmp(app.active_widget->name, "items_edit_menu") == 0) {
            app.active_widget->hidden = 1;
        }
        
        e_InitEditor();
        return;
    }

    a_DoWidget();
}

static void ie_EditDraw(float dt)
{
    (void)dt;
    
    iee_DrawItemGrid();
    iee_DrawPropertiesPanel();
    iee_DrawMaterialGrid();
    iee_DrawItemPreview();
    iee_DrawUIHints();

    a_DrawWidgets();
}


// =============================================================================
// LOGIC HELPERS
// =============================================================================

static void iee_HandleMouseInput(void)
{
    if (app.mouse.button == 1) { // Left Click
        app.mouse.button = 0;
        iee_HandleItemGridSelection();
        iee_HandlePropertySelection();
        
        // FIX: Provide valid pointers for the grid coordinates.
        // These temporary variables satisfy the function and prevent the crash.
        uint8_t temp_grid_x, temp_grid_y;
        ie_MaterialSelectionMouseCheck(&selected_material_index, &temp_grid_x, &temp_grid_y);
    }
    
    if (app.mouse.button == 3) { // Right Click
        app.mouse.button = 0;
        iee_HandleMaterialApplication();
    }
}

static void iee_HandleKeyboardInput(void)
{
    ie_ItemTypeKeyboardCheck(&filter_type, &property_adjustment);
    
    if (property_adjustment != 0 && selected_item_index >= 0 && editing_properties) {
        Item_t* selected_item = &ie_GetDatabase()[selected_item_index];
        ie_AdjustItemProperty(selected_item, selected_property_index, property_adjustment);
        property_adjustment = 0;
    }

    if (app.keyboard[SDL_SCANCODE_F] == 1) {
        app.keyboard[SDL_SCANCODE_F] = 0;
        filter_type = (ItemType_t)((filter_type + 1) % 5);
    }
}

static void iee_HandleItemGridSelection(void)
{
    if (!item_grid_panel_widget) return;

    uint8_t grid_x = 0, grid_y = 0;
    ie_GetItemCellAtMouse(ITEMS_GRID_WIDTH, ITEMS_GRID_HEIGHT, &grid_x, &grid_y, item_grid_panel_widget->x, item_grid_panel_widget->y);
    int clicked_index = (grid_y * ITEMS_GRID_WIDTH) + grid_x;

    Item_t* filtered_items[MAX_ITEMS];
    int filtered_count = ie_FilterItemsByType(ie_GetDatabase(), ie_GetItemCount(), filter_type, filtered_items);

    if (clicked_index >= 0 && clicked_index < filtered_count) {
        // Find the actual index in the main database
        for (int i = 0; i < ie_GetItemCount(); i++) {
            if (&ie_GetDatabase()[i] == filtered_items[clicked_index]) {
                selected_item_index = i;
                editing_properties = false;
                d_LogInfoF("Selected item: %s", d_PeekString(ie_GetDatabase()[i].name));
                break;
            }
        }
    }
}

static void iee_HandlePropertySelection(void)
{
    if (!properties_panel_widget) return;

    if (selected_item_index < 0) return;
    
    Item_t* selected_item = &ie_GetDatabase()[selected_item_index];
    ie_ItemPropertiesMouseCheck(selected_item, &selected_property_index, properties_panel_widget->x, properties_panel_widget->y);
}

static void iee_HandleMaterialApplication(void)
{
    if (!material_grid_panel_widget) return;

    if (selected_item_index < 0 || selected_material_index < 0 || selected_material_index >= ie_GetMaterialsCount()) return;

    Item_t* item = &ie_GetDatabase()[selected_item_index];
    Material_t* material = &ie_GetMaterialsDatabase()[selected_material_index];
    ie_ApplyMaterialToItem(item, material);

    d_LogInfoF("Applied material '%s' to item '%s'", d_PeekString(material->name), d_PeekString(item->name));
}


// =============================================================================
// DRAWING HELPERS
// =============================================================================

static void iee_DrawItemGrid(void)
{
    if (!item_grid_panel_widget) return;

    Item_t* db = ie_GetDatabase();
    int count = ie_GetItemCount();
    if (!db || count == 0) return;

    Item_t* filtered_items[MAX_ITEMS];
    int filtered_count = ie_FilterItemsByType(db, count, filter_type, filtered_items);

    for (int i = 0; i < filtered_count && i < ITEMS_GRID_MAX; i++) {
        bool is_selected = (selected_item_index != -1 && filtered_items[i] == &db[selected_item_index]);
        ie_DrawItemCell(i, filtered_items[i], item_grid_panel_widget->x, item_grid_panel_widget->y, is_selected);
    }
}

static void iee_DrawPropertiesPanel(void)
{
    if (!properties_panel_widget) return;

    if (selected_item_index < 0 || selected_item_index >= ie_GetItemCount()) return;

    Item_t* item = &ie_GetDatabase()[selected_item_index];
    ie_DrawItemProperties(item, properties_panel_widget->x, properties_panel_widget->y, selected_property_index);

    if (editing_properties) {
        a_DrawRect(properties_panel_widget->x - 2, properties_panel_widget->y - 2,
                   properties_panel_widget->w + 4, properties_panel_widget->h + 4,
                   255, 255, 0, 255);
    }
}

static void iee_DrawMaterialGrid(void)
{
    if (!material_grid_panel_widget) return;

    if (ie_GetMaterialsDatabase() && ie_GetMaterialsCount() > 0) {
        ie_DrawMaterialGrid(ie_GetMaterialsDatabase(), ie_GetMaterialsCount(), selected_material_index, material_grid_panel_widget->x, material_grid_panel_widget->y);
    }
}

static void iee_DrawItemPreview(void)
{
    if (!properties_panel_widget) return;

    if (selected_item_index < 0 || selected_item_index >= ie_GetItemCount()) return;
    
    Item_t* item = &ie_GetDatabase()[selected_item_index];
    
    int preview_x = properties_panel_widget->x + (properties_panel_widget->w / 2) - 32; // Center in panel
    int preview_y = properties_panel_widget->y + properties_panel_widget->h - 100; // Towards bottom of panel

    if (game_glyphs && item->glyph < game_glyphs->count) {
        SDL_Rect rect = game_glyphs->rects[item->glyph];
        a_DrawFilledRect(preview_x, preview_y, rect.w * 4, rect.h * 4, 50, 50, 50, 255);
        a_BlitTextureRect(game_glyphs->texture, rect, preview_x, preview_y, 4);
    }
    
    a_DrawText(d_PeekString(item->name), preview_x, preview_y + 80, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0);
}

static void iee_DrawUIHints(void)
{
    a_DrawText("Left Click: Select | Right Click: Apply Material | F: Filter | +/-: Adjust",
               10, SCREEN_HEIGHT - 100, 200, 200, 200, app.font_type, TEXT_ALIGN_LEFT, 0);
}

