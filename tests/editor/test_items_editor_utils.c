// tests/editor/test_items_editor_utils.c
// Unit tests for Items Editor utility functions

#include "tests.h"
#include "items.h"
#include "structs.h"
#include "defs.h"
#include "Daedalus.h"
#include "Archimedes.h"
#include "editor.h"
#include "items_editor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Global test counters (managed by tests.h)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// =============================================================================
// HELPER FUNCTIONS FOR TESTING
// =============================================================================

/**
 * @brief Creates a standardized item database for utility tests.
 *
 * @param count Pointer to an integer that will store the number of created items.
 * @return A pointer to the heap-allocated item database.
 */
static Item_t* create_test_database_for_utils(int* count)
{
    *count = 3;
    Item_t* db = malloc(sizeof(Item_t) * (*count));
    if (!db) {
        *count = 0;
        return NULL;
    }

    // --- Item 0: Valid item (All fields initialized) ---
    db[0].name = d_InitString();
    d_SetString(db[0].name, "Vorpal Sword", 0);
    db[0].id = d_InitString();
    d_SetString(db[0].id, "weapon_sword_vorpal", 0);
    db[0].description = d_InitString();
    d_SetString(db[0].description, "A legendary blade.", 0);
    // FIX: Initialize the rarity string so the item is truly valid.
    db[0].rarity = d_InitString();
    d_SetString(db[0].rarity, "legendary", 0);
    db[0].type = ITEM_TYPE_WEAPON;
    db[0].weight_kg = 3.5f;

    // --- Item 1: Another valid item ---
    db[1].name = d_InitString();
    d_SetString(db[1].name, "Iron Key", 0);
    db[1].id = d_InitString();
    d_SetString(db[1].id, "key_iron_01", 0);
    db[1].description = d_InitString();
    d_SetString(db[1].description, "Unlocks a rusty door.", 0);
    // FIX: Initialize the rarity string.
    db[1].rarity = d_InitString();
    d_SetString(db[1].rarity, "common", 0);
    db[1].type = ITEM_TYPE_KEY;
    db[1].weight_kg = 0.1f;

    // --- Item 2: An item with invalid data for validation testing ---
    db[2].name = NULL; // Intentionally invalid name
    db[2].id = d_InitString();
    d_SetString(db[2].id, "item_invalid_01", 0);
    db[2].description = d_InitString();
    d_SetString(db[2].description, "This item is broken.", 0);
    db[2].rarity = d_InitString(); // Still initialize, even if empty
    d_SetString(db[2].rarity, "junk", 0);
    db[2].type = ITEM_TYPE_CONSUMABLE;
    db[2].weight_kg = -1.0f; // Intentionally invalid weight

    return db;
}

/**
 * @brief Cleans up the memory used by the test database.
 *
 * @param database The item database to free.
 * @param count The number of items in the database.
 */
static void cleanup_test_database_for_utils(Item_t* database, int count)
{
    if (!database) return;
    ie_FreeItemsDatabase(database, count);
}


// =============================================================================
// TEST CASES
// =============================================================================

/**
 * @brief Tests finding an item that exists in the database.
 */
