#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "structs.h"
#include "world_editor.h"
#include "init_editor.h"
#include "items_editor.h"
#include "entity_editor.h"

static void e_WorldEditorDoLoop( float );
static void e_WorldEditorRenderLoop( float );
static void we_CreationDoLoop( float );
static void we_CreationRenderLoop( float );

static aWidget_t* w;

World_t* map;

void e_InitWorldEditor( void )
{
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/world.json" );
  
  app.active_widget = a_GetWidget( "world" );
  app.active_widget->action = e_InitWorldEditor;

  w = a_GetWidget( "item" );
  w->action = e_InitItemsEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
  
  w = a_GetWidget( "creation" );
  w->action = we_creation;
  w = a_GetWidget( "edit" );
  w->action = we_edit;
  w = a_GetWidget( "save" );
  w->action = we_save;
  w = a_GetWidget( "load" );
  w->action = we_load;
}

void we_creation( void )
{
  app.delegate.logic = we_CreationDoLoop;
  app.delegate.draw  = we_CreationRenderLoop;
  
  app.active_widget = a_GetWidget( "world_size" );
  app.active_widget->hidden = 0;
  w = a_GetWidget( "region_size" );
  w->hidden = 0;
  w = a_GetWidget( "local_size" );
  w->hidden = 0;
  w = a_GetWidget( "z_height" );
  w->hidden = 0;
  w = a_GetWidget( "generate" );
  w->hidden = 0;

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
  a_DrawFilledRect( 240, 240, 400, 265, 255, 0, 255, 255 );
  
  a_DrawWidgets();

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
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.running = 0;
  }

  a_DoWidget();
}

static void e_WorldEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 255, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 255, 255 );

  a_DrawWidgets();
}

void e_CleanUpWorldEditor( void )
{
  free_world( map, map->world_size, map->region_size );
}

