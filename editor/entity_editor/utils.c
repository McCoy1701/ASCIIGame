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
#include "colors.h"
#include "editor.h"
#include "entity_editor.h"

void ee_DrawAttributes( void )
{
  aRect_t attributes_rect = (aRect_t){ .x = 25, .y = 110, .w = 259, .h = 527 };
  a_DrawFilledRect( attributes_rect,
                    master_colors[APOLLO_PALETE][APOLLO_BLUE_2] ); //Attributes BG

}

void ee_DrawSkills( void )
{
  aRect_t skills_rect = (aRect_t){ .x = 290, .y = 110, .w = 255, .h = 527 };
  a_DrawFilledRect( skills_rect,
                    master_colors[APOLLO_PALETE][APOLLO_BLUE_0] ); //Skills BG

}

void ee_DrawAdvantages( void )
{
  aRect_t advantages_rect = (aRect_t){ .x = 551, .y = 110, .w = 255, .h = 260 };
  a_DrawFilledRect( advantages_rect,
                    master_colors[APOLLO_PALETE][APOLLO_GREEN_2] ); //Advantages BG
  
}

void ee_DrawDisadvantages( void )
{
  aRect_t disadvantages_rect = (aRect_t){ .x = 812, .y = 110, .w = 255, .h = 260 };
  a_DrawFilledRect( disadvantages_rect,
                    master_colors[APOLLO_PALETE][APOLLO_RED_3] ); //Disadvantages BG

}

void ee_DrawVirtues( void )
{
  aRect_t virtues_rect = (aRect_t){ .x = 551, .y = 376, .w = 255, .h = 260 };
  a_DrawFilledRect( virtues_rect,
                    master_colors[APOLLO_PALETE][APOLLO_TAN_4] ); //Virtues BG
  
}

void ee_DrawVices( void )
{
  aRect_t vices_rect = (aRect_t){ .x = 812, .y = 376, .w = 255, .h = 260 };
  a_DrawFilledRect( vices_rect,
                    master_colors[APOLLO_PALETE][APOLLO_RED_4] ); //Vices BG
  
}

