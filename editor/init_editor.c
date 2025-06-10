#include <stdlib.h>
#include <stdio.h>

#include "defs.h"
#include "init_editor.h"

World_t* init_world( void )
{
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    return NULL;
  }

  new_world->regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t* ) * WORLD_WIDTH * WORLD_HEIGHT );
  if ( new_world->regions == NULL )
  {
    printf("Failed to allocate memory for world->regions\n");
    free( new_world );
    return NULL;
  }

  for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
  {
    new_world->regions[i].cells = ( LocalCell_t* )malloc( sizeof( LocalCell_t ) * REGION_SIZE * REGION_SIZE );
    if ( new_world->regions[i].cells == NULL )
    {
      for ( int j = 0; j < i; j++ )
      {
        free( new_world->regions[j].cells );
      }

      free( new_world->regions );
      free( new_world );
      return NULL;
    }
    
    for ( int j = 0; j < LOCAL_SIZE * LOCAL_SIZE; j++ )
    {
      new_world->regions[i].cells[j].tiles = (GameTile_t*)malloc( sizeof(GameTile_t ) * LOCAL_SIZE * LOCAL_SIZE * Z_HEIGHT );
      if ( new_world->regions[i].cells[j].tiles == NULL )
      {
        for ( int k = 0; k < i; k++ )
        {
          for ( int l = 0; l < j; l++ )
          {
            free( new_world->regions[k].cells[j].tiles );
          }

          free( new_world->regions[k].cells );
        }

        free( new_world->regions );
        free( new_world );
        return NULL;
      }
    }
  }



  return new_world;
}

GameTile_t*** CreateTile( void )
{
  GameTile_t*** new_tile = ( GameTile_t*** )malloc( sizeof( GameTile_t** ) * Z_HEIGHT );
  if ( new_tile == NULL )
  {
    printf("Failed to allocate memory for GameTile_t\n");
    return NULL;
  }
  for ( int z = 0; z < Z_HEIGHT; z++ )
  {
    new_tile[z] = ( GameTile_t** )malloc( sizeof( GameTile_t* ) * LOCAL_SIZE );
  }

  return new_tile;
}

