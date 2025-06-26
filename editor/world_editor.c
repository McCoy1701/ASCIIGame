#include <stdio.h>

#include "Archimedes.h"
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

World_t* map = NULL;

void e_InitWorldEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;
  
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
  printf( "%d %d %d %d\n", new_world_size, new_region_size, new_local_size, new_z_height );

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
      int edge_x = ( SCREEN_WIDTH  / 2 ) - ( ( map->world_width  * CELL_SIZE ) / 2 );
      int edge_y = ( SCREEN_HEIGHT / 2 ) - ( ( map->world_height * CELL_SIZE ) / 2 );
      mousex = ( ( app.mouse.x - edge_x ) / CELL_SIZE );
      mousey = ( ( app.mouse.y - edge_y ) / CELL_SIZE );
      int grid_x = ( mousex < 0 ) ? 0 : ( ( mousex >= map->world_width  )
        ? map->world_width  - 1 : mousex );
      int grid_y = ( mousey < 0 ) ? 0 : ( ( mousey >= map->world_height )
        ? map->world_height - 1 : mousey );
      
      printf( "%d, %d\n", grid_x, grid_y );
    }
 
   
  }

  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
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
    for ( int i = 0; i < ( map->world_width * map->world_height ); i++ )
    {
      int row = ( i / map->world_height );
      int col = ( i % map->world_height );
      int x = ( ( SCREEN_WIDTH / 2 )  - ( ( map->world_width  * CELL_SIZE ) / 2 ) )
        + ( row * CELL_SIZE );
      int y = ( ( SCREEN_HEIGHT / 2 ) - ( ( map->world_height * CELL_SIZE ) / 2 ) )
        + ( col * CELL_SIZE );
      int w = CELL_SIZE;
      int h = CELL_SIZE;

      a_DrawRect( x, y, w, h, 255, 255, 0, 255 );
    }

  }
  a_DrawFilledRect( 100, 100, 32, 32, 255, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 255, 255 );

  a_DrawWidgets();
}

void e_DestroyWorldEditor( void )
{
  free_world( map, ( map->world_width * map->world_height ),
                   ( map->region_width * map->region_height ) );
}

