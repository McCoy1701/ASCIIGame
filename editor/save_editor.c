#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init_editor.h"
#include "defs.h"

int SaveTest( LocalCell_t* local, const char* filename )
{
  FILE* file;
  file = fopen( filename, "wb" );
  if ( file == NULL )
  {
    printf( "Failed to open %s\n", filename );
    return 1;
  }
  
  FileHeader_t header;
  memcpy( header.magic, MAGIC_NUMBER, 8 );
  header.version     = FILE_VERSION;
  header.world_x     = 0;
  header.world_y     = 0;
  header.region_x    = 0;
  header.region_y    = 0;
  header.local_x     = 0;
  header.local_y     = 0;
  header.world_size  = WORLD_WIDTH * WORLD_HEIGHT;
  header.region_size = REGION_SIZE * REGION_SIZE;
  header.local_size  = LOCAL_SIZE * LOCAL_SIZE;
  header.z_height    = Z_HEIGHT;

  fwrite( &header, sizeof( FileHeader_t ), 1, file );

  size_t num_of_local_tiles = LOCAL_SIZE * LOCAL_SIZE * Z_HEIGHT;
  fwrite( local->tiles, sizeof( GameTile_t ), num_of_local_tiles, file );

  fclose( file );
  printf( "Saved file: %s\n", filename );
  return 0;
}

LocalCell_t* LoadTest( const char* filename )
{
  FILE* file;

  file = fopen( filename, "rb");
  if ( file == NULL )
  {
    printf( "Failed to read %s\n", filename );
    return NULL;
  }

  FileHeader_t header;
  fread( &header, sizeof( FileHeader_t ), 1, file );

  if ( memcmp( header.magic, MAGIC_NUMBER, 8 ) != 0 )
  {
    printf( "Invalid magic number got: %s, needed: %s\n", header.magic, MAGIC_NUMBER );
    fclose( file );
    return NULL;
  }

  if ( header.version > FILE_VERSION )
  {
    printf( "Current file version: %d outdates editor version: %d\n", header.version, FILE_VERSION );
    fclose( file );
    return NULL;
  }

  LocalCell_t* new_cell = ( LocalCell_t* )malloc( sizeof( LocalCell_t ) );
  if ( new_cell == NULL )
  {
    printf( "Failed to allocate memory for new_cell\n" );
    return NULL;
  }

  new_cell->tiles = ( GameTile_t* )malloc( sizeof( GameTile_t ) * Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE );
  if ( new_cell->tiles == NULL )
  {
    printf( "Failed to allocate memory for new_cell->tiles\n" );
    return NULL;
  }

  size_t num_of_local_tiles = Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE;
  fread( new_cell->tiles, sizeof( GameTile_t ), num_of_local_tiles, file );

  fclose( file );
  printf( "Loaded file: %s\n", filename );

  return new_cell;
}

int SaveWorld( World_t* world, const char* filename )
{
  FILE* file;
  file = fopen( filename, "wb" );
  if ( file == NULL )
  {
    printf( "Failed to open %s\n", filename );
    return 1;
  }
  
  FileHeader_t header;
  memcpy( header.magic, MAGIC_NUMBER, 8 );
  header.version     = FILE_VERSION;
  header.world_x     = 0;
  header.world_y     = 0;
  header.region_x    = 0;
  header.region_y    = 0;
  header.local_x     = 0;
  header.local_y     = 0;
  header.world_size  = WORLD_WIDTH * WORLD_HEIGHT;
  header.region_size = REGION_SIZE * REGION_SIZE;
  header.local_size  = LOCAL_SIZE * LOCAL_SIZE;
  header.z_height    = Z_HEIGHT;

  fwrite( &header, sizeof( FileHeader_t ), 1, file );
  
  size_t num_of_world_tiles = WORLD_WIDTH * WORLD_HEIGHT;
  fwrite( world->regions, sizeof( RegionCell_t ), num_of_world_tiles, file );
  
  for ( int i = 0; i < WORLD_WIDTH* WORLD_HEIGHT; i++ )
  {
    size_t num_of_region_tiles = REGION_SIZE * REGION_SIZE;
    fwrite( world->regions[i].cells, sizeof( LocalCell_t ), num_of_region_tiles, file );

    for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
    {
      size_t num_of_local_tiles = LOCAL_SIZE * LOCAL_SIZE * Z_HEIGHT;
      fwrite( world->regions[i].cells[j].tiles, sizeof( GameTile_t ), num_of_local_tiles, file );
    }
  }

  fclose( file );

  return 0;
}

World_t* LoadWorld( const char* filename )
{
  FILE* file;

  file = fopen( filename, "rb");
  if ( file == NULL )
  {
    printf( "Failed to read %s\n", filename );
    return NULL;
  }

  FileHeader_t header;
  fread( &header, sizeof( FileHeader_t ), 1, file );

  if ( memcmp( header.magic, MAGIC_NUMBER, 8 ) != 0 )
  {
    printf( "Invalid magic number got: %s, needed: %s\n", header.magic, MAGIC_NUMBER );
    fclose( file );
    return NULL;
  }

  if ( header.version > FILE_VERSION )
  {
    printf( "Current file version: %d outdates editor version: %d\n", header.version, FILE_VERSION );
    fclose( file );
    return NULL;
  }
  
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

  size_t num_of_world_tiles = WORLD_WIDTH * WORLD_HEIGHT;
  fread( new_world->regions, sizeof( GameTile_t ), num_of_world_tiles, file );

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
  
    size_t num_of_region_tiles = REGION_SIZE * REGION_SIZE;
    fread( new_world->regions[i].cells, sizeof( GameTile_t ), num_of_region_tiles, file );
    
    for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
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

      size_t num_of_local_tiles = Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE;
      fread( new_world->regions[i].cells[j].tiles, sizeof( GameTile_t ), num_of_local_tiles, file );
    }
  }
  
  fclose( file );

  return new_world;
}

