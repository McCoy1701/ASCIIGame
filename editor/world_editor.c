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
static void we_CreationDoLoop( float );
static void we_CreationRenderLoop( float );

static void wec_GenerateWorld( void );

int world_size_index    = 0;
int region_size_index   = 0;
int local_size_index    = 0;
int z_height_size_index = 0;

int grid_x = 0;
int grid_y = 0;

World_t* map = NULL;

World_t* selected_world       = NULL;
RegionCell_t* selected_region = NULL;
GameTile_t* selected_gametile = NULL;

World_Position_t current_pos;
char* pos_text;

void e_InitWorldEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;

  pos_text = malloc( sizeof(char) * 50 );

  current_pos = (World_Position_t){ .world_index = 0, .region_index = 0,
    .local_index = 0, .level = 0, .local_x = 0, .local_y = 0, .local_z = 0 };
  snprintf(pos_text, 50, "%d,%d,%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.region_index, current_pos.local_index, current_pos.level,
           current_pos.local_x, current_pos.local_y, current_pos.local_z );

  a_InitWidgets( "resources/widgets/editor/world.json" );

  app.active_widget = a_GetWidget( "tab_bar" );

  aContainerWidget_t* tab_container = ( aContainerWidget_t* )app.active_widget->data;
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

void we_creation( void )
{
  app.delegate.logic = we_CreationDoLoop;
  app.delegate.draw  = we_CreationRenderLoop;


  app.active_widget = a_GetWidget( "generation_menu" );
  aContainerWidget_t* container = a_GetContainerFromWidget( "generation_menu" );
  app.active_widget->hidden = 0;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 0;

    if ( strcmp( current->name, "generate" ) == 0 )
    {
      current->action = wec_GenerateWorld;
    }
  }

}

static void we_CreationDoLoop( float dt )
{
  a_DoInput();

  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    
    e_InitWorldEditor();
  }

  a_DoWidget();

}

static void we_CreationRenderLoop( float dt )
{
  a_DrawWidgets();

}

static void wec_GenerateWorld( void )
{

  int new_world_size  = 0;
  int new_region_size = 0;
  int new_local_size  = 0;
  int new_z_height    = 0;

  aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strcmp( current->name, "world_size" ) == 0 )
    {
      aSelectWidget_t* world_size = ( aSelectWidget_t* )current->data;
      world_size_index = world_size->value;
    }

    if ( strcmp( current->name, "region_size" ) == 0 )
    {
      aSelectWidget_t* region_size = ( aSelectWidget_t* )current->data;
      region_size_index = region_size->value;
    }

    if ( strcmp( current->name, "local_size" ) == 0 )
    {
      aSelectWidget_t* local_size = ( aSelectWidget_t* )current->data;
      local_size_index = local_size->value;
    }

    if ( strcmp( current->name, "z_height" ) == 0 )
    {
      aSelectWidget_t* z_height = ( aSelectWidget_t* )current->data;
      z_height_size_index = z_height->value;
    }
  }

  switch ( world_size_index ) {

    case 0: //small
      new_world_size = WORLD_WIDTH_SMALL;
    break;

    case 1: //medium
      new_world_size = WORLD_WIDTH_MEDIUM;
    break;

    case 2: //large
      new_world_size = WORLD_WIDTH_LARGE;
    break;

    default:
    break;
  }

  switch ( region_size_index ) {

    case 0: //small
      new_region_size = REGION_SIZE_SMALL;
    break;

    case 1: //medium
      new_region_size = REGION_SIZE_MEDIUM;
    break;

    case 2: //large
      new_region_size = REGION_SIZE_LARGE;
    break;

    default:
    break;
  }

  switch ( local_size_index ) {

    case 0: //small
      new_local_size = LOCAL_SIZE_SMALL;
    break;

    case 1: //medium
      new_local_size = LOCAL_SIZE_MEDIUM;
    break;

    case 2: //large
      new_local_size = LOCAL_SIZE_LARGE;
    break;

    default:
    break;
  }

  switch ( z_height_size_index ) {

    case 0: //small
      new_z_height = Z_HEIGHT_SMALL;
    break;

    case 1: //medium
      new_z_height = Z_HEIGHT_MEDIUM;
    break;

    case 2: //large
      new_z_height = Z_HEIGHT_LARGE;
    break;

    default:
    break;
  }

  if ( map != NULL )
  {
    free_world( map, ( map->world_width * map->world_height ),
                     ( map->region_width * map->region_height ) );
  }

  map = init_world( new_world_size, new_world_size, new_region_size, new_region_size,
                    new_local_size, new_local_size, new_z_height );

}

