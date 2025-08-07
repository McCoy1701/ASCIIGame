/*
 * entity_editor/creation.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <string.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "colors.h"
#include "editor.h"
#include "entity_editor.h"

static void ee_CreationLogic( float );
static void ee_CreationDraw( float );

static uint8_t selected_glyph_x = 0, selected_glyph_y = 0;
static uint8_t selected_fg_x = 0, selected_fg_y = 0;
static uint8_t selected_bg_x = 0, selected_bg_y = 0;

static int glyph_index = 0;
static int fg_index = 0;
static int bg_index = 0;

dVec2_t color_pallete_pos = (dVec2_t){ .x = 1177, .y = 110 };
dVec2_t glyph_pallete_pos = (dVec2_t){ .x = 1088, .y = 263 };
dVec2_t glyph_bg_pos = (dVec2_t){ .x = 1083, .y = 105 };
dVec2_t add_list_bg_pos = (dVec2_t){ .x = 1083, .y = 534 };
dVec2_t randomize_bg_pos = (dVec2_t){ .x = 1083, .y = 582 };
dVec2_t glyph_selected_pos = (dVec2_t){ .x = 1150, .y = 110 };

void ee_Creation( void )
{
  app.delegate.logic = ee_CreationLogic;
  app.delegate.draw  = ee_CreationDraw;

}

static void ee_CreationLogic( float dt )
{
  a_DoInput();

  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    
    e_InitEntityEditor();
  }
  
  if ( app.mouse.button == 1 )
  {
    e_ColorMouseCheck( color_pallete_pos.x, color_pallete_pos.y, &fg_index,
                       &selected_fg_x, &selected_fg_y, 0 );

    e_GlyphMouseCheck( glyph_pallete_pos.x, glyph_pallete_pos.y, &glyph_index,
                       &selected_glyph_x, &selected_glyph_y, 0 );
  }
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }
  
  if ( app.mouse.button == 2 )
  {
    printf( "x: %d, y: %d\n", app.mouse.x, app.mouse.y );
  }
  
  if ( app.mouse.button == 3 )
  {
    app.mouse.button = 0;
    e_ColorMouseCheck( color_pallete_pos.x, color_pallete_pos.y, &bg_index,
                       &selected_bg_x, &selected_bg_y, 0 );
  }

  a_DoWidget();

}

static void ee_CreationDraw( float dt )
{
  aRect_t creation_menu_bg = (aRect_t){ .x = 20, .y = 105, .w = 1052, .h = 537 };
  a_DrawFilledRect( creation_menu_bg, master_colors[APOLLO_PALETE][APOLLO_RED_2] );

  ee_DrawAttributes();
  ee_DrawSkills();
  ee_DrawAdvantages();
  ee_DrawDisadvantages();
  ee_DrawVirtues();
  ee_DrawVices();
  
  a_DrawText( "Whores", 100, 100, black, white, FONT_CODE_PAGE_437, TEXT_ALIGN_CENTER, 0 );
  aRect_t glyph_bg_rect = (aRect_t){ .x = glyph_bg_pos.x, .y = glyph_bg_pos.y,
                                     .w = 153, .h = 418 };
  a_DrawFilledRect( glyph_bg_rect, blue ); //Glyph grid background
  
  aRect_t add_list_bg_rect = (aRect_t){ .x = add_list_bg_pos.x,
                                        .y = add_list_bg_pos.y,
                                        .w = 153, .h = 42 };
  a_DrawFilledRect( add_list_bg_rect,
                    master_colors[APOLLO_PALETE][APOLLO_GREEN_2] ); //Add to list background
  aRect_t randomize_bg_rect = (aRect_t){ .x = randomize_bg_pos.x,
                                         .y = randomize_bg_pos.y,
                                         .w = 153, .h = 43 };
  a_DrawFilledRect( randomize_bg_rect,
                    master_colors[APOLLO_PALETE][APOLLO_PURPLE_3] ); //Randomize background
  
  e_DrawColorPalette( color_pallete_pos.x, color_pallete_pos.y,
                      fg_index, bg_index );
  e_DrawGlyphPalette( glyph_pallete_pos.x, glyph_pallete_pos.y, glyph_index );
  
  aRect_t glyph_selected_rect = (aRect_t){ 
    .x = glyph_selected_pos.x,
    .y = glyph_selected_pos.y,
    .w = ( game_glyphs->rects[glyph_index].w * 2 ),
    .h = ( game_glyphs->rects[glyph_index].h * 2 ) };
  a_DrawFilledRect( glyph_selected_rect, 
                    master_colors[APOLLO_PALETE][bg_index] );
  
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[glyph_index],
                     glyph_selected_pos.x, glyph_selected_pos.y, 2,
                     master_colors[APOLLO_PALETE][fg_index] );

  a_DrawWidgets();
}

