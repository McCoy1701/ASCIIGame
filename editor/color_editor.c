#include <stdio.h>
#include <string.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "entity_editor.h"
#include "item_editor.h"

static void e_ColorEditorDoLoop( float dt );
static void e_ColorEditorRenderLoop( float dt );

static aWidget_t* w;

void e_InitColorEditor( void )
{
  app.delegate.logic = e_ColorEditorDoLoop;
  app.delegate.draw  = e_ColorEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/colors.json" );
  
  app.active_widget = a_GetWidget( "menu_bar" );
  app.active_widget->action = e_InitColorEditor;

  aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;
  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strncmp( current->name, "world", sizeof( current->name ) ) == 0 )
    {
      current->action = e_InitWorldEditor;
    }
    
    if ( strncmp( current->name, "item", sizeof( current->name ) ) == 0 )
    {
      current->action = e_InitItemEditor;
    }
    
    if ( strncmp( current->name, "entity", sizeof( current->name ) ) == 0 )
    {
      current->action = e_InitEntityEditor;
    }
    
    if ( strncmp( current->name, "colors", sizeof( current->name ) ) == 0 )
    {
      current->action = e_InitColorEditor;
    }
  }

}

static void e_ColorEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }

  a_DoWidget();
}

static void e_ColorEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 0, 255 );

  a_DrawWidgets();
}

void e_DestroyColorEditor( void )
{
}

