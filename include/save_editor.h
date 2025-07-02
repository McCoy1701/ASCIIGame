#ifndef __SAVE_EDITOR_H__
#define __SAVE_EDITOR_H__

#include "structs.h"

int SaveWorld( World_t* world, const char* filename );
World_t* LoadWorld( const char* filename );

#endif

