/*
 * world_editor/edit.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "editor.h"
#include "structs.h"
#include "world_editor.h"

static void we_EditLogic( float dt );
static void we_EditDraw( float dt );

static WorldPosition_t selected_pos;
static WorldPosition_t highlighted_pos;
int glyph_index = 0;
int fg_index = 0;
int bg_index = 0;
uint8_t selected_glyph_x = 0, selected_glyph_y = 0;
uint8_t selected_fg_x = 0, selected_fg_y = 0;
uint8_t selected_bg_x = 0, selected_bg_y = 0;
int editor_mode = 0;
GameTileArray_t* clipboard = NULL;
static char* pos_text;

char* wem_strings[WEM_MAX+1] =
{
  "WEM_NONE",
  "WEM_BRUSH",
  "WEM_COPY",
  "WEM_PASTE",
  "WEM_MASS_CHANGE",
  "WEM_SELECT",
  "WEM_MAX"
};

void we_Edit( void )
{
  app.delegate.logic = we_EditLogic;
  app.delegate.draw  = we_EditDraw;

  app.active_widget = a_GetWidget( "generation_menu" );
  aContainerWidget_t* container = a_GetContainerFromWidget( "generation_menu" );
  app.active_widget->hidden = 1;

  for ( int i = 0; i < container->num_components; i++ )
  {
    aWidget_t* current = &container->components[i];
    current->hidden = 1;

  }
  
  pos_text = malloc( sizeof(char) * 50 );

  selected_pos = (WorldPosition_t){ .world_index = 0, .region_index = 0,
    .local_index = 0, .level = 0, .local_z = 0 };
  snprintf(pos_text, 50, "%d,%d,%d,%d,%d\n", selected_pos.world_index,
           selected_pos.region_index, selected_pos.local_index, selected_pos.level,
           selected_pos.local_z );

}

static void we_EditLogic( float dt )
{
  a_DoInput();
  
  if ( map != NULL )
  {
    e_MapMouseCheck( &highlighted_pos );
    
  }
  
  if ( app.mouse.button == 1 )
  {
    if ( map != NULL )
    {
      e_MapMouseCheck( &selected_pos );
      highlighted_pos.local_z = selected_pos.local_z;
      highlighted_pos.level   = selected_pos.level;

    }

    e_GlyphMouseCheck( &glyph_index, &selected_glyph_x, &selected_glyph_y );
    e_ColorMouseCheck( &fg_index, &selected_fg_x, &selected_fg_y );

  }
  
  if ( app.mouse.button == 2 )
  {
    app.mouse.button = 0;
    if ( map != NULL )
    {
      if ( editor_mode == WEM_NONE )
      {
        switch ( selected_pos.level ) {
          case WORLD_LEVEL:
            map[selected_pos.world_index].tile.glyph = glyph_index;
            break;

          case REGION_LEVEL:
            map[selected_pos.world_index].regions[selected_pos.region_index].
              tile.glyph = glyph_index;
            break;

          case LOCAL_LEVEL:
            map[selected_pos.world_index].regions[selected_pos.region_index].
              tiles[selected_pos.local_index].glyph = glyph_index;
            break;
        }
      }
      
      if ( editor_mode == WEM_PASTE )
      {
        e_ChangeGameTile( map, selected_pos, clipboard );
        editor_mode = WEM_NONE;

      }

    }
  }
  
  if ( app.mouse.button == 3 )
  {
    app.mouse.button = 0;
    e_ColorMouseCheck( &bg_index, &selected_bg_x, &selected_bg_y );
    
    if ( map != NULL )
    {
      if ( editor_mode == WEM_MASS_CHANGE )
      {
        GameTileArray_t* temp = e_GetSelectGrid( map, selected_pos, highlighted_pos );
        e_ChangeGameTileGlyph( map, selected_pos, temp, glyph_index );
        editor_mode = WEM_NONE;
      }

      if ( editor_mode == WEM_COPY )
      {
        clipboard = e_GetSelectGrid( map, selected_pos, highlighted_pos );
        editor_mode = WEM_PASTE;
      }

    }

  }

  if ( app.keyboard[SDL_SCANCODE_S] == 1 )
  {
    app.keyboard[SDL_SCANCODE_S] = 0;

    editor_mode = WEM_SELECT;
  }
  
  if ( app.keyboard[SDL_SCANCODE_X] == 1 )
  {
    app.keyboard[SDL_SCANCODE_X] = 0;

    editor_mode = WEM_MASS_CHANGE;
  }

  if ( app.keyboard[SDL_SCANCODE_C] == 1 )
  {
    app.keyboard[SDL_SCANCODE_C] = 0;

    editor_mode = WEM_COPY;
  }
  
  if ( app.keyboard[SDL_SCANCODE_V] == 1 )
  {
    app.keyboard[SDL_SCANCODE_V] = 0;

    editor_mode = WEM_PASTE;
  }
  
  if ( app.keyboard[SDL_SCANCODE_F] == 1 )
  {
    app.keyboard[SDL_SCANCODE_F] = 0;

    if ( map != NULL )
    {
      GameTileArray_t* temp;

      switch ( editor_mode )
      {
      case WEM_NONE:
        switch ( selected_pos.level )
        {
        case WORLD_LEVEL:
          map[selected_pos.world_index].tile.fg = fg_index;
          break;

        case REGION_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
          tile.fg = fg_index;
          break;

        case LOCAL_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
          tiles[selected_pos.local_index].fg = fg_index;
          break;
        }
        break;

      case WEM_BRUSH:
        break;

      case WEM_MASS_CHANGE:
        temp = e_GetSelectGrid( map, selected_pos, highlighted_pos );
        e_ChangeGameTileColor( map, selected_pos, temp, fg_index, 1 );
        editor_mode = WEM_NONE;
        break;
      }
      
    }
  }
  
  if ( app.keyboard[SDL_SCANCODE_B] == 1 )
  {
    app.keyboard[SDL_SCANCODE_B] = 0;

    if ( map != NULL )
    {
      GameTileArray_t* temp;

      switch ( editor_mode )
      {
      case WEM_NONE:
        switch ( selected_pos.level ) {
        case WORLD_LEVEL:
          map[selected_pos.world_index].tile.bg = bg_index;
          break;

        case REGION_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
          tile.bg = bg_index;
          break;

        case LOCAL_LEVEL:
          map[selected_pos.world_index].regions[selected_pos.region_index].
          tiles[selected_pos.local_index].bg = bg_index;
          break;
        }
        break;

      case WEM_BRUSH:
        break;

      case WEM_MASS_CHANGE:
        temp = e_GetSelectGrid( map, selected_pos, highlighted_pos );
        e_ChangeGameTileColor( map, selected_pos, temp, bg_index, 0 );
        editor_mode = WEM_NONE;
        break;
      }

    }
  }
  
  e_LevelZHeightCheck( &selected_pos );
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    switch ( editor_mode )
    {
      case WEM_NONE:
        if ( clipboard != NULL )
        {
          free( clipboard );
        }
        free( pos_text );

        e_InitWorldEditor();
        break;

      case WEM_BRUSH:
        editor_mode = WEM_NONE;
        break;

      case WEM_COPY:
        editor_mode = WEM_NONE;
        break;

      case WEM_PASTE:
        editor_mode = WEM_NONE;
        break;

      case WEM_MASS_CHANGE:
        editor_mode = WEM_NONE;
        break;
      
      case WEM_SELECT:
        editor_mode = WEM_NONE;
        break;
    }
  }

  a_DoWidget();

}

static void we_EditDraw( float dt )
{
  if ( map != NULL )
  {
    int draw_size = 0;
    switch ( selected_pos.level )
    {
      case WORLD_LEVEL:
        draw_size = map->world_width * map->world_height;         
        break;

      case REGION_LEVEL:
        draw_size = map->region_width * map->region_height;
        break;

      case LOCAL_LEVEL:
        draw_size = map->local_width * map->local_height;                      
        break;
    }

    for ( uint16_t i = 0; i < draw_size; i++ )
    {
      we_DrawWorldCell( i, map, selected_pos, highlighted_pos );
    }

    if ( editor_mode == WEM_SELECT || editor_mode == WEM_COPY ||
      editor_mode == WEM_MASS_CHANGE )
    {
      e_DrawSelectGrid( map, selected_pos, highlighted_pos );
    }

  }
  
  a_DrawFilledRect( 1120, 95, 145, 442, 0, 0, 255, 255 ); //Glyph grid background
  
  int cx = 0, cy = 0;
  for ( int i = 0; i < MAX_COLOR_PALETTE; i++ )
  {

    if ( cx + GLYPH_WIDTH > ( 6 * GLYPH_WIDTH ) )
    {
      cx = 0;
      cy += GLYPH_HEIGHT + 1;
      if ( cy > ( 6 * GLYPH_HEIGHT ) )
      {
//        printf( "color grid is too large!\n" );
//        return;
      }

    }
    a_DrawFilledRect( cx + 1152, cy + 100, GLYPH_WIDTH, GLYPH_HEIGHT,
                      master_colors[APOLLO_PALETE][i].r,
                      master_colors[APOLLO_PALETE][i].g,
                      master_colors[APOLLO_PALETE][i].b,
                      master_colors[APOLLO_PALETE][i].a );

    if ( i == fg_index )
    {
      a_DrawRect( cx + 1152, cy + 100,
                  GLYPH_WIDTH, GLYPH_HEIGHT, 255, 0, 255, 255 );
    }
    
    if ( i == bg_index )
    {
      a_DrawRect( cx + 1152, cy + 100,
                  GLYPH_WIDTH, GLYPH_HEIGHT, 255, 255, 0, 255 );
    }
    
    cx += GLYPH_WIDTH;

  }
  
  int gx = 0, gy = 0;
  for ( int i = 0; i < game_glyphs->count; i++ )
  {
    if ( gx + GLYPH_WIDTH > ( 15 * GLYPH_WIDTH ) )
    {
      gx = 0;
      gy += GLYPH_HEIGHT + 1;
      if ( gy > ( 20 * GLYPH_HEIGHT ) )
      {
//        printf( "color grid is too large!\n" );
//        return;
      }

    }

    a_DrawFilledRect( gx + 1125,
                      gy + 245,
                      game_glyphs->rects[i].w,
                      game_glyphs->rects[i].h, 0, 0, 0, 255 );

    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[i],
                       gx + 1125,
                       gy + 245, 1, (aColor_t){.r = 255,
                      .g = 255, . b = 255, .a = 255 } );
    
    if ( i == glyph_index )
    {
      a_DrawRect( gx + 1125,
                  gy + 245,
                  game_glyphs->rects[i].w,
                  game_glyphs->rects[i].h, 255, 0, 255, 255 );
    }

    gx += GLYPH_WIDTH;
  }
  
  a_DrawFilledRect( 1125, 100, game_glyphs->rects[glyph_index].w * 2,
                    game_glyphs->rects[glyph_index].h * 2, 255, 0, 0, 255 );

  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[glyph_index],
                     1125, 100, 2, (aColor_t){.r = 255, .g = 255, .b = 255,
                                             .a = 255 } );

  a_DrawText( wem_strings[editor_mode], 1100, 10, 255, 255, 255, app.font_type,
             TEXT_ALIGN_CENTER, 0 );
  
  snprintf(pos_text, 50, "%d,%d,%d,%d,%d\n", selected_pos.world_index,
           selected_pos.region_index, selected_pos.local_index, selected_pos.level,
           selected_pos.local_z );
  
  a_DrawText( pos_text, 1170, 680, 255, 255, 255, app.font_type,
             TEXT_ALIGN_CENTER, 0 );

  a_DrawWidgets();

}

