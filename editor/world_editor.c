#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "structs.h"
#include "editor.h"
#include "world_editor.h"
#include "init_editor.h"
#include "item_editor.h"
#include "entity_editor.h"
#include "color_editor.h"
#include "ui_editor.h"

static void e_WorldEditorDoLoop( float );
static void e_WorldEditorRenderLoop( float );
static void we_CreationDoLoop( float );
static void we_CreationRenderLoop( float );

static aWidget_t* w;

World_t* map;

void e_InitWorldEditor( void )
{
  app.delegate.logic = e_WorldEditorDoLoop;
  app.delegate.draw  = e_WorldEditorRenderLoop;
  
  a_InitWidgets( "resources/widgets/editor/world.json" );
  
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
  
  w = a_GetWidget( "menu_bar" );
  container = ( aContainerWidget_t* )w->data;
  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strcmp( current->name, "creation" ) == 0 )
    {
      current->action = we_creation;
    }
    
    if ( strcmp( current->name, "edit" ) == 0 )
    {
      current->action = we_edit;
    }
    
    if ( strcmp( current->name, "save" ) == 0 )
    {
      current->action = we_save;
    }
    
    if ( strcmp( current->name, "load" ) == 0 )
    {
      current->action = we_load;
    }
  }

}

void we_creation( void )
{
  app.delegate.logic = we_CreationDoLoop;
  app.delegate.draw  = we_CreationRenderLoop;
  
  
  app.active_widget = a_GetWidget( "generation_menu" );
  aContainerWidget_t* container = ( aContainerWidget_t* )app.active_widget->data;
  app.active_widget->hidden = 0;
  
  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 0;
  }

}

static void we_CreationDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitWorldEditor();
  }

  a_DoWidget();

}

static void we_CreationRenderLoop( float dt )
{
//  a_DrawFilledRect( 240, 240, 400, 265, 255, 0, 255, 255 );
  
  a_DrawWidgets();

}

void we_edit( void )
{

}

void we_save( void )
{

}

void we_load( void )
{

}

static void e_WorldEditorDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitEditor();
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

