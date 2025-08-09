/*
 * color_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>
#include <string.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "entity_editor.h"
#include "item_editor.h"
#include "ui_editor.h"

static void e_ColorEditorLogic( float dt );
static void e_ColorEditorDraw( float dt );

void e_InitColorEditor( void )
{
  app.delegate.logic = e_ColorEditorLogic;
  app.delegate.draw  = e_ColorEditorDraw;
  
  a_InitWidgets( "resources/widgets/editor/colors.json" );
  
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

static void e_ColorEditorLogic( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }

  a_DoWidget();
}

static void e_ColorEditorDraw( float dt )
{
  aRect_t test_rect_0 = (aRect_t){ .x = 100, .y = 100, .w = 32, .h = 32 };
  a_DrawFilledRect( test_rect_0, blue );
  aRect_t test_rect_1 = (aRect_t){ .x = 300, .y = 300, .w = 32, .h = 32 };
  a_DrawFilledRect( test_rect_1, green );

  a_DrawWidgets();
}

void e_DestroyColorEditor( void )
{
}

