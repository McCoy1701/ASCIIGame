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

static void GetSelectGridSize( WorldPosition_t pos, WorldPosition_t highlight,
                               int* grid_w, int* grid_h, int* current_x,
                               int* current_y, int* current_z );

static GameTile_t GetTileAtPos( World_t* world, WorldPosition_t pos, int i, int j,
                                int current_x, int current_y, int current_z,
                                int* current_index, int* current_width,
                                int* current_height );

void we_DrawWorld( World_t* world, WorldPosition_t selected_pos,
                   WorldPosition_t highlighted_pos )
{
  int draw_size = 0;
  int x = 0, y = 0, w = 0, h = 0;
  uint8_t selected_world_index = 0;
  uint8_t hightlight_world_index = 0;
  uint16_t selected_index = 0;
  uint16_t highlight_index = 0;
  GameTile_t current_tile = {0};

  switch ( selected_pos.level )
  {
    case REALM_LEVEL:
      for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
      {
        int realm_x = 0, realm_y = 0;
        realm_x = i / WORLD_HEIGHT; 
        realm_y = i % WORLD_HEIGHT;

        for( int j = 0; j < ( world->realm_width * world->realm_height ); j++ )
        {
          e_GetCellSize( j, world->realm_width, world->realm_height,
                        &x, &y, &w, &h );

          selected_index         = selected_pos.realm_index;
          selected_world_index   = selected_pos.world_index;
          highlight_index        = highlighted_pos.realm_index;
          hightlight_world_index = highlighted_pos.world_index;
          current_tile           = world[i].realms[j].tile;

          we_DrawWorldCell( j, i, &current_tile,
                              selected_index, highlight_index,
                              selected_world_index, hightlight_world_index,
                         ( x + ( realm_x * world->realm_width * CELL_WIDTH ) -
                         ( world->realm_width * CELL_WIDTH ) ),
                         ( y + ( realm_y * world->realm_height * CELL_HEIGHT) -
                         ( world->realm_height * CELL_HEIGHT ) ), w, h );
        }

      }
      break;

    case REGION_LEVEL:
      draw_size = world->region_width * world->region_height;
      for ( uint8_t i = 0; i < draw_size; i++ )
      {
        e_GetCellSize( i, world->region_width, world->region_height,
                      &x, &y, &w, &h );

        selected_index         = selected_pos.region_index;
        highlight_index        = highlighted_pos.region_index;
        selected_world_index   = selected_pos.world_index;
        hightlight_world_index = highlighted_pos.world_index;

        current_tile   = world[selected_pos.world_index].
          realms[selected_pos.realm_index].regions[i].tile;

        we_DrawWorldCell( i, selected_pos.world_index, 
                            &current_tile,
                            selected_index, highlight_index,
                            selected_world_index, hightlight_world_index, 
                            x, y, w, h );
      }
      break;

    case LOCAL_LEVEL:
      for ( uint16_t i = 0; i < world->local_width * world->local_height; i++ )
      {
        e_GetCellSize( i, world->local_width, world->local_height,
                      &x, &y, &w, &h );

        int index = ( ( selected_pos.local_z * 
          ( world->local_width * world->local_height ) + i ) );

        selected_index         = selected_pos.local_index;
        highlight_index        = highlighted_pos.local_index;
        selected_world_index   = selected_pos.world_index;
        hightlight_world_index = highlighted_pos.world_index;

        current_tile = world[selected_pos.world_index].
          realms[selected_pos.realm_index].regions[selected_pos.region_index].
          tiles[i];

        we_DrawWorldCell( index, selected_pos.world_index,
                            &current_tile,
                            selected_index, highlight_index,
                            selected_world_index, hightlight_world_index,
                            x, y, w, h );
      }
      break;
  }

}

