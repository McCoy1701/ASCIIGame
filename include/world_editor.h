#ifndef __WORLD_EDITOR_H__
#define __WORLD_EDITOR_H__

#include "structs.h"

extern World_t* map;

/*
 * Initialize the World Editor subsystem and set up the UI framework
 *
 * -- Sets up the delegate functions for world editor logic and rendering loops
 * -- Initializes widget system from "resources/widgets/editor/world.json"
 * -- Configures tab container with action callbacks for all editor modes
 * -- Sets up world menu bar with creation, edit, save, and load actions
 * -- Must be called before any other world editor functions
 * -- Automatically switches application state to world editor mode
 * -- Safe to call multiple times (reinitializes UI components)
 */
void e_InitWorldEditor( void );

/*
 * Clean up and destroy the World Editor subsystem
 *
 * -- Frees the global world map if it exists
 * -- Cleans up allocated world regions and memory structures
 * -- Does not destroy the widget system (handled separately)
 * -- Should be called when exiting world editor mode
 * -- Safe to call even if no world is currently loaded
 * -- After calling, world editor functions become unsafe to use
 */
void e_DestroyWorldEditor( void );
void e_GetCellSize( int index, int width, int height,
                    int* x, int* y, int* w, int* h );
void e_GetCellAtMouse( int width, int height, int originx, int originy,
                       int cell_width, int cell_height, uint8_t* grid_x,
                       uint8_t* grid_y, int centered );
void e_MapMouseCheck( WorldPosition_t* pos );
void e_GlyphMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y );
void e_ColorMouseCheck( int* index, uint8_t* grid_x, uint8_t* grid_y );
void e_LevelZHeightCheck( WorldPosition_t* pos );
void e_LoadColorPalette( aColor_t palette[MAX_COLOR_GROUPS][MAX_COLOR_PALETTE],
                       const char * filename );

/*
 * Enter world creation mode with generation parameters UI
 *
 * -- Switches to world creation logic and rendering loops
 * -- Shows the generation menu with size selection dropdowns
 * -- Sets up callbacks for world generation with user parameters
 * -- User can select world size, region size, local size, and Z-height
 * -- ESC key returns to main world editor interface
 * -- Does not generate a world until user clicks "generate" button
 * -- Replaces current world map if one exists
 */
void we_creation( void );

/*
 * Enter world editing mode (currently unimplemented placeholder)
 *
 * -- Intended for future implementation of world modification tools
 * -- Currently safe to call but performs no operations
 * -- Will eventually provide tile editing, region modification, etc.
 * -- Placeholder prevents crashes when UI buttons are activated
 * -- Implementation pending based on world structure requirements
 */
void we_edit( void );

/*
 * Save the current world to persistent storage (currently unimplemented)
 *
 * -- Intended for serializing world data to file format
 * -- Currently safe to call but performs no operations
 * -- Will eventually support multiple save formats and locations
 * -- Placeholder prevents crashes when save button is activated
 * -- Implementation requires definition of world file format
 * -- Should validate world data before attempting save operation
 */
void we_save( void );

/*
 * Load a world from persistent storage (currently unimplemented)
 *
 * -- Intended for deserializing world data from file format
 * -- Currently safe to call but performs no operations
 * -- Will eventually support file browser and format validation
 * -- Placeholder prevents crashes when load button is activated
 * -- Implementation requires world file format specification
 * -- Should free existing world before loading new one
 */
void we_load( void );

void we_DrawWorldCell( int index, World_t* map, WorldPosition_t pos, WorldPosition_t highlight );
void we_DrawSelectGrid( World_t* map, WorldPosition_t pos, WorldPosition_t highlight );
void wec_GenerateWorld( void );

#endif
