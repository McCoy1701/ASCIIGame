#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "structs.h"
#include "init_editor.h"

World_t* init_world( const int world_width, const int world_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height,
                     const int z_height )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) *
                                          ( world_width * world_height ) );

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
      
      new_world[i].regions[j].tiles = ( GameTile_t* )malloc( 
        sizeof( GameTile_t ) * ( local_width * local_height * z_height ) );
  
      if ( new_world[i].regions[j].tiles == NULL )
      {
        free_world( new_world, i, j );
        return NULL;
      }

      for( int k = 0; k < ( local_width * local_height * z_height); k++ )
      {
        new_world[i].regions[j].tiles[k] = (GameTile_t){.glyph = 2,
          .elevation = 0, .temperature = 20, .is_passable = 0 };

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
    if ( dest.x + dest.w >= FONT_TEXTURE_SIZE )
    {
      dest.x = 0;
      dest.y += dest.h + 1;
      if ( dest.y + dest.h >= FONT_TEXTURE_SIZE )
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