int test_find_item_by_id_success(void)
{
    d_LogInfo("Testing find_item_by_id (success case)...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // Search for an item that we know exists
    Item_t* found_item = ie_FindItemByID(db, count, "key_iron_01");
    
    TEST_ASSERT(found_item != NULL, "Should find an existing item by ID");
    if (found_item) {
        TEST_ASSERT(strcmp(found_item->name->str, "Iron Key") == 0, "Found item should have the correct name");
    }
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...find_item_by_id (success case) test completed.");
    return 1;
}

/**
 * @brief Tests finding an item that does NOT exist in the database.
 */
int test_find_item_by_id_failure(void)
{
    d_LogInfo("Testing find_item_by_id (failure case)...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // Search for an item ID that does not exist
    Item_t* found_item = ie_FindItemByID(db, count, "non_existent_id");
    
    TEST_ASSERT(found_item == NULL, "Should return NULL for a non-existent ID");
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...find_item_by_id (failure case) test completed.");
    return 1;
}

/**
 * @brief Tests the item validator with a valid item.
 */
int test_validate_item_data_valid(void)
{
    d_LogInfo("Testing item data validator (valid case)...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // Test with the first item, which is valid
    bool is_valid = ie_ValidateItem(&db[0]);
    
    TEST_ASSERT(is_valid == true, "Validator should pass for a valid item");
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...item data validator (valid case) test completed.");
    return 1;
}

/**
 * @brief Tests the item validator with an invalid item.
 */
int test_validate_item_data_invalid(void)
{
    d_LogInfo("Testing item data validator (invalid case)...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // Test with the third item, which has a NULL name and negative weight
    bool is_valid = ie_ValidateItem(&db[2]);
    
    TEST_ASSERT(is_valid == false, "Validator should fail for an invalid item");
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...item data validator (invalid case) test completed.");
    return 1;
}

// =============================================================================
// DEBUG HUNTING & EDGE CASE TESTS
// =============================================================================

/**
 * @brief Tests finding an item in a database with zero items.
 */
int test_find_item_in_empty_database(void)
{
    d_LogInfo("Debug Hunt: Testing find_item_by_id with count = 0...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // Search for an item in a database that has 0 items to search
    Item_t* found_item = ie_FindItemByID(db, 0, "any_id");
    
    TEST_ASSERT(found_item == NULL, "Should return NULL when searching with count = 0");
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...find_item_by_id with count = 0 test completed.");
    return 1;
}

/**
 * @brief Tests passing a NULL id to the search function.
 */
int test_find_item_with_null_id(void)
{
    d_LogInfo("Debug Hunt: Testing find_item_by_id with a NULL id...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);
    
    // The function should be defensive and not crash when given a NULL search term
    Item_t* found_item = ie_FindItemByID(db, count, NULL);
    
    TEST_ASSERT(found_item == NULL, "Should return NULL when searching for a NULL ID");
    
    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...find_item_by_id with NULL id test completed.");
    return 1;
}

/**
 * @brief Tests validating an item with an out-of-bounds type enum.
 */
int test_validate_item_with_invalid_type(void)
{
    d_LogInfo("Debug Hunt: Testing validator with an out-of-bounds type...");
    Item_t bad_item = {0};
    bad_item.name = d_InitString();
    d_SetString(bad_item.name, "Corrupt Item", 0);
    bad_item.id = d_InitString();
    d_SetString(bad_item.id, "corrupt_id_01", 0);
    bad_item.rarity = d_InitString();
    d_SetString(bad_item.rarity, "common", 0);
    bad_item.description = d_InitString();
    d_SetString(bad_item.description, "A bad item.", 0);
    bad_item.weight_kg = 1.0f;
    
    // Set an item type that is outside the valid enum range
    bad_item.type = (ItemType_t)99; // Invalid type
    
    bool is_valid = ie_ValidateItem(&bad_item);
    
    TEST_ASSERT(is_valid == false, "Validator should fail for an out-of-bounds item type");

    // Clean up the manually created item's strings
    d_DestroyString(bad_item.name);
    d_DestroyString(bad_item.id);
    d_DestroyString(bad_item.rarity);
    d_DestroyString(bad_item.description);
    
    d_LogInfo("...validator with out-of-bounds type test completed.");
    return 1;
}

/**
 * @brief Tests validating an item that has an empty string for a required field.
 */
int test_validate_item_with_empty_string_id(void)
{
    d_LogInfo("Debug Hunt: Testing validator with an empty ID string...");
    Item_t bad_item = {0};
    bad_item.name = d_InitString();
    d_SetString(bad_item.name, "Item With No ID", 0);
    bad_item.id = d_InitString();
    d_SetString(bad_item.id, "", 0); // Set an empty string for the ID
    bad_item.rarity = d_InitString();
    d_SetString(bad_item.rarity, "common", 0);
    bad_item.description = d_InitString();
    d_SetString(bad_item.description, "An item with an empty ID.", 0);
    bad_item.type = ITEM_TYPE_KEY;
    bad_item.weight_kg = 1.0f;
    
    // Your updated validator should catch that the ID string has a length of 0
    bool is_valid = ie_ValidateItem(&bad_item);
    
    TEST_ASSERT(is_valid == false, "Validator should fail for an empty ID string");
    
    // Clean up
    d_DestroyString(bad_item.name);
    d_DestroyString(bad_item.id);
    d_DestroyString(bad_item.rarity);
    d_DestroyString(bad_item.description);
    
    d_LogInfo("...validator with an empty ID string test completed.");
    return 1;
}

// =============================================================================
// EPIC & STRESS TESTS
// =============================================================================

/* tests/editor/test_items_editor_utils.c */

/**
 * @brief Tests the ie_FilterItemsByType function for correctness.
 * This is an 'epic' test because it targets a new, more complex utility function.
 */
int test_filter_items_by_type(void)
{
    d_LogInfo("Epic Test: Filtering a mixed database by type...");
    int count = 0;
    Item_t* db = create_test_database_for_utils(&count);

    // Log the database for debugging
    for (int i = 0; i < count; i++) {
        d_LogDebugF("Item %d: %s", i, d_PeekString(db[i].name));
    }
    
    // We expect 1 WEAPON, 1 KEY, 1 CONSUMABLE from the helper function
    TEST_ASSERT(count == 3, "Database should contain 3 items for filtering");

    Item_t* filtered_list[3]; // A buffer to hold the results
    
    // FIX: Pass 'db' (Item_t*) directly, not '&db' (Item_t**).
    int weapon_count = ie_FilterItemsByType(db, count, ITEM_TYPE_WEAPON, filtered_list);
    TEST_ASSERT(weapon_count == 1, "Should find exactly one weapon");
    if (weapon_count == 1) {
        TEST_ASSERT(strcmp(filtered_list[0]->id->str, "weapon_sword_vorpal") == 0, "Filtered weapon should be the Vorpal Sword");
    }

    // FIX: Pass 'db' (Item_t*) directly, not '&db' (Item_t**).
    int key_count = ie_FilterItemsByType(db, count, ITEM_TYPE_KEY, filtered_list);
    TEST_ASSERT(key_count == 1, "Should find exactly one key");
    if (key_count == 1) {
        TEST_ASSERT(strcmp(filtered_list[0]->id->str, "key_iron_01") == 0, "Filtered key should be the Iron Key");
    }

    cleanup_test_database_for_utils(db, count);
    d_LogInfo("...item filtering test completed.");
    return 1;
}


/**
 * @brief Stress tests the validator by creating and validating items in a tight loop.
 * This uses your custom LOOP_TEST macros.
 */
int test_validator_stress_loop(void)
{
    d_LogInfo("Epic Test: Stress-testing the validator in a high-frequency loop...");
    
    LOOP_TEST_START();
    for (int i = 0; i < 100; ++i) {
        Item_t test_item = {0};
        test_item.name = d_InitString();
        test_item.id = d_InitString();
        test_item.rarity = d_InitString();
        test_item.description = d_InitString();
        
        // Alternate between creating a valid and invalid item
        if (i % 2 == 0) {
            d_SetString(test_item.name, "Valid Looping Item", 0);
            d_SetString(test_item.id, "loop_valid", 0);
            d_SetString(test_item.rarity, "common", 0);
            d_SetString(test_item.description, "desc", 0);
            test_item.type = ITEM_TYPE_WEAPON;
            test_item.weight_kg = 1.0f;
            TEST_ASSERT(ie_ValidateItem(&test_item), "Looped validation should pass for valid item");
        } else {
            d_SetString(test_item.name, "Invalid Looping Item", 0);
            d_SetString(test_item.id, "", 0); // Invalid empty ID
            d_SetString(test_item.rarity, "common", 0);
            d_SetString(test_item.description, "desc", 0);
            test_item.type = ITEM_TYPE_ARMOR;
            test_item.weight_kg = 1.0f;
            TEST_ASSERT(!ie_ValidateItem(&test_item), "Looped validation should fail for invalid item");
        }
        
        // Cleanup inside the loop to test memory stability
        d_DestroyString(test_item.name);
        d_DestroyString(test_item.id);
        d_DestroyString(test_item.rarity);
        d_DestroyString(test_item.description);
    }
    LOOP_TEST_END();
    
    d_LogInfo("...validator stress test completed.");
    return 1;
}

/**
 * @brief Tests if ie_FindItemByID can safely navigate a database with corrupted items.
 */
int test_find_item_in_corrupted_database(void)
{
    d_LogInfo("Epic Test: Searching for an item past a corrupted entry...");
    int count = 3;
    Item_t* db = malloc(sizeof(Item_t) * count);

    // Item 0: Valid
    db[0].name = d_InitString(); d_SetString(db[0].name, "First Item", 0);
    db[0].id = d_InitString(); d_SetString(db[0].id, "first_item", 0);
    db[0].rarity = d_InitString(); d_SetString(db[0].rarity, "common", 0);
    db[0].description = d_InitString(); d_SetString(db[0].description, "desc", 0);

    // Item 1: Corrupted (NULL id pointer)
    db[1].name = d_InitString(); d_SetString(db[1].name, "Corrupt Item", 0);
    db[1].id = NULL; // The whole dString object is NULL
    db[1].rarity = d_InitString(); d_SetString(db[1].rarity, "common", 0);
    db[1].description = d_InitString(); d_SetString(db[1].description, "desc", 0);

    // Item 2: The target item, after the corrupted one
    db[2].name = d_InitString(); d_SetString(db[2].name, "Target Item", 0);
    db[2].id = d_InitString(); d_SetString(db[2].id, "target_item", 0);
    db[2].rarity = d_InitString(); d_SetString(db[2].rarity, "common", 0);
    db[2].description = d_InitString(); d_SetString(db[2].description, "desc", 0);

    // The function should safely skip item 1 and find item 2
    Item_t* found_item = ie_FindItemByID(db, count, "target_item");
    TEST_ASSERT(found_item != NULL, "Should find item even with corrupted data in list");
    if (found_item) {
        TEST_ASSERT(found_item == &db[2], "Should return a pointer to the correct target item");
    }

    // Manual cleanup since the helper assumes valid items
    d_DestroyString(db[0].name); d_DestroyString(db[0].id); d_DestroyString(db[0].rarity); d_DestroyString(db[0].description);
    d_DestroyString(db[1].name); /* id is NULL */ d_DestroyString(db[1].rarity); d_DestroyString(db[1].description);
    d_DestroyString(db[2].name); d_DestroyString(db[2].id); d_DestroyString(db[2].rarity); d_DestroyString(db[2].description);
    free(db);

    d_LogInfo("...corrupted database search test completed.");
    return 1;
}

/**
 * @brief Tests the validator against a dString whose internal 'str' is NULL.
 */
int test_validate_item_with_corrupt_internal_string(void)
{
    d_LogInfo("Epic Test: Validating an item with an internally corrupt dString...");
    Item_t bad_item = {0};
    bad_item.name = d_InitString(); // The dString object itself is valid...
    free(bad_item.name->str);
    bad_item.name->str = NULL;   // ...but its internal character pointer is corrupt (NULL).
    bad_item.name->len = 0;
    bad_item.name->alloced = 0;

    // Fill out other fields to be valid
    bad_item.id = d_InitString(); d_SetString(bad_item.id, "valid_id", 0);
    bad_item.rarity = d_InitString(); d_SetString(bad_item.rarity, "common", 0);
    bad_item.description = d_InitString(); d_SetString(bad_item.description, "desc", 0);
    bad_item.type = ITEM_TYPE_KEY;
    bad_item.weight_kg = 1.0f;
    
    // The validator should be robust enough to handle this specific corruption.
    bool is_valid = ie_ValidateItem(&bad_item);
    
    TEST_ASSERT(is_valid == false, "Validator should fail if a dString's internal 'str' is NULL");
    
    // Manual cleanup
    free(bad_item.name); // Since we broke the dString, we can't use d_DestroyString
    d_DestroyString(bad_item.id);
    d_DestroyString(bad_item.rarity);
    d_DestroyString(bad_item.description);
    
    d_LogInfo("...internally corrupt dString test completed.");
    return 1;
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void)
{
    // Initialize the Daedalus logger
    dLogConfig_t config = { .default_level = D_LOG_LEVEL_DEBUG, .colorize_output = true };
    d_SetGlobalLogger(d_CreateLogger(config));

    d_LogInfo("Starting Items Editor Utility Function Tests");
    
    TEST_SUITE_START("Items Editor Utility Tests");

    // Core functionality tests
    RUN_TEST(test_find_item_by_id_success);
    RUN_TEST(test_find_item_by_id_failure);
    RUN_TEST(test_validate_item_data_valid);
    RUN_TEST(test_validate_item_data_invalid);

    // Debug hunting & edge case tests
    RUN_TEST(test_find_item_in_empty_database);
    RUN_TEST(test_find_item_with_null_id);
    RUN_TEST(test_validate_item_with_invalid_type);
    RUN_TEST(test_validate_item_with_empty_string_id);

    // Epic & stress tests
    RUN_TEST(test_filter_items_by_type);
    RUN_TEST(test_validator_stress_loop);
    RUN_TEST(test_find_item_in_corrupted_database);
    RUN_TEST(test_validate_item_with_corrupt_internal_string);

    // Clean up the logger
    d_DestroyLogger(d_GetGlobalLogger());

    TEST_SUITE_END();
}