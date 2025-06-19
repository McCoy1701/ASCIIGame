#include <stdio.h>

#include "Archimedes.h"
#include "world_editor.h"
#include "items_editor.h"
#include "entity_editor.h"

static void aDoLoop( float );
static void aRenderLoop( float );

static aWidget_t* w;

void aInitGame( void )
{
  app.delegate.logic = aDoLoop;
  app.delegate.draw  = aRenderLoop;

  a_InitWidgets( "resources/widgets/editor/editor.json" );
  
  app.active_widget = a_GetWidget( "world" );
  app.active_widget->action = e_InitWorldEditor;

  w = a_GetWidget( "item" );
  w->action = e_InitItemsEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
}

static void aDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.running = 0;
  }

  a_DoWidget();
}

static void aRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 0, 0, 255 );
  
  a_DrawWidgets();
}

void aMainloop( void )
{
  a_PrepareScene();

  app.delegate.logic( a_GetDeltaTime() );
  app.delegate.draw( a_GetDeltaTime() );
  
  a_PresentScene();
}

int main( void )
{
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  aInitGame();
  
  while( app.running ) {
    aMainloop();
  }
  
  a_Quit();

  return 0;
}

