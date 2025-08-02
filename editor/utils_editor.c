/*
 * world_editor/utils.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Archimedes.h"
#include "defs.h"
#include "editor.h"
#include "structs.h"

void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h )
{
  int row = ( index / height );
  int col = ( index % height );
  *x = ( ( SCREEN_WIDTH / 2 )  - ( ( width  * CELL_WIDTH ) / 2 ) )
    + ( row * CELL_WIDTH );
  *y = ( ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_HEIGHT ) / 2 ) )
    + ( col * CELL_HEIGHT );
  *w = CELL_WIDTH;
  *h = CELL_HEIGHT;
}

void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered )
{
  int edge_x = 0;
  int edge_y = 0;

  if ( centered == 1 )
  {
    edge_x = originx - ( ( width  * cell_width  ) / 2 );
    edge_y = originy - ( ( height * cell_height ) / 2 );

  }
  
  else
  {
    edge_x = originx;
    edge_y = originy;

  }

  if ( app.mouse.x > edge_x && app.mouse.x <= ( edge_x + ( width  * cell_width ) ) &&
       app.mouse.y > edge_y && app.mouse.y <= ( edge_y + ( height * cell_height ) ) )
  {
    int mousex = ( ( app.mouse.x - edge_x ) / cell_width  );
    int mousey = ( ( app.mouse.y - edge_y ) / cell_height );
    
    *grid_x = mousex;
    *grid_y = mousey;

  }

}

void e_DrawColorPalette( int originx, int originy, int fg_index, int bg_index )
{
  int cx = 0, cy = 0;
  for ( int i = 0; i < MAX_COLOR_PALETTE; i++ )
  {

    if ( cx + GLYPH_WIDTH > ( 6 * GLYPH_WIDTH ) )
    {
      cx = 0;
      cy += GLYPH_HEIGHT;
      if ( cy > ( 6 * GLYPH_HEIGHT ) )
      {
        //        printf( "color grid is too large!\n" );
        //        return;
      }

    }
    a_DrawFilledRect( cx + originx, cy + originy, GLYPH_WIDTH, GLYPH_HEIGHT,
                     master_colors[APOLLO_PALETE][i].r,
                     master_colors[APOLLO_PALETE][i].g,
                     master_colors[APOLLO_PALETE][i].b,
                     master_colors[APOLLO_PALETE][i].a );

    if ( i == fg_index )
    {
      a_DrawRect( cx + originx, cy + originy,
                 GLYPH_WIDTH, GLYPH_HEIGHT, 255, 0, 255, 255 );
    }

    if ( i == bg_index )
    {
      a_DrawRect( cx + originx, cy + originy,
                 GLYPH_WIDTH, GLYPH_HEIGHT, 255, 255, 0, 255 );
    }

    cx += GLYPH_WIDTH;
  }
}

void e_DrawGlyphPalette( int originx, int originy, int glyph_index )
{
  int gx = 0, gy = 0;
  int width = 16, height = 20;
  for ( int i = 0; i < game_glyphs->count; i++ )
  {
    if ( gx + GLYPH_WIDTH > ( width * GLYPH_WIDTH ) )
    {
      gx = 0;
      gy += GLYPH_HEIGHT;
      if ( gy > ( height * GLYPH_HEIGHT ) )
      {
        //        printf( "color grid is too large!\n" );
        //        return;
      }

    }

    a_DrawFilledRect( gx + originx, gy + originy,
                      game_glyphs->rects[i].w,
                      game_glyphs->rects[i].h, 0, 0, 0, 255 );

    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[i],
                       gx + originx, gy + originy, 1,
                       (aColor_t){.r = 255, .g = 255, . b = 255, .a = 255 } );

    if ( i == glyph_index )
    {
      a_DrawRect( gx + originx, gy + originy,
                 game_glyphs->rects[i].w,
                 game_glyphs->rects[i].h, 255, 0, 255, 255 );
    }

    gx += GLYPH_WIDTH;
  }
}

void e_GlyphMouseCheck( int originx, int originy, int* index, uint8_t* grid_x,
                        uint8_t* grid_y, int centered )
{

  int glyph_grid_w       = 16;
  int glyph_grid_h       = 16;

  e_GetCellAtMouse( glyph_grid_w, glyph_grid_h,
                    originx, originy,
                    GLYPH_WIDTH, GLYPH_HEIGHT,
                    grid_x, grid_y, centered );

  *index = INDEX_2( *grid_y, *grid_x, glyph_grid_w );

}

void e_ColorMouseCheck( int originx, int originy, int* index, uint8_t* grid_x,
                        uint8_t* grid_y, int centered )
{
  int color_grid_w       = 6;
  int color_grid_h       = 8;

  e_GetCellAtMouse( color_grid_w, color_grid_h,
                    originx, originy,
                    GLYPH_WIDTH, GLYPH_HEIGHT,
                    grid_x, grid_y, centered );

  *index = INDEX_2( *grid_y, *grid_x, color_grid_w );
}

void e_LoadColorPalette( aColor_t palette[MAX_COLOR_GROUPS][MAX_COLOR_PALETTE],
                       const char * filename )
{
  FILE* file;
  char line[8];
  unsigned int hex_value;
  uint8_t r, g, b;
  int i = 0;

  file = fopen( filename, "rb" );
  if ( file == NULL )
  {
    printf( "Failed to open file %s\n", filename );
    return;
  } 

  while( fgets( line, sizeof( line ), file ) != NULL )
  {
    hex_value = ( unsigned int )strtol( line, NULL, 16 );
    r = hex_value >> 16;
    g = hex_value >> 8;
    b = hex_value;
    
    if ( i >= 0 && i < MAX_COLOR_PALETTE )
    {
      palette[APOLLO_PALETE][i].r = r;
      palette[APOLLO_PALETE][i].g = g;
      palette[APOLLO_PALETE][i].b = b;
      palette[APOLLO_PALETE][i].a = 255;

    }

    i++;

  }

  fclose( file );

}

void e_DrawGlyphText( const char* message, int x, int y, int bg, int fg,
                      int align, int max_width )
{
  int message_size = strlen( message );

  int x1 = 0;
  
  for ( int i = 0; i < ( message_size + 1 ); i++ )
  {
    int index = message[i] - 1;
    
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[index],
                       100 + x1, 200, 1,
                       (aColor_t){.r = 255, .g = 255, . b = 255, .a = 255 } );
    x1 += 9;

  }
}

