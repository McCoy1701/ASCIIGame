#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "entity_editor.h"
#include "color_editor.h"

static void e_ItemEditorDoLoop( float );
static void e_ItemEditorRenderLoop( float );

static aWidget_t* w;

void e_InitItemEditor( void )
{
  app.delegate.logic = e_ItemEditorDoLoop;
  app.delegate.draw  = e_ItemEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/items.json" );
  
  app.active_widget = a_GetWidget( "item" );
  app.active_widget->action = e_InitItemEditor;

  w = a_GetWidget( "world" );
  w->action = e_InitWorldEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
  
  w = a_GetWidget( "colors" );
  w->action = e_InitColorEditor;
  
}

static void e_ItemEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }

  a_DoWidget();
}

static void e_ItemEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 0, 255 );

  a_DrawWidgets();
}

void e_DestroyItemEditor( void )
{
}

