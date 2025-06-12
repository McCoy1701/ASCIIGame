#ifndef __SAVE_EDITOR_H__
#define __SAVE_EDITOR_H__

#include "init_editor.h"

int SaveTest( LocalCell_t* local, const char* filename );
LocalCell_t* LoadTest( const char* filename );
int SaveWorld( World_t* world, const char* filename );
int LoadWorld( World_t* world, const char* filename );

#endif

