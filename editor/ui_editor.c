#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"
#include "items_editor.h"
#include "entity_editor.h"
#include "color_editor.h"

static void e_UIEditorDoLoop( float );
static void e_UIEditorRenderLoop( float );

static void uie_NewButton( void );
static void uie_NewSelect( void );
static void uie_NewSlider( void );
static void uie_NewInput( void );
static void uie_NewControl( void );
static void uie_NewContainer( void );

void e_InitUIEditor( void )
{
  aWidget_t* w;
  app.delegate.logic = e_UIEditorDoLoop;
  app.delegate.draw  = e_UIEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/ui.json" );
  
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

  w = a_GetWidget( "ui_menu_bar" );
  container = ( aContainerWidget_t* )w->data;
  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strcmp( current->name, "button" ) == 0 )
    {
      current->action = uie_NewButton;
    }
    
    if ( strcmp( current->name, "slider" ) == 0 )
    {
      current->action = uie_NewSlider;
    }
    
    if ( strcmp( current->name, "control" ) == 0 )
    {
      current->action = uie_NewControl;
    }
    
    if ( strcmp( current->name, "container" ) == 0 )
    {
      current->action = uie_NewContainer;
    }

    if ( strcmp( current->name, "select" ) == 0 )
    {
      current->action = uie_NewSelect;
    }

    if ( strcmp( current->name, "input" ) == 0 )
    {
      current->action = uie_NewInput;
    }
  }
  
}

static void e_UIEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
  }

  a_DoWidget();
}

static void e_UIEditorRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, 0, 255, 0, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 255, 0, 255 );

  a_DrawWidgets();
}

static void uie_NewButton( void )
{
  printf("Button\n");

}

static void uie_NewSelect( void )
{
  printf("Select\n");

}

static void uie_NewSlider( void )
{
  printf("Slider\n");

}

static void uie_NewInput( void )
{
  printf("Input\n");

}

static void uie_NewControl( void )
{
  printf("Control\n");

}

static void uie_NewContainer( void )
{
  printf("Container\n");

}

void e_DestroyUIEditor( void )
{
}

