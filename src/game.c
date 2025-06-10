#include "Archimedes.h"
#include "structs.h"

void g_DrawWorldMap( World_t* world, int originX, int originY, aColor_t color )
{
  for ( int i = 0; i < WORLD_HEIGHT * WORLD_WIDTH; i++ )
  {
    int row = ( i / WORLD_HEIGHT );
    int col = ( i % WORLD_HEIGHT );
    int x = originX + ( row - WORLD_WIDTH / 2 ) * CELL_SIZE;
    int y = originY + ( col - WORLD_WIDTH / 2 ) * CELL_SIZE;
    int w = CELL_SIZE;
    int h = CELL_SIZE;

    a_DrawRect( x, y, w, h, color );
  }

}

void g_DrawRegionMap( RegionCell_t* region, int originX, int originY, aColor_t color )
{
  for ( int i = 0; i < REGION_SIZE * REGION_SIZE; i++ )
  {
    int row = ( i / REGION_SIZE );
    int col = ( i % REGION_SIZE );
    int x = originX + ( row - REGION_SIZE / 2 ) * CELL_SIZE;
    int y = originY + ( col - REGION_SIZE / 2 ) * CELL_SIZE;
    int w = CELL_SIZE;
    int h = CELL_SIZE;

    a_DrawRect( x, y, w, h, color );
  }

}

void g_DrawLocalMap( LocalCell_t* cell, int originX, int originY, aColor_t color )
{
  for ( int i = 0; i < LOCAL_SIZE * LOCAL_SIZE; i++ )
  {
    int row = ( i / LOCAL_SIZE );
    int col = ( i % LOCAL_SIZE );
    int x = originX + ( row - LOCAL_SIZE / 2 ) * CELL_SIZE;
    int y = originY + ( col - LOCAL_SIZE / 2 ) * CELL_SIZE;
    int w = CELL_SIZE;
    int h = CELL_SIZE;

    a_DrawRect( x, y, w, h, color );
  }

}

