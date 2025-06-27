#ifndef __WORLD_EDITOR_H__
#define __WORLD_EDITOR_H__

void e_InitWorldEditor( void );
void e_DestroyWorldEditor( void );
void e_GetCellSize( int index, int width, int height, int* x, int* y, int* w, int* h );
void e_GetCellAtMouse( int width, int height, int* grid_x, int* grid_y );

void we_creation( void );
void we_edit( void );
void we_save( void );
void we_load( void );

#endif

