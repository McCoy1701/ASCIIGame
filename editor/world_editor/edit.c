#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "world_editor.h"

static void we_EditLogic( float dt );
static void we_EditDraw( float dt );

WorldPosition_t selected_pos;

void we_edit( void )
{
  app.delegate.logic = we_EditLogic;
  app.delegate.draw  = we_EditDraw;

  app.active_widget = a_GetWidget( "edit_menu" );
  aContainerWidget_t* container = a_GetContainerFromWidget( "edit_menu" );
  app.active_widget->hidden = 0;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 0;

  }

}

static void we_EditLogic( float dt )
{
  a_DoInput();

  if ( app.mouse.button == 1 )
  {
    e_MouseCheck( &selected_pos );

  }
  
  e_LevelZHeightCheck( &selected_pos );
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    e_InitWorldEditor();
  }

  a_DoWidget();

}

static void we_EditDraw( float dt )
{
  if ( map != NULL )
  {
    int draw_size = 0;
    switch ( selected_pos.level )
    {
      case WORLD_LEVEL:
        draw_size = map->world_width * map->world_height;         
        break;

      case REGION_LEVEL:
        draw_size = map->region_width * map->region_height;
        break;

      case LOCAL_LEVEL:
        draw_size = map->local_width * map->local_height;                      
        break;
    }

    for ( uint16_t i = 0; i < draw_size; i++ )
    {
      we_DrawWorldCell( i, map, selected_pos );
    }

  }

  a_DrawFilledRect( 100, 100, 32, 32, 0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 0, 255, 0, 255 );

  a_DrawWidgets();

}

