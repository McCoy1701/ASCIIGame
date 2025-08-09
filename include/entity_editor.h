/*
 * entity_editor.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __ENTITY_EDITOR_H__
#define __ENTITY_EDITOR_H__

#define COLOR_PALLETE_X 1177
#define COLOR_PALLETE_Y 110
#define GLYPH_PALLETE_X 1088
#define GLYPH_PALLETE_Y 263
#define GLYPH_BG_X 1083
#define GLYPH_BG_Y 105
#define ADD_LIST_BG_X 1083
#define ADD_LIST_BG_Y 534
#define RANDOMIZE_BG_X 1083
#define RANDOMIZE_BG_Y 582
#define GLYPH_SELECTED_X 1150
#define GLYPH_SELECTED_Y 110

void e_InitEntityEditor( void );
void e_DestroyEntityEditor( void );

void ee_Creation( void );
void ee_Edit( void );

/* UTILS */

void ee_DrawAttributes( void );
void ee_DrawSkills( void );
void ee_DrawAdvantages( void );
void ee_DrawDisadvantages( void );
void ee_DrawVirtues( void );
void ee_DrawVices( void );

#endif