void we_edit( void )
{

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

  
  if ( app.mouse.button == 1 )
  {
    int mousex, mousey = 0;
    //app.mouse.button = 0;
    if ( map != NULL )
    {
      int grid_x, grid_y;
      
      switch (current_pos.level) {
        case WORLD_LEVEL: 
          e_GetCellAtMouse( map->world_width, map->world_height, &grid_x, &grid_y );

          current_pos.world_index = INDEX_2(grid_y, grid_x, map->world_width);
          selected_world    = &map[current_pos.world_index];
          selected_region   = NULL;
          selected_gametile = NULL;
          break;

        case REGION_LEVEL:
          e_GetCellAtMouse( map->region_width, map->region_height, &grid_x, &grid_y );
          
          current_pos.region_index = INDEX_2( grid_y, grid_x, map->region_width );
          selected_world    = NULL;
          selected_region   = &map[current_pos.world_index].regions[current_pos.region_index];
          selected_gametile = NULL;
          break;

        case LOCAL_LEVEL:
          e_GetCellAtMouse( map->local_width, map->local_height, &grid_x, &grid_y );
          
          current_pos.local_index = INDEX_3( grid_y, grid_x, current_pos.local_z,
                                             map->local_width, map->local_height );
          selected_world    = NULL;
          selected_region   = NULL;
          selected_gametile = &map[current_pos.world_index].regions[current_pos.region_index].
            tiles[current_pos.local_index];
          break;

        default:
          break;
      }
    } 
  }
  
  if ( app.keyboard[SDL_SCANCODE_RETURN] == 1 )
  {
    app.keyboard[SDL_SCANCODE_RETURN] = 0;

    if ( current_pos.level >= 0 && current_pos.level < LOCAL_LEVEL )
    {
      current_pos.level++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_BACKSPACE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

    if ( current_pos.level > 0 && current_pos.level <= LOCAL_LEVEL )
    {
      current_pos.level--;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_EQUALS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_EQUALS] = 0;
    if ( current_pos.local_z >= 0 && current_pos.local_z < map->z_height - 1 )
    {
      current_pos.local_z++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_MINUS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_MINUS] = 0;
    if ( current_pos.local_z > 0 && current_pos.local_z <= map->z_height )
    {
      current_pos.local_z--;
    }

  }

  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_DestroyWorldEditor();

    e_InitEditor();
  }

  a_DoWidget();
}

static void e_WorldEditorRenderLoop( float dt )
{
  if ( map != NULL )
  {
    if ( current_pos.level == 0 )
    {
      for ( uint16_t i = 0; i < ( map->world_width * map->world_height ); i++ )
      {
        int x, y, w, h;
        e_GetCellSize( i, map->world_width, map->world_height, &x, &y, &w, &h );

        if ( i == current_pos.world_index )
        {
          a_DrawRect( x, y, w, h, 255, 255, 0, 255 );
        }

        else
        {
          a_DrawRect( x, y, w, h, 0, 128, 128, 255 );
        }
      }
    }

    else if ( current_pos.level == 1 )
    {
      for ( uint16_t i = 0; i < ( map->region_width * map->region_height ); i++ )
      {
        int x, y, w, h;
        e_GetCellSize( i, map->region_width, map->region_height, &x, &y, &w, &h );

        if ( i == current_pos.region_index )
        {
          a_DrawRect( x, y, w, h, 255, 255, 0, 255 );
        } 

        else
        {
          a_DrawRect( x, y, w, h, 0, 128, 128, 255 );
        }
      }
    }
    
    else if ( current_pos.level == 2 )
    {
      for ( int i = 0; i < ( map->local_width * map->local_height ); i++ )
      {
        int x, y, w, h;
        e_GetCellSize( i, map->local_width, map->local_height, &x, &y, &w, &h );
        
        uint32_t index = ( ( current_pos.local_z * ( map->local_width * map->local_height ) ) + i );

        if ( index == current_pos.local_index )
        {
          a_DrawRect( x, y, w, h, 255, 255, 0, 255 );
        } 

        else
        {
          a_DrawRect( x, y, w, h, 0, 128, 128, 255 );
        }
      }
    }
    

    snprintf(pos_text, 50, "%d,%d,%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.region_index, current_pos.local_index, current_pos.level,
           current_pos.local_x, current_pos.local_y, current_pos.local_z );
    a_DrawText( pos_text, 750, 10, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0 );

  }

  a_DrawFilledRect( 100, 100, 32, 32, 255, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 255, 255 );

  a_DrawWidgets();
}

void e_GetCellSize( int index, int width, int height, int* x, int* y, int* w, int* h )
{
  int row = ( index / height );
  int col = ( index % height );
  *x = ( ( SCREEN_WIDTH / 2 )  - ( ( width  * CELL_SIZE ) / 2 ) )
    + ( row * CELL_SIZE );
  *y = ( ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 ) )
    + ( col * CELL_SIZE );
  *w = CELL_SIZE;
  *h = CELL_SIZE;
}

void e_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y )
{
  int edge_x = ( SCREEN_WIDTH  / 2 ) - ( ( width  * CELL_SIZE ) / 2 );
  int edge_y = ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 );
  int mousex = ( ( app.mouse.x - edge_x ) / CELL_SIZE );
  int mousey = ( ( app.mouse.y - edge_y ) / CELL_SIZE );
  *grid_x = ( mousex < 0 ) ? 0 : ( ( mousex >= width  )
    ? width  - 1 : mousex );
  *grid_y = ( mousey < 0 ) ? 0 : ( ( mousey >= height )
    ? height - 1 : mousey );
}

void e_DestroyWorldEditor( void )
{
  free_world( map, ( map->world_width * map->world_height ),
                   ( map->region_width * map->region_height ) );
  map = NULL;
  free( pos_text );
  pos_text = NULL;
}
