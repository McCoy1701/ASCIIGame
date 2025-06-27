#include "Archimedes.h"
#include "structs.h"

#ifndef __GAME_H__
#define __GAME_H__

World_t* init_world( const int world_width, const int world_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height, const int z_height );
void free_world( World_t* world, int world_index, int region_index );

void g_GetCellSize( int index, int width, int height, int* x, int* y, int* w, int* h );
void g_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y );

#endif

