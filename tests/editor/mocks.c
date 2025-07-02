// tests/editor/mocks.c
// Mock implementations for editor dependencies to enable isolated testing

#include "Archimedes.h"
#include "Daedalus.h"
#include "defs.h"
#include "structs.h"
#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// =============================================================================
// MOCK GLOBAL VARIABLES
// =============================================================================

/*
 * Mock game glyphs array for testing without full asset loading
 */
GlyphArray_t* game_glyphs = NULL;

/*
 * Mock master colors array for testing without color palette loading
 */
aColor_t master_colors[MAX_COLOR_GROUPS][48] = {0};

/*
 * Mock application structure for testing
 */
aApp_t app = {0};

// =============================================================================
// MOCK EDITOR FUNCTIONS
// =============================================================================

/*
 * Mock main editor initialization - minimal setup for testing
 */
void e_InitEditor(void)
{
    d_LogInfo("MOCK: e_InitEditor called - minimal test setup");
    
    // Initialize basic app structure for testing
    memset(&app, 0, sizeof(aApp_t));
    app.running = 1;
    app.delegate.logic = NULL;
    app.delegate.draw = NULL;
    app.active_widget = NULL;
    
    // Clear keyboard state
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) {
        app.keyboard[i] = 0;
    }
    
    // Initialize mock game_glyphs if not already done
    if (game_glyphs == NULL) {
        game_glyphs = malloc(sizeof(GlyphArray_t));
        if (game_glyphs != NULL) {
            game_glyphs->count = 256; // Mock 256 glyphs
            game_glyphs->texture = NULL; // No actual texture in tests
            
            // Initialize mock glyph rectangles
            for (int i = 0; i < 256; i++) {
                game_glyphs->rects[i].x = (i % 16) * GLYPH_WIDTH;
                game_glyphs->rects[i].y = (i / 16) * GLYPH_HEIGHT;
                game_glyphs->rects[i].w = GLYPH_WIDTH;
                game_glyphs->rects[i].h = GLYPH_HEIGHT;
            }
            
            d_LogInfo("MOCK: Initialized mock game_glyphs array");
        }
    }
    
    // Initialize mock master colors
    for (int group = 0; group < MAX_COLOR_GROUPS; group++) {
        for (int color = 0; color < 48; color++) {
            master_colors[group][color].r = (group * 16) % 256;
            master_colors[group][color].g = (color * 5) % 256;
            master_colors[group][color].b = 128;
            master_colors[group][color].a = 255;
        }
    }
    
    d_LogInfo("MOCK: Editor initialization completed for testing");
}

/*
 * Mock editor destruction - cleanup test resources
 */
void e_DestroyEditor(void)
{
    d_LogInfo("MOCK: e_DestroyEditor called - cleaning up test resources");
    
    if (game_glyphs != NULL) {
        free(game_glyphs);
        game_glyphs = NULL;
        d_LogInfo("MOCK: Freed mock game_glyphs");
    }
    
    // Reset app structure
    memset(&app, 0, sizeof(aApp_t));
}

// =============================================================================
// MOCK ARCHIMEDES FUNCTIONS
// =============================================================================

/*
 * Mock input processing - minimal implementation for tests
 */
void a_DoInput(void)
{
    // Mock implementation - just reset mouse button state
    app.mouse.button = 0;
}

/*
 * Mock widget processing - no-op for tests
 */
void a_DoWidget(void)
{
    // Mock implementation - no actual widget processing
}

/*
 * Mock widget retrieval - returns NULL for tests
 */
aWidget_t* a_GetWidget(char* name)
{
    d_LogInfoF("MOCK: a_GetWidget called for '%s' - returning NULL", name);
    return NULL;
}

/*
 * Mock container widget retrieval - returns NULL for tests
 */
aContainerWidget_t* a_GetContainerFromWidget(char* name)
{
    d_LogInfoF("MOCK: a_GetContainerFromWidget called for '%s' - returning NULL", name);
    return NULL;
}

/*
 * Mock widget initialization - no-op for tests
 */
void a_InitWidgets(const char* filename)
{
    d_LogInfoF("MOCK: a_InitWidgets called for '%s' - no actual loading", filename);
}

/*
 * Mock widget drawing - no-op for tests
 */
void a_DrawWidgets(void)
{
    // Mock implementation - no actual drawing
}

/*
 * Mock scene preparation - no-op for tests
 */
void a_PrepareScene(void)
{
    // Mock implementation - no actual scene preparation
}

