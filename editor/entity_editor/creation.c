/*
 * entity_editor/creation.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <string.h>

#include "Archimedes.h"
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
    e_ColorMouseCheck( 1177, 110, &fg_index, &selected_fg_x, &selected_fg_y,
                       0 );
    e_GlyphMouseCheck( 1088, 263, &glyph_index, &selected_glyph_x,
                       &selected_glyph_y, 0 );
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
    e_ColorMouseCheck( 1177, 110, &bg_index, &selected_bg_x, &selected_bg_y,
                       0 );
  }

  a_DoWidget();

}

static void ee_CreationDraw( float dt )
{
  a_DrawFilledRect( 20, 105, 1052, 537, 117, 36, 56, 255 ); //Background
  
  ee_DrawAttributes();
  ee_DrawSkills();
  ee_DrawAdvantages();
  ee_DrawDisadvantages();
  ee_DrawVirtues();
  ee_DrawVices();
  
  e_DrawGlyphText( "Whores", 100, 100, 1, 0, TEXT_ALIGN_CENTER, 0 );
  
  a_DrawFilledRect( 1083, 105, 153, 418, 0, 0, 255, 255 ); //Glyph grid background
  
  a_DrawFilledRect( 1083, 534, 153, 42, 70, 130, 50, 255 ); //Add to list background
  a_DrawFilledRect( 1083, 582, 153, 43, 162, 62, 140, 255 ); //Randomize background
  
  e_DrawColorPalette( 1177, 110, fg_index, bg_index );
  e_DrawGlyphPalette( 1088, 263, glyph_index );
  
  a_DrawFilledRect( 1150, 110, game_glyphs->rects[glyph_index].w * 2,
                    game_glyphs->rects[glyph_index].h * 2, 
                    master_colors[APOLLO_PALETE][bg_index].r,
                    master_colors[APOLLO_PALETE][bg_index].g,
                    master_colors[APOLLO_PALETE][bg_index].b,
                    master_colors[APOLLO_PALETE][bg_index].a );
  
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[glyph_index],
                     1150, 110, 2, master_colors[APOLLO_PALETE][fg_index] );

  a_DrawWidgets();
}

