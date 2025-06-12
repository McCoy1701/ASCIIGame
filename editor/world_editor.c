#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "init_editor.h"
#include "save_editor.h"

static void aDoLoop( float );
static void aRenderLoop( float );

LocalCell_t* test_cells;
World_t* map;

void aInitGame( void )
{
  app.delegate.logic = aDoLoop;
  app.delegate.draw  = aRenderLoop;
  
  /*for ( int i = 0; i < ( Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE ); i++ )
  {
    new_local->tiles[i].elevation   = 0;
    new_local->tiles[i].terrain     = 'G';
    new_local->tiles[i].temperature = 20;
    new_local->tiles[i].isOccupied  = 0;
  }*/

  map = init_world();

  for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ )
  {
    for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
    {
      for ( int k = 0; k < Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE; k++ )
      {
        map->regions[i].cells[j].tiles[k].terrain     = 'G';
        map->regions[i].cells[j].tiles[k].isOccupied  = 0;
        map->regions[i].cells[j].tiles[k].temperature = 20;
        map->regions[i].cells[j].tiles[k].elevation   = 0;
      }
    }
  }

  SaveWorld( map, "resources/world/world_test.dat" );
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

int main( void )
{
  a_Init( SCREEN_WIDTH, SCREEN_HEIGHT, "Archimedes" );

  aInitGame();
  
  while( app.running ) {
    aMainloop();
  }

  a_Quit();

  return 0;
}

