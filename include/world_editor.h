/*
 * world_editor.h:
 *
 * Copyright (c) 2025 Jacob Kellum <jkellum819@gmail.com>
 *                    Mathew Storm <smattymat@gmail.com>
 ************************************************************************
 */

#ifndef __WORLD_EDITOR_H__
#define __WORLD_EDITOR_H__

#include "structs.h"

extern World_t* map;

enum
{
  WEM_NONE,
  WEM_BRUSH,
  WEM_COPY,
  WEM_PASTE,
  WEM_MASS_CHANGE,
  WEM_SELECT,
  WEM_MAX
};

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
void we_Creation( void );

/*
 * Enter world editing mode (currently unimplemented placeholder)
 *
 * -- Intended for future implementation of world modification tools
 * -- Currently safe to call but performs no operations
 * -- Will eventually provide tile editing, region modification, etc.
 * -- Placeholder prevents crashes when UI buttons are activated
 * -- Implementation pending based on world structure requirements
 */
void we_Edit( void );

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
void we_Save( void );

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
void we_Load( void );

void we_DrawWorld( World_t* world, WorldPosition_t selected_pos,
                   WorldPosition_t highlighted_pos );

void we_DrawWorldCell( int i, int j,
                          GameTile_t* current_tile,
                          int selected_index, int highlight_index,
                          int selected_world_index, int highlight_world_index,
                          int x, int y, int w, int h );

void we_DrawEditorHotKeys( int x, int y, int key, int abbv0, int abbv1,
                           int abbv2, int abbv3 );

void wec_GenerateWorld( void );

void wes_SaveYes( void );
void wes_SaveNo( void );

#endif
