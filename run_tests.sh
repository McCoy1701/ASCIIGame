#!/bin/bash
# File: run_tests.sh - Clean ASCIIGame Test Runner

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
PURPLE='\033[0;35m'
NC='\033[0m'

echo -e "${PURPLE}🚀 ASCIIGame Test Suite Runner${NC}"
echo "==================================="

# Build test dependencies
echo -e "${BLUE}🔧 Building test dependencies...${NC}"
if ! make -s always > /dev/null 2>&1; then
    echo -e "${RED}❌ Failed to build test dependencies${NC}"
    exit 1
fi

# Counter variables
TOTAL_FILES=0
PASSED_FILES=0
FAILED_FILES=()

# Function to run a test
run_test() {
    local test_name="$1"
    local make_target="$2"

    echo -e "${BLUE}📋 Running $test_name...${NC}"

    TOTAL_FILES=$((TOTAL_FILES + 1))

    # Run the test and capture exit code
    if make -s "$make_target" > /dev/null 2>&1; then
        echo -e "  ${GREEN}✅ $test_name: All tests passed${NC}"
        PASSED_FILES=$((PASSED_FILES + 1))
    else
        echo -e "  ${RED}❌ $test_name: Some tests failed${NC}"
        FAILED_FILES+=("$test_name:$make_target")
    fi
}

# Run all tests
echo ""
run_test "Item Creation & Destruction" "run-test-items-creation-destruction"
run_test "Item Type Checking & Access" "run-test-items-type-checking"
run_test "Material System" "run-test-items-material-system"
run_test "Item Properties & Stats" "run-test-items-properties"
run_test "Item Durability System" "run-test-items-durability"
run_test "Inventory Management" "run-test-items-inventory"
run_test "Item Usage & Effects" "run-test-items-usage"



# Final Summary
FAILED_COUNT=$((TOTAL_FILES - PASSED_FILES))

echo ""
echo -e "${PURPLE}📊 Global Test Results Summary${NC}"
echo "=============================================="
echo -e "Test Files Run: ${YELLOW}$TOTAL_FILES${NC}"
echo -e "✅ Passed:      ${GREEN}$PASSED_FILES${NC}"
echo -e "❌ Failed:      ${RED}$FAILED_COUNT${NC}"

if [ ${#FAILED_FILES[@]} -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 All tests passed across all files! Excellent work!${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}💥 Some tests failed in the following files:${NC}"
    for failed in "${FAILED_FILES[@]}"; do
        name=$(echo "$failed" | cut -d':' -f1)
        target=$(echo "$failed" | cut -d':' -f2)
        echo -e "  ${RED}• $name${NC}"
        echo -e "    To see detailed failures, run: ${YELLOW}make $target${NC}"
    done
    echo ""
    echo -e "${YELLOW}💡 Tip: Run individual test files to see specific function failures${NC}"
    exit 1
fi
