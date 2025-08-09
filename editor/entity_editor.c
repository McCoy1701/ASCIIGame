/*
 * entity_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>

#include "Archimedes.h"
#include "colors.h"
#include "editor.h"
#include "entity_editor.h"
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
  aWidget_t* w;
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
  
  w = a_GetWidget( "entity_menu_bar" );
  aContainerWidget_t* world_menu_container = ( aContainerWidget_t* )w->data;
  for ( int i = 0; i < world_menu_container->num_components; i++ )
  {
    aWidget_t* current = &world_menu_container->components[i];

    if ( strcmp( current->name, "creation" ) == 0 )
    {
      current->action = ee_Creation;
    }

    if ( strcmp( current->name, "edit" ) == 0 )
    {
      current->action = ee_Edit;
    }
  } 
}



static void e_EntityEditorLogic( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }
  
  a_DoWidget();
}

static void e_EntityEditorDraw( float dt )
{
  aRect_t entity_bg_rect = (aRect_t){ .x = 512, .y = 110,
                                      .w = 266, .h = 522 };
  a_DrawFilledRect( entity_bg_rect,
                    master_colors[APOLLO_PALETE][APOLLO_RED_2] );

  aRect_t character_sheet_rect = (aRect_t){ .x = 517, .y = 115,
                                            .w = 256, .h = 512 };
  a_DrawFilledRect( character_sheet_rect,
                    master_colors[APOLLO_PALETE][APOLLO_BLUE_2] );
  
  a_DrawWidgets();
}

void e_DestroyEntityEditor( void )
{
}

