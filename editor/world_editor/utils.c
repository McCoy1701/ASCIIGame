#include "Archimedes.h"
#include "defs.h"

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

