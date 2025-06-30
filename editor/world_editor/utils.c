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
      current_glyph = map[i].tile.glyph;
      break;

    case REGION_LEVEL:
      e_GetCellSize( index, map->region_width, map->region_height,
                     &x, &y, &w, &h );

      i = index;

      current_index = pos.region_index;
      current_glyph = map[pos.world_index].regions[i].tile.glyph;
      break;

    case LOCAL_LEVEL:
      e_GetCellSize( index, map->local_width, map->local_height,
                     &x, &y, &w, &h );

      i = ( ( pos.local_z * ( map->local_width * map->local_height ) 
        + index ) );

      current_index = pos.local_index;
      current_glyph = map[pos.world_index].regions[pos.region_index].
        tiles[i].glyph;

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

void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered )
{
  int edge_x = 0;
  int edge_y = 0;

  if ( centered == 1 )
  {
    edge_x = originx - ( ( width  * cell_width  ) / 2 );
    edge_y = originy - ( ( height * cell_height ) / 2 );

  }
  
  else
  {
    edge_x = originx;
    edge_y = originy;

  }

  if ( app.mouse.x > edge_x && app.mouse.x <= edge_x + ( width * cell_width ) &&
       app.mouse.y > edge_y && app.mouse.y <= edge_y + ( height* cell_height ) )
  {
    int mousex = ( ( app.mouse.x - edge_x ) / cell_width  );
    int mousey = ( ( app.mouse.y - edge_y ) / cell_height );
    *grid_x = mousex;
    *grid_y = mousey;

  }

}

void e_MapMouseCheck( WorldPosition_t* pos )
{
  switch (pos->level) {
    case WORLD_LEVEL: 
      e_GetCellAtMouse( map->world_width, map->world_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->world_index = INDEX_2( pos->y, pos->x, map->world_height );
      break;

    case REGION_LEVEL:
      e_GetCellAtMouse( map->region_width, map->region_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->region_index = INDEX_2( pos->y, pos->x,
                                         map->region_height );
      break;

    case LOCAL_LEVEL:
      e_GetCellAtMouse( map->local_width, map->local_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->local_index = INDEX_3( pos->y, pos->x,
                                  pos->local_z, map->local_width,
                                  map->local_height );
      break;

    default:
      break;
  }
} 

void e_GlyphMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y )
{

  e_GetCellAtMouse( 28, 11, 900, 200, GLYPH_WIDTH, GLYPH_HEIGHT,
                    grid_x, grid_y, 0 );

  *index = INDEX_2( *grid_x, *grid_y, 28 );

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

