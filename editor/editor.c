#include <stdio.h>

#include "Archimedes.h"
#include "Daedalus.h"
#include "world_editor.h"
#include "item_editor.h"
#include "entity_editor.h"
#include "color_editor.h"
#include "ui_editor.h"

static void eDoLoop( float );
static void eRenderLoop( float );

static aWidget_t* w;

void e_InitEditor( void )
{
  app.delegate.logic = eDoLoop;
  app.delegate.draw  = eRenderLoop;

  a_InitWidgets( "resources/widgets/editor/editor.json" );
  
  app.active_widget = a_GetWidget( "tab_bar" );

  aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;
  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

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
  dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG };
  dLogger_t* logger = d_CreateLogger( config );
  d_SetGlobalLogger( logger );
  d_AddLogHandler( d_GetGlobalLogger(), d_ConsoleLogHandler, NULL );
  
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  e_InitEditor();
  
  while( app.running ) {
    e_Mainloop();
  }

  a_Quit();

  d_DestroyLogger( d_GetGlobalLogger() );
  return 0;
}

