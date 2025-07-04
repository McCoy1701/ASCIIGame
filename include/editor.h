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

void e_InitEditor( void );
void e_Mainloop( void );
void e_DestroyEditor( void );

#endif

