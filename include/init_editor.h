#ifndef __INIT_EDITOR_H__
#define __INIT_EDITOR_H__

#include "structs.h"

World_t* init_world( const int world_width, const int world_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height, const int z_height );
void free_world( World_t* world, int world_index, int region_index );

#endif

