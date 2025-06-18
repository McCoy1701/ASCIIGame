#ifndef __INIT_EDITOR_H__
#define __INIT_EDITOR_H__

#include <stdint.h>

#define MAGIC_NUMBER "CAFEBABE"
#define FILE_VERSION 1

typedef struct
{
  uint16_t glyph;
  uint8_t temperature;
  uint8_t elevation;
  uint8_t is_passable;
} GameTile_t;

typedef struct
{
  GameTile_t* tiles;
  GameTile_t tile;
} RegionCell_t;

typedef struct
{
  RegionCell_t* regions;
  GameTile_t tile;
} World_t;

typedef struct
{
  char magic[8];
  uint32_t version;
  int32_t  world_x;
  int32_t  world_y;
  int32_t  region_x;
  int32_t  region_y;
  int32_t  local_x;
  int32_t  local_y;
  uint32_t world_size;
  uint32_t region_size;
  uint32_t local_size;
  uint32_t z_height;
} FileHeader_t;

World_t* init_world( void );
void free_world( World_t* world, int world_index, int region_index );

#endif

