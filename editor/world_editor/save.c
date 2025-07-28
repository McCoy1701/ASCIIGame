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
#include "save_editor.h"
#include "structs.h"
#include "world_editor.h"

static void wes_SaveLogic( float dt );
static void wes_SaveDraw( float dt );

void we_Save( void )
{
  app.delegate.logic = wes_SaveLogic;
  app.delegate.draw  = wes_SaveDraw;
  
  app.active_widget = a_GetWidget( "save_menu" );
  aContainerWidget_t* container =
    a_GetContainerFromWidget( "save_menu" );

  app.active_widget->hidden = 0;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 0;

    if ( strcmp( current->name, "save_yes" ) == 0 )
    {
      current->action = wes_SaveYes;
    }
    
    if ( strcmp( current->name, "save_no" ) == 0 )
    {
      current->action = wes_SaveNo;
    }
  }
  
}

static void wes_SaveLogic( float dt )
{
  a_DoInput();

  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitWorldEditor();
  }
  
  a_DoWidget();
}

static void wes_SaveDraw( float dt )
{
  a_DrawText( "Save?", 635, 270, 255, 255, 255, app.font_type,
             TEXT_ALIGN_CENTER, 0 );

  a_DrawWidgets();
}

void wes_SaveYes( void )
{
  if ( map != NULL )
  {
    SaveWorld( map, "resources/world/world.dat" );

    for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
    {
      char filename[MAX_FILENAME_LENGTH];
      sprintf( filename, "resources/world/realm%d.dat", i );
      
      SaveRegion( map, filename, i );
    }
  }

  e_InitEditor();

}

void wes_SaveNo( void )
{
  if ( map != NULL )
  {
    e_DestroyWorldEditor();

  }
  e_InitEditor();

}

