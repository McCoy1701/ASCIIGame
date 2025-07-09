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

static WorldPosition_t current_pos;
static WorldPosition_t highlighted_pos;
char* pos_text;

void e_InitWorldEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_WorldEditorLogic;
  app.delegate.draw  = e_WorldEditorDraw;

  pos_text = malloc( sizeof(char) * 50 );

  current_pos = ( WorldPosition_t ){ .world_index = 0, .realm_index = 0,
    .region_index = 0, .local_index = 0, .level = 0, .local_z = 0 };

  snprintf(pos_text, 50, "%d,%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.realm_index, current_pos.region_index,
           current_pos.local_index, current_pos.level, current_pos.local_z );

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
    //map = LoadPartialWorld( "resources/world/map.dat" );
  }
}

static void e_WorldEditorLogic( float dt )
{
  a_DoInput();
  
  if ( map!= NULL )
  {
    //e_MapMouseCheck( &highlighted_pos );

  }
  
  if ( app.mouse.button == 1 )
  {
    if ( map != NULL )
    {
      e_MapMouseCheck( &current_pos );

    }
  }
  
  e_LevelZHeightCheck( &current_pos );
  highlighted_pos.level = current_pos.level;
  highlighted_pos.local_z = current_pos.local_z;

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
    int draw_size = 0;
    int x = 0, y = 0, w = 0, h = 0;
    uint32_t i = 0;
    uint16_t current_index = 0;
    uint16_t highlight_index = 0;
    int current_glyph = 0;
    int current_bg = 0;
    int current_fg = 0;

    switch ( current_pos.level )
    {
      case REALM_LEVEL:
        for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
        {
          int realm_x = 0, realm_y = 0;
          realm_x = i / WORLD_HEIGHT; 
          realm_y = i % WORLD_HEIGHT;

          for( int j = 0; j < ( map->realm_width * map->realm_height ); j++ )
          {
            if ( ( x + w ) > map->realm_width )
            {
              x = 0;
              y += GLYPH_HEIGHT;
            }

            e_GetCellSize( j, map->realm_width, map->realm_height,
                          &x, &y, &w, &h );
            
            current_index   = current_pos.realm_index;
            highlight_index = highlighted_pos.realm_index;
            current_glyph   = map[i].realms[j].tile.glyph;
            current_bg      = map[i].realms[j].tile.bg;
            current_fg      = map[i].realms[j].tile.fg;

            we_newDrawWorldCell( j, current_index, current_glyph, current_bg,
                    current_fg, highlight_index,
                    ( x + ( realm_x * map->realm_width * CELL_WIDTH ) -
                                ( map->realm_width * CELL_WIDTH ) ),
                    ( y + ( realm_y * map->realm_height * CELL_HEIGHT) -
                                ( map->realm_height * CELL_HEIGHT ) ), w, h );
          }

        }
        break;

      case REGION_LEVEL:
        draw_size = map->region_width * map->region_height;
        for ( uint8_t i = 0; i < draw_size; i++ )
        {
          e_GetCellSize( i, map->region_width, map->region_height,
                        &x, &y, &w, &h );

          current_index   = current_pos.region_index;
          highlight_index = highlighted_pos.region_index;
          current_glyph   = map[current_pos.world_index].
            realms[current_pos.realm_index].regions[i].tile.glyph;
          
          current_bg      = map[current_pos.world_index].
            realms[current_pos.realm_index].regions[i].tile.bg;
          
          current_fg      = map[current_pos.world_index].
            realms[current_pos.realm_index].regions[i].tile.fg;

          we_newDrawWorldCell( i, current_index, current_glyph, current_bg,
                               current_fg, highlight_index, x, y, w, h );
        }
        break;

      case LOCAL_LEVEL:
        draw_size = map->local_width * map->local_height;
        for ( uint16_t i = 0; i < draw_size; i++ )
        {
          e_GetCellSize( i, map->local_width, map->local_height,
                        &x, &y, &w, &h );

          int index = ( ( current_pos.local_z * ( map->local_width * map->local_height ) 
            + i ) );

          current_index   = current_pos.local_index;
          highlight_index = highlighted_pos.local_index;
          current_glyph   = map[current_pos.world_index].realms[current_pos.realm_index].
            regions[current_pos.region_index].tiles[i].glyph;
          current_bg      = map[current_pos.world_index].realms[current_pos.realm_index].
            regions[current_pos.region_index].tiles[i].bg;
          current_fg      = map[current_pos.world_index].realms[current_pos.realm_index].
            regions[current_pos.region_index].tiles[i].fg;

          we_newDrawWorldCell( index, current_index, current_glyph, current_bg,
                               current_fg, highlight_index, x, y, w, h );
        }
        break;
    }



    snprintf( pos_text, 50, "%d,%d,%d,%d,%d,%d\n", current_pos.world_index,
              current_pos.realm_index, current_pos.region_index,
              current_pos.local_index, current_pos.level,
              current_pos.local_z );

    a_DrawText( pos_text, 750, 10, 255, 255, 255, app.font_type,
                TEXT_ALIGN_CENTER, 0 );

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

