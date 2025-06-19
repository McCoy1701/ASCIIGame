#include <stdlib.h>
#include <stdio.h>

#include "structs.h"

World_t* init_world( const int world_size, const int region_size, const int local_size )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * world_size );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    free( new_world );
    return NULL;
  }

  new_world->world_size  = world_size;
  new_world->region_size = region_size;
  new_world->local_size  = local_size;
  
  for ( int i = 0; i < world_size; i++ )
  {
    new_world[i].tile = (GameTile_t){.glyph = 0, .elevation = 0, 
      .temperature = 20, .is_passable = 0 };

    new_world[i].regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) * region_size );
    
    if ( new_world[i].regions == NULL )
    {
      free_world( new_world, i, 0 );
      return NULL;
    }

    for ( int j = 0; j < ( region_size ); j++ )
    {
      new_world[i].regions[j].tile = (GameTile_t){.glyph = 0, .elevation = 0,
        .temperature = 20, .is_passable = 0 };
      
      new_world[i].regions[j].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t ) * local_size );
  
      if ( new_world[i].regions[j].tiles == NULL )
      {
        free_world( new_world, i, j );
        return NULL;
      }    

    }
  }

  return new_world;
}

void free_world( World_t* world, int world_index, int region_index )
{
  if ( world == NULL ) return;
  
  for ( int i = 0; i <= world_index; i++ )
  {
    if ( world[i].regions != NULL)
    {
      int max_regions = ( i == world_index ) ? region_index : ( world->region_size - 1 );

      for ( int j = 0; j < max_regions; j++ )
      {
          free( world[i].regions[j].tiles );
          world[i].regions[j].tiles = NULL;
      }

      free( world[i].regions );
      world[i].regions = NULL;
    }

  }

  free( world );
}

