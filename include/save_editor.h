/*
 * save_editor.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __SAVE_EDITOR_H__
#define __SAVE_EDITOR_H__

#include "structs.h"

int SaveWorld( World_t* world, const char* filename );
int SaveRegion( World_t* world, const char* filename, const int realm_index );

World_t* LoadWorld( const char* filename );
int LoadRegion( World_t* world, const char* filename, const int realm_index );

World_t* LoadPartialWorld( const char* filename );
int LoadPartialRegion( WorldPosition_t* pos, World_t* world, const char* filename );

#endif

