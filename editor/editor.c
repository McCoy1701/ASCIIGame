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

    const float delta_time = a_GetDeltaTime();

    // Update logic and draw using the SAME delta time
    app.delegate.logic( delta_time );
    app.delegate.draw( delta_time );

    // 4. Log with a UNIQUE IDENTIFIER
//    d_LogRateLimitedF(D_LOG_RATE_LIMIT_FLAG_HASH_FORMAT_STRING, D_LOG_LEVEL_DEBUG, 1, 10.0,
//                      "[e_MainLoop(void)] Delta Time; %.7f - Should be Limited to 1 per 10 seconds [void]", delta_time);

    // 5. Present the scene
    a_PresentScene();
}
int main( void )
{
  // Initialize Daedalus Logger
  dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG };
  dLogger_t* logger = d_CreateLogger(config);
  d_SetGlobalLogger(logger);
  d_AddLogHandler(d_GetGlobalLogger(), d_ConsoleLogHandler, NULL);
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  e_InitEditor();

  while( app.running ) {
    e_Mainloop();
  }

  a_Quit();

  // Clean up the logger before finishing
  d_DestroyLogger(d_GetGlobalLogger());


  return 0;
}
