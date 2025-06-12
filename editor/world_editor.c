#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "init_editor.h"
#include "save_editor.h"

static void aDoLoop( float );
static void aRenderLoop( float );

static void free_world( void );

LocalCell_t* test_cells;
World_t* map;

void aInitGame( void )
{
  app.delegate.logic = aDoLoop;
  app.delegate.draw  = aRenderLoop;
  
/* map = init_world();

  for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
  {
    for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
    {
      for ( int k = 0; k < Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE; k++ )
      {
        map->regions[i].cells[j].tiles[k].glyph       = 'G';
        map->regions[i].cells[j].tiles[k].is_passable = 1;
        map->regions[i].cells[j].tiles[k].temperature = 20;
        map->regions[i].cells[j].tiles[k].elevation   = 0;
      }
    }
  }

  SaveWorld( map, "resources/world/world_test.dat" );*/
  map = LoadWorld( "resources/world/world_test.dat" );
  
  for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
  {
    for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
    {
      for ( int k = 0; k < Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE; k++ )
      {
        printf( "%d, %d, %d, %c, %d, %d, %d\n", i, j, k, map->regions[i].cells[j].tiles[k].glyph,
               map->regions[i].cells[j].tiles[k].temperature,
               map->regions[i].cells[j].tiles[k].is_passable,
               map->regions[i].cells[j].tiles[k].elevation );
      }
    }
  }
  
}

static void aDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.running = 0;
  }
}

static void aRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32, blue );
  a_DrawFilledRect( 300, 300, 32, 32, red );
}

void aMainloop( void )
{
  a_PrepareScene();

  app.delegate.logic( a_GetDeltaTime() );
  app.delegate.draw( a_GetDeltaTime() );
  
  a_PresentScene();
}

static void free_world( void )
{
  for ( int k = 0; k < WORLD_WIDTH * WORLD_HEIGHT; k++ )
  {
    for ( int l = 0; l < REGION_SIZE * REGION_SIZE; l++ )
    {
      free( map->regions[k].cells[l].tiles );
    }

    free( map->regions[k].cells );
  }

  free( map->regions );
  free( map );
}

int main( void )
{
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  aInitGame();
  
  while( app.running ) {
    aMainloop();
  }

  free_world();
  
  a_Quit();

  return 0;
}