void we_DrawWorldCell( int i, int j, GameTile_t* current_tile,
                       int selected_index, int highlight_index,
                       int selected_world_index, int highlight_world_index,
                       int x, int y, int w, int h )
{
  if ( i == selected_index && j == selected_world_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_tile->glyph].w * 2,
                      game_glyphs->rects[current_tile->glyph].h * 2, 
                      255, 255, 0, 255 );

    a_BlitTextureRect( game_glyphs->texture, 
                       game_glyphs->rects[current_tile->glyph],
                    x, y, 2, master_colors[APOLLO_PALETE][current_tile->fg] );
  } 

  else
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_tile->glyph].w * 2,
                     game_glyphs->rects[current_tile->glyph].h * 2,
                     master_colors[APOLLO_PALETE][current_tile->bg].r, 
                     master_colors[APOLLO_PALETE][current_tile->bg].g,
                     master_colors[APOLLO_PALETE][current_tile->bg].b, 255 );

    a_BlitTextureRect( game_glyphs->texture,
                       game_glyphs->rects[current_tile->glyph],
                    x, y, 2, master_colors[APOLLO_PALETE][current_tile->fg] );

  }
  
  if( i == highlight_index && j == highlight_world_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_tile->glyph].w * 2,
                     game_glyphs->rects[current_tile->glyph].h * 2, 
                     255, 0, 255, 255 );

    a_BlitTextureRect( game_glyphs->texture,
                       game_glyphs->rects[current_tile->glyph],
                    x, y, 2, master_colors[APOLLO_PALETE][current_tile->fg] );

  }

}

void e_DrawSelectGrid( World_t* world, WorldPosition_t pos,
                                   WorldPosition_t highlight )
{
  int grid_w = 0, grid_h = 0;
  int current_x = 0, current_y = 0, current_z = 0;

  GetSelectGridSize( pos, highlight, &grid_w, &grid_h,
                     &current_x, &current_y, &current_z );

  int x, y, w, h;

  GameTile_t current_tile = {0};
  int current_index = 0, current_width = 0, current_height = 0;
  
  for ( int i = 0; i < grid_w + 1; i++ )
  {
    for ( int j = 0; j < grid_h + 1; j++ )
    {
      current_tile = GetTileAtPos( world, pos, i, j,
                                   current_x, current_y, current_z,
                                   &current_index, &current_width,
                                   &current_height );
      
      if ( pos.level == REALM_LEVEL )
      {
        int world_x = ( current_x + i ) / world->realm_width;
        int world_y = ( current_y + j ) / world->realm_height;
        int realm_x = ( current_x + i ) % world->realm_width;
        int realm_y = ( current_y + j ) % world->realm_height;
        current_width = world->realm_width;
        current_height = world->realm_height;

        x = ( ( SCREEN_WIDTH / 2 )  - ( ( world->realm_width  * CELL_WIDTH ) / 2 ) )
          + ( realm_x * CELL_WIDTH );
        y = ( ( SCREEN_HEIGHT / 2 ) - ( ( world->realm_height * CELL_HEIGHT ) / 2 ) )
          + ( realm_y * CELL_HEIGHT );
  
        x = ( x + ( world_x * world->realm_width * CELL_WIDTH ) -
          ( world->realm_width * CELL_WIDTH ) );

        y = ( y + ( world_y * world->realm_height * CELL_HEIGHT ) -
          ( world->realm_height * CELL_HEIGHT ) );
      }
      
      else
      {
        e_GetCellSize( current_index, current_width, current_height,
                      &x, &y, &w, &h );
      }
      
      a_DrawFilledRect( x, y, game_glyphs->rects[current_tile.glyph].w * 2,
                       game_glyphs->rects[current_tile.glyph].h * 2, 
                       255, 0, 255, 255 );

      a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_tile.glyph],
                        x, y, 2, master_colors[APOLLO_PALETE][current_tile.fg] );
      
    }
  }
}

GameTileArray_t* e_GetSelectGrid( World_t* map, WorldPosition_t pos,
                                   WorldPosition_t highlight )
{
  int grid_w    = 0;
  int grid_h    = 0;
  int current_x = 0, current_y = 0, current_z = 0;
  
  GetSelectGridSize( pos, highlight, &grid_w, &grid_h,
                     &current_x, &current_y, &current_z );
  
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

  new_game_tile_array->data = ( GameTile_t* )malloc( sizeof( GameTile_t ) *
                                            ( grid_w + 1 ) * ( grid_h + 1 ) *
                                              map->z_height );
  GameTile_t current_tile = {0};
  int current_index = 0, current_width = 0, current_height = 0;
  int k = 0;

  for ( int i = 0; i < grid_w + 1; i++ )
  {
    for ( int j = 0; j < grid_h + 1; j++ )
    {
      current_tile = GetTileAtPos( map, pos, i, j,
                                   current_x, current_y, current_z,
                                   &current_index, &current_width,
                                   &current_height );

      new_game_tile_array->data[k++] = current_tile;
    }
  }

  new_game_tile_array->count = k;

  return new_game_tile_array;
}

