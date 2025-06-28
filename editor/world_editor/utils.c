#include "Archimedes.h"
#include "defs.h"
#include "structs.h"
#include "world_editor.h"

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos )
{
  int x, y, w, h;
  uint32_t i = 0;
  uint16_t current_index = 0;

  switch (pos.level) {
    case WORLD_LEVEL:
      e_GetCellSize( index, map->world_width, map->world_height, &x, &y, &w, &h );
      i = ( ( pos.local_z * ( map->world_width * map->world_height ) ) + index );
      current_index = pos.world_index;
      break;

    case REGION_LEVEL:
      e_GetCellSize( index, map->region_width, map->region_height, &x, &y, &w, &h );
      i = ( ( pos.local_z * ( map->region_width * map->region_height ) ) + index );
      current_index = pos.region_index;
      break;

    case LOCAL_LEVEL:
      e_GetCellSize( index, map->local_width, map->local_height, &x, &y, &w, &h );
      i = ( ( pos.local_z * ( map->local_width * map->local_height ) ) + index );
      current_index = pos.local_index;
      break;

    default:
      break;
  }

  if ( i == current_index )
  {
    a_DrawRect( x, y, w, h, 255, 255, 0, 255 );
  } 

  else
  {
    a_DrawRect( x, y, w, h, 0, 128, 128, 255 );
  }

}

void e_GetCellSize( int index, int width, int height, int* x, int* y, int* w, int* h )
{
  int row = ( index / height );
  int col = ( index % height );
  *x = ( ( SCREEN_WIDTH / 2 )  - ( ( width  * CELL_SIZE ) / 2 ) )
    + ( row * CELL_SIZE );
  *y = ( ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 ) )
    + ( col * CELL_SIZE );
  *w = CELL_SIZE;
  *h = CELL_SIZE;
}

void e_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y )
{
  int edge_x = ( SCREEN_WIDTH  / 2 ) - ( ( width  * CELL_SIZE ) / 2 );
  int edge_y = ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 );
  int mousex = ( ( app.mouse.x - edge_x ) / CELL_SIZE );
  int mousey = ( ( app.mouse.y - edge_y ) / CELL_SIZE );
  *grid_x = ( mousex < 0 ) ? 0 : ( ( mousex >= width  )
    ? width  - 1 : mousex );
  *grid_y = ( mousey < 0 ) ? 0 : ( ( mousey >= height )
    ? height - 1 : mousey );
}

