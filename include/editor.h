/*
 * editor.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "structs.h"

extern GlyphArray_t* game_glyphs;
extern aColor_t master_colors[MAX_COLOR_GROUPS][48];
extern float zoom_level;

void e_InitEditor( void );
void e_Mainloop( void );
void e_DestroyEditor( void );
void e_UpdateZoom( void );

void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h );

void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered );

void e_MapMouseCheck( WorldPosition_t* pos );
void e_GlyphMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y );
void e_ColorMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y );
void e_LevelZHeightCheck( WorldPosition_t* pos );
void e_LoadColorPalette( aColor_t palette[MAX_COLOR_GROUPS][MAX_COLOR_PALETTE],
                       const char * filename );

GameTileArray_t* e_GetSelectGrid( World_t* map, WorldPosition_t pos,
                                   WorldPosition_t highlight );

void e_DrawSelectGrid( World_t* map, WorldPosition_t pos,
                                   WorldPosition_t highlight );

void e_ChangeGameTile( World_t* map, WorldPosition_t pos,
                            GameTileArray_t* tile_array, int glyph_index,
                            int bg_index, int fg_index );

void e_PasteGameTile( World_t* map, WorldPosition_t pos,
                       GameTileArray_t* tile_array );

void e_DrawPastePreview( World_t* map, WorldPosition_t pos,
                       GameTileArray_t* tile_array );

#endif

