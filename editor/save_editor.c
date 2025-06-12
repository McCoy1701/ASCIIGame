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

  size_t num_of_local_tiles = LOCAL_SIZE * LOCAL_SIZE * Z_HEIGHT;
  fwrite( world->regions->cells->tiles, sizeof( GameTile_t ), num_of_local_tiles, file );

  fclose( file );

  return 0;
}

int LoadWorld( World_t* world, const char* filename )
{

  return 0;
}

