#include <stdio.h>

#include "Archimedes.h"
#include "world_editor.h"
#include "item_editor.h"
#include "entity_editor.h"
#include "color_editor.h"

static void eDoLoop( float );
static void eRenderLoop( float );

static aWidget_t* w;

void e_InitEditor( void )
{
  app.delegate.logic = eDoLoop;
  app.delegate.draw  = eRenderLoop;

  a_InitWidgets( "resources/widgets/editor/editor.json" );
  
  app.active_widget = a_GetWidget( "world" );
  app.active_widget->action = e_InitWorldEditor;

  w = a_GetWidget( "item" );
  w->action = e_InitItemEditor;

  w = a_GetWidget( "entity" );
  w->action = e_InitEntityEditor;
  
  w = a_GetWidget( "colors" );
  w->action = e_InitColorEditor;
  
}

static void eDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    app.running = 0;
  }

  a_DoWidget();
}

static void eRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 0, 0, 255 );
  
  a_DrawWidgets();
}

void e_Mainloop( void )
{
  a_PrepareScene();

  app.delegate.logic( a_GetDeltaTime() );
  app.delegate.draw( a_GetDeltaTime() );
  
  a_PresentScene();
}

int main( void )
{
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  e_InitEditor();
  
  while( app.running ) {
    e_Mainloop();
  }
  
  a_Quit();

  return 0;
}

