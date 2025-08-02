/*
 * entity_editor.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __ENTITY_EDITOR_H__
#define __ENTITY_EDITOR_H__

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

