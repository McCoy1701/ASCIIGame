
/*
 * entity_editor/utils.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "entity_editor.h"

void ee_DrawAttributes( void )
{
  a_DrawFilledRect( 25, 110, 259, 527, 60, 94, 139, 255 ); //Attributes BG

}

void ee_DrawSkills( void )
{
  a_DrawFilledRect( 290, 110, 255, 527, 23, 32, 56, 255 ); //Skills BG

}

void ee_DrawAdvantages( void )
{
  a_DrawFilledRect( 551, 110, 255, 260, 70, 130, 50, 255 ); //Advantages BG
  
}

void ee_DrawDisadvantages( void )
{
  a_DrawFilledRect( 812, 110, 255, 260, 165, 48, 48, 255 ); //Disadvantages BG
  
}

void ee_DrawVirtues( void )
{
  a_DrawFilledRect( 551, 376, 255, 260, 222, 168, 65, 255 ); //Virtues BG
  
}

void ee_DrawVices( void )
{
  a_DrawFilledRect( 812, 376, 255, 260, 207, 87, 60, 255 ); //Vices BG
  
}

