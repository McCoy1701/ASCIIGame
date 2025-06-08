#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Archimedes.h"
#include "game.h"

void g_ChangeColor( uint32_t hex_color_value );
static void aDoLoop( float );
static void aRenderLoop( float );

SDL_Surface* surf;
World_t* g_world;
aColor_t grid_color;

EMSCRIPTEN_KEEPALIVE
void g_ChangeColor( uint32_t hex_color_value )
{
  uint8_t a = 0xff;
  uint8_t b = hex_color_value;
  uint8_t g = hex_color_value >> 8;
  uint8_t r = hex_color_value >> 16;
  aColor_t new_color = { r, g, b, a };
  grid_color = new_color;
}

void aInitGame( void )
{
  app.delegate.logic = aDoLoop;
  app.delegate.draw  = aRenderLoop;
  
  surf = a_Image( "resources/assets/bullet.png" );
  if ( surf == NULL )
  {
    printf( "Failed to load image\n" );
  }

  g_world = ( World_t* )malloc( sizeof( World_t ) );
  if ( g_world == NULL )
  {
    aError_t new_error;
    new_error.error_type = FATAL;
    snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world",
              log_level_strings[new_error.error_type] );
    LOG( new_error.error_msg );
  }

  g_world->regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) * REGION_SIZE * REGION_SIZE );
  if ( g_world->regions == NULL )
  {
    aError_t new_error;
    new_error.error_type = FATAL;
    snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world->regions",
              log_level_strings[new_error.error_type] );
    LOG( new_error.error_msg );

  }

//  grid_color = blue;
  //g_ChangeColor( 0xff0000 );
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
  g_DrawWorldMap( g_world, grid_color );
  //a_DrawFilledRect( 100, 100, 32, 32, blue );
  //a_DrawFilledRect( 300, 300, 32, 32, red );
  //a_Blit( surf, 200, 200 );

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

  #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop( aMainloop, 0, 1 );
  #endif

  #ifndef __EMSCRIPTEN__
    while( app.running ) {
      aMainloop();
    }
  #endif

  a_Quit();

  return 0;
}

