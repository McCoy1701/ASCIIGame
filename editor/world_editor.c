#include <stdio.h>
#include <stdlib.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "defs.h"
#include "structs.h"
#include "editor.h"
#include "world_editor.h"
#include "init_editor.h"
#include "item_editor.h"
#include "entity_editor.h"
#include "color_editor.h"
#include "ui_editor.h"

static void e_WorldEditorDoLoop( float );
static void e_WorldEditorRenderLoop( float );

World_t* map = NULL;

WorldPosition_t current_pos;
char* pos_text;

void e_InitWorldEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;

  pos_text = malloc( sizeof(char) * 50 );

  current_pos = (WorldPosition_t){ .world_index = 0, .region_index = 0,
    .local_index = 0, .level = 0, .local_z = 0 };
  snprintf(pos_text, 50, "%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.region_index, current_pos.local_index, current_pos.level,
           current_pos.local_z );

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
      current->action = we_creation;
    }

    if ( strcmp( current->name, "edit" ) == 0 )
    {
      current->action = we_edit;
    }

    if ( strcmp( current->name, "save" ) == 0 )
    {
      current->action = we_save;
    }

    if ( strcmp( current->name, "load" ) == 0 )
    {
      current->action = we_load;
    }
  }

}

void we_save( void )
{

}

void we_load( void )
{

}

static void e_WorldEditorDoLoop( float dt )
{
  a_DoInput();
  if ( map!= NULL )
  {
    if ( ( current_pos.world_index % map->world_width ) == 0 )
    {
      //printf( "top\n" );
    }
    
    else if ( ( ( current_pos.world_index + 1 ) % map->world_width ) == 0 )
    {
      //printf( "bottom\n" );
    }
    
    else if ( current_pos.world_index < map->world_width )
    {
      //printf( "left\n" );
    }
    
    else if ( current_pos.world_index >= 
      ( ( map->world_width * map->world_width ) - map->world_width ) )
    {
      //printf( "right\n" );
    }

  }
  
  if ( app.mouse.button == 1 )
  {
    e_MapMouseCheck( &current_pos );
  }
  
  e_LevelZHeightCheck( &current_pos );

  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    if ( map != NULL )
    {
      e_DestroyWorldEditor();

    }

    e_InitEditor();
  }

  a_DoWidget();
}

static void e_WorldEditorRenderLoop( float dt )
{
  if ( map != NULL )
  {
    int draw_size = 0;
    switch ( current_pos.level )
    {
      case WORLD_LEVEL:
        draw_size = map->world_width * map->world_height;
        break;

      case REGION_LEVEL:
        draw_size = map->region_width * map->region_height;
        break;

      case LOCAL_LEVEL:
        draw_size = map->local_width * map->local_height;
        break;
    }

    for ( uint16_t i = 0; i < draw_size; i++ )
    {
      we_DrawWorldCell( i, map, current_pos );
    }


    snprintf( pos_text, 50, "%d,%d,%d,%d,%d\n", current_pos.world_index,
              current_pos.region_index, current_pos.local_index,
              current_pos.level, current_pos.local_z );
    a_DrawText( pos_text, 750, 10, 255, 255, 255, app.font_type,
                TEXT_ALIGN_CENTER, 0 );

  }

  a_DrawFilledRect( 100, 100, 32, 32, 255, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 255, 255 );

  a_DrawWidgets();
}

void e_DestroyWorldEditor( void )
{
  free_world( map, ( map->world_width * map->world_height ),
                   ( map->region_width * map->region_height ) );
  map = NULL;
  free( pos_text );
  pos_text = NULL;
}

