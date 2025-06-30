#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include "Archimedes.h"

#define MAGIC_NUMBER "CAFEBABE"
#define FILE_VERSION 1

#define WORLD_WIDTH_SMALL   12
#define WORLD_HEIGHT_SMALL   9
#define REGION_WIDTH_SMALL  12
#define REGION_HEIGHT_SMALL  9
#define LOCAL_WIDTH_SMALL   15
#define LOCAL_HEIGHT_SMALL  12
#define Z_HEIGHT_SMALL      10

#define WORLD_WIDTH_MEDIUM   15
#define WORLD_HEIGHT_MEDIUM  12
#define REGION_WIDTH_MEDIUM  15
#define REGION_HEIGHT_MEDIUM 12
#define LOCAL_WIDTH_MEDIUM   18
#define LOCAL_HEIGHT_MEDIUM  15
#define Z_HEIGHT_MEDIUM      50

#define WORLD_WIDTH_LARGE    18
#define WORLD_HEIGHT_LARGE   15
#define REGION_WIDTH_LARGE   18
#define REGION_HEIGHT_LARGE  15
#define LOCAL_WIDTH_LARGE    21
#define LOCAL_HEIGHT_LARGE   18
#define Z_HEIGHT_LARGE       128

#define CELL_WIDTH  18
#define CELL_HEIGHT 32

#define GAME_GLYPH_TEXTURE_SIZE 256
#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 256
#define MAX_ID_LENGTH 16

#define SCREEN_ORIGIN_X SCREEN_WIDTH  / 2
#define SCREEN_ORIGIN_Y SCREEN_HEIGHT / 2

// inventory defs
#define INVENTORY_SIZE 24

#define INDEX_3( x, y, z, width, height ) ( ( z * ( width * height ) ) + ( y * height ) + x )
#define INDEX_2( x, y, height ) ( ( y * height ) + x )

#endif

