/*
 * save_editor.c:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init_editor.h"
#include "defs.h"
#include "structs.h"

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
  header.version       = FILE_VERSION;
  header.realm_width   = world->realm_width;
  header.realm_height  = world->realm_height;
  header.region_width  = world->region_width;
  header.region_height = world->region_height;
  header.local_width   = world->local_width;
  header.local_height  = world->local_height;
  header.z_height      = world->z_height;

  fwrite( &header, sizeof( FileHeader_t ), 1, file );
  
  size_t num_of_world_tiles = WORLD_WIDTH * WORLD_HEIGHT;
  fwrite( world, sizeof( World_t ), num_of_world_tiles, file );
  
  for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
  {
    size_t num_of_realm_tiles = world->realm_width * world->realm_height;
    fwrite( world[i].realms, sizeof( RegionCell_t ), num_of_realm_tiles,
           file );
  }

  fclose( file );

  return 0;
}

int SaveRegion( World_t* world, const char* filename, const int realm_index )
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
  header.version       = FILE_VERSION;
  header.realm_width   = world->realm_width;
  header.realm_height  = world->realm_height;
  header.region_width  = world->region_width;
  header.region_height = world->region_height;
  header.local_width   = world->local_width;
  header.local_height  = world->local_height;
  header.z_height      = world->z_height;

  fwrite( &header, sizeof( FileHeader_t ), 1, file );

  for ( int i = 0; i < ( world->realm_width * world->realm_height ); i++ )
  {
    size_t num_of_region_tiles = world->region_width * world->region_height;
    fwrite( world[realm_index].realms[i].regions, sizeof( RegionCell_t ), num_of_region_tiles,
           file );

    for ( int j = 0; j < ( world->region_width * world->region_height ); j++ )
    {
      size_t num_of_local_tiles = world->local_width * world->local_height * world->z_height;
      fwrite( world[realm_index].realms[i].regions[j].tiles, sizeof( GameTile_t ), num_of_local_tiles,
             file );
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
    printf( "Invalid magic number got: %s, needed: %s\n", header.magic,
           MAGIC_NUMBER );

    fclose( file );
    return NULL;
  }

  if ( header.version > FILE_VERSION )
  {
    printf( "Current file version: %d outdates editor version: %d\n",
           header.version, FILE_VERSION );

    fclose( file );
    return NULL;
  }
  
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * ( WORLD_WIDTH *
                                          WORLD_HEIGHT ) );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    return NULL;
  }

  new_world->realm_width   = header.realm_width;
  new_world->realm_height  = header.realm_height;
  new_world->region_width  = header.region_width;
  new_world->region_height = header.region_height;
  new_world->local_width   = header.local_width;
  new_world->local_height  = header.local_height;
  new_world->z_height      = header.z_height;

  size_t num_of_world_tiles = WORLD_WIDTH * WORLD_HEIGHT;
  fread( new_world, sizeof( World_t ), num_of_world_tiles, file );

  for ( int i = 0; i < ( WORLD_WIDTH * WORLD_HEIGHT ); i++ )
  {
    new_world[i].realms = ( Realm_t* )malloc( sizeof(Realm_t) *
      new_world->realm_width * new_world->realm_height );

    if ( new_world[i].realms == NULL )
    {
      free_world( new_world, i, 0, 0 );
      return NULL;
    }
    
    size_t num_of_realm_tiles = new_world->realm_width * new_world->realm_height;
    fread( new_world[i].realms, sizeof( Realm_t ), num_of_realm_tiles,
           file );
  }

  fclose( file );

  return new_world;
}

int LoadRegion( World_t* world, const char* filename, const int realm_index )
{
  FILE* file;

  file = fopen( filename, "rb");
  if ( file == NULL )
  {
    printf( "Failed to read %s\n", filename );
    return 1;
  }

  FileHeader_t header;
  fread( &header, sizeof( FileHeader_t ), 1, file );

  if ( memcmp( header.magic, MAGIC_NUMBER, 8 ) != 0 )
  {
    printf( "Invalid magic number got: %s, needed: %s\n", header.magic,
           MAGIC_NUMBER );

    fclose( file );
    return 1;
  }

  if ( header.version > FILE_VERSION )
  {
    printf( "Current file version: %d outdates editor version: %d\n",
           header.version, FILE_VERSION );

    fclose( file );
    return 1;
  }
  
  for ( int i = 0; i < ( world->realm_width * world->realm_height ); i++ )
  {
    world[realm_index].realms[i].regions = ( RegionCell_t* )malloc( 
      sizeof( RegionCell_t ) * world->region_width * world->region_height );

    if ( world[realm_index].realms[i].regions == NULL )
    {
      free_world( world, realm_index, i, 0 );
      return 1;
    }

    size_t num_of_region_tiles = world->region_width * world->region_height;
    fread( world[realm_index].realms[i].regions, sizeof( RegionCell_t ), num_of_region_tiles,
           file );

    for ( int j = 0; j < ( world->region_width * world->region_height ); j++ )
    {
      world[realm_index].realms[i].regions[j].tiles = ( GameTile_t* )malloc( 
        sizeof( GameTile_t ) * world->local_width * world->local_height * world->z_height);

      if ( world[realm_index].realms[i].regions[j].tiles == NULL )
      {
        free_world( world, realm_index, i, j );
        return 0;
      }

      size_t num_of_local_tiles = world->local_width * world->local_height * world->z_height;
      fread( world[realm_index].realms[i].regions[j].tiles, sizeof( GameTile_t ), num_of_local_tiles,
             file );
    }
  }

  fclose( file );

  return 0;
}