/*
 * Mock scene presentation - no-op for tests
 */
void a_PresentScene(void)
{
    // Mock implementation - no actual scene presentation
}

/*
 * Mock delta time calculation - returns fixed value for tests
 */
float a_GetDeltaTime(void)
{
    return 0.016666f; // Mock 60 FPS (1/60 seconds)
}

/*
 * Mock text drawing - no-op for tests
 */
void a_DrawText(char* text, int x, int y, int r, int g, int b, int font_type, int align, int max_width)
{
    // Mock implementation - no actual text drawing
    (void)text; (void)x; (void)y; (void)r; (void)g; (void)b; (void)font_type; (void)align; (void)max_width;
}

/*
 * Mock rectangle drawing - no-op for tests
 */
void a_DrawFilledRect(const int x, const int y, const int w, const int h, const int r, const int g, const int b, const int a)
{
    // Mock implementation - no actual drawing
    (void)x; (void)y; (void)w; (void)h; (void)r; (void)g; (void)b; (void)a;
}

/*
 * Mock rectangle outline drawing - no-op for tests
 */
void a_DrawRect(const int x, const int y, const int w, const int h, const int r, const int g, const int b, const int a)
{
    // Mock implementation - no actual drawing
    (void)x; (void)y; (void)w; (void)h; (void)r; (void)g; (void)b; (void)a;
}

/*
 * Mock texture rectangle blitting - no-op for tests
 */
void a_BlitTextureRect(SDL_Texture* texture, SDL_Rect src, const int x, const int y, const int scale)
{
    // Mock implementation - no actual blitting
    (void)texture; (void)src; (void)x; (void)y; (void)scale;
}

// =============================================================================
// MOCK EDITOR SUBSYSTEM FUNCTIONS
// =============================================================================

/*
 * Mock world editor initialization
 */
void e_InitWorldEditor(void)
{
    d_LogInfo("MOCK: e_InitWorldEditor called");
}

/*
 * Mock entity editor initialization
 */
void e_InitEntityEditor(void)
{
    d_LogInfo("MOCK: e_InitEntityEditor called");
}

/*
 * Mock color editor initialization
 */
void e_InitColorEditor(void)
{
    d_LogInfo("MOCK: e_InitColorEditor called");
}

/*
 * Mock UI editor initialization
 */
void e_InitUIEditor(void)
{
    d_LogInfo("MOCK: e_InitUIEditor called");
}

// =============================================================================
// MOCK INITIALIZATION FUNCTIONS
// =============================================================================

/*
 * Mock glyph initialization - creates minimal test data
 */
GlyphArray_t* e_InitGlyphs(const char* filename, int glyph_width, int glyph_height)
{
    d_LogInfoF("MOCK: e_InitGlyphs called for '%s' (%dx%d)", filename, glyph_width, glyph_height);
    
    if (game_glyphs == NULL) {
        game_glyphs = malloc(sizeof(GlyphArray_t));
        if (game_glyphs != NULL) {
            game_glyphs->count = 256;
            game_glyphs->texture = NULL;
            
            // Initialize mock rectangles
            for (int i = 0; i < 256; i++) {
                game_glyphs->rects[i].x = (i % 16) * glyph_width;
                game_glyphs->rects[i].y = (i / 16) * glyph_height;
                game_glyphs->rects[i].w = glyph_width;
                game_glyphs->rects[i].h = glyph_height;
            }
        }
    }
    
    return game_glyphs;
}

/*
 * Mock color palette loading - creates test colors
 */
void e_LoadColorPalette(aColor_t colors[MAX_COLOR_GROUPS][48], const char* filename)
{
    d_LogInfoF("MOCK: e_LoadColorPalette called for '%s'", filename);
    
    // Initialize with test color pattern
    for (int group = 0; group < MAX_COLOR_GROUPS; group++) {
        for (int color = 0; color < 48; color++) {
            colors[group][color].r = (group * 32) % 256;
            colors[group][color].g = (color * 8) % 256;
            colors[group][color].b = ((group + color) * 16) % 256;
            colors[group][color].a = 255;
        }
    }
}

// =============================================================================
// MOCK CLEANUP FUNCTION
// =============================================================================

/*
 * Clean up all mock resources for test teardown
 */
void mock_cleanup(void)
{
    d_LogInfo("MOCK: Cleaning up all mock resources");
    
    e_DestroyEditor();
    
    // Additional cleanup if needed
    memset(&app, 0, sizeof(aApp_t));
    memset(master_colors, 0, sizeof(master_colors));
}

