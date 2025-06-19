#ifndef __INIT_EDITOR_H__
#define __INIT_EDITOR_H__

#include "structs.h"

World_t* init_world( const int world_size, const int region_size, const int local_size );
void free_world( World_t* world, int world_index, int region_index );

#endif

