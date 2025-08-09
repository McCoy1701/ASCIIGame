
/*
 * entity_editor/edit.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "colors.h"
#include "editor.h"
#include "entity_editor.h"

static void ee_EditLogic( float dt );
static void ee_EditDraw( float dt );

static uint8_t selected_glyph_x = 0, selected_glyph_y = 0;
static uint8_t selected_fg_x = 0, selected_fg_y = 0;
static uint8_t selected_bg_x = 0, selected_bg_y = 0;

static int glyph_index = 0;
static int fg_index = 0;
static int bg_index = 0;

void ee_Edit( void )
{
  app.delegate.logic = ee_EditLogic;
  app.delegate.draw  = ee_EditDraw;

}

static void ee_EditLogic( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    
    e_InitEntityEditor();
  }
  
  if ( app.mouse.button == 1 )
  {
    e_ColorMouseCheck( COLOR_PALLETE_X, COLOR_PALLETE_Y, &fg_index,
                       &selected_fg_x, &selected_fg_y, 0 );
    e_GlyphMouseCheck( GLYPH_PALLETE_X, GLYPH_PALLETE_Y, &glyph_index,
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
    e_ColorMouseCheck( COLOR_PALLETE_X, COLOR_PALLETE_Y, &bg_index,
                       &selected_bg_x, &selected_bg_y, 0 );
  }

  a_DoWidget();
}

static void ee_EditDraw( float dt )
{
  aRect_t background_rect = (aRect_t){ .x = 20, .y = 105,
                                       .w = 1052, .h = 537 };
  a_DrawFilledRect( background_rect,
                    master_colors[APOLLO_PALETE][APOLLO_RED_2] ); //Background
  
  aRect_t glyph_grid_bg_rect = (aRect_t){ .x = 1083, .y = 105,
                                          .w = 153, .h = 423 };
  a_DrawFilledRect( glyph_grid_bg_rect, blue ); //Glyph grid background
  
  e_DrawColorPalette( COLOR_PALLETE_X, COLOR_PALLETE_Y, fg_index, bg_index );
  e_DrawGlyphPalette( GLYPH_PALLETE_X, GLYPH_PALLETE_Y, glyph_index );
  
  aRect_t glyph_selected_rect = (aRect_t){ 
    .x = GLYPH_SELECTED_X,
    .y = GLYPH_SELECTED_Y,
    .w = ( game_glyphs->rects[glyph_index].w * 2 ),
    .h = ( game_glyphs->rects[glyph_index].h * 2 ) };
  a_DrawFilledRect( glyph_selected_rect, 
                    master_colors[APOLLO_PALETE][bg_index] );
  
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[glyph_index],
                     GLYPH_SELECTED_X, GLYPH_SELECTED_X, 2,
                     master_colors[APOLLO_PALETE][fg_index] );

  a_DrawWidgets();
}

