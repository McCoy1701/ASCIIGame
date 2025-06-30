#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "structs.h"

extern GlyphArray_t* game_glyphs;

void e_InitEditor( void );
void e_Mainloop( void );
void e_DestroyEditor( void );

#endif

