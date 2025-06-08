#include "Archimedes.h"
#include "structs.h"

void g_DrawWorldMap( World_t* world, aColor_t color )
{
  for ( int i = 0; i < WORLD_HEIGHT * WORLD_WIDTH; i++ )
  {
    int x = ( i / WORLD_HEIGHT ) * CELL_SIZE;
    int y = ( i % WORLD_HEIGHT ) * CELL_SIZE;
    int w = CELL_SIZE;
    int h = CELL_SIZE;

    a_DrawRect( x, y, w, h, color );
  }

}

void g_DrawRegionMap( RegionCell_t* region )
{

}

void g_DrawLocalMap( LocalCell_t* cell )
{

}

