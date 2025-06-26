#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#define MAGIC_NUMBER "CAFEBABE"
#define FILE_VERSION 1

#define WORLD_WIDTH_SMALL    8
#define WORLD_HEIGHT_SMALL   8
#define REGION_SIZE_SMALL    8
#define LOCAL_SIZE_SMALL     16
#define Z_HEIGHT_SMALL       10

#define WORLD_WIDTH_MEDIUM   12
#define WORLD_HEIGHT_MEDIUM  12
#define REGION_SIZE_MEDIUM   12
#define LOCAL_SIZE_MEDIUM    24
#define Z_HEIGHT_MEDIUM      50

#define WORLD_WIDTH_LARGE    16
#define WORLD_HEIGHT_LARGE   16
#define REGION_SIZE_LARGE    16
#define LOCAL_SIZE_LARGE     48
#define Z_HEIGHT_LARGE       128

#define WORLD_WIDTH  8
#define WORLD_HEIGHT 8
#define REGION_SIZE  8
#define LOCAL_SIZE   48
#define Z_HEIGHT     10

#define CELL_SIZE            32

#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 256
#define MAX_ID_LENGTH 16

// inventory defs
#define INVENTORY_SIZE 24

#define INDEX(z, y, x) ((z) * LOCAL_SIZE * LOCAL_SIZE + (y) * LOCAL_SIZE + (x))

#endif

