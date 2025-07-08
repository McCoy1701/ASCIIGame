/*
 * init_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>

#include "Archimedes.h"
#include "structs.h"
#include "init_editor.h"

World_t* init_world( const int realm_width, const int realm_height,
                     const int region_width, const int region_height,
                     const int local_width, const int local_height,
                     const int z_height )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) *
                                 ( WORLD_WIDTH * WORLD_HEIGHT ) );

  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    free( new_world );
    return NULL;
  }
  
  for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
  {
    new_world[i].realms = ( Realm_t* )malloc( sizeof( Realm_t ) *
                                    realm_width * realm_height );
  
    if ( new_world[i].realms == NULL )
    {
      printf("Failed to allocate memory for world[%d].realms\n", i );
      free_world( new_world, i, 0, 0 );
      return NULL;
    }

    new_world->realm_width   = realm_width;
    new_world->realm_height  = realm_height;
    new_world->region_width  = region_width;
    new_world->region_height = region_height;
    new_world->local_width   = local_width;
    new_world->local_height  = local_height;
    new_world->z_height      = z_height;
    
    for ( int j = 0; j < ( realm_width * realm_height ); j++ )
    {
      new_world[i].realms[j].tile = (GameTile_t){.glyph = 0, .elevation = 0, 
        .temperature = 20, .is_passable = 0, .fg = 8, .bg = 16 };

      new_world[i].realms[j].regions = ( RegionCell_t* )malloc( 
        sizeof( RegionCell_t ) * ( region_width * region_height ) );

      if ( new_world[i].realms[j].regions == NULL )
      {
        free_world( new_world, i, j, 0 );
        return NULL;
      }

      for ( int k = 0; k < ( region_width * region_height ); k++ )
      {
        new_world[i].realms[j].regions[k].tile = ( GameTile_t ){
          .glyph = 1, .elevation = 0, .temperature = 20, .is_passable = 0,
          .fg = 16, .bg = 24 };

        new_world[i].realms[j].regions[k].tiles = ( GameTile_t* )malloc( 
          sizeof( GameTile_t ) * ( local_width * local_height * z_height ) );

        if ( new_world[i].realms[j].regions[k].tiles == NULL )
        {
          free_world( new_world, i, j, k );
          return NULL;
        }

        for( int l = 0; l < ( local_width * local_height * z_height); l++ )
        {
          new_world[i].realms[j].regions[k].tiles[l] = ( GameTile_t ){
            .glyph = 2, .elevation = 0, .temperature = 20, .is_passable = 0,
            .fg = 24, .bg = 32 };

        }
      }
    }
  }
  
  return new_world;
}

void free_world( World_t* world, int world_index, int realm_index,
                 int region_index )
{
  if ( world == NULL ) return;
  
  for ( int i = 0; i < world_index; i++ )
  {
    if ( world[i].realms != NULL)
    {
      int max_realms = ( i == world_index ) ? realm_index : ( 
        ( world->realm_width * world->realm_height ) - 1 );

      for ( int j = 0; j < max_realms; j++ )
      {
      
      if ( world[i].realms[j].regions != NULL )
      {
      
      int max_regions = ( j == realm_index ) ? region_index : ( 
      ( world->region_width * world->region_height ) - 1 );
          
      for ( int k = 0; k < max_regions; k++ )
      {
      
      if ( world[i].realms[j].regions[k].tiles != NULL )
      {
      
      free( world[i].realms[j].regions[k].tiles );
      world[i].realms[j].regions[k].tiles = NULL;

      }
      }
      
      free( world[i].realms[j].regions );
      world[i].realms[j].regions = NULL;
      } 
      }
   
      free( world[i].realms );
      world[i].realms = NULL;
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

