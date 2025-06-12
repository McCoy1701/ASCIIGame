#include "Archimedes.h"
#include "structs.h"

#ifndef __GAME_H__
#define __GAME_H__

void g_DrawWorldMap( World_t* world, int originX, int originY, aColor_t color );
void g_DrawRegionMap( RegionCell_t* region, int originX, int originY, aColor_t color );
void g_DrawLocalMap( LocalCell_t* cell, int originX, int originY, aColor_t color );

#endif

