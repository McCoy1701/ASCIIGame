#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#define WORLD_WIDTH   8
#define WORLD_HEIGHT  8
#define REGION_SIZE   8
#define LOCAL_SIZE    48
#define Z_HEIGHT      10
#define CELL_SIZE     32

#define INDEX(z, y, x) ((z) * LOCAL_SIZE * LOCAL_SIZE + (y) * LOCAL_SIZE + (x))

#endif

