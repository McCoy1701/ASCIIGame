#include "Archimedes.h"
#include "structs.h"

#ifndef __GAME_H__
#define __GAME_H__

void g_DrawWorldMap( World_t* world, aColor_t color );
void g_DrawRegionMap( RegionCell_t* region, aColor_t color );
void g_DrawLocalMap( LocalCell_t* cell, aColor_t color );

#endif

