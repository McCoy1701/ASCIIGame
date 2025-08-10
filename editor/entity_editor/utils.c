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
#include "traits.h"

static int attribute_offset = 0;
static int skill_offset = 0;
static int advantage_offset = 0;
static int disadvantage_offset = 0;
static int virtue_offset = 0;
static int vice_offset = 0;

char* attribute_strings[MAX_ATTRIBUTE+1] =
{
  "Strength",
  "Dexterity",
  "Constitution",
  "Wisdom",
  "Intelligence",
  "Charisma",
  "MAX_ATTRIBUTE"
};

char* skill_strings[MAX_SKILL+1] =
{
  "Woodcutting",
  "Crafting",
  "Mining",
  "Fishing",
  "Smithing",
  "Cooking",
  "Thieving",
  "Farming",
  "Construction",
  "Climb",
  "Concentration",
  "Forgery",
  "Heal",
  "Hide",
  "Jump",
  "Intimidate",
  "Disable Device",
  "Listen",
  "Open Lock",
  "Spot",
  "Speak Language",
  "Spellcraft",
  "Survival",
  "Swim",
  "Move Silently",
  "Perform",
  "Sense Motive",
  "Sleight Of Hand",
  "Search",
  "Ride",
  "Gambling",
  "Computer Programming",
  "Engineer",
  "Electronics Repair",
  "Explosives",
  "Escape",
  "Acting",
  "Melee Weapons",
  "Leadership",
  "Influence Skill",
  "Law",
  "Missile Weapons",
  "Interrogation",
  "Mathematics",
  "Mechanics",
  "Navigation",
  "Occultism",
  "Pick Pocketing",
  "Public Speaking",
  "Smuggling",
  "Tracking",
  "Traps",
  "Stealth",
  "Throwing",
  "Vehicle Skill",
  "Writing",
  "Ritual Magic",
  "Physics",
  "MAX_SKILL"
};

char* advantage_strings[MAX_ADVANTAGE+1] =
{
  "Ambidexterity",
  "Combat_Reflexes",
  "Daredevil",
  "Empathy",
  "Hard To Kill",
  "Luck",
  "Night Vision",
  "Talented",
  "Perfect Balance",
  "MAX_ADVANTAGE"
};

char* disadvantage_strings[MAX_DISADVANTAGE+1] =
{
  "Bad Sight",
  "Bad Temper",
  "Hard Of Hearing",
  "Honesty",
  "Jealousy",
  "Lecherousness",
  "Obsession",
  "Impulsiveness",
  "Delusions",
  "MAX_DISADVANTAGE"
};

char* virtue_strings[MAX_VIRTUE+1] =
{
  "Humility",
  "Charity",
  "Chastity",
  "Kindness",
  "Temperance",
  "Patience",
  "Diligence",
  "MAX_VIRTUE"
};

char* vice_strings[MAX_VIRTUE+1] =
{
  "Pride",
  "Greed",
  "Lust",
  "Envy",
  "Gluttony",
  "Wrath",
  "Sloth",
  "MAX_VICE"
};

void e_update( void )
{
  if ( app.mouse.wheel == 1 || app.keyboard[SDL_SCANCODE_DOWN] == 1 )
  {
    e_UpdateList( MAX_ATTRIBUTE, &attribute_offset, -1 );
    e_UpdateList( MAX_SKILL, &skill_offset, -1 );
    e_UpdateList( MAX_ADVANTAGE, &advantage_offset, -1 );
    e_UpdateList( MAX_DISADVANTAGE, &disadvantage_offset, -1 );
    e_UpdateList( MAX_VIRTUE, &virtue_offset, -1 );
    e_UpdateList( MAX_VICE, &vice_offset, -1 );
    app.mouse.wheel = 0;
    app.keyboard[SDL_SCANCODE_DOWN] = 0;
  }

  else if ( app.mouse.wheel == -1 || app.keyboard[SDL_SCANCODE_UP] == 1 )
  {
    e_UpdateList( MAX_ATTRIBUTE, &attribute_offset, 1 );
    e_UpdateList( MAX_SKILL, &skill_offset, 1 );
    e_UpdateList( MAX_ADVANTAGE, &advantage_offset, 1 );
    e_UpdateList( MAX_DISADVANTAGE, &disadvantage_offset, 1 );
    e_UpdateList( MAX_VIRTUE, &virtue_offset, 1 );
    e_UpdateList( MAX_VICE, &vice_offset, 1 );
    app.mouse.wheel = 0;
    app.keyboard[SDL_SCANCODE_UP] = 0;
  }

}

