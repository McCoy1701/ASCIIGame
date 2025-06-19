#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "structs.h"
#include "init_editor.h"
#include "items_editor.h"
#include "entity_editor.h"

static void e_WorldEditorDoLoop( float );
static void e_WorldEditorRenderLoop( float );

static aWidget_t* w;

World_t* map;

void e_InitWorldEditor( void )
{
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/editor.json" );
  
  app.active_widget = a_GetWidget( "world" );
  app.active_widget->action = e_InitWorldEditor;

  w = a_GetWidget( "item" );
  w->action = e_InitItemsEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
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

