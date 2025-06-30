#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "world_editor.h"

static void we_EditLogic( float dt );
static void we_EditDraw( float dt );

WorldPosition_t selected_pos;
int glyph_index = 0;
uint8_t selected_glyph_x = 0, selected_glyph_y = 0;

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
    e_MapMouseCheck( &selected_pos );
    
    e_GlyphMouseCheck( &glyph_index, &selected_glyph_x, &selected_glyph_y );
    
  }

  if ( map != NULL )
  {
    if ( app.mouse.button == 2 )
    {
      switch ( selected_pos.level ) {
        case WORLD_LEVEL:
          map[selected_pos.world_index].tile.glyph = glyph_index;
          break;

        case REGION_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
            tile.glyph = glyph_index;
          break;

        case LOCAL_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
            tiles[selected_pos.local_index].glyph = glyph_index;
          break;
      }
    }
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
  
  a_DrawFilledRect( 895, 195, 262, 180, 0, 0, 255, 255 );
  
  for ( int i = 0; i < game_glyphs->count; i++ )
  {
    a_DrawFilledRect( game_glyphs->rects[i].x + 900,
                      game_glyphs->rects[i].y + 200,
                      game_glyphs->rects[i].w,
                      game_glyphs->rects[i].h, 0, 0, 0, 255 );

    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[i],
                       game_glyphs->rects[i].x + 900,
                       game_glyphs->rects[i].y + 200, 1 );
    
    if ( i == glyph_index )
    {
      a_DrawRect( game_glyphs->rects[i].x + 900,
                  game_glyphs->rects[i].y + 200,
                  game_glyphs->rects[i].w,
                  game_glyphs->rects[i].h, 255, 255, 0, 255 );
    }
    
  }
  
  a_DrawFilledRect( 900, 100, game_glyphs->rects[glyph_index].w * 2,
                    game_glyphs->rects[glyph_index].h * 2, 255, 0, 0, 255 );

  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[glyph_index],
                     900, 100, 2 );

  a_DrawWidgets();

}

