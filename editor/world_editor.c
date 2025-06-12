#include <stdio.h>

#include "Archimedes.h"
#include "defs.h"
#include "init_editor.h"
#include "save_editor.h"

static void aDoLoop( float );
static void aRenderLoop( float );

LocalCell_t* test_cells;

void aInitGame( void )
{
  app.delegate.logic = aDoLoop;
  app.delegate.draw  = aRenderLoop;
  /*LocalCell_t* new_local = ( LocalCell_t* )malloc( sizeof(LocalCell_t) );
  if ( new_local == NULL )
  {
    printf( "Failed to allocate memory for new_local\n" );
  }
  
  new_local->tiles = ( GameTile_t* )malloc( sizeof(GameTile_t) * Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE );
  if ( new_local->tiles == NULL )
  {
    printf( "Failed to allocate memory for new_local->tiles\n" );
  }

  for ( int i = 0; i < ( Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE ); i++ )
  {
    new_local->tiles[i].elevation   = 0;
    new_local->tiles[i].terrain     = 'G';
    new_local->tiles[i].temperature = 20;
    new_local->tiles[i].isOccupied  = 0;
  }*/

  //SaveTest( new_local, "resources/world/test.dat" );
  test_cells = LoadTest( "resources/world/test.dat" );

  for ( int i = 0; i < Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE; i++ )
  {
    printf( "tile: %d %c\n",i, test_cells->tiles[i].terrain  );
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

