/*
 * entity_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "item_editor.h"
#include "color_editor.h"
#include "ui_editor.h"

static void e_EntityEditorLogic( float );
static void e_EntityEditorDraw( float );

static uint8_t selected_glyph_x = 0, selected_glyph_y = 0;
static uint8_t selected_fg_x = 0, selected_fg_y = 0;
static uint8_t selected_bg_x = 0, selected_bg_y = 0;

static int glyph_index = 0;
static int fg_index = 0;
static int bg_index = 0;

void e_InitEntityEditor( void )
{
  app.delegate.logic = e_EntityEditorLogic;
  app.delegate.draw  = e_EntityEditorDraw;
  
  a_InitWidgets( "resources/widgets/editor/entity.json" );
  
  app.active_widget = a_GetWidget( "tab_bar" );

  aContainerWidget_t* container =
    ( aContainerWidget_t* )app.active_widget->data;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strcmp( current->name, "world" ) == 0 )
    {
      current->action = e_InitWorldEditor;
    }
    
    if ( strcmp( current->name, "item" ) == 0 )
    {
      current->action = e_InitItemEditor;
    }
    
    if ( strcmp( current->name, "entity" ) == 0 )
    {
      current->action = e_InitEntityEditor;
    }
    
    if ( strcmp( current->name, "colors" ) == 0 )
    {
      current->action = e_InitColorEditor;
    }
    
    if ( strcmp( current->name, "ui" ) == 0 )
    {
      current->action = e_InitUIEditor;
    }
  }
  
}

static void e_EntityEditorLogic( float dt )
{
  a_DoInput();
  
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

static void e_EntityEditorDraw( float dt )
{
  a_DrawFilledRect( 20, 105, 1052, 537, 117, 36, 56, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 255, 0, 255 );
  
  a_DrawFilledRect( 1083, 105, 153, 423, 0, 0, 255, 255 ); //Glyph grid background
  
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

void e_DestroyEntityEditor( void )
{
}

