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


/* Utils */
void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h );

void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered );

void e_DrawColorPalette( int originx, int originy, int fg_index, int bg_index );
void e_DrawGlyphPalette( int originx, int originy, int glyph_index );

void e_ColorMouseCheck( int originx, int originy, int* index, uint8_t* grid_x,
                        uint8_t* grid_y, int centered );

void e_GlyphMouseCheck( int originx, int originy, int* index, uint8_t* grid_x,
                        uint8_t* grid_y, int centered );

void e_LoadColorPalette( aColor_t palette[MAX_COLOR_GROUPS][MAX_COLOR_PALETTE],
                       const char * filename );

void e_DrawGlyphText( const char* message, int x, int y, int bg, int fg,
                      int align, int max_width );

#endif

