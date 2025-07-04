/*
 * world_editor/creation.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include "Archimedes.h"
#include "init_editor.h"
#include "structs.h"
#include "world_editor.h"

static void we_CreationLogic( float );
static void we_CreationDraw( float );

int world_size_index    = 0;
int region_size_index   = 0;
int local_size_index    = 0;
int z_height_size_index = 0;

void we_Creation( void )
{
  app.delegate.logic = we_CreationLogic;
  app.delegate.draw  = we_CreationDraw;


  app.active_widget = a_GetWidget( "generation_menu" );
  aContainerWidget_t* container =
    a_GetContainerFromWidget( "generation_menu" );

  app.active_widget->hidden = 0;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 0;

    if ( strcmp( current->name, "generate" ) == 0 )
    {
      current->action = wec_GenerateWorld;
    }
  }

}

static void we_CreationLogic( float dt )
{
  a_DoInput();

  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    
    e_InitWorldEditor();
  }

  a_DoWidget();

}

static void we_CreationDraw( float dt )
{
  a_DrawWidgets();

}

void wec_GenerateWorld( void )
{
  int new_world_width   = 0;
  int new_world_height  = 0;
  int new_region_width  = 0;
  int new_region_height = 0;
  int new_local_width   = 0;
  int new_local_height  = 0;
  int new_z_height      = 0;

  aContainerWidget_t* container =
    ( aContainerWidget_t* )app.active_widget->data;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];

    if ( strcmp( current->name, "world_size" ) == 0 )
    {
      aSelectWidget_t* world_size = ( aSelectWidget_t* )current->data;
      world_size_index = world_size->value;
    }

    if ( strcmp( current->name, "region_size" ) == 0 )
    {
      aSelectWidget_t* region_size = ( aSelectWidget_t* )current->data;
      region_size_index = region_size->value;
    }

    if ( strcmp( current->name, "local_size" ) == 0 )
    {
      aSelectWidget_t* local_size = ( aSelectWidget_t* )current->data;
      local_size_index = local_size->value;
    }

    if ( strcmp( current->name, "z_height" ) == 0 )
    {
      aSelectWidget_t* z_height = ( aSelectWidget_t* )current->data;
      z_height_size_index = z_height->value;
    }
  }

  switch ( world_size_index ) {

    case 0: //small
      new_world_width  = WORLD_WIDTH_SMALL;
      new_world_height = WORLD_HEIGHT_SMALL;
    break;

    case 1: //medium
      new_world_width  = WORLD_WIDTH_MEDIUM;
      new_world_height = WORLD_HEIGHT_MEDIUM;
    break;

    case 2: //large
      new_world_width  = WORLD_WIDTH_LARGE;
      new_world_height = WORLD_HEIGHT_LARGE;
    break;

    default:
    break;
  }

  switch ( region_size_index ) {

    case 0: //small
      new_region_width  = REGION_WIDTH_SMALL;
      new_region_height = REGION_HEIGHT_SMALL;
    break;

    case 1: //medium
      new_region_width  = REGION_WIDTH_MEDIUM;
      new_region_height = REGION_HEIGHT_MEDIUM;
    break;

    case 2: //large
      new_region_width  = REGION_WIDTH_LARGE;
      new_region_height = REGION_HEIGHT_LARGE;
    break;

    default:
    break;
  }

  switch ( local_size_index ) {

    case 0: //small
      new_local_width  = LOCAL_WIDTH_SMALL;
      new_local_height = LOCAL_HEIGHT_SMALL;
    break;

    case 1: //medium
      new_local_width  = LOCAL_WIDTH_MEDIUM;
      new_local_height = LOCAL_HEIGHT_MEDIUM;
    break;

    case 2: //large
      new_local_width  = LOCAL_WIDTH_LARGE;
      new_local_height = LOCAL_WIDTH_LARGE;
    break;

    default:
    break;
  }

  switch ( z_height_size_index ) {

    case 0: //small
      new_z_height = Z_HEIGHT_SMALL;
    break;

    case 1: //medium
      new_z_height = Z_HEIGHT_MEDIUM;
    break;

    case 2: //large
      new_z_height = Z_HEIGHT_LARGE;
    break;

    default:
    break;
  }

  if ( map != NULL )
  {
    free_world( map, ( map->world_width * map->world_height ),
                     ( map->region_width * map->region_height ) );
  }

  map = init_world( new_world_width,  new_world_height, new_region_width,
                    new_region_height, new_local_width, new_local_height,
                    new_z_height );

}