void e_DrawList( int originx, int originy, char** list, int list_len,
                 int list_offset, int display_count )
{
  for ( int i = 0; i < display_count; i++ )
  {
    if ( list_offset+i < list_len )
    {
      a_DrawText( list[list_offset + i], originx, 
                 ( originy + ( GLYPH_HEIGHT * i ) ),
                 black, white, FONT_CODE_PAGE_437, TEXT_ALIGN_LEFT, 0 );
    }
  }

}

void e_UpdateList( int list_len, int* list_offset, int value )
{
  if ( *list_offset <= list_len )
  {
    if ( value == -1 )
    {
      if ( *list_offset > 0 && *list_offset <= list_len )
      {
        *list_offset = *list_offset - 1;
      }
    }

    else if ( value == 1 )
    {
      if ( *list_offset >= 0 && *list_offset < list_len )
      {
        *list_offset = *list_offset + 1;
      }
    }
  }

}

void ee_DrawAttributes( void )
{
  aRect_t attributes_rect = (aRect_t){ .x = 25, .y = 110, .w = 259, .h = 527 };
  a_DrawFilledRect( attributes_rect,
    master_colors[APOLLO_PALETE][APOLLO_BLUE_2] ); //Attributes BG
  
  e_DrawList( ( attributes_rect.x + 10), ( attributes_rect.y + 10 ),
    attribute_strings, MAX_ATTRIBUTE, attribute_offset, MAX_ATTRIBUTE );

}

void ee_DrawSkills( void )
{
  aRect_t skills_rect = (aRect_t){ .x = 290, .y = 110, .w = 255, .h = 527 };
  a_DrawFilledRect( skills_rect,
    master_colors[APOLLO_PALETE][APOLLO_BLUE_0] ); //Skills BG
  
  e_DrawList( ( skills_rect.x + 10), ( skills_rect.y + 10 ),
             skill_strings, MAX_SKILL, skill_offset, MAX_SKILL / 2 );

}

void ee_DrawAdvantages( void )
{
  aRect_t advantages_rect = (aRect_t){ .x = 551, .y = 110, .w = 255, .h = 260 };
  a_DrawFilledRect( advantages_rect,
    master_colors[APOLLO_PALETE][APOLLO_GREEN_2] ); //Advantages BG
  
  e_DrawList( ( advantages_rect.x + 10), ( advantages_rect.y + 10 ),
              advantage_strings, MAX_ADVANTAGE, advantage_offset,
              MAX_ADVANTAGE );
  
}

void ee_DrawDisadvantages( void )
{
  aRect_t disadvantages_rect = (aRect_t){ .x = 812, .y = 110, .w = 255,
                                                              .h = 260 };
  a_DrawFilledRect( disadvantages_rect,
    master_colors[APOLLO_PALETE][APOLLO_RED_3] ); //Disadvantages BG
  
  e_DrawList( ( disadvantages_rect.x + 10), ( disadvantages_rect.y + 10 ),
              disadvantage_strings, MAX_DISADVANTAGE, disadvantage_offset,
              MAX_DISADVANTAGE );

}

void ee_DrawVirtues( void )
{
  aRect_t virtues_rect = (aRect_t){ .x = 551, .y = 376, .w = 255, .h = 260 };
  a_DrawFilledRect( virtues_rect,
    master_colors[APOLLO_PALETE][APOLLO_ORANGE_4] ); //Virtues BG
  
  e_DrawList( ( virtues_rect.x + 10), ( virtues_rect.y + 10 ),
             virtue_strings, MAX_VIRTUE, virtue_offset, MAX_VIRTUE );
  
}

void ee_DrawVices( void )
{
  aRect_t vices_rect = (aRect_t){ .x = 812, .y = 376, .w = 255, .h = 260 };
  a_DrawFilledRect( vices_rect,
    master_colors[APOLLO_PALETE][APOLLO_RED_4] ); //Vices BG
  
  e_DrawList( ( vices_rect.x + 10), ( vices_rect.y + 10 ),
             vice_strings, MAX_VICE, vice_offset, MAX_VICE );
  
}

