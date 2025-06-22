// ASCIIGame/test_framework_demo.c
// Test program to demonstrate the improved test framework features

#include "include/tests.h"

// Global test counters (required by framework)
int total_tests = 0;
int tests_passed = 0;
int tests_failed = 0;

// Demo test function that passes all assertions
int test_passing_function() {
    TEST_ASSERT(1 == 1, "Basic equality should work");
    TEST_ASSERT(2 + 2 == 4, "Basic arithmetic should work");
    TEST_ASSERT(5 > 3, "Basic comparison should work");
    TEST_ASSERT("hello" != NULL, "String literal should not be NULL");
    return 1;
}

// Demo test function that fails on the third assertion
int test_failing_function() {
    TEST_ASSERT(1 == 1, "First assertion should pass");
    TEST_ASSERT(2 + 2 == 4, "Second assertion should pass");
    TEST_ASSERT(5 < 3, "Third assertion should fail - this is intentional");
    TEST_ASSERT("unreachable" != NULL, "This should not be reached");
    return 1;
}

// Demo test function with fast execution
int test_fast_function() {
    TEST_ASSERT(1, "Quick assertion 1");
    TEST_ASSERT(1, "Quick assertion 2");
    TEST_ASSERT(1, "Quick assertion 3");
    return 1;
}

// Demo test function with many assertions for combo testing
int test_combo_function() {
    TEST_ASSERT(1, "Combo test 1");
    TEST_ASSERT(1, "Combo test 2");
    TEST_ASSERT(1, "Combo test 3");
    TEST_ASSERT(1, "Combo test 4");
    TEST_ASSERT(1, "Combo test 5");
    TEST_ASSERT(1, "Combo test 6");
    TEST_ASSERT(1, "Combo test 7");
    TEST_ASSERT(1, "Combo test 8");
    TEST_ASSERT(1, "Combo test 9");
    TEST_ASSERT(1, "Combo test 10");
    return 1;
}

// Test that shows XP penalty system
int test_penalty_demo() {
    TEST_ASSERT(1 == 1, "This should pass");
    TEST_ASSERT(10 == 5, "This should fail - demonstrating XP penalty");
    return 1;
}

// Another failing test to show escalating penalties
int test_another_failure() {
    TEST_ASSERT(1 == 1, "This passes first");
    TEST_ASSERT(0 == 1, "This fails - second failure means -200 XP penalty");
    return 1;
}

int main() {
    TEST_SUITE_START("Framework Demo Suite");

    RUN_TEST(test_passing_function);
    RUN_TEST(test_failing_function);
    RUN_TEST(test_fast_function);
    RUN_TEST(test_combo_function);
    R
