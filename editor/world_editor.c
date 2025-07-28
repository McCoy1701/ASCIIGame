/*
 * world_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "Archimedes.h"
#include "color_editor.h"
#include "Daedalus.h"
#include "defs.h"
#include "editor.h"
#include "entity_editor.h"
#include "init_editor.h"
#include "item_editor.h"
#include "save_editor.h"
#include "structs.h"
#include "ui_editor.h"
#include "world_editor.h"

static void e_WorldEditorLogic( float );
static void e_WorldEditorDraw( float );

World_t* map = NULL;

static WorldPosition_t selected_pos;
static WorldPosition_t highlighted_pos;
uint8_t selected_world_x = 0, selected_world_y = 0;
uint8_t selected_realm_x = 0, selected_realm_y = 0;
uint8_t highlighted_world_x = 0, highlighted_world_y = 0;
uint8_t highlighted_realm_x = 0, highlighted_realm_y = 0;
char* pos_text;

static int originx = 0;
static int originy = 0;

void e_InitWorldEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_WorldEditorLogic;
  app.delegate.draw  = e_WorldEditorDraw;

  pos_text = malloc( sizeof(char) * 50 );

  selected_pos = ( WorldPosition_t ){ .world_index = 0, .realm_index = 0,
    .region_index = 0, .local_index = 0, .level = 0, .local_z = 0 };

  snprintf(pos_text, 50, "%d,%d,%d,%d,%d,%d\n", selected_pos.world_index,
           selected_pos.realm_index, selected_pos.region_index,
           selected_pos.local_index, selected_pos.level, selected_pos.local_z );

  a_InitWidgets( "resources/widgets/editor/world.json" );

  app.active_widget = a_GetWidget( "tab_bar" );

  aContainerWidget_t* tab_container = 
    ( aContainerWidget_t* )app.active_widget->data;

  for ( int i = 0; i < tab_container->num_components; i++ )
  {
    aWidget_t* current = &tab_container->components[i];

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

  w = a_GetWidget( "world_menu_bar" );
  aContainerWidget_t* world_menu_container = ( aContainerWidget_t* )w->data;
  for ( int i = 0; i < world_menu_container->num_components; i++ )
  {
    aWidget_t* current = &world_menu_container->components[i];

    if ( strcmp( current->name, "creation" ) == 0 )
    {
      current->action = we_Creation;
    }

    if ( strcmp( current->name, "edit" ) == 0 )
    {
      current->action = we_Edit;
    }

    if ( strcmp( current->name, "save" ) == 0 )
    {
      current->action = we_Save;
    }

    if ( strcmp( current->name, "load" ) == 0 )
    {
      current->action = we_Load;
    }
  }

}

void we_Load( void )
{
  if ( map == NULL ) 
  {
    map = LoadWorld( "resources/world/world.dat" );

    for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
    {
      char filename[MAX_FILENAME_LENGTH];
      sprintf( filename, "resources/world/realm%d.dat", i );
      
      LoadRegion( map, filename, i );
    }
  }
}

static void e_WorldEditorLogic( float dt )
{
  a_DoInput();
  
  if ( map!= NULL )
  {
    e_MapMouseCheck( &highlighted_pos );

  }
  
  if ( app.mouse.button == 1 )
  {
    if ( map != NULL )
    {
      e_MapMouseCheck( &selected_pos );

    }
  }
  
  e_LevelZHeightCheck( &selected_pos );
  highlighted_pos.level = selected_pos.level;
  highlighted_pos.local_z = selected_pos.local_z;

  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    if ( map != NULL )
    {
      we_Save();

    }

    else
    {
      e_InitEditor();

    }

  }

  a_DoWidget();
}

static void e_WorldEditorDraw( float dt )
{
  if ( map != NULL )
  {
    originx = ( SCREEN_ORIGIN_X ) - ( ( map->realm_width 
      * CELL_WIDTH ) / 2 ) - ( map->realm_width * CELL_WIDTH );

    originy = ( SCREEN_ORIGIN_Y ) - ( ( map->realm_height 
      * CELL_HEIGHT ) / 2 ) - ( map->realm_height * CELL_HEIGHT );
    
    we_DrawWorld( map, selected_pos, highlighted_pos );

    snprintf( pos_text, 50, "%d,%d,%d,%d,%d,%d\n", selected_pos.world_index,
             selected_pos.realm_index, selected_pos.region_index,
             selected_pos.local_index, selected_pos.level,
             selected_pos.local_z );

    a_DrawText( pos_text, 750, 10, 255, 255, 255, app.font_type,
                TEXT_ALIGN_CENTER, 0 );

    int realm_x = 0, realm_y = 0;
    realm_x = selected_pos.world_index / WORLD_HEIGHT; 
    realm_y = selected_pos.world_index % WORLD_HEIGHT;

    if ( realm_x >= 0 && realm_x < WORLD_WIDTH &&
         realm_y >= 0 && realm_y < WORLD_HEIGHT )
    {
      a_DrawRect( originx + ( realm_x * map->realm_width * CELL_WIDTH ),
                 originy + ( realm_y * map->realm_height * CELL_HEIGHT ),
                 ( map->realm_width * CELL_WIDTH ), 
                 ( map->realm_height * CELL_HEIGHT ),
                 255, 255, 0, 255 );
    }

  }
  
  a_DrawRect( 451, 120, 378, 480, 255, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 255, 255 );

  a_DrawWidgets();
}

void e_DestroyWorldEditor( void )
{
  free_world( map, ( WORLD_WIDTH * WORLD_HEIGHT ),
              ( map->realm_width * map->realm_height ),
              ( map->region_width * map->region_height ) );
  map = NULL;
  free( pos_text );
  pos_text = NULL;
}

