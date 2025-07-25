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

World_t* g_world;
aColor_t grid_color;

WorldPosition_t current_pos;
char* pos_text;

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
  
  pos_text = malloc( sizeof(char) * 50 );

  current_pos = (WorldPosition_t){ .world_index = 0, .region_index = 0,
    .local_index = 0, .level = 0, .local_z = 0 };
  snprintf(pos_text, 50, "%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.region_index, current_pos.local_index, current_pos.level,
           current_pos.local_z );
  
  game_glyphs = e_InitGlyphs( "resources/fonts/CodePage737Font.png",
                               9, 16 );
  g_world = init_world( WORLD_WIDTH_SMALL, WORLD_HEIGHT_SMALL, 
                        REGION_WIDTH_SMALL, REGION_HEIGHT_SMALL,
                        LOCAL_WIDTH_SMALL, LOCAL_HEIGHT_SMALL, Z_HEIGHT_SMALL );

  originX = SCREEN_WIDTH/2;
  originY = SCREEN_HEIGHT/2;

  grid_color = blue;
  //g_ChangeColor( 0xff0000 );
}

static void aDoLoop( float dt )
{
  a_DoInput();
  
  if ( app.mouse.button == 1 )
  {
    app.mouse.button = 0;
    if ( g_world != NULL )
    {
      e_MapMouseCheck( g_world, &current_pos );
    }
  }
  
  if ( app.keyboard[SDL_SCANCODE_RETURN] == 1 )
  {
    app.keyboard[SDL_SCANCODE_RETURN] = 0;

    if ( current_pos.level >= 0 && current_pos.level < LOCAL_LEVEL )
    {
      current_pos.level++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_BACKSPACE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

    if ( current_pos.level > 0 && current_pos.level <= LOCAL_LEVEL )
    {
      current_pos.level--;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_EQUALS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_EQUALS] = 0;
    if ( current_pos.local_z >= 0 && current_pos.local_z < g_world->z_height - 1 )
    {
      current_pos.local_z++;
    }

  }
  
  if ( app.keyboard[SDL_SCANCODE_MINUS] == 1 )
  {
    app.keyboard[SDL_SCANCODE_MINUS] = 0;
    if ( current_pos.local_z > 0 && current_pos.local_z <= g_world->z_height )
    {
      current_pos.local_z--;
    }

  }

  if ( app.keyboard[SDL_SCANCODE_ESCAPE] == 1 )
  {
    app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
    app.running = 0;
  }
}

static void aRenderLoop( float dt )
{
  if ( g_world != NULL )
  {
    int draw_size = 0;
    switch ( current_pos.level )
    {
      case WORLD_LEVEL:
        draw_size = g_world->world_width * g_world->world_height;         
        break;

      case REGION_LEVEL:
        draw_size = g_world->region_width * g_world->region_height;
        break;

      case LOCAL_LEVEL:
        draw_size = g_world->local_width * g_world->local_height;                      
        break;
    }

    for ( uint16_t i = 0; i < draw_size; i++ )
    {
      we_DrawWorldCell( i, g_world, current_pos );
    }

    snprintf(pos_text, 50, "%d,%d,%d,%d,%d\n", current_pos.world_index,
           current_pos.region_index, current_pos.local_index, current_pos.level,
           current_pos.local_z );
    a_DrawText( pos_text, 750, 10, 255, 255, 255, app.font_type, TEXT_ALIGN_CENTER, 0 );

  }

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

  free_world( g_world, ( g_world->world_width * g_world->world_height ),
                   ( g_world->region_width * g_world->region_height ) );
  g_world = NULL;
  free( pos_text );
  pos_text = NULL;
  
  a_Quit();

  return 0;
}

