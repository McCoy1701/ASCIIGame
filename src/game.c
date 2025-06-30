#include "Archimedes.h"
#include "game.h"
#include "structs.h"

GlyphArray_t* game_glyphs = NULL;

World_t* init_world( const int world_width, const int world_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height, const int z_height )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * ( world_width * world_height ) );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    free( new_world );
    return NULL;
  }

  new_world->world_width   = world_width;
  new_world->world_height  = world_height;
  new_world->region_width  = region_width;
  new_world->region_height = region_height;
  new_world->local_width   = local_width;
  new_world->local_height  = local_height;
  new_world->z_height      = z_height;
  
  for ( int i = 0; i < ( world_width * world_height ); i++ )
  {
    new_world[i].tile = (GameTile_t){.glyph = 0, .elevation = 0, 
      .temperature = 20, .is_passable = 0 };
    
    new_world[i].world_width   = world_width;
    new_world[i].world_height  = world_height;
    new_world[i].region_width  = region_width;
    new_world[i].region_height = region_height;
    new_world[i].local_width   = local_width;
    new_world[i].local_height  = local_height;
    new_world[i].z_height      = z_height;

    new_world[i].regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) *
                                                    ( region_width * region_height ) );
    
    if ( new_world[i].regions == NULL )
    {
      free_world( new_world, i, 0 );
      return NULL;
    }

    for ( int j = 0; j < ( region_width * region_height ); j++ )
    {
      new_world[i].regions[j].tile = (GameTile_t){.glyph = 1, .elevation = 0,
        .temperature = 20, .is_passable = 0 };
      
      new_world[i].regions[j].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t ) *
                                                           ( local_width * local_height * z_height ) );
  
      if ( new_world[i].regions[j].tiles == NULL )
      {
        free_world( new_world, i, j );
        return NULL;
      }

      for( int k = 0; k < ( local_width * local_height * z_height); k++ )
      {
        new_world[i].regions[j].tiles[k] = (GameTile_t){.glyph = 2, .elevation = 0,
        .temperature = 20, .is_passable = 0 };

      }

    }
  }

  return new_world;
}

void free_world( World_t* world, int world_index, int region_index )
{
  if ( world == NULL ) return;
  
  for ( int i = 0; i < world_index; i++ )
  {
    if ( world[i].regions != NULL)
    {
      int max_regions = ( i == world_index ) ? region_index : ( 
        ( world->region_width * world->region_height ) - 1 );

      for ( int j = 0; j < max_regions; j++ )
      {
        if ( world[i].regions[j].tiles != NULL )
        {
          free( world[i].regions[j].tiles );
          world[i].regions[j].tiles = NULL;
        }
      }

      free( world[i].regions );
      world[i].regions = NULL;
    }

  }

  free( world );
  world = NULL;
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

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos )
{
  int x, y, w, h;
  uint32_t i = 0;
  uint16_t current_index = 0;
  int current_glyph = 0;

  switch ( pos.level ) {
    case WORLD_LEVEL:
      e_GetCellSize( index, map->world_width, map->world_height,
                     &x, &y, &w, &h );

      i = index;

      current_index = pos.world_index;
      current_glyph = map[i].tile.glyph;
      break;

    case REGION_LEVEL:
      e_GetCellSize( index, map->region_width, map->region_height,
                     &x, &y, &w, &h );

      i = index;

      current_index = pos.region_index;
      current_glyph = map[pos.world_index].regions[i].tile.glyph;
      break;

    case LOCAL_LEVEL:
      e_GetCellSize( index, map->local_width, map->local_height,
                     &x, &y, &w, &h );

      i = ( ( pos.local_z * ( map->local_width * map->local_height ) 
        + index ) );

      current_index = pos.local_index;
      current_glyph = map[pos.world_index].regions[pos.region_index].
        tiles[i].glyph;

      break;

    default:
      break;
  }

  if ( i == current_index )
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2, 255, 255, 0, 255 );
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2 );
  } 

  else
  {
    a_DrawFilledRect( x, y, game_glyphs->rects[current_glyph].w * 2,
                      game_glyphs->rects[current_glyph].h * 2, 0, 0, 0, 255 );
    a_BlitTextureRect( game_glyphs->texture, game_glyphs->rects[current_glyph],
                      x, y, 2 );
  }

}

GlyphArray_t* e_InitGlyphs( const char* filename, int glyph_width,
                            int glyph_height )
{
  SDL_Surface* surface, *glyph_surf;
  SDL_Rect dest, rect;
  
  GlyphArray_t* new_glyphs = ( GlyphArray_t* )malloc( sizeof( GlyphArray_t ) );
  if ( new_glyphs == NULL )
  {
    printf( "Failed to allocate memory for new_glyphs %s\n", filename );
    return NULL;
  }
  new_glyphs->texture = NULL;
  new_glyphs->count = 0;
  
  glyph_surf = a_Image( filename );
  if( glyph_surf == NULL )
  {
    printf( "Failed to open font surface %s, %s", filename, SDL_GetError() );
    return NULL;
  }

  surface = SDL_CreateRGBSurface( 0, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 32,
                                  0, 0, 0, 0xff );

  SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGBA( surface->format, 0, 0, 0,
                                                   0 ) );

  dest.x = dest.y = 0;
  rect.x = rect.y = 0;
  rect.w = dest.w = glyph_width;
  rect.h = dest.h = glyph_height;

  while ( rect.x < glyph_surf->w )
  {
    if ( dest.x + dest.w >= GAME_GLYPH_TEXTURE_SIZE )
    {
      dest.x = 0;
      dest.y += dest.h + 1;
      if ( dest.y + dest.h >= GAME_GLYPH_TEXTURE_SIZE )
      {
        SDL_LogMessage( SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL,
                        "Out of glyph space in %dx%d font atlas texture map.",
                        FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE );
        return NULL;
      }
    }

    SDL_BlitSurface( glyph_surf, &rect, surface, &dest );
    
    new_glyphs->rects[new_glyphs->count++] = dest;

    dest.x += dest.w;
    rect.x += rect.w;
  }

  new_glyphs->texture = a_ToTexture( surface, 1 );
  
  return new_glyphs;
}

void e_MapMouseCheck( World_t* map, WorldPosition_t* pos )
{
  switch (pos->level) {
    case WORLD_LEVEL: 
      e_GetCellAtMouse( map->world_width, map->world_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->world_index = INDEX_2( pos->y, pos->x, map->world_height );
      break;

    case REGION_LEVEL:
      e_GetCellAtMouse( map->region_width, map->region_height,
                       SCREEN_ORIGIN_X, SCREEN_ORIGIN_Y, CELL_WIDTH,
                       CELL_HEIGHT, &pos->x, &pos->y, 1 );

      pos->region_index = INDEX_2( pos->y, pos->x,
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

    default:
      break;
  }
} 

