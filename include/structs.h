#include "defs.h"

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

typedef struct
{
  char terrain;
  int temperature;
  int elevation;
  int isOccupied;
} GameTile_t;

typedef struct
{
  GameTile_t* tiles; //Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE
} LocalCell_t;

typedef struct
{
  LocalCell_t* cells; //REGION_SIZE * REGION_SIZE
} RegionCell_t;

typedef struct
{
  RegionCell_t* regions; //WORLD_HEIGHT * WORLD_WIDTH
} World_t;

#endif