void e_ChangeGameTile( World_t* map, WorldPosition_t pos,
                            GameTileArray_t* tile_array, int glyph_index,
                            int bg_index, int fg_index )
{
  GameTile_t index = {0};

  for ( int i = 0; i < tile_array->count; i++ )
  {
    index = tile_array->data[i];
    index.glyph = glyph_index;
    index.bg    = bg_index;
    index.fg    = fg_index;
  }

  free( tile_array );

}

void e_PasteGameTile( World_t* world, WorldPosition_t pos,
                       GameTileArray_t* tile_array )
{
  GameTile_t index  = {0};
  int current_index = 0;
  int current_world_index = 0;
  int world_x = 0, world_y = 0;
  int new_x = 0, new_y = 0;
  int k = 0;
 
  int pos_world_x = pos.world_index / WORLD_HEIGHT;
  int pos_world_y = pos.world_index % WORLD_HEIGHT;
  int global_pos_x = ( pos_world_x * map->realm_width )  + pos.x;
  int global_pos_y = ( pos_world_y * map->realm_height ) + pos.y;
  int current_x = global_pos_x, current_y = global_pos_y;
  
  for ( int i = 0; i < tile_array->w; i++ )
  {
    for ( int j = 0; j < tile_array->h; j++ )
    {
      index = tile_array->data[k];
      switch (pos.level)
      {
        case REALM_LEVEL:
          world_x = ( current_x + i ) / world->realm_width;
          world_y = ( current_y + j ) / world->realm_height;
          new_x   = ( current_x + i ) % world->realm_width;
          new_y   = ( current_y + j ) % world->realm_height;

          current_world_index = INDEX_2( world_x, world_y, WORLD_HEIGHT );

          current_index = INDEX_2( new_x, new_y, world->realm_height );

          world[current_world_index].realms[current_index].tile = index;
          break;

        case REGION_LEVEL:
          current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->region_height );

          map[pos.world_index].realms[pos.realm_index].regions[current_index].
            tile = index;
          break;
          

        case LOCAL_LEVEL:
          current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     pos.local_z, map->local_width,
                                     map->local_height );
          
          map[pos.world_index].realms[pos.realm_index].
            regions[pos.region_index].tiles[current_index] = index;

          break;
      }
      
      if ( k < tile_array->count )
      {
        k++;
      }
    }

  }

}

