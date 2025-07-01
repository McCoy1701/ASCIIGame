#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "item_editor.h"
#include "color_editor.h"
#include "ui_editor.h"

static void e_EntityEditorDoLoop( float );
static void e_EntityEditorRenderLoop( float );

void e_InitEntityEditor( void )
{
  app.delegate.logic = e_EntityEditorDoLoop;
  app.delegate.draw  = e_EntityEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/entity.json" );
  
  app.active_widget = a_GetWidget( "tab_bar" );

  aContainerWidget_t* container =
    ( aContainerWidget_t* )app.active_widget->data;

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

static void e_EntityEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }
  
  if ( app.mouse.button == 2 )
  {
    app.mouse.button = 0;
    printf( "x: %d, y: %d\n", app.mouse.x, app.mouse.y );
  }

  a_DoWidget();
}

static void e_EntityEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 128, 128, 128, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 255, 0, 255 );

  a_DrawWidgets();
}

void e_DestroyEntityEditor( void )
{
}

