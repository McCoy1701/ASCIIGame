#include "Archimedes.h"
#include "game.h"
#include "structs.h"

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

void g_GetCellSize( int index, int width, int height, int* x, int* y, int* w, int* h )
{
  int row = ( index / height );
  int col = ( index % height );
  *x = ( ( SCREEN_WIDTH / 2 )  - ( ( width  * CELL_SIZE ) / 2 ) )
    + ( row * CELL_SIZE );
  *y = ( ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 ) )
    + ( col * CELL_SIZE );
  *w = CELL_SIZE;
  *h = CELL_SIZE;
}

void g_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y )
{
  int edge_x = ( SCREEN_WIDTH  / 2 ) - ( ( width  * CELL_SIZE ) / 2 );
  int edge_y = ( SCREEN_HEIGHT / 2 ) - ( ( height * CELL_SIZE ) / 2 );
  int mousex = ( ( app.mouse.x - edge_x ) / CELL_SIZE );
  int mousey = ( ( app.mouse.y - edge_y ) / CELL_SIZE );
  *grid_x = ( mousex < 0 ) ? 0 : ( ( mousex >= width  )
    ? width  - 1 : mousex );
  *grid_y = ( mousey < 0 ) ? 0 : ( ( mousey >= height )
    ? height - 1 : mousey );
}

