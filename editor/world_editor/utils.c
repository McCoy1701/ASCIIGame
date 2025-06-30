#include "Archimedes.h"
#include "defs.h"
#include "editor.h"
#include "structs.h"
#include "world_editor.h"

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos )
{
  int x, y, w, h;
  uint32_t i = 0;
  uint16_t current_index = 0;
  int current_glyph = 0;

  switch ( pos.level ) {
    case WORLD_LEVEL:
      e_GetCellSize( index, map->world_width, map->world_height,
                     &x, &y, &w, &h );

      i = index;

      current_index = pos.world_index;
      current_glyph = map[pos.world_index].tile.glyph;
      break;

    case REGION_LEVEL:
      e_GetCellSize( index, map->region_width, map->region_height,
                     &x, &y, &w, &h );

      i = index;

      current_index = pos.region_index;
      current_glyph = map[pos.world_index].regions[pos.region_index].tile.glyph;
      break;

    case LOCAL_LEVEL:
      e_GetCellSize( index, map->local_width, map->local_height,
                     &x, &y, &w, &h );

      i = ( ( pos.local_z * ( map->local_width * map->local_height ) + index ) );

      current_index = pos.local_index;
      current_glyph = map[pos.world_index].regions[pos.region_index].tiles[pos.local_index].glyph;
      break;

    default:
      break;
  }

  if ( i == current_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2, 255, 255, 0, 255 );
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2 );
  } 

  else
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2, 0, 0, 0, 255 );
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2 );
  }

}

void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h )
{
  int row = ( index / height );
  int col = ( index % height );
  *x = ( ( SCREEN_WIDTH / 2 )  - ( ( width  * CELL_WIDTH ) / 2 ) )
    + ( row * CELL_WIDTH );
  *y = ( ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_HEIGHT ) / 2 ) )
    + ( col * CELL_HEIGHT );
  *w = CELL_WIDTH;
  *h = CELL_HEIGHT;
}

void e_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y )
{
  int edge_x = ( SCREEN_WIDTH  / 2 ) - ( ( width  * CELL_WIDTH  ) / 2 );
  int edge_y = ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_HEIGHT ) / 2 );
  int mousex = ( ( app.mouse.x - edge_x ) / CELL_WIDTH  );
  int mousey = ( ( app.mouse.y - edge_y ) / CELL_HEIGHT );
  *grid_x = ( mousex < 0 ) ? 0 : ( ( mousex >= width  )
    ? width  - 1 : mousex );
  *grid_y = ( mousey < 0 ) ? 0 : ( ( mousey >= height )
    ? height - 1 : mousey );
}

void e_MouseCheck( WorldPosition_t* pos )
{
  if ( map != NULL )
  {
    int grid_x, grid_y;

    switch (pos->level) {
      case WORLD_LEVEL: 
        e_GetCellAtMouse( map->world_width, map->world_height, &grid_x,
                         &grid_y );

        pos->world_index = INDEX_2( grid_y, grid_x, map->world_height );
        break;

      case REGION_LEVEL:
        e_GetCellAtMouse( map->region_width, map->region_height, &grid_x,
                         &grid_y );

        pos->region_index = INDEX_2( grid_y, grid_x,
                                           map->region_height );
        break;

      case LOCAL_LEVEL:
        e_GetCellAtMouse( map->local_width, map->local_height, &grid_x,
                         &grid_y );

        pos->local_index = INDEX_3( grid_y, grid_x,
                                    pos->local_z, map->local_width,
                                    map->local_height );
        break;

      default:
        break;
    }
  } 

}

void e_LevelZHeightCheck( WorldPosition_t* pos )
{
  if ( app.keyboard[SDL_SCANCODE_RETURN] == 1 )
  {
    app.keyboard[SDL_SCANCODE_RETURN] = 0;

    if ( pos->level >= 0 && pos->level < LOCAL_LEVEL )
    {
      pos->level++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_BACKSPACE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

    if ( pos->level > 0 && pos->level <= LOCAL_LEVEL )
    {
      pos->level--;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_EQUALS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_EQUALS] = 0;
    if ( pos->local_z >= 0 && pos->local_z < map->z_height - 1 )
    {
      pos->local_z++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_MINUS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_MINUS] = 0;
    if ( pos->local_z > 0 && pos->local_z <= map->z_height )
    {
      pos->local_z--;
    }

  }

}

