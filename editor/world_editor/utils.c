/*
 * world_editor/utils.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "Archimedes.h"
#include "defs.h"
#include "editor.h"
#include "glyphs.h"
#include "save_editor.h"
#include "structs.h"
#include "world_editor.h"

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos, WorldPosition_t highlight )
{
  int x, y, w, h;
  uint32_t i = 0;
  uint16_t current_index = 0;
  uint16_t highlight_index = 0;
  int current_glyph = 0;
  int current_bg = 0;
  int current_fg = 0;

  switch ( pos.level ) {
    case WORLD_LEVEL:
      e_GetCellSize( index, map->world_width, map->world_height,
                     &x, &y, &w, &h );

      i = index;

      current_index   = pos.world_index;
      highlight_index = highlight.world_index;
      current_glyph   = map[i].tile.glyph;
      current_bg      = map[i].tile.bg;
      current_fg      = map[i].tile.fg;
      break;

    case REGION_LEVEL:
      e_GetCellSize( index, map->region_width, map->region_height,
                     &x, &y, &w, &h );

      i = index;

      current_index   = pos.region_index;
      highlight_index = highlight.region_index;
      current_glyph   = map[pos.world_index].regions[i].tile.glyph;
      current_bg      = map[pos.world_index].regions[i].tile.bg;
      current_fg      = map[pos.world_index].regions[i].tile.fg;
      break;

    case LOCAL_LEVEL:
      e_GetCellSize( index, map->local_width, map->local_height,
                     &x, &y, &w, &h );

      i = ( ( pos.local_z * ( map->local_width * map->local_height ) 
        + index ) );

      current_index   = pos.local_index;
      highlight_index = highlight.local_index;
      current_glyph   = map[pos.world_index].regions[pos.region_index].
        tiles[i].glyph;
      current_bg      = map[pos.world_index].regions[pos.region_index].tiles[i]
        .bg;
      current_fg      = map[pos.world_index].regions[pos.region_index].tiles[i]
        .fg;

      break;

    default:
      break;
  }

  if ( i == current_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2, 
                      255, 255, 0, 255 );

    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2, master_colors[APOLLO_PALETE][current_fg] );
  } 

  else
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2,
                      master_colors[APOLLO_PALETE][current_bg].r, 
                      master_colors[APOLLO_PALETE][current_bg].g,
                      master_colors[APOLLO_PALETE][current_bg].b, 255 );
    
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2, master_colors[APOLLO_PALETE][current_fg] );

  }
  
  if( i == highlight_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                     game_glyphs->rects[current_glyph].h * 2, 
                     255, 0, 255, 255 );

    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2, master_colors[APOLLO_PALETE][current_fg] );

  }

}

void e_DrawSelectGrid( World_t* map, WorldPosition_t pos,
                                   WorldPosition_t highlight )
{
  int grid_w    = ( highlight.x - pos.x );
  int grid_h    = ( highlight.y - pos.y );
  int current_x = pos.x, current_y  = pos.y;
  int current_z = pos.local_z;

  if ( grid_w < 0 )
  {
    grid_w = ( pos.x - highlight.x );
    current_x = highlight.x;

  }

  if ( grid_h < 0 )
  {
    grid_h = ( pos.y - highlight.y );
    current_y = highlight.y;

  }

  int x, y, w, h;

  int current_index = 0;
  int current_glyph = 0;
  int current_width = 0, current_height = 0;
  int current_fg    = 0;

  switch ( pos.level )
  {
    case WORLD_LEVEL:
      current_width  = map->world_width;
      current_height = map->world_height;
      break;
    
    case REGION_LEVEL:
      current_width  = map->region_width;
      current_height = map->region_height;
      break;
    
    case LOCAL_LEVEL:
      current_width  = map->local_width;
      current_height = map->local_height;
      break;

    default:
      break;
  }
  
  int k = 0;

  for ( int i = 0; i < grid_w + 1; i++ )
  {
    for ( int j = 0; j < grid_h + 1; j++ )
    {
      switch ( pos.level )
      {
        case WORLD_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  current_height );

          current_glyph = map[current_index].tile.glyph;
          current_fg = map[current_index].tile.fg;
          
          break;

        case REGION_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  current_height );

          current_glyph = map[pos.world_index].regions[current_index].
            tile.glyph;
          
          current_fg = map[pos.world_index].regions[current_index].
            tile.fg;
          

          break;

        case LOCAL_LEVEL:
          current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     current_z, current_width, current_height );
          
          current_fg = map[pos.world_index].regions[pos.region_index].
            tiles[current_index].fg;

          current_glyph = map[pos.world_index].regions[pos.region_index].
            tiles[current_index].glyph;

          current_index -= ( map->local_width * map->local_height * pos.local_z );

          break;

        default:
          break;
      }

      e_GetCellSize( current_index, current_width, current_height,
                     &x, &y, &w, &h );

      a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                       game_glyphs->rects[current_glyph].h * 2, 
                       255, 0, 255, 255 );

      a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                        x, y, 2, master_colors[APOLLO_PALETE][current_fg] );
      
    }
  }

}

GameTileArray_t* e_GetSelectGrid( World_t* map, WorldPosition_t pos,
                                   WorldPosition_t highlight )
{
  int grid_w    = ( highlight.x - pos.x );
  int grid_h    = ( highlight.y - pos.y );
  int current_x = pos.x, current_y  = pos.y;
  int current_z = pos.local_z;

  if ( grid_w < 0 )
  {
    grid_w = ( pos.x - highlight.x );
    current_x = highlight.x;

  }

  if ( grid_h < 0 )
  {
    grid_h = ( pos.y - highlight.y );
    current_y = highlight.y;

  }

  GameTileArray_t* new_game_tile_array = ( GameTileArray_t* )malloc(
    sizeof( GameTileArray_t ) );

  if ( new_game_tile_array == NULL )
  {
    printf( "Failed to allocate memory for new_game_tile_array\n" );
    return NULL;
  }
  
  new_game_tile_array->w            = grid_w + 1;
  new_game_tile_array->h            = grid_h + 1;
  new_game_tile_array->level        = pos.level;
  new_game_tile_array->world_index  = pos.world_index;
  new_game_tile_array->region_index = pos.region_index;
  new_game_tile_array->local_index  = pos.local_index;

  new_game_tile_array->data = ( int* )malloc( sizeof( int ) *
                                            ( grid_w + 1 ) * ( grid_h + 1 ) *
                                              map->z_height );

  int current_index = 0;
  
  int k = 0;

  for ( int i = 0; i < grid_w + 1; i++ )
  {
    for ( int j = 0; j < grid_h + 1; j++ )
    {
      switch ( pos.level )
      {
        case WORLD_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->world_height );

          new_game_tile_array->data[k++] = current_index;
          break;

        case REGION_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->region_height );

          new_game_tile_array->data[k++] = current_index;

          break;

        case LOCAL_LEVEL:
          current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     current_z, map->local_width,
                                     map->local_height );
          
          new_game_tile_array->data[k++] = current_index;
          break;
      }
    }
  }

  new_game_tile_array->count = k;

  return new_game_tile_array;
}

void e_ChangeGameTile( World_t* map, WorldPosition_t pos,
                            GameTileArray_t* tile_array, int glyph_index,
                            int bg_index, int fg_index )
{
  int index = 0;

  for ( int i = 0; i < tile_array->count; i++ )
  {
    index = tile_array->data[i];
    switch (pos.level)
    {
      case WORLD_LEVEL:
        map[index].tile.glyph = glyph_index;
       
        map[index].tile.bg    = bg_index;
       
        map[index].tile.fg    = fg_index;
        break;

      case REGION_LEVEL:
        map[pos.world_index].regions[index].tile.glyph = glyph_index;
        
        map[pos.world_index].regions[index].tile.bg    = bg_index;
        
        map[pos.world_index].regions[index].tile.fg    = fg_index;
        break;

      case LOCAL_LEVEL:
        map[pos.world_index].regions[pos.region_index].
          tiles[index].glyph = glyph_index;
        
        map[pos.world_index].regions[pos.region_index].
          tiles[index].bg = bg_index;
        
        map[pos.world_index].regions[pos.region_index].
          tiles[index].fg = fg_index;

        break;
    }
  }

  free( tile_array );

}

void e_PasteGameTile( World_t* map, WorldPosition_t pos,
                       GameTileArray_t* tile_array )
{
  int index = 0;
  int current_index = 0;
  int current_x = pos.x;
  int current_y = pos.y;
  int k = 0;
  
  for ( int i = 0; i < tile_array->w; i++ )
  {
    for ( int j = 0; j < tile_array->h; j++ )
    {
      index = tile_array->data[k];
      switch (pos.level)
      {
        case WORLD_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->world_height );

          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              map[current_index].tile = map[index].tile;
              break;

            case REGION_LEVEL:
              map[current_index].tile = 
                map[tile_array->world_index].regions[index].tile;
              break;

            case LOCAL_LEVEL:
              map[current_index].tile = map[tile_array->world_index].
                regions[tile_array->region_index].tiles[index];
              break;
          }
          break;

        case REGION_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->region_height );
          
          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              map[pos.world_index].regions[current_index].tile = 
                map[index].tile;
              break;

            case REGION_LEVEL:
              map[pos.world_index].regions[current_index].tile = 
                map[tile_array->world_index].regions[index].tile;
              break;

            case LOCAL_LEVEL:
              map[pos.world_index].regions[current_index].tile =
                map[tile_array->world_index].regions[tile_array->region_index].tiles[index];
              break;
          }
          break;

        case LOCAL_LEVEL:
          current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     pos.local_z, map->local_width,
                                     map->local_height );
          
          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              map[pos.world_index].regions[pos.region_index]
                .tiles[current_index] = map[index].tile;
              break;

            case REGION_LEVEL:
              map[pos.world_index].regions[pos.region_index]
                .tiles[current_index] = map[pos.world_index]
                .regions[index].tile;
              break;

            case LOCAL_LEVEL:
              map[pos.world_index].regions[pos.region_index]
                .tiles[current_index] = map[pos.world_index]
                .regions[pos.region_index].tiles[index];
              break;
          }
          break;
      }
      
      if ( k < tile_array->count )
      {
        k++;
      }
    }

  }

}

void e_DrawPastePreview( World_t* map, WorldPosition_t pos,
                       GameTileArray_t* tile_array )
{
  int index = 0;
  int current_index = 0;
  int current_x = pos.x;
  int current_y = pos.y;
  int x = 0, y = 0, w = 0, h = 0;
  int current_width = 0, current_height = 0;
  GameTile_t current_tile = {0};
  int k = 0;
  
  for ( int i = 0; i < tile_array->w; i++ )
  {
    for ( int j = 0; j < tile_array->h; j++ )
    {
      index = tile_array->data[k];
      switch (pos.level)
      {
        case WORLD_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->world_height );
          
          current_width  = map->world_width;
          current_height = map->world_height;

          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              current_tile = map[index].tile;
              break;

            case REGION_LEVEL:
              current_tile = 
                map[tile_array->world_index].regions[index].tile;
              break;

            case LOCAL_LEVEL:
              current_tile = map[tile_array->world_index].
                regions[tile_array->region_index].tiles[index];
              break;
          }
          break;

        case REGION_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->region_height );
          
          current_width  = map->region_width;
          current_height = map->region_height;

          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              current_tile = 
                map[index].tile;
              break;

            case REGION_LEVEL:
              current_tile =
                map[tile_array->world_index].regions[index].tile;
              break;

            case LOCAL_LEVEL:
              current_tile =
                map[tile_array->world_index].regions[tile_array->region_index].tiles[index];
              break;
          }
          break;

        case LOCAL_LEVEL:
          current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     pos.local_z, map->local_width,
                                     map->local_height );
          
          current_index -= ( map->local_width * map->local_height * pos.local_z );
          
          current_width  = map->local_width;
          current_height = map->local_height;
          
          switch ( tile_array->level ) 
          {
            case WORLD_LEVEL:
              current_tile = map[index].tile;
              break;

            case REGION_LEVEL:
              current_tile = map[tile_array->world_index]
                .regions[index].tile;
              break;

            case LOCAL_LEVEL:
              current_tile = map[tile_array->world_index]
                .regions[tile_array->region_index].tiles[index];
              break;
          }
          break;
      }
      
      e_GetCellSize( current_index, current_width, current_height,
                     &x, &y, &w, &h );

      a_DrawFilledRect( x, y, game_glyphs->rects[current_tile.glyph].w * 2,
                       game_glyphs->rects[current_tile.glyph].h * 2, 
                       255, 0, 255, 255 );

      a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_tile.glyph],
                        x, y, 2, master_colors[APOLLO_PALETE][current_tile.fg] );
      
      
      if ( k < tile_array->count )
      {
        k++;
      }
    }

  }

}

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

  if ( app.mouse.x > edge_x && app.mouse.x <= edge_x + ( width * cell_width ) &&
       app.mouse.y > edge_y && app.mouse.y <= edge_y + ( height* cell_height ) )
  {
    int mousex = ( ( app.mouse.x - edge_x ) / cell_width  );
    int mousey = ( ( app.mouse.y - edge_y ) / cell_height );
    *grid_x = mousex;
    *grid_y = mousey;

  }

}

void e_MapMouseCheck( WorldPosition_t* pos )
{
  switch (pos->level) {
    case WORLD_LEVEL: 
      e_GetCellAtMouse( map->world_width, map->world_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->world_index = INDEX_2( pos->x, pos->y, map->world_height );
      break;

    case REGION_LEVEL:
      e_GetCellAtMouse( map->region_width, map->region_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->region_index = INDEX_2( pos->x, pos->y,
                                         map->region_height );
      break;

    case LOCAL_LEVEL:
      e_GetCellAtMouse( map->local_width, map->local_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->local_index = INDEX_3( pos->y, pos->x,
                                  pos->local_z, map->local_width,
                                  map->local_height );
      break;
  }
} 

void e_GlyphMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y )
{

  e_GetCellAtMouse( 17, 17, 1125, 245, GLYPH_WIDTH, GLYPH_HEIGHT,
                    grid_x, grid_y, 0 );

  *index = INDEX_2( *grid_y, *grid_x, 16 );

}

void e_ColorMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y )
{

  e_GetCellAtMouse( 7, 9, 1152, 100, GLYPH_WIDTH, GLYPH_HEIGHT,
                    grid_x, grid_y, 0 );

  *index = INDEX_2( *grid_y, *grid_x, 6 );

}

void e_LevelZHeightCheck( WorldPosition_t* pos )
{
  if ( app.keyboard[SDL_SCANCODE_RETURN] == 1 )
  {
    app.keyboard[SDL_SCANCODE_RETURN] = 0;

    if ( pos->level >= WORLD_LEVEL && pos->level < LOCAL_LEVEL )
    {
      if ( map[pos->world_index].regions == NULL )
      {
        printf("change level\n");
        LoadPartialRegion( pos, map, "resources/world/map.dat" );

      }
      pos->level++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_BACKSPACE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

    if ( pos->level > WORLD_LEVEL && pos->level <= LOCAL_LEVEL )
    {
      pos->level--;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_EQUALS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_EQUALS] = 0;
    if ( pos->local_z >= 0 && pos->local_z < map->z_height - 1 )
    {
      pos->local_z++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_MINUS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_MINUS] = 0;
    if ( pos->local_z > 0 && pos->local_z <= map->z_height )
    {
      pos->local_z--;
    }

  }

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

void we_DrawEditorHotKeys( int x, int y, int key, int abbv0, int abbv1,
                           int abbv2, int abbv3 )
{
  a_DrawFilledRect( x, y, GLYPH_WIDTH * 6, GLYPH_HEIGHT,
                   master_colors[APOLLO_PALETE][36].r,
                   master_colors[APOLLO_PALETE][36].g,
                   master_colors[APOLLO_PALETE][36].b,
                   master_colors[APOLLO_PALETE][36].a );

  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[key],
                    x, y, 1, master_colors[APOLLO_PALETE][10] );
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[GLYPH_COLON],
                    x + 9, y, 1, master_colors[APOLLO_PALETE][10] );
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[abbv0],
                    x + 18, y, 1, master_colors[APOLLO_PALETE][10] );
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[abbv1],
                    x + 27, y, 1, master_colors[APOLLO_PALETE][10] );
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[abbv2],
                    x + 36, y, 1, master_colors[APOLLO_PALETE][10] );
  a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[abbv3],
                    x + 42, y, 1, master_colors[APOLLO_PALETE][10] );

}

