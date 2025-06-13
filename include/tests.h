#ifndef __TESTS_H__
#define __TESTS_H__

#include <stdio.h>

// Test color codes for better visibility
#define TEST_COLOR_RESET   "\033[0m"
#define TEST_COLOR_GREEN   "\033[32m"
#define TEST_COLOR_RED     "\033[31m"
#define TEST_COLOR_BLUE    "\033[34m"
#define TEST_COLOR_YELLOW  "\033[33m"

// Global test counters (to be defined in each test file)
extern int total_tests;
extern int tests_passed;
extern int tests_failed;

// Simple test framework macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(TEST_COLOR_RED "❌ FAIL: %s" TEST_COLOR_RESET "\n", message); \
            printf("   Line %d in %s\n", __LINE__, __FILE__); \
            return 0; \
        } else { \
            printf(TEST_COLOR_GREEN "✅ PASS: %s" TEST_COLOR_RESET "\n", message); \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf(TEST_COLOR_BLUE "\n🧪 Running %s..." TEST_COLOR_RESET "\n", #test_func); \
        if (test_func()) { \
            printf(TEST_COLOR_GREEN "✅ %s PASSED" TEST_COLOR_RESET "\n", #test_func); \
            tests_passed++; \
        } else { \
            printf(TEST_COLOR_RED "❌ %s FAILED" TEST_COLOR_RESET "\n", #test_func); \
            tests_failed++; \
        } \
        total_tests++; \
    } while(0)

// Test suite header and footer macros
#define TEST_SUITE_START(suite_name) \
    printf(TEST_COLOR_YELLOW "🚀 Starting %s\n" TEST_COLOR_RESET, suite_name); \
    printf("==============================================\n");

#define TEST_SUITE_END() \
    printf(TEST_COLOR_YELLOW "\n📊 Test Results Summary\n" TEST_COLOR_RESET); \
    printf("======================\n"); \
    printf("Total Tests: %d\n", total_tests); \
    printf(TEST_COLOR_GREEN "✅ Passed: %d\n" TEST_COLOR_RESET, tests_passed); \
    printf(TEST_COLOR_RED "❌ Failed: %d\n" TEST_COLOR_RESET, tests_failed); \
    if (tests_failed == 0) { \
        printf(TEST_COLOR_GREEN "\n🎉 All tests passed! Great job!\n" TEST_COLOR_RESET); \
        return 0; \
    } else { \
        printf(TEST_COLOR_RED "\n💥 Some tests failed. Check the output above.\n" TEST_COLOR_RESET); \
        return 1; \
    }

// Quiet version for when running multiple test files
#define TEST_SUITE_END_QUIET() \
    printf("Total Tests: %d\n", total_tests); \
    printf("✅ Passed: %d\n", tests_passed); \
    printf("❌ Failed: %d\n", tests_failed); \
    if (tests_failed == 0) { \
        return 0; \
    } else { \
        return 1; \
    }

#endif
