// tests/editor/test_items_editor_edit.c
// Unit tests for the Items Editor Edit Mode

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include "Archimedes.h"
#include "editor.h"
#include "items_editor.h"
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =============================================================================
// TEST SETUP & MOCKING
// =============================================================================

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// --- Mocking Flags & Stubs ---
static bool g_e_InitEditor_called = false;
static bool g_ie_GetItemCellAtMouse_called = false;
static bool g_ie_FilterItemsByType_called = false;
static bool g_ie_MaterialSelectionMouseCheck_called = false;
static bool g_ie_ItemPropertiesMouseCheck_called = false;
static bool g_ie_ApplyMaterialToItem_called = false;
static bool g_ie_ItemTypeKeyboardCheck_called = false;
static bool g_ie_AdjustItemProperty_called = false;
static bool g_a_DoInput_called = false;
static bool g_a_DoWidget_called = false;
static bool g_a_DrawWidgets_called = false;
static bool g_a_GetWidget_called = false;

// Mock data for items and materials
static Item_t mock_items_db[MAX_ITEMS];
static int mock_items_count = 0;
static Material_t mock_materials_db[MAX_MATERIALS];
static int mock_materials_count = 0;

// Mock app.active_widget and its components
static aContainerWidget_t mock_workspace_container;
static aWidget_t mock_item_grid_panel_widget;
static aWidget_t mock_properties_panel_widget;
static aWidget_t mock_material_grid_panel_widget;
static aWidget_t mock_item_editor_workspace_widget;

// Mock functions for external dependencies
void e_InitEditor(void) {
    g_e_InitEditor_called = true;
}

Item_t* ie_GetDatabase(void) {
    return mock_items_db;
}

int ie_GetItemCount(void) {
    return mock_items_count;
}

int ie_FilterItemsByType(Item_t* db, int count, ItemType_t filter_type, Item_t** filtered_items) {
    g_ie_FilterItemsByType_called = true;
    // Simple mock: return all items if no specific filtering logic is needed for tests
    for (int i = 0; i < count; i++) {
        filtered_items[i] = &db[i];
    }
    return count;
}

void ie_GetItemCellAtMouse(int grid_width, int grid_height, uint8_t* grid_x, uint8_t* grid_y, int panel_x, int panel_y) {
    g_ie_GetItemCellAtMouse_called = true;
    *grid_x = 0; // Default to first cell for testing
    *grid_y = 0;
}

void ie_MaterialSelectionMouseCheck(int* selected_material_index, uint8_t* grid_x, uint8_t* grid_y) {
    g_ie_MaterialSelectionMouseCheck_called = true;
}

void ie_ItemPropertiesMouseCheck(Item_t* item, int* selected_property_index, int panel_x, int panel_y) {
    g_ie_ItemPropertiesMouseCheck_called = true;
}

void ie_ApplyMaterialToItem(Item_t* item, Material_t* material) {
    g_ie_ApplyMaterialToItem_called = true;
}

void ie_ItemTypeKeyboardCheck(ItemType_t* filter_type, int* property_adjustment) {
    g_ie_ItemTypeKeyboardCheck_called = true;
}

void ie_AdjustItemProperty(Item_t* item, int selected_property_index, int property_adjustment) {
    g_ie_AdjustItemProperty_called = true;
}

Material_t* ie_GetMaterialsDatabase(void) {
    return NULL;
}

int ie_GetMaterialsCount(void) {
    return mock_materials_count;
}

void ie_DrawItemCell(int index, Item_t* item, int panel_x, int panel_y, bool is_selected) {}
void ie_DrawItemProperties(Item_t* item, int panel_x, int panel_y, int selected_property_index) {}
void ie_DrawMaterialGrid(Material_t* db, int count, int selected_index, int panel_x, int panel_y) {}

// Mock Archimedes functions
void a_DoInput(void) { g_a_DoInput_called = true; }
void a_DoWidget(void) { g_a_DoWidget_called = true; }
void a_DrawWidgets(void) { g_a_DrawWidgets_called = true; }
aWidget_t* a_GetWidget(char* name) {
    g_a_GetWidget_called = true;
    if (strcmp(name, "item_editor_workspace") == 0) {
        return &mock_item_editor_workspace_widget;
    }
    return NULL;
}

// C-file inclusion for testing static functions and variables.
#include "../../editor/items_editor/edit.c"

/**
 * @brief Resets all global mock flags and app state before each test.
 */
static void reset_mock_flags(void)
{
    g_e_InitEditor_called = false;
    g_ie_GetItemCellAtMouse_called = false;
    g_ie_FilterItemsByType_called = false;
    g_ie_MaterialSelectionMouseCheck_called = false;
    g_ie_ItemPropertiesMouseCheck_called = false;
    g_ie_ApplyMaterialToItem_called = false;
    g_ie_ItemTypeKeyboardCheck_called = false;
    g_ie_AdjustItemProperty_called = false;
    g_a_DoInput_called = false;
    g_a_DoWidget_called = false;
    g_a_DrawWidgets_called = false;
    g_a_GetWidget_called = false;

    selected_item_index = -1;
    selected_property_index = 0;
    selected_material_index = 0;
    filter_type = ITEM_TYPE_WEAPON;
    editing_properties = false;
    property_adjustment = 0;

    item_grid_panel_widget = NULL;
    properties_panel_widget = NULL;
    material_grid_panel_widget = NULL;

    memset(&app, 0, sizeof(aApp_t));
    memset(&mock_items_db, 0, sizeof(mock_items_db));
    mock_items_count = 0;
    memset(&mock_materials_db, 0, sizeof(mock_materials_db));
    mock_materials_count = 0;

    // Setup mock widgets
    memset(&mock_workspace_container, 0, sizeof(mock_workspace_container));
    memset(&mock_item_grid_panel_widget, 0, sizeof(mock_item_grid_panel_widget));
    memset(&mock_properties_panel_widget, 0, sizeof(mock_properties_panel_widget));
    memset(&mock_material_grid_panel_widget, 0, sizeof(mock_material_grid_panel_widget));
    memset(&mock_item_editor_workspace_widget, 0, sizeof(mock_item_editor_workspace_widget));

    // Link mock widgets
    strcpy(mock_item_grid_panel_widget.name, "item_grid_panel");
    strcpy(mock_properties_panel_widget.name, "properties_panel");
    strcpy(mock_material_grid_panel_widget.name, "material_grid_panel");

    mock_workspace_container.components = (aWidget_t*)malloc(sizeof(aWidget_t) * 3);
    mock_workspace_container.num_components = 3;
    mock_workspace_container.components[0] = mock_item_grid_panel_widget;
    mock_workspace_container.components[1] = mock_properties_panel_widget;
    mock_workspace_container.components[2] = mock_material_grid_panel_widget;

    mock_item_editor_workspace_widget.type = WT_CONTAINER;
    strcpy(mock_item_editor_workspace_widget.name, "item_editor_workspace");
    mock_item_editor_workspace_widget.data = &mock_workspace_container;
}

// =============================================================================
// TEST CASES
// =============================================================================

/**
 * @brief Tests if the edit mode initializes correctly.
 */
int test_edit_mode_initialization(void)
{
    d_LogInfo("Testing edit mode initialization...");
    reset_mock_flags();

    ie_edit();

    TEST_ASSERT(app.delegate.logic == ie_EditLogic, "Logic delegate should be set to edit loop");
    TEST_ASSERT(app.delegate.draw == ie_EditDraw, "Draw delegate should be set to edit render");
    TEST_ASSERT(g_a_GetWidget_called, "a_GetWidget should be called during initialization");
    TEST_ASSERT(app.active_widget == &mock_item_editor_workspace_widget, "app.active_widget should be set to item_editor_workspace");
    TEST_ASSERT(app.active_widget->hidden == 0, "item_editor_workspace should be visible");
    TEST_ASSERT(item_grid_panel_widget != NULL, "item_grid_panel_widget should be set");
    TEST_ASSERT(properties_panel_widget != NULL, "properties_panel_widget should be set");
    TEST_ASSERT(material_grid_panel_widget != NULL, "material_grid_panel_widget should be set");

    d_LogInfo("...initialization test completed.");
    return 1;
}

/**
 * @brief Tests the main logic loop.
 */
int test_edit_logic_loop(void)
{
    d_LogInfo("Testing edit logic loop...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    ie_EditLogic(0.016f);

    TEST_ASSERT(g_a_DoInput_called, "a_DoInput should be called");
    TEST_ASSERT(g_a_DoWidget_called, "a_DoWidget should be called");
    TEST_ASSERT(g_ie_MaterialSelectionMouseCheck_called, "ie_MaterialSelectionMouseCheck should be called");
    TEST_ASSERT(g_ie_ItemTypeKeyboardCheck_called, "ie_ItemTypeKeyboardCheck should be called");

    d_LogInfo("...logic loop test completed.");
    return 1;
}

/**
 * @brief Tests the main draw loop.
 */
int test_edit_draw_loop(void)
{
    d_LogInfo("Testing edit draw loop...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    ie_EditDraw(0.016f);

    TEST_ASSERT(g_a_DrawWidgets_called, "a_DrawWidgets should be called");

    d_LogInfo("...draw loop test completed.");
    return 1;
}

/**
 * @brief Tests ESC key behavior to exit edit mode.
 */
int test_escape_from_edit_mode(void)
{
    d_LogInfo("Testing escape key behavior...");
    reset_mock_flags();
    ie_edit(); // Enter edit mode

    app.keyboard[SDL_SCANCODE_ESCAPE] = 1; // Simulate ESC press
    ie_EditLogic(0.016f);

    TEST_ASSERT(g_e_InitEditor_called, "Escape key should call e_InitEditor to exit");
    TEST_ASSERT(app.keyboard[SDL_SCANCODE_ESCAPE] == 0, "Escape key state should be reset");

    d_LogInfo("...escape key test completed.");
    return 1;
}

/**
 * @brief Tests item grid selection with left click.
 */
int test_item_grid_selection(void)
{
    d_LogInfo("Testing item grid selection...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    // Setup mock data
    mock_items_count = 1;
        strcpy((char*)d_PeekString(mock_items_db[0].name), "Test Item");

    app.mouse.button = 1; // Simulate left click
    iee_HandleMouseInput();

    TEST_ASSERT(g_ie_GetItemCellAtMouse_called, "ie_GetItemCellAtMouse should be called");
    TEST_ASSERT(g_ie_FilterItemsByType_called, "ie_FilterItemsByType should be called");
    TEST_ASSERT(selected_item_index == 0, "Selected item index should be updated");
    TEST_ASSERT(editing_properties == false, "editing_properties should be false after item selection");

    d_LogInfo("...item grid selection test completed.");
    return 1;
}

/**
 * @brief Tests property selection with left click.
 */
int test_property_selection(void)
{
    d_LogInfo("Testing property selection...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    // Setup mock data
    selected_item_index = 0; // Simulate an item already selected

    app.mouse.button = 1; // Simulate left click
    iee_HandleMouseInput();

    TEST_ASSERT(g_ie_ItemPropertiesMouseCheck_called, "ie_ItemPropertiesMouseCheck should be called");

    d_LogInfo("...property selection test completed.");
    return 1;
}

/**
 * @brief Tests material application with right click.
 */
int test_material_application(void)
{
    d_LogInfo("Testing material application...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    // Setup mock data
    selected_item_index = 0;
    selected_material_index = 0;
    mock_materials_count = 1;

    app.mouse.button = 3; // Simulate right click
    iee_HandleMouseInput();

    TEST_ASSERT(g_ie_ApplyMaterialToItem_called, "ie_ApplyMaterialToItem should be called");

    d_LogInfo("...material application test completed.");
    return 1;
}

/**
 * @brief Tests filter type cycling with 'F' key.
 */
int test_filter_type_cycling(void)
{
    d_LogInfo("Testing filter type cycling...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    ItemType_t initial_filter_type = filter_type;
    app.keyboard[SDL_SCANCODE_F] = 1; // Simulate 'F' key press
    iee_HandleKeyboardInput();

    TEST_ASSERT(filter_type == (ItemType_t)((initial_filter_type + 1) % 5), "Filter type should cycle");
    TEST_ASSERT(app.keyboard[SDL_SCANCODE_F] == 0, "F key state should be reset");

    d_LogInfo("...filter type cycling test completed.");
    return 1;
}

/**
 * @brief Tests property adjustment.
 */
int test_property_adjustment(void)
{
    d_LogInfo("Testing property adjustment...");
    reset_mock_flags();
    ie_edit(); // Initialize state

    selected_item_index = 0;
    editing_properties = true;
    property_adjustment = 1; // Simulate adjustment

    iee_HandleKeyboardInput();

    TEST_ASSERT(g_ie_AdjustItemProperty_called, "ie_AdjustItemProperty should be called");
    TEST_ASSERT(property_adjustment == 0, "property_adjustment should be reset");

    d_LogInfo("...property adjustment test completed.");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger for test output
    dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG, .colorize_output = true };
    d_SetGlobalLogger(d_CreateLogger(config));

    d_LogInfo("Starting Items Editor Edit Mode Tests");
    
    TEST_SUITE_START("Items Editor Edit Mode Tests");

    RUN_TEST(test_edit_mode_initialization);
    RUN_TEST(test_edit_logic_loop);
    RUN_TEST(test_edit_draw_loop);
    RUN_TEST(test_escape_from_edit_mode);
    RUN_TEST(test_item_grid_selection);
    RUN_TEST(test_property_selection);
    RUN_TEST(test_material_application);
    RUN_TEST(test_filter_type_cycling);
    RUN_TEST(test_property_adjustment);

    // Clean up the logger before finishing
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}
