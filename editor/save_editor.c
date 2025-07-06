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
  header.world_width   = world->world_width;
  header.world_height  = world->world_height;
  header.region_width  = world->region_width;
  header.region_height = world->region_height;
  header.local_width   = world->local_width;
  header.local_height  = world->local_height;
  header.z_height      = world->z_height;

  fwrite( &header, sizeof( FileHeader_t ), 1, file );
  
  size_t num_of_world_tiles = world->world_width * world->world_height;
  fwrite( world, sizeof( World_t ), num_of_world_tiles, file );
  
  for ( int i = 0; i < ( world->world_width * world->world_height ); i++ )
  {
    size_t num_of_region_tiles = world->region_width * world->region_height;
    fwrite( world[i].regions, sizeof( RegionCell_t ), num_of_region_tiles,
           file );

    for ( int j = 0; j < ( world->region_width * world->region_height ); j++ )
    {
      size_t num_of_local_tiles = world->local_width * world->local_height *
        world->z_height;
      fwrite( world[i].regions[j].tiles, sizeof( GameTile_t ),
             num_of_local_tiles, file );
    }
  }

  fclose( file );

  return 0;
}

World_t* LoadWorld( const char* filename )
{
  FILE* file;
  int where = 0;
  int world_width = 0, world_height = 0, region_width = 0, region_height = 0,
      local_width = 0, local_height = 0, z_height = 0;

  file = fopen( filename, "rb");
  if ( file == NULL )
  {
    printf( "Failed to read %s\n", filename );
    return NULL;
  }

  FileHeader_t header;
  
  where = ftell( file );
  printf( "H: %d\n", where );

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
  
  world_width   = header.world_width;
  world_height  = header.world_height;
  region_width  = header.region_width;
  region_height = header.region_height;
  local_width   = header.local_width;
  local_height  = header.local_height;
  z_height      = header.z_height;
  
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * ( world_width *
                                          world_height ) );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    return NULL;
  }

  size_t num_of_world_tiles = world_width * world_height;
  
  where = ftell( file );
  printf( "W: %d\n", where );


  fread( new_world, sizeof( World_t ), num_of_world_tiles, file );

  for ( int i = 0; i < ( world_width * world_height ); i++ )
  {
    new_world[i].regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) *
                                                region_width * region_height );
    if ( new_world[i].regions == NULL )
    {
      free_world( new_world, i, 0 );
      return NULL;
    }

    size_t num_of_region_tiles = region_width * region_height;
  
    where = ftell( file );
    printf( "R: %d\n", where );

    fread( new_world[i].regions, sizeof( RegionCell_t ), num_of_region_tiles,
          file );

    for ( int j = 0; j < ( region_width * region_height ); j++ )
    {
      new_world[i].regions[j].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t )
                                      * local_width * local_height * z_height );
      if ( new_world[i].regions[j].tiles == NULL )
      {
        free_world( new_world, i, j );
        return NULL;
      }

      size_t num_of_local_tiles = local_width * local_height * z_height;

      //where = ftell( file );
      //printf( "local: %d\n", where );

      fread( new_world[i].regions[j].tiles, sizeof( GameTile_t ),
            num_of_local_tiles, file );
    }
  }

  fclose( file );

  return new_world;
}

World_t* LoadPartialWorld( const char* filename )
{
  FILE* file;
  int world_width = 0, world_height = 0, region_width = 0, region_height = 0,
      local_width = 0, local_height = 0, z_height = 0;

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
  
  world_width   = header.world_width;
  world_height  = header.world_height;
  region_width  = header.region_width;
  region_height = header.region_height;
  local_width   = header.local_width;
  local_height  = header.local_height;
  z_height      = header.z_height;
  
  World_t* new_world = ( World_t* )malloc( sizeof( World_t ) * ( world_width *
                                          world_height ) );
  if ( new_world == NULL )
  {
    printf("Failed to allocate memory for world\n");
    return NULL;
  }

  size_t num_of_world_tiles = world_width * world_height;
  fread( new_world, sizeof( World_t ), num_of_world_tiles, file );

  for ( int i = 0; i < ( world_width * world_height ); i++ )
  {
    new_world[i].regions = NULL;
  }


  fclose( file );

  return new_world;
}

int LoadPartialRegion( WorldPosition_t* pos, World_t* world, const char* filename )
{
  if ( world == NULL ) return 1;

  FILE* file;
  int where = 0;
  int world_width = 0, world_height = 0, region_width = 0, region_height = 0,
      local_width = 0, local_height = 0, z_height = 0;

  file = fopen( filename, "rb");
  if ( file == NULL )
  {
    printf( "Failed to read %s\n", filename );
    return 1;
  }

  FileHeader_t header;
  
  where = ftell( file );
  //printf( "Hs: %d\n", where );

  fread( &header, sizeof( FileHeader_t ), 1, file );
  
  where = ftell( file );
  //printf( "He: %d\n", where );

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
  
  world_width   = header.world_width;
  world_height  = header.world_height;
  region_width  = header.region_width;
  region_height = header.region_height;
  local_width   = header.local_width;
  local_height  = header.local_height;
  z_height      = header.z_height;

  where = ftell( file );
  //printf( "Ws: %d\n", where );

  fseek( file, ( ( sizeof( World_t ) * world_width * world_height ) +
        sizeof( FileHeader_t) ), SEEK_SET );
  
  where = ftell( file );
  //printf( "We: %d\n", where );


  for ( int i = 0; i < ( world_width * world_height ); i++ )
  {
    world[i].regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) *
                                                region_width * region_height );
    if ( world[i].regions == NULL )
    {
      free_world( world, i, 0 );
      return NULL;
    }

    size_t num_of_region_tiles = region_width * region_height;
    
    where = ftell( file );
    //printf( "Rs: %d\n", where );

    fread( world[i].regions, sizeof( RegionCell_t ), num_of_region_tiles,
          file );
    
    where = ftell( file );
    //printf( "Re: %d\n", where );

    for ( int j = 0; j < ( region_width * region_height ); j++ )
    {
      world[i].regions[j].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t )
                                      * local_width * local_height * z_height );
      if ( world[i].regions[j].tiles == NULL )
      {
        free_world( world, i, j );
        return NULL;
      }

      size_t num_of_local_tiles = local_width * local_height * z_height;
      fread( world[i].regions[j].tiles, sizeof( GameTile_t ),
            num_of_local_tiles, file );
    }
  }

  fclose( file );

  return 0;
}