void e_DrawPastePreview( World_t* world, WorldPosition_t pos,
                       GameTileArray_t* tile_array )
{
  GameTile_t index  = {0};
  int current_index = 0;
  int x = 0, y = 0, w = 0, h = 0;
  int current_width = 0, current_height = 0;
  GameTile_t current_tile = {0};
  int world_x = 0, world_y = 0;
  int realm_x = 0, realm_y = 0;
  int k = 0;
  
  int pos_world_x = pos.world_index / WORLD_HEIGHT;
  int pos_world_y = pos.world_index % WORLD_HEIGHT;
  int global_pos_x = ( pos_world_x * map->realm_width )  + pos.x;
  int global_pos_y = ( pos_world_y * map->realm_height ) + pos.y;
  int current_x = global_pos_x, current_y = global_pos_y;
  
  for ( int i = 0; i < tile_array->w; i++ )
  {
    for ( int j = 0; j < tile_array->h; j++ )
    {
      index = tile_array->data[k];
      switch (pos.level)
      {
        case REALM_LEVEL:
          world_x = ( current_x + i ) / world->realm_width;
          world_y = ( current_y + j ) / world->realm_height;
          realm_x = ( current_x + i ) % world->realm_width;
          realm_y = ( current_y + j ) % world->realm_height;

          x = ( ( SCREEN_WIDTH / 2 )  - ( ( world->realm_width  * CELL_WIDTH ) / 2 ) )
            + ( realm_x * CELL_WIDTH );
          y = ( ( SCREEN_HEIGHT / 2 ) - ( ( world->realm_height * CELL_HEIGHT ) / 2 ) )
            + ( realm_y * CELL_HEIGHT );

          x = ( x + ( world_x * world->realm_width * CELL_WIDTH ) -
            ( world->realm_width * CELL_WIDTH ) );

          y = ( y + ( world_y * world->realm_height * CELL_HEIGHT ) -
            ( world->realm_height * CELL_HEIGHT ) );
          
          current_width  = world->realm_width;
          current_height = world->realm_height;
          current_tile   = index;

          break;

        case REGION_LEVEL:
          current_index  = INDEX_2( ( current_x + i ), ( current_y + j ),
                                  map->region_height );
          
          current_width  = map->region_width;
          current_height = map->region_height;
          current_tile   = index;
          
          e_GetCellSize( current_index, current_width, current_height,
                        &x, &y, &w, &h );

          break;

        case LOCAL_LEVEL:
          current_index  = INDEX_3( ( current_y + j ), ( current_x + i ),
                                     pos.local_z, map->local_width,
                                     map->local_height );
          
          current_index -= ( map->local_width * map->local_height * pos.local_z );
          
          current_width  = map->local_width;
          current_height = map->local_height;
          current_tile   = index;
          
          e_GetCellSize( current_index, current_width, current_height,
                        &x, &y, &w, &h );
          
          break;
      }
      

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

  if ( app.mouse.x > edge_x && app.mouse.x <= ( edge_x + ( width  * cell_width ) ) &&
       app.mouse.y > edge_y && app.mouse.y <= ( edge_y + ( height * cell_height ) ) )
  {
    int mousex = ( ( app.mouse.x - edge_x ) / cell_width  );
    int mousey = ( ( app.mouse.y - edge_y ) / cell_height );
    
    *grid_x = mousex;
    *grid_y = mousey;

  }

}

void e_MapMouseCheck( WorldPosition_t* pos )
{
  int originx = 0;
  int originy = 0;
  uint8_t world_x = pos->world_index / WORLD_HEIGHT, world_y = pos->world_index % WORLD_HEIGHT;
  uint8_t realm_x = 0, realm_y = 0;
  
  switch ( pos->level ) {
    case REALM_LEVEL:
      originx = ( SCREEN_ORIGIN_X ) - ( ( map->realm_width 
        * CELL_WIDTH ) / 2 ) - ( map->realm_width * CELL_WIDTH );

      originy = ( SCREEN_ORIGIN_Y ) - ( ( map->realm_height 
        * CELL_HEIGHT ) / 2 ) - ( map->realm_height * CELL_HEIGHT );

      e_GetCellAtMouse( WORLD_WIDTH, WORLD_HEIGHT, originx, originy,
                      ( map->realm_width * CELL_WIDTH ), 
                      ( map->realm_height * CELL_HEIGHT ),
                        &world_x, &world_y, 0 );
      
      pos->world_index = INDEX_2( world_x, world_y, WORLD_HEIGHT );
 
      if ( app.mouse.x > originx && app.mouse.x <= originx + 
           ( WORLD_WIDTH * map->realm_width * CELL_WIDTH ) && 
           app.mouse.y > originy && app.mouse.y <= originy + 
           ( WORLD_HEIGHT * map->realm_height * CELL_HEIGHT ) )
      {
        int mousex = ( ( app.mouse.x - ( originx + ( world_x * map->realm_width
          * CELL_WIDTH ) ) ) / CELL_WIDTH );

        int mousey = ( ( app.mouse.y - ( originy + ( world_y * map->realm_height
          * CELL_HEIGHT ) ) ) / CELL_HEIGHT );

        pos->x = ( mousex < 0 ) ? 0 : ( mousex > ( map->realm_width )
          ? ( map->realm_width  - 1 ) : mousex );

        pos->y = ( mousey < 0 ) ? 0 : ( mousey > ( map->realm_height )
          ? ( map->realm_height - 1 ) : mousey );

        pos->realm_index = INDEX_2( pos->x, pos->y, map->realm_height );
      }

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

    if ( pos->level >= REALM_LEVEL && pos->level < LOCAL_LEVEL )
    {
      if ( map[pos->world_index].realms[pos->realm_index].regions == NULL )
      {
        printf("change level\n");
        //LoadPartialRegion( pos, map, "resources/world/map.dat" );

      }
      pos->level++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_BACKSPACE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

    if ( pos->level > REALM_LEVEL && pos->level <= LOCAL_LEVEL )
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

static void GetSelectGridSize( WorldPosition_t pos, WorldPosition_t highlight,
                               int* grid_w, int* grid_h, int* current_x,
                               int* current_y, int* current_z )
{
  if ( pos.level == REALM_LEVEL )
  {
    int pos_world_x = pos.world_index / WORLD_HEIGHT;
    int pos_world_y = pos.world_index % WORLD_HEIGHT;
    int global_pos_x = ( pos_world_x * map->realm_width )  + pos.x;
    int global_pos_y = ( pos_world_y * map->realm_height ) + pos.y;
    int highlight_world_x = highlight.world_index / WORLD_HEIGHT;
    int highlight_world_y = highlight.world_index % WORLD_HEIGHT;
    int global_highlight_x = ( highlight_world_x * map->realm_width )  + highlight.x;
    int global_highlight_y = ( highlight_world_y * map->realm_height ) + highlight.y;

    *current_x = global_pos_x;
    *current_y = global_pos_y;
    *current_z = pos.local_z;
    
    *grid_w = ( global_highlight_x - global_pos_x );
    *grid_h = ( global_highlight_y - global_pos_y );

    if ( *grid_w < 0 )
    {
      *grid_w = ( global_pos_x - global_highlight_x );
      *current_x = global_highlight_x;
    }

    if ( *grid_h < 0 )
    {
      *grid_h = ( global_pos_y - global_highlight_y );
      *current_y = global_highlight_y;
    }

  }
  
  else
  {
    *grid_w    = ( highlight.x - pos.x );
    *grid_h    = ( highlight.y - pos.y );
    *current_x = pos.x;
    *current_y = pos.y;
    *current_z = pos.local_z;

    if ( *grid_w < 0 )
    {
      *grid_w = ( pos.x - highlight.x );
      *current_x = highlight.x;

    }

    if ( *grid_h < 0 )
    {
      *grid_h = ( pos.y - highlight.y );
      *current_y = highlight.y;

    }
  }

}

static GameTile_t GetTileAtPos( World_t* world, WorldPosition_t pos, 
                                int i, int j, int current_x, int current_y,
                                int current_z, int* current_index,
                                int* current_width, int* current_height )
{
  int current_world_index = 0;
  int new_x = 0;
  int new_y = 0;
  uint8_t world_x = 0;
  uint8_t world_y = 0;

  GameTile_t return_tile;
  
  switch ( pos.level )
  {
    case REALM_LEVEL:
      *current_width = world->realm_width;
      *current_height = world->realm_height;
      world_x = ( current_x + i ) / world->realm_width;
      world_y = ( current_y + j ) / world->realm_height;
      new_x   = ( current_x + i ) % world->realm_width;
      new_y   = ( current_y + j ) % world->realm_height;

      current_world_index = INDEX_2( world_x, world_y, WORLD_HEIGHT );

      *current_index = INDEX_2( new_x, new_y, world->realm_height );

      return_tile = world[current_world_index].
        realms[*current_index].tile;

      break;

    case REGION_LEVEL:
      *current_width  = map->region_width;
      *current_height = map->region_height;
      *current_index = INDEX_2( ( current_x + i ), ( current_y + j ),
                              *current_height );

      return_tile = map[pos.world_index].realms[pos.realm_index].
        regions[*current_index].tile;

      break;

    case LOCAL_LEVEL:
      *current_width  = map->local_width;
      *current_height = map->local_height;
      *current_index = INDEX_3( ( current_y + j ), ( current_x + i ),
                              current_z, *current_width, *current_height );

      return_tile = map[pos.world_index].realms[pos.realm_index].
        regions[pos.region_index].tiles[*current_index];

      *current_index -= ( map->local_width * map->local_height * pos.local_z );

      break;

  }

  return return_tile;
}

