#include <stdlib.h>
#include <stdio.h>

#include "defs.h"
#include "init_editor.h"

World_t* init_world( void )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * WORLD_WIDTH * WORLD_HEIGHT );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    free( new_world );
    return NULL;
  }
  
  for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
  {
    new_world[i].tile = (GameTile_t){.glyph = 0, .elevation = 0, 
      .temperature = 20, .is_passable = 0 };

    new_world[i].regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) * 
                                                   REGION_SIZE * REGION_SIZE );
    
    if ( new_world[i].regions == NULL )
    {
      free_world( new_world, i, 0 );
      return NULL;
    }

    for ( int j = 0; j < ( REGION_SIZE * REGION_SIZE ); j++ )
    {
      new_world[i].regions[j].tile = (GameTile_t){.glyph = 0, .elevation = 0,
        .temperature = 20, .is_passable = 0 };
      
      new_world[i].regions[j].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t ) 
                                                           * LOCAL_SIZE * LOCAL_SIZE * Z_HEIGHT );
  
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
      int max_regions = ( i == world_index ) ? region_index : ( REGION_SIZE * REGION_SIZE - 1 );

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

