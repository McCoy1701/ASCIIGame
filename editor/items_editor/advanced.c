// editor/items_editor/advanced.c
// Advanced features for the items editor, like saving and loading.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "items_editor.h"
#include "Daedalus.h"
#include "items.h"

#define SAVE_FILE_VERSION 1

/**
 * @brief Saves the current item database to a file.
 */
int ie_save(const char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        d_LogErrorF("Failed to open file for writing: %s", filename);
        return -1;
    }

    d_LogInfoF("Saving %d items to %s...", ie_GetItemCount(), filename);
    fprintf(fp, "VERSION %d\n", SAVE_FILE_VERSION);
    fprintf(fp, "COUNT %d\n", ie_GetItemCount());

    Item_t* db = ie_GetDatabase();
    for (int i = 0; i < ie_GetItemCount(); i++) {
        fprintf(fp, "ITEM %d\n", i);
        fprintf(fp, "  name %s\n", d_PeekString(db[i].name));
        fprintf(fp, "  id %s\n", d_PeekString(db[i].id));
        fprintf(fp, "  type %d\n", db[i].type);
        // Add other properties to save here...
        fprintf(fp, "END_ITEM\n");
    }

    fclose(fp);
    d_LogInfo("Database saved successfully.");
    return 0;
}


/**
 * @brief Loads an item database from a file, replacing the current one.
 */
int ie_load(const char* filename)
{
    // For a real implementation, you would free the old database first.
    // e_DestroyItemEditor(); ...then load the new one.
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        d_LogErrorF("Failed to open file for reading: %s", filename);
        return -1;
    }
    
    d_LogInfoF("Loading item database from %s...", filename);

    // This is a placeholder for a more robust parser. A real implementation
    // would read each line, parse the key-value pairs, and use create_weapon,
    // create_armor, etc., to reconstruct the items.
    
    // For now, we'll just log that the action is not fully implemented.
    d_LogWarning("Item database loading is a STUB and does not fully parse the file yet.");

    fclose(fp);
    return 0;
}