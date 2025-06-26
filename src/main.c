#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Archimedes.h"
#include "game.h"

void g_ChangeColor( uint32_t hex_color_value );
static void aDoLoop( float );
static void aRenderLoop( float );
static void aRenderWorld( float dt );
static void aRenderRegion( float dt );
static void aRenderLocal( float dt );

SDL_Surface* surf;
World_t* g_world;
aColor_t grid_color;

int originX;
int originY;
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
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

  /*g_world = ( World_t* )malloc( sizeof( World_t ) );
  if ( g_world == NULL )
  {
    aError_t new_error;
    new_error.error_type = FATAL;
    snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world",
              log_level_strings[new_error.error_type] );
    LOG( new_error.error_msg );
  }

  g_world->regions = ( RegionCell_t* )malloc( sizeof( RegionCell_t ) * WORLD_WIDTH * WORLD_HEIGHT );
  if ( g_world->regions == NULL )
  {
    aError_t new_error;
    new_error.error_type = FATAL;
    snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world->regions",
              log_level_strings[new_error.error_type] );
    LOG( new_error.error_msg );
    
    free( g_world );
  }

  for ( int r = 0; r < REGION_SIZE * REGION_SIZE; r++ )
  {
    g_world->regions[r].cells = ( LocalCell_t* )malloc( sizeof( LocalCell_t ) * REGION_SIZE * REGION_SIZE );
    if ( g_world->regions[r].cells == NULL )
    {
      aError_t new_error;
      new_error.error_type = FATAL;
      snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world->regions",
               log_level_strings[new_error.error_type] );
      LOG( new_error.error_msg );

      for ( int i = 0; i < r; i++ )
      {
        free( g_world->regions[i].cells );
      }

      free( g_world->regions );
      free( g_world );
    }

    for ( int c = 0; c < REGION_SIZE * REGION_SIZE; c++ )
    {
      g_world->regions[r].cells[c].tiles = ( GameTile_t* )malloc( sizeof( GameTile_t ) * Z_HEIGHT * LOCAL_SIZE * LOCAL_SIZE );
      if ( g_world->regions[r].cells[c].tiles == NULL )
      {
        aError_t new_error;
        new_error.error_type = FATAL;
        snprintf( new_error.error_msg, MAX_LINE_LENGTH, "%s: Failed to allocate memory for g_world->regions",
                 log_level_strings[new_error.error_type] );
        LOG( new_error.error_msg );
        
        for ( int i = 0; i < r; i++ )
        {
          for ( int j = 0; j < REGION_SIZE * REGION_SIZE; j++ )
          {
            if ( g_world->regions[i].cells[j].tiles != NULL )
            {
              free( g_world->regions[i].cells[j].tiles );
            }
          }

          free( g_world->regions[i].cells );
        }

        for ( int j = 0; j < c; j++ )
        {
          if ( g_world->regions[r].cells[j].tiles != NULL )
          {
            free( g_world->regions[r].cells[j].tiles );
          }
        }
        
        free( g_world->regions[r].cells );
        free( g_world->regions );
        free( g_world );
      }
    }

  }*/
  
  originX = SCREEN_WIDTH/2;
  originY = SCREEN_HEIGHT/2;

  grid_color = blue;
  //g_ChangeColor( 0xff0000 );
}

static void aDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.keyboard[ SDL_SCANCODE_ESCAPE ] == 1 )
  {
    app.running = 0;
  }
  
  if ( app.keyboard[ SDL_SCANCODE_W ] == 1 )
  {
    app.delegate.draw = aRenderWorld;
  }
  
  if ( app.keyboard[ SDL_SCANCODE_R ] == 1 )
  {
    app.delegate.draw = aRenderRegion;
  }
  
  if ( app.keyboard[ SDL_SCANCODE_L ] == 1 )
  {
    app.delegate.draw = aRenderLocal;
  }

  if ( app.mouse.wheel == 1 )
  {
    printf("scroll up\n");
    app.mouse.wheel = 0;
  }
  
}

static void aRenderLoop( float dt )
{
  a_DrawFilledRect( 100, 100, 32, 32,   0, 0, 255, 255 );
  a_DrawFilledRect( 300, 300, 32, 32, 255, 0,   0, 255 );
  a_Blit( surf, 200, 200 );

}

static void aRenderWorld( float dt )
{
  a_DrawText( "world", 600, 100, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0 );
  g_DrawWorldMap( originX, originY, grid_color );
  //a_DrawFilledRect( 100, 100, 32, 32, blue );
  //a_DrawFilledRect( 300, 300, 32, 32, red );
  //a_Blit( surf, 200, 200 );

}

static void aRenderRegion( float dt )
{
  a_DrawText( "region", 600, 100, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0 );
  g_DrawRegionMap( originX, originY, grid_color );
  //a_DrawFilledRect( 100, 100, 32, 32, blue );
  //a_DrawFilledRect( 300, 300, 32, 32, red );
  //a_Blit( surf, 200, 200 );

}

static void aRenderLocal( float dt )
{
  a_DrawText( "region", 600, 100, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0 );
  g_DrawLocalMap( originX, originY, grid_color );
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

