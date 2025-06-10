#ifndef __INIT_EDITOR_H__
#define __INIT_EDITOR_H__

typedef struct
{
  char terrain;
  int temperature;
  int elevation;
  int isOccupied;
} GameTile_t;

typedef struct
{
  GameTile_t* tiles;
} LocalCell_t;

typedef struct
{
  LocalCell_t* cells;
} RegionCell_t;

typedef struct
{
  RegionCell_t* regions;
} World_t;

World_t* init_world( void );
GameTile_t* CreateTile( void );

#endif

