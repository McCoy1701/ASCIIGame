/*
 * game.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include "Archimedes.h"
#include "structs.h"

#ifndef __GAME_H__
#define __GAME_H__

extern GlyphArray_t* game_glyphs;

World_t* init_world( const int world_width, const int world_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height,
                     const int z_height );

void free_world( World_t* world, int world_index, int region_index );

void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h );

void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered );

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos );


GlyphArray_t* e_InitGlyphs( const char* filename, int glyph_width,
                            int glyph_height );

void e_MapMouseCheck( World_t* map, WorldPosition_t* pos );

#endif

