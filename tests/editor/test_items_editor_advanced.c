// tests/editor/test_items_editor_advanced.c
#include "tests.h"
#include "items.h"
#include "editor.h"
#include "items_editor.h"

// --- Global Test Counters ---
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief Helper to check if a file exists.
 * @param filename The file to check.
 * @return true if the file exists, false otherwise.
 */
static bool file_exists(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

/**
 * @brief Tests the basic save and load cycle.
 */
int test_save_and_load_cycle(void)
{
    d_LogInfo("Testing basic save and load cycle...");
    const char* test_filename = "test_db.dat";

    // 1. Initialize the editor to get a database
    e_InitItemEditor();
    TEST_ASSERT(ie_GetItemCount() > 0, "Database should have items to save");

    // 2. Save the database
    int result = ie_save(test_filename);
    TEST_ASSERT(result == 0, "ie_save should return 0 on success");
    TEST_ASSERT(file_exists(test_filename), "Save file should be created on disk");
    
    // 3. (Placeholder) Call the load function
    result = ie_load(test_filename);
    TEST_ASSERT(result == 0, "ie_load stub should return 0 on success");
    
    // 4. Cleanup
    remove(test_filename);
    e_DestroyItemEditor();
    
    d_LogInfo("...save/load cycle test complete.");
    return 1;
}

/**
 * @brief Tests that loading a non-existent file is handled gracefully.
 */
int test_load_non_existent_file(void)
{
    d_LogInfo("Testing loading a non-existent file...");
    
    int result = ie_load("non_existent_file.dat");
    TEST_ASSERT(result == -1, "ie_load should return -1 for a file that does not exist");
    
    d_LogInfo("...non-existent file test complete.");
    return 1;
}


// --- Main Test Runner ---
int main(void)
{
    dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG, .colorize_output = true };
    d_SetGlobalLogger(d_CreateLogger(config));
    d_LogInfo("Starting Items Editor Advanced Tests");

    TEST_SUITE_START("Items Editor Advanced Tests");
    RUN_TEST(test_save_and_load_cycle);
    RUN_TEST(test_load_non_existent_file);
    TEST_SUITE_END();
}