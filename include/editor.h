// include/editor.h - Editor Subsystem
// Core editor framework and main application loop management

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "structs.h"

extern GlyphArray_t* game_glyphs;
extern aColor_t master_colors[MAX_COLOR_GROUPS][48];

/*
 * Initialize the complete Editor subsystem and application framework
 *
 * -- Sets up main application delegates for logic and drawing loops
 * -- Initializes global glyph array from CodePage737Font.png (9x16 glyphs)
 * -- Loads master color palette from resources/assets/colorpalette/colors.hex
 * -- Configures widget system from resources/widgets/editor/editor.json
 * -- Sets up tab container with callbacks for all editor modes (world, item, entity, colors, ui)
 * -- Establishes active widget as the main tab bar for editor navigation
 * -- Must be called after Archimedes initialization but before main loop
 * -- Safe to call multiple times (reinitializes components)
 * -- Prepares the complete editing environment for user interaction
 */
void e_InitEditor( void );

/*
 * Execute one frame of the main editor loop cycle
 *
 * -- Prepares scene rendering with Archimedes scene management
 * -- Calculates consistent delta time for frame-rate independent updates
 * -- Executes logic update using calculated delta time
 * -- Performs drawing operations using the same delta time value
 * -- Presents completed frame to the display buffer
 * -- Should be called continuously while application is running
 * -- Handles all timing synchronization and frame pacing
 * -- Maintains consistent performance across different hardware
 * -- Each call represents one complete render cycle
 */
void e_Mainloop( void );

/*
 * Clean up and destroy the Editor subsystem
 *
 * -- Frees allocated game_glyphs array and associated texture resources
 * -- Does not handle Archimedes cleanup (call a_Quit() separately)
 * -- Does not destroy individual editor subsystems (world, item, etc.)
 * -- Should be called before application termination
 * -- Safe to call even if initialization failed partially
 * -- After calling, editor functions become unsafe to use
 * -- Prevents memory leaks during application shutdown
 */
void e_DestroyEditor( void );

#endif