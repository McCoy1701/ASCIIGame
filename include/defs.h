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

#define MAX_COLOR_GROUPS        16
#define MAX_COLOR_PALETTE       48
#define GAME_GLYPH_TEXTURE_SIZE 256
#define MAX_NAME_LENGTH         32
#define MAX_DESCRIPTION_LENGTH  256
#define MAX_ID_LENGTH           16

#define SCREEN_ORIGIN_X SCREEN_WIDTH  / 2
#define SCREEN_ORIGIN_Y SCREEN_HEIGHT / 2

// inventory defs
#define INVENTORY_SIZE 24

// item editor defs
#define ITEMS_GRID_WIDTH         8
#define ITEMS_GRID_HEIGHT        6
#define ITEMS_GRID_START_X       50
#define ITEMS_GRID_START_Y       100
#define ITEMS_CELL_WIDTH         18
#define ITEMS_CELL_HEIGHT        24
#define ITEM_PROPERTIES_PANEL_X  400
#define ITEM_PROPERTIES_PANEL_Y  100
#define ITEM_PROPERTIES_PANEL_WIDTH 250
#define MATERIALS_GRID_WIDTH     4
#define MATERIALS_GRID_HEIGHT    3
#define MATERIALS_PANEL_X        100
#define MATERIALS_PANEL_Y        300
#define MATERIALS_CELL_WIDTH     60
#define MATERIALS_CELL_HEIGHT    40
#define MATERIALS_GRID_MAX       12
// Item preview display area
#define ITEM_PREVIEW_X          700
#define ITEM_PREVIEW_Y          100
// Item system limits  
#define MAX_ITEMS               1000
#define ITEMS_GRID_MAX          48    
// Additional useful constants for completeness
#define ITEM_PREVIEW_SIZE       64    // Size of the large preview icon
#define MAX_FILTERED_ITEMS      256   // Maximum items after type filtering

#define INDEX_3( x, y, z, width, height ) ( ( z * ( width * height ) )\
    + ( y * height ) + x )

#define INDEX_2( x, y, height ) ( ( y * height ) + x )

#endif

