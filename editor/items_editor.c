#include <stdio.h>

#include "Archimedes.h"
#include "world_editor.h"
#include "entity_editor.h"

static void e_ItemsEditorDoLoop( float );
static void e_ItemsEditorRenderLoop( float );

static aWidget_t* w;

void e_InitItemsEditor( void )
{
  app.delegate.logic = e_ItemsEditorDoLoop;
  app.delegate.draw  = e_ItemsEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/items.json" );
  
  app.active_widget = a_GetWidget( "item" );
  app.active_widget->action = e_InitItemsEditor;

  w = a_GetWidget( "world" );
  w->action = e_InitWorldEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
}

static void e_ItemsEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.running = 0;
  }

  a_DoWidget();
}

static void e_ItemsEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 0, 255 );

  a_DrawWidgets();
}

void e_DestroyItemsEditor( void )
{
}

