#!/bin/bash
# File: run_tests.sh - Enhanced Test Runner with Sisyphus Progress Tracking
# This testing framework is designed for Dopamine-driven development
# It should Gamify, and help coders with ADHD feel a sense of accomplishment.
# It should also provide a thematic experience within the world of Greek mythology.
# The Sisyphus legend serves as a metaphor for perseverance and the pursuit of perfection.

# Set environment variable to indicate this is a full test run (1/N XP where N = successfully run files, no achievements)
export SISYPHUS_FULL_TEST_MODE=1

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
PURPLE='\033[0;35m'
BOLD_WHITE='\033[1;37m'
CYAN='\033[0;36m'
ORANGE='\033[0;93m'
NC='\033[0m'

# Sisyphus archive system for tracking progress
SISYPHUS_DIR=".sisyphus"
SISYPHUS_FILE=".sisyphus"  # Legacy file for migration
SISYPHUS_LEGEND="sisyphus.legend"  # Permanent lifetime stats

# Color codes for faded italic explanations
FADED='\033[2;3m'  # Faded italic text
GRAY='\033[2m'     # Dim/faded text

# Function to format numbers with commas (e.g., 107020 -> 107,020)
format_number_with_commas() {
    local number="$1"
    echo "$number" | sed ':a;s/\B[0-9]\{3\}\>/,&/;ta'
}

# Function to map make targets to correct test file paths
# Used for generating clickable hyperlinks in terminal output

map_target_to_file_path() {
    local target="$1"
    case "$target" in
        "run-test-items-creation-destruction")
            echo "tests/items/test_items_creation_destruction.c"
            ;;
        "run-test-items-type-checking")
            echo "tests/items/test_items_type_checking_and_access.c"
            ;;
        "run-test-items-material-system")
            echo "tests/items/test_items_material_system.c"
            ;;
        "run-test-items-properties")
            echo "tests/items/test_items_properties.c"
            ;;
        "run-test-items-durability")
            echo "tests/items/test_items_durability.c"
            ;;
        "run-test-items-inventory")
            echo "tests/items/test_items_inventory.c"
            ;;
        "run-test-items-usage")
            echo "tests/items/test_items_usage.c"
            ;;
        "run-test-items-helper-functions")
            echo "tests/items/test_items_helper_functions.c"
            ;;
        *)
            # Fallback for unknown targets
            echo "tests/${target#*test-}.c"
            ;;
    esac
}
# Function to copy text to clipboard using OSC 52 (works in many terminals)
copy_to_clipboard() {
    local text="$1"
    # OSC 52 sequence to copy to clipboard
    printf '\e]52;c;%s\e\\' "$(printf '%s' "$text" | base64 | tr -d '\n')"
}

# Greek Mythology-themed Sisyphus messages for dopamine-driven development
LEGENDARY_MESSAGES=(
    "✨ ZEUS HIMSELF APPLAUDS! Your boulder has reached Olympus!"
    "👑 DIVINE ASCENSION! You've earned the respect of the gods!"
    "🏛️ LEGENDARY TRIUMPH! Sisyphus weeps with joy at your mastery!"
    "🌟 IMMORTAL ACHIEVEMENT! Your name shall echo through eternity!"
    "🔥 PROMETHEUS PROUD! You've stolen the fire of perfect code!"
)

EXCELLENT_MESSAGES=(
    "💎 HERCULEAN STRENGTH! Your twelve labors of debugging are complete!"
    "🚀 ICARUS ASCENDANT! Flying high without melting your wings!"
    "⭐ ATHENA'S WISDOM! Strategic debugging guided by divine intellect!"
    "💪 ATLAS STRONG! Carrying the weight of complex code with ease!"
    "🎯 APOLLO'S AIM! Each bug elimination precise as golden arrows!"
)

GOOD_MESSAGES=(
    "✨ SISYPHEAN PROGRESS! Each push moves the boulder higher!"
    "🎨 HEPHAESTUS CRAFT! Forging code with divine artisanship!"
    "🛠️ DAEDALUS BUILDER! Constructing labyrinths of elegant logic!"
    "🏗️ FOUNDATION OF TROY! Your code stands strong against all sieges!"
    "📈 HERMES SPEED! Swift progress carries messages of success!"
)

ENCOURAGEMENT_MESSAGES=(
    "🌱 PERSEPHONE'S RETURN! From darkness comes renewed growth!"
    "🎪 ODYSSEY CONTINUES! Every hero faces trials before triumph!"
    "🦾 TITAN STRENGTH! Growing mightier with each challenge!"
    "🧩 SPHINX'S RIDDLE! You love solving the impossible, don't you?"
    "✨ SPARTAN SPIRIT! This code shall not pass without a fight!"
    "🏛️ EPICTETUS REMINDS! 'It's not what happens, but how you react!'"
    "📚 SENECA'S WISDOM! 'Every new beginning comes from some other beginning's end!'"
    "✨ MARCUS AURELIUS! 'The impediment to action advances action. What stands in the way becomes the way!'"
    "🎯 ARISTOTLE'S VIRTUE! Excellence is not an act, but a habit - keep coding!"
    "🌟 SOCRATIC METHOD! Question everything, improve iteratively!"
)

STABLE_MESSAGES=(
    "🧘 STOIC PHILOSOPHER! Wisdom lies in consistent excellence!"
    "🛡️ TROJAN WALLS! Your defenses hold strong against chaos!"
    "⚖️ THEMIS BALANCE! Divine justice maintains perfect equilibrium!"
    "🎯 ARTEMIS FOCUS! Patient hunter waiting for the perfect moment!"
    "🔒 PANDORA'S BOX! Your code remains sealed from corruption!"
    "🧘 EPICTETUS TEACHES! Focus only on what you can control - your code!"
    "⚖️ ARISTOTLE'S MEAN! Perfect balance between haste and contemplation!"
    "🏛️ SOCRATIC WISDOM! 'I know that I know nothing' - but my tests pass!"
    "📜 MARCUS AURELIUS! 'Confine yourself to the present' - this moment, this code!"
    "🌊 ZENO'S PARADOX! Each step brings you infinitely closer to perfection!"
    "🎭 PLATO'S CAVE! Your stable code reflects eternal Forms of Truth!"
)

ACHIEVEMENT_BADGES=(
    "🏺 AMPHORA" "✨ THUNDERBOLT" "🏛️ TEMPLE" "👑 LAUREL" "🔱 TRIDENT"
    "🔥 TORCH" "🌟 CONSTELLATION" "🚀 PEGASUS" "💪 HERCULES" "🎯 ARTEMIS"
)

echo -e "${PURPLE}🚀 Daedalus Test Suite Runner${NC}"
echo -e "${CYAN}Enhanced with Sisyphus Progress Archive & Historical Analysis${NC}"
echo -e "${GRAY}Run './run_tests.sh help' for archive commands${NC}"
echo "=================================================="

# Function to read lifetime legend stats
read_legend_stats() {
    if [ -f "$SISYPHUS_LEGEND" ]; then
        local total_runs=0
        local total_quests=0
        local hydra_beasts_slain=0
        local blueprint_flaws_fixed=0
        local labyrinth_corridors_explored=0
        local divine_streaks_achieved=0
        local longest_streak=0
        local perfect_ascensions=0
        local first_quest_date=""
        local total_project_xp=0
        local project_level=1

        while IFS='=' read -r key value; do
            case "$key" in
                "TOTAL_RUNS") total_runs="$value";;
                "TOTAL_QUESTS") total_quests="$value";;
                "HYDRA_BEASTS_SLAIN") hydra_beasts_slain="$value";;
                "BLUEPRINT_FLAWS_FIXED") blueprint_flaws_fixed="$value";;
                "LABYRINTH_CORRIDORS_EXPLORED") labyrinth_corridors_explored="$value";;
                "DIVINE_STREAKS_ACHIEVED") divine_streaks_achieved="$value";;
                "LONGEST_STREAK") longest_streak="$value";;
                "PERFECT_ASCENSIONS") perfect_ascensions="$value";;
                "FIRST_QUEST_DATE") first_quest_date="$value";;
                "TOTAL_PROJECT_XP") total_project_xp="$value";;
                "PROJECT_LEVEL") project_level="$value";;
            esac
        done < "$SISYPHUS_LEGEND"

        # Set first quest date if empty
        if [ -z "$first_quest_date" ]; then
            first_quest_date="$(date '+%Y-%m-%d')"
        fi
        echo "$total_runs|$total_quests|$hydra_beasts_slain|$blueprint_flaws_fixed|$labyrinth_corridors_explored|$divine_streaks_achieved|$longest_streak|$perfect_ascensions|$first_quest_date|$total_project_xp|$project_level"
    else
        echo "0|0|0|0|0|0|0|0|$(date '+%Y-%m-%d')|0|1"
    fi
}

# Function to update lifetime legend stats
update_legend_stats() {
    local curr_runtime_errors="$1"
    local curr_compile_errors="$2"
    local curr_test_failures="$3"
    local curr_streak="$4"
    local is_perfect_run="$5"
    local curr_challenge="$6"
    local session_xp="${7:-0}"

    # Read current legend stats
    local legend_line="$(read_legend_stats)"
    IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"
    # CRITICAL FIX: Actually add session XP to total project XP!
    total_project_xp=$((total_project_xp + session_xp))
    # Get previous run stats for intelligent quest tracking
    local prev_stats_line="$(read_previous_stats)"
    IFS='|' read -r prev_errors prev_passes prev_failures prev_compile_errors prev_runtime_errors prev_efficiency_ratio prev_pure_test_time prev_improvement_streak prev_current_challenge prev_timestamp <<< "$prev_stats_line"

    # Set first quest date if empty
    if [ -z "$first_quest" ]; then
        first_quest="$(date '+%Y-%m-%d')"
    fi

    # Always increment run count
    total_runs=$((total_runs + 1))

    # Quest completion logic - only count completed quests
    local quest_completed=0

    # Check if quest was completed this run
    if [ "$curr_challenge" = "🏛️ Mount Olympus Achieved" ] && [ "$prev_current_challenge" != "🏛️ Mount Olympus Achieved" ]; then
        # Achieved perfection - quest complete
        quest_completed=0
    elif [ "$prev_current_challenge" != "" ] && [ "$prev_current_challenge" != "$curr_challenge" ] && [ "$prev_current_challenge" != "🏛️ Mount Olympus Achieved" ]; then
        # Challenge type changed (e.g., from Hydra to Workshop) - previous quest complete
        quest_completed=1
    elif [ "$prev_current_challenge" != "" ] && [ "$prev_current_challenge" != "🏛️ Mount Olympus Achieved" ]; then
        # Check if same quest type but count reached zero
        prev_challenge_type=$(echo "$prev_current_challenge" | sed 's/([^)]*)//g' | sed 's/[0-9]//g')
        curr_challenge_type=$(echo "$curr_challenge" | sed 's/([^)]*)//g' | sed 's/[0-9]//g')

        if [ "$prev_challenge_type" = "$curr_challenge_type" ]; then
            # Same challenge type - extract numbers to see if we completed
            prev_count=$(echo "$prev_current_challenge" | grep -o '[0-9]\+' | head -1)
            curr_count=$(echo "$curr_challenge" | grep -o '[0-9]\+' | head -1)

            if [ -n "$prev_count" ] && [ "$prev_count" -gt 0 ] && ([ -z "$curr_count" ] || [ "$curr_count" -eq 0 ]); then
                quest_completed=1
            fi
        fi
    fi

    # Increment quest count only if quest was completed
    if [ "$quest_completed" -eq 1 ]; then
        total_quests=$((total_quests + 1))
    fi

    # Intelligent monster slaying - only count actual progress (reductions)
    local runtime_beasts_slayed=0
    local compile_flaws_fixed=0
    local corridors_explored=0

    # Only count if we actually made progress (reduced errors)
    if [ -n "$prev_runtime_errors" ] && [ "$prev_runtime_errors" -gt "$curr_runtime_errors" ]; then
        runtime_beasts_slayed=$((prev_runtime_errors - curr_runtime_errors))
        hydra_beasts=$((hydra_beasts + runtime_beasts_slayed))
    fi

    if [ -n "$prev_compile_errors" ] && [ "$prev_compile_errors" -gt "$curr_compile_errors" ]; then
        compile_flaws_fixed=$((prev_compile_errors - curr_compile_errors))
        blueprint_flaws=$((blueprint_flaws + compile_flaws_fixed))
    fi

    if [ -n "$prev_failures" ] && [ "$prev_failures" -gt "$curr_test_failures" ]; then
        corridors_explored=$((prev_failures - curr_test_failures))
        oracle_mysteries=$((oracle_mysteries + corridors_explored))
    fi

    # Update streak achievements
    if [ "$curr_streak" -ge 3 ] && [ "$curr_streak" -gt "$longest_streak" ]; then
        divine_streaks_achieved=$((divine_streaks_achieved + 1))
        longest_streak="$curr_streak"
    fi

    # Update perfect runs
    if [ "$is_perfect_run" = "true" ]; then
        perfect_ascensions=$((perfect_ascensions + 1))
    fi

    # Calculate correct project level from total XP
    local correct_project_level=1
    local cumulative_xp=0
    while true; do
        local xp_needed=$(( 1000 + (correct_project_level * correct_project_level * correct_project_level * 200) + (correct_project_level * correct_project_level * 600) + (correct_project_level * 300) ))
        if [ $((cumulative_xp + xp_needed)) -gt $total_project_xp ]; then
            break
        fi
        cumulative_xp=$((cumulative_xp + xp_needed))
        correct_project_level=$((correct_project_level + 1))
    done

    # Write updated legend stats
    cat > "$SISYPHUS_LEGEND" << EOF
# Sisyphus Eternal Legend - Lifetime Achievements
# Hero's journey began: $first_quest
TOTAL_RUNS=$total_runs
TOTAL_QUESTS=$total_quests
HYDRA_BEASTS_SLAIN=$hydra_beasts
BLUEPRINT_FLAWS_FIXED=$blueprint_flaws
LABYRINTH_CORRIDORS_EXPLORED=$oracle_mysteries
DIVINE_STREAKS_ACHIEVED=$divine_streaks
LONGEST_STREAK=$longest_streak
PERFECT_ASCENSIONS=$perfect_ascensions
FIRST_QUEST_DATE=$first_quest
TOTAL_PROJECT_XP=$total_project_xp
PROJECT_LEVEL=$correct_project_level
EOF
}

# Function to migrate legacy .sisyphus file to new archive system
migrate_legacy_sisyphus() {
    if [ -f "$SISYPHUS_FILE" ] && [ ! -d "$SISYPHUS_DIR" ]; then
        echo -e "${YELLOW}🏛️ Migrating to Sisyphus Archive System...${NC}"
        mkdir -p "$SISYPHUS_DIR"

        # Get timestamp from legacy file or use current time
        local legacy_timestamp=$(grep "TIMESTAMP=" "$SISYPHUS_FILE" | cut -d'=' -f2 | tr -d ' ')
        if [ -z "$legacy_timestamp" ] || [ "$legacy_timestamp" = "never" ]; then
            legacy_timestamp=$(date '+%Y-%m-%d %H:%M:%S')
        fi

        # Convert timestamp to reverse chronological filename
        local filename=$(echo "$legacy_timestamp" | sed 's/[: -]//g' | sed 's/\(..\)\(..\)\(..\)\(..\)\(..\)\(..\)/\6\5\4\3\2\1/')

        # Copy legacy file to archive with new name
        cp "$SISYPHUS_FILE" "$SISYPHUS_DIR/${filename}.sp-stats"

        # Keep legacy file as backup
        mv "$SISYPHUS_FILE" "${SISYPHUS_FILE}.backup"
        echo -e "${GREEN}✅ Migration complete! Legacy file backed up.${NC}"
    fi
}

# Function to get latest stats file from archive
get_latest_stats_file() {
    if [ ! -d "$SISYPHUS_DIR" ]; then
        echo ""
        return
    fi

    # Find the latest .sp-stats file (chronological order - newest sorts last)
    local latest_file=$(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | tail -1)
    echo "$latest_file"
}

# Function to generate timestamped filename (chronological for natural sorting)
generate_stats_filename() {
    local timestamp=$(date '+%Y%m%d%H%M%S')
    # Use normal timestamp format so newer files sort last alphabetically
    echo "${SISYPHUS_DIR}/${timestamp}.sp-stats"
}

# Function to read previous stats from archive system
read_previous_stats() {
    # First migrate if needed
    migrate_legacy_sisyphus

    local latest_file=$(get_latest_stats_file)

    if [ -n "$latest_file" ] && [ -f "$latest_file" ]; then
        local prev_errors=0
        local prev_passes=0
        local prev_failures=0
        local prev_compile_errors=0
        local prev_runtime_errors=0
        local prev_efficiency_ratio=0
        local prev_pure_test_time=0
        local prev_improvement_streak=0
        local prev_current_challenge=""
        local prev_timestamp=""

        while IFS='=' read -r key value; do
            case "$key" in
                "ERRORS") prev_errors="$value";;
                "PASSES") prev_passes="$value";;
                "FAILURES") prev_failures="$value";;
                "COMPILE_ERRORS") prev_compile_errors="$value";;
                "RUNTIME_ERRORS") prev_runtime_errors="$value";;
                "EFFICIENCY_RATIO") prev_efficiency_ratio="$value";;
                "PURE_TEST_TIME") prev_pure_test_time="$value";;
                "IMPROVEMENT_STREAK") prev_improvement_streak="$value";;
                "CURRENT_CHALLENGE") prev_current_challenge="$value";;
                "TIMESTAMP") prev_timestamp="$value";;
            esac
        done < "$latest_file"

        echo "$prev_errors|$prev_passes|$prev_failures|$prev_compile_errors|$prev_runtime_errors|$prev_efficiency_ratio|$prev_pure_test_time|$prev_improvement_streak|$prev_current_challenge|$prev_timestamp"
    else
        echo "0|0|0|0|0|0|0.000000|0||never"
    fi
}

# Function to get random message from array
get_random_message() {
    local -n arr=$1
    echo "${arr[$RANDOM % ${#arr[@]}]}"
}

# Function to get random achievement badge
get_random_badge() {
    echo "${ACHIEVEMENT_BADGES[$RANDOM % ${#ACHIEVEMENT_BADGES[@]}]}"
}

# Function to generate current challenge
# Generate current challenge based on remaining issues
generate_current_challenge() {
    local failed_files=("$@")

    if [ ${#failed_files[@]} -eq 0 ]; then
        echo "none"
        return
    fi

    # Get random failed test
    local random_index=$((RANDOM % ${#failed_files[@]}))
    local selected_failure="${failed_files[$random_index]}"

    # Extract the make target from the failure string
    local make_target=$(echo "$selected_failure" | cut -d':' -f2)
    echo "$make_target"
}

# New technical achievement detection
check_technical_achievements() {
    local curr_errors="$1"
    local curr_passes="$2"
    local curr_efficiency="$3"
    local test_time="$4"

    # Check for new achievement unlocks
    if [ "$curr_errors" -eq 0 ] && [ "$curr_passes" -gt 50 ]; then
        echo -e "${PURPLE}🏆 ACHIEVEMENT: Marathon Runner - 50+ tests passing with zero errors${NC}"
    fi

    if (( $(echo "$test_time < 0.001" | bc -l) )) && [ "$curr_passes" -gt 10 ]; then
        echo -e "${CYAN}🏆 ACHIEVEMENT: Lightning Fast - 10+ tests in under 1ms${NC}"
    fi

    if [ "$curr_efficiency" -gt 1000 ]; then
        echo -e "${YELLOW}🏆 ACHIEVEMENT: Optimization Master - 1000x+ execution efficiency${NC}"
    fi

    # Streak-based achievements
    local stats_line="$(read_previous_stats)"
    IFS='|' read -r prev_errors prev_passes prev_failures prev_compile_errors prev_runtime_errors prev_efficiency_ratio prev_pure_test_time prev_improvement_streak prev_current_challenge prev_timestamp <<< "$stats_line"

    if [ "$prev_improvement_streak" -ge 7 ]; then
        echo -e "${GREEN}🏆 ACHIEVEMENT: Consistency Expert - 7+ consecutive improvement sessions${NC}"
    fi
}

# Function to analyze historical trends from archive
analyze_historical_trends() {
    if [ ! -d "$SISYPHUS_DIR" ]; then
        return
    fi

    local files=($(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null))
    local file_count=${#files[@]}

    if [ "$file_count" -lt 2 ]; then
        return
    fi

    # Analyze last 5 runs for trends
    local recent_files=($(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | head -5))
    local improvements=0
    local regressions=0
    local max_streak=0
    local perfect_runs=0

    for file in "${recent_files[@]}"; do
        local errors=$(grep "ERRORS=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local streak=$(grep "IMPROVEMENT_STREAK=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')

        if [ -n "$errors" ] && [ "$errors" -eq 0 ]; then
            perfect_runs=$((perfect_runs + 1))
        fi

        if [ -n "$streak" ] && [ "$streak" != "" ] && [[ "$streak" =~ ^[0-9]+$ ]] && [ "$streak" -gt "$max_streak" ]; then
            max_streak=$streak
        fi
    done

    # Return analysis results
    echo "$file_count|$perfect_runs|$max_streak"
}

# Function to check for milestone achievements
check_milestones() {
    local analysis="$1"
    IFS='|' read -r total_runs perfect_runs max_streak <<< "$analysis"

    local milestones=()

    # Milestone celebrations
    if [ "$total_runs" -eq 10 ]; then
        milestones+=("🏛️ TEMPLE BUILT! 10 coding sessions completed!")
    elif [ "$total_runs" -eq 50 ]; then
        milestones+=("🏰 FORTRESS STRONG! 50 legendary battles fought!")
    elif [ "$total_runs" -eq 100 ]; then
        milestones+=("👑 IMMORTAL STATUS! 100 divine trials conquered!")
    fi

    if [ "$perfect_runs" -ge 3 ]; then
        milestones+=("💎 TRINITY PERFECTION! 3+ flawless victories achieved!")
    fi

    if [ "$max_streak" -ge 5 ]; then
        milestones+=("🔥 DIVINE STREAK! ${max_streak} consecutive triumphs!")
    fi

    if [ "$max_streak" -eq 7 ]; then
        milestones+=("🏛️ SEVEN SAGES! Your wisdom rivals the ancient philosophers!")
    elif [ "$max_streak" -eq 4 ]; then
        milestones+=("📚 CARDINAL VIRTUES! Wisdom, Justice, Fortitude, Temperance achieved!")
    fi

    if [ "$perfect_runs" -eq 3 ]; then
        milestones+=("🎭 SOCRATIC TRINITY! Know thyself, know thy code, know thy tests!")
    fi

    # Display milestones
    for milestone in "${milestones[@]}"; do
        echo -e "${PURPLE}🎊 MILESTONE: $milestone${NC}"
        echo -e "    ${FADED}legendary achievement unlocked in your coding odyssey${NC}"
    done

    # Show archive statistics if significant milestones
    if [ ${#milestones[@]} -gt 0 ]; then
        show_archive_stats
    fi
}

# Function to cleanup old archive files (keep last 30 runs)
cleanup_archive() {
    if [ ! -d "$SISYPHUS_DIR" ]; then
        return
    fi

    # Count .sp-stats files
    local file_count=$(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | wc -l)

    if [ "$file_count" -gt 30 ]; then
        # Remove oldest files, keep newest 30
        ls -1 "$SISYPHUS_DIR"/*.sp-stats | tail -n +31 | xargs rm -f
        echo -e "${GRAY}🗂️ Archived old progress data (keeping last 30 runs)${NC}"
    fi
}

# Function to view historical data (for future CLI commands)
view_sisyphus_history() {
    local count=${1:-10}

    if [ ! -d "$SISYPHUS_DIR" ]; then
        echo -e "${RED}🔴 No Sisyphus archive found${NC}"
        return 1
    fi

    echo -e "${PURPLE}🏛️ Sisyphus Progress History (Last $count Runs)${NC}"

    local files=($(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | head -$count))
    local run_number=1

    for file in "${files[@]}"; do
        local timestamp=$(grep "TIMESTAMP=" "$file" | cut -d'=' -f2 | tr -d '\n\r')
        local errors=$(grep "ERRORS=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local passes=$(grep "PASSES=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local streak=$(grep "IMPROVEMENT_STREAK=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local challenge=$(grep "CURRENT_CHALLENGE=" "$file" | cut -d'=' -f2 | tr -d '\n\r')
        local efficiency=$(grep "EFFICIENCY_RATIO=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')

        # Status indicator
        local status="🔴"
        if [ "$errors" -eq 0 ]; then
            status="🟢"
        elif [ "$errors" -le 3 ]; then
            status="🟡"
        fi

        echo -e "${CYAN}Run #$run_number${NC} - $timestamp $status"
        echo -e "  Issues: $errors | Tests: $passes | Streak: ${streak:-0} | Efficiency: ${efficiency}x"
        if [ -n "$challenge" ] && [ "$challenge" != "" ]; then
            echo -e "  Challenge: $challenge"
        fi
        echo ""
        run_number=$((run_number + 1))
    done
}

# Function to show archive statistics
show_archive_stats() {
    if [ ! -d "$SISYPHUS_DIR" ]; then
        return
    fi

    local files=($(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null))
    local total_runs=${#files[@]}
    local perfect_runs=0
    local total_tests=0
    local max_efficiency=0

    for file in "${files[@]}"; do
        local errors=$(grep "ERRORS=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local passes=$(grep "PASSES=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
        local efficiency=$(grep "EFFICIENCY_RATIO=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')

        if [ -n "$errors" ] && [[ "$errors" =~ ^[0-9]+$ ]] && [ "$errors" -eq 0 ]; then
            perfect_runs=$((perfect_runs + 1))
        fi

        if [ -n "$passes" ] && [[ "$passes" =~ ^[0-9]+$ ]]; then
            total_tests=$((total_tests + passes))
        fi

        if [ -n "$efficiency" ] && [[ "$efficiency" =~ ^[0-9]+$ ]] && [ "$efficiency" -gt "$max_efficiency" ]; then
            max_efficiency=$efficiency
        fi
    done

    if [ "$total_runs" -gt 0 ]; then
        local success_rate=$((perfect_runs * 100 / total_runs))
        echo -e "${PURPLE}📊 LEGENDARY STATISTICS${NC}"
        echo -e "  🏛️ Total Quests: $total_runs"
        echo -e "  💎 Perfect Runs: $perfect_runs ($success_rate%)"
        echo -e "  ✨ Peak Efficiency: ${max_efficiency}x"
        echo -e "  🧪 Total Tests Passed: $total_tests"
    fi
}

# Function to write current stats to archive system
write_current_stats() {
    local total_errors="$1"
    local total_passes="$2"
    local total_failures="$3"
    local compile_errors="$4"
    local runtime_errors="$5"
    local efficiency_ratio="$6"
    local pure_test_time="$7"
    local improvement_streak="$8"
    local current_challenge="$9"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')

    # Ensure pure_test_time has leading zero if needed
    if [[ "$pure_test_time" =~ ^\. ]]; then
        pure_test_time="0$pure_test_time"
    fi

    # Create archive directory if it doesn't exist
    mkdir -p "$SISYPHUS_DIR"

    # Generate timestamped filename
    local stats_file=$(generate_stats_filename)

    cat > "$stats_file" << EOF
# Sisyphus Test Progress Archive Entry
# Last updated: $timestamp
ERRORS=$total_errors
PASSES=$total_passes
FAILURES=$total_failures
COMPILE_ERRORS=$compile_errors
RUNTIME_ERRORS=$runtime_errors
EFFICIENCY_RATIO=$efficiency_ratio
PURE_TEST_TIME=$pure_test_time
IMPROVEMENT_STREAK=$improvement_streak
CURRENT_CHALLENGE=$current_challenge
TIMESTAMP=$timestamp
EOF

    # Cleanup old files
    cleanup_archive

    # Update lifetime legend stats
        local is_perfect_run="false"
        if [ "$total_errors" -eq 0 ]; then
            is_perfect_run="true"
        fi
        # Calculate final XP: apply bulk mode reduction first, then penalties
        local final_xp=$TOTAL_SESSION_XP
        # Apply bulk mode reduction first (makes failures more punishing)
        if [ "$SISYPHUS_FULL_TEST_MODE" = "1" ] && [ "$SUCCESSFULLY_RUN_FILES" -gt 0 ]; then
            final_xp=$((final_xp / SUCCESSFULLY_RUN_FILES))
        fi
        # Then apply failure penalties
        if [ "$TOTAL_BULK_FAILURES" -gt 0 ]; then
            final_xp=$((final_xp > BULK_FAILURE_PENALTY ? final_xp - BULK_FAILURE_PENALTY : 0))
        fi

        # Debug XP calculation
        echo -e "${GRAY}[DEBUG] Session XP: $TOTAL_SESSION_XP, Bulk mode: 1/$SUCCESSFULLY_RUN_FILES, Failures: $TOTAL_BULK_FAILURES, Penalty: $BULK_FAILURE_PENALTY, Final: $final_xp${NC}" >&2
        # Temporarily set TOTAL_SESSION_XP for legend update
        local saved_session_xp=$TOTAL_SESSION_XP
        TOTAL_SESSION_XP=$final_xp
        update_legend_stats "$RUNTIME_ERRORS" "$COMPILE_ERRORS" "$FAILED_INDIVIDUAL_TESTS" "$improvement_streak" "$is_perfect_run" "$CURRENT_CHALLENGE" "$final_xp"
        # Restore original value
        TOTAL_SESSION_XP=$saved_session_xp
}

# Function to show legend achievements
show_legend_achievements() {
    local legend_line="$(read_legend_stats)"
    IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

    # Only show if we have significant achievements
    if [ "$total_quests" -ge 5 ] || [ "$perfect_ascensions" -ge 1 ] || [ "$longest_streak" -ge 3 ]; then
        echo ""
        echo -e "${PURPLE}🏛️ ETERNAL LEGEND STATUS${NC}"
        echo -e "    ${FADED}your immortal achievements since ${first_quest:-'the beginning'}${NC}"

        if [ "$perfect_ascensions" -gt 0 ]; then
            echo -e "${GREEN}🏛️ Perfect Ascensions: $perfect_ascensions${NC}"
        fi

        if [ "$longest_streak" -ge 3 ]; then
            echo -e "${YELLOW}🔥 Greatest Streak: $longest_streak divine victories${NC}"
        fi

        if [ "$hydra_beasts" -gt 0 ]; then
            echo -e "${ORANGE}🐍 Hydra Beasts Slain: $hydra_beasts${NC}"
        fi

        if [ "$blueprint_flaws" -gt 0 ]; then
            echo -e "${CYAN}⚒️ Blueprint Flaws Fixed: $blueprint_flaws${NC}"
        fi

        if [ "$oracle_mysteries" -gt 0 ]; then
            echo -e "${BLUE}🔮 Labyrinth Corridors Explored: $oracle_mysteries${NC}"
        fi
    fi
}

# Function to show progress comparison
show_progress() {
    local curr_errors="$1"
    local curr_passes="$2"
    local curr_failures="$3"
    local curr_compile_errors="$4"
    local curr_runtime_errors="$5"
    local curr_efficiency_ratio="$6"
    local curr_pure_test_time="$7"

    # Read previous stats using pipe separation to avoid timestamp parsing issues
    local stats_line="$(read_previous_stats)"
    IFS='|' read -r prev_errors prev_passes prev_failures prev_compile_errors prev_runtime_errors prev_efficiency_ratio prev_pure_test_time prev_improvement_streak prev_current_challenge prev_timestamp <<< "$stats_line"
    echo ""
    echo -e "${PURPLE}📈 Sisyphus Progress Since Last Run${NC}"
    echo -e "Last run: ${YELLOW}$prev_timestamp${NC}"
    echo ""

    # Calculate changes
    local error_change=$((curr_errors - prev_errors))
    local pass_change=$((curr_passes - prev_passes))
    local failure_change=$((curr_failures - prev_failures))
    local compile_change=$((curr_compile_errors - prev_compile_errors))
    local runtime_change=$((curr_runtime_errors - prev_runtime_errors))

    # Helper function to format change
        format_change() {
            local change=$1
            local label=$2
            if [ "$change" -gt 0 ]; then
                echo -e "${RED}+$change $label${NC}"
            elif [ "$change" -lt 0 ]; then
                echo -e "${GREEN}$change $label${NC}"  # negative is good for errors
            else
                echo -e "${YELLOW}±0 $label${NC}"
            fi
        }

        # Helper function to format change for passes (opposite logic - more passes is good)
        format_pass_change() {
            local change=$1
            local label=$2
            if [ "$change" -gt 0 ]; then
                echo -e "${GREEN}+$change $label${NC}"  # positive is good for passes
            elif [ "$change" -lt 0 ]; then
                echo -e "${RED}$change $label${NC}"    # negative is bad for passes
            else
                echo -e "${YELLOW}±0 $label${NC}"
            fi
        }

        echo -e "Total File Errors:   ${YELLOW}$prev_errors${NC} → ${YELLOW}$curr_errors${NC} ($(format_change $error_change "errors"))"
        echo -e "Individual Passes:   ${YELLOW}$prev_passes${NC} → ${YELLOW}$curr_passes${NC} ($(format_pass_change $pass_change "tests"))"
        echo -e "Individual Failures: ${YELLOW}$prev_failures${NC} → ${YELLOW}$curr_failures${NC} ($(format_change $failure_change "tests"))"
        echo -e "Compile Errors:      ${YELLOW}$prev_compile_errors${NC} → ${YELLOW}$curr_compile_errors${NC} ($(format_change $compile_change "files"))"
        echo -e "Runtime Errors:      ${YELLOW}$prev_runtime_errors${NC} → ${YELLOW}$curr_runtime_errors${NC} ($(format_change $runtime_change "files"))"

    # Overall progress indicator
    local total_curr_issues=$((curr_errors + curr_failures + curr_compile_errors + curr_runtime_errors))
    local total_prev_issues=$((prev_errors + prev_failures + prev_compile_errors + prev_runtime_errors))
    local total_change=$((total_curr_issues - total_prev_issues))

    echo ""
    if [ "$total_change" -lt 0 ]; then
        echo -e "${GREEN}🎯 Overall Progress: IMPROVING${NC} (${GREEN}$total_change issues${NC})"
    elif [ "$total_change" -gt 0 ]; then
        echo -e "${RED}⚠️  Overall Progress: REGRESSING${NC} (${RED}+$total_change issues${NC})"
    else
        echo -e "${YELLOW}📊 Overall Progress: STABLE${NC} (no change in issues)"
    fi

    # Success rate comparison
    if [ "$curr_passes" -gt 0 ] || [ "$prev_passes" -gt 0 ]; then
        local curr_total_tests=$((curr_passes + curr_failures))
        local prev_total_tests=$((prev_passes + prev_failures))

        if [ "$curr_total_tests" -gt 0 ] && [ "$prev_total_tests" -gt 0 ]; then
            local curr_rate=$(echo "scale=1; $curr_passes * 100 / $curr_total_tests" | bc -l)
            local prev_rate=$(echo "scale=1; $prev_passes * 100 / $prev_total_tests" | bc -l)
            local rate_change=$(echo "scale=1; $curr_rate - $prev_rate" | bc -l)

            echo -e "Success Rate:        ${YELLOW}$(printf "%.1f" $prev_rate)%${NC} → ${YELLOW}$(printf "%.1f" $curr_rate)%${NC}"

            if (( $(echo "$rate_change > 0" | bc -l) )); then
                echo -e "                     ${GREEN}▲ +$(printf "%.1f" $rate_change)% improvement${NC}"
            elif (( $(echo "$rate_change < 0" | bc -l) )); then
                echo -e "                     ${RED}▼ $(printf "%.1f" $rate_change)% regression${NC}"
            fi
        fi
    fi

    # Performance comparison - only if we have valid previous data
    # Validate that all values are proper numbers first
    if [[ "$prev_pure_test_time" =~ ^[0-9]+\.[0-9]+$ ]] && [[ "$curr_pure_test_time" =~ ^[0-9]+\.[0-9]+$ ]] && \
       [[ "$prev_efficiency_ratio" =~ ^[0-9]+$ ]] && [[ "$curr_efficiency_ratio" =~ ^[0-9]+$ ]] && \
       [ "$prev_pure_test_time" != "0.000000" ] && [ "$prev_efficiency_ratio" != "0" ] && (( $(echo "$curr_pure_test_time > 0" | bc -l) )); then

        local time_change=$(echo "scale=6; $curr_pure_test_time - $prev_pure_test_time" | bc -l)
        local efficiency_change=$((curr_efficiency_ratio - prev_efficiency_ratio))

        echo ""
        echo -e "🔥 Performance Changes:"
        echo -e "Pure Test Time:      ${YELLOW}$(printf "%.6f" "$prev_pure_test_time")s${NC} → ${YELLOW}$(printf "%.6f" "$curr_pure_test_time")s${NC}"

        if (( $(echo "$time_change < 0" | bc -l) )); then
            local abs_change=$(echo "$time_change" | sed 's/^-//')
            echo -e "                     ${GREEN}▼ ${abs_change}s faster${NC}"
        elif (( $(echo "$time_change > 0" | bc -l) )); then
            echo -e "                     ${RED}▲ +$(printf "%.6f" $time_change)s slower${NC}"
        else
            echo -e "                     ${YELLOW}± No change${NC}"
        fi

        echo -e "Code Efficiency:     ${YELLOW}${prev_efficiency_ratio}x${NC} → ${YELLOW}${curr_efficiency_ratio}x${NC}"

        if [ "$efficiency_change" -gt 0 ]; then
            echo -e "                     ${GREEN}▲ +${efficiency_change}x more efficient${NC}"
        elif [ "$efficiency_change" -lt 0 ]; then
            echo -e "                     ${RED}▼ ${efficiency_change}x less efficient${NC}"
        else
            echo -e "                     ${YELLOW}± No change${NC}"
        fi
    fi
}

# Build test dependencies
echo -e "${BLUE}🔧 Building test dependencies...${NC}"
build_start=$(date +%s.%N)
if ! make -s always > /dev/null 2>&1; then
    echo -e "${RED}🔴 Failed to build test dependencies${NC}"
    exit 1
fi
build_end=$(date +%s.%N)
build_time=$(echo "$build_end - $build_start" | bc -l)
echo -e "${BOLD_WHITE}   Build completed in $(printf "%.6f" $build_time) seconds${NC}"

# Counter variables for files
TOTAL_FILES=0
PASSED_FILES=0
FAILED_FILES=()
COMPILE_ERROR_FILES=()
RUNTIME_ERROR_FILES=()
SUCCESSFULLY_RUN_FILES=0  # Files that ran tests (no compile/runtime errors)

# Counter variables for individual tests
TOTAL_INDIVIDUAL_TESTS=0
PASSED_INDIVIDUAL_TESTS=0
FAILED_INDIVIDUAL_TESTS=0

# Error tracking
COMPILE_ERRORS=0
RUNTIME_ERRORS=0

# Timing variables
TOTAL_TEST_TIME=0
TOTAL_SHELL_OVERHEAD=0

# XP tracking for overall test run (will be calculated from project XP difference)
TOTAL_SESSION_XP=0
SESSION_XP_FROM_TESTS=0
SESSION_XP_FROM_COMBOS=0
SESSION_XP_FROM_ACHIEVEMENTS=0
TOTAL_BULK_FAILURES=0
BULK_FAILURE_PENALTY=0

# Read starting project XP to calculate session XP later
STARTING_LEGEND_LINE="$(read_legend_stats)"
IFS='|' read -r _start_runs _start_quests _start_hydra _start_blueprint _start_oracle _start_divine _start_longest _start_perfect _start_first_quest STARTING_PROJECT_XP _start_level <<< "$STARTING_LEGEND_LINE"

# Debug XP initialization
echo -e "${GRAY}[DEBUG] XP tracking initialized. Bulk mode: Dynamic (1/N where N = successfully run files), Starting Project XP: $STARTING_PROJECT_XP${NC}" >&2

# Function to extract test counts from output
extract_test_counts() {
    local output="$1"

    # Look for the Sisyphus test summary line: "🧪 Test Functions Executed: 6 | 🏆 Won: 4 | 💀 Lost: 2"
    local total=$(echo "$output" | grep -o "Test Functions Executed: [0-9]*" | grep -o "[0-9]*" || echo "0")
    local passed=$(echo "$output" | grep -o "🏆 Won: [0-9]*" | grep -o "[0-9]*" || echo "0")
    local failed=$(echo "$output" | grep -o "💀 Lost: [0-9]*" | grep -o "[0-9]*" || echo "0")

    # Return as space-separated values
    echo "$total $passed $failed"
}


# Function to detect error type from output
detect_error_type() {
    local output="$1"
    local exit_code="$2"

    # Check for compilation errors
    if echo "$output" | grep -q -E "(error:|fatal error:|undefined reference|ld:|collect2:)"; then
        echo "COMPILE_ERROR"
        return
    fi

    # Check for test failures BEFORE checking for runtime errors
    # Look for the Sisyphus test output patterns
    if echo "$output" | grep -q "FINAL COLOSSEUM STATS"; then
        # It's a test run that completed - check if tests failed
        if echo "$output" | grep -q "💀 Lost: [1-9]"; then
            echo "TEST_FAILURE"
            return
        elif echo "$output" | grep -q "🏆 Won:"; then
            echo "SUCCESS"
            return
        fi
    fi

    # Check for runtime errors (actual crashes)
    if echo "$output" | grep -q -E "(Segmentation fault|core dumped|Aborted|Bus error|Floating point exception)"; then
        echo "RUNTIME_ERROR"
        return
    fi

    # If exit code is non-zero but we can't classify it
    if [ "$exit_code" -ne 0 ]; then
        echo "RUNTIME_ERROR"
        return
    fi

    echo "SUCCESS"
}


# Function to run a test
run_test() {
    local test_name="$1"
    local make_target="$2"

    echo -ne "${BLUE}📋 Running $test_name...${NC}"

    TOTAL_FILES=$((TOTAL_FILES + 1))

    # Capture start time for full process
    process_start=$(date +%s.%N)

    # Run the test and capture both exit code and output
    test_output=$(make -s "$make_target" 2>&1)
    exit_code=$?

    process_end=$(date +%s.%N)
    process_elapsed=$(echo "$process_end - $process_start" | bc -l)

    # Detect error type
    error_type=$(detect_error_type "$test_output" "$exit_code")

    case "$error_type" in
        "SUCCESS")
            # Extract timing from the test output if available (format: ⏱️  Total Test Time: 0.000155 seconds)
            actual_test_time=$(echo "$test_output" | grep -o "Total Test Time: [0-9]*\.[0-9]* seconds" | grep -o "[0-9]\+\.[0-9]\+" || echo "0.000000")
            # Ensure proper formatting with leading zero if needed
            if [[ "$actual_test_time" =~ ^\. ]]; then
                actual_test_time="0$actual_test_time"
            fi
            shell_overhead=$(echo "$process_elapsed - $actual_test_time" | bc -l)

            # Extract individual test counts
            counts=$(extract_test_counts "$test_output")
            read -r file_total file_passed file_failed <<< "$counts"

            # Add to global counters
            TOTAL_INDIVIDUAL_TESTS=$((TOTAL_INDIVIDUAL_TESTS + file_total))
            PASSED_INDIVIDUAL_TESTS=$((PASSED_INDIVIDUAL_TESTS + file_passed))
            FAILED_INDIVIDUAL_TESTS=$((FAILED_INDIVIDUAL_TESTS + file_failed))

            # Extract Suite XP from the progress bar line: "🎯 Suite XP [...] 928/2100 (Level 1)"
            session_xp=$(echo "$test_output" | grep -o "Suite XP \[.*\] [0-9]*/[0-9]*" | grep -o "[0-9]*/" | grep -o "[0-9]*" | head -1 || echo "0")
            # Extract XP breakdowns from: "💰 XP from tests: 935, XP from combos: 213, XP from achievements: 80"
            tests_xp=$(echo "$test_output" | grep -o "XP from tests: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")
            combos_xp=$(echo "$test_output" | grep -o "XP from combos: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")
            achievements_xp=$(echo "$test_output" | grep -o "XP from achievements: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")

            # Add to session totals
            TOTAL_SESSION_XP=$((TOTAL_SESSION_XP + session_xp))
            SESSION_XP_FROM_TESTS=$((SESSION_XP_FROM_TESTS + tests_xp))
            SESSION_XP_FROM_COMBOS=$((SESSION_XP_FROM_COMBOS + combos_xp))
            SESSION_XP_FROM_ACHIEVEMENTS=$((SESSION_XP_FROM_ACHIEVEMENTS + achievements_xp))

            TOTAL_TEST_TIME=$(echo "$TOTAL_TEST_TIME + $actual_test_time" | bc -l)
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $shell_overhead" | bc -l)

            echo -e " ${BOLD_WHITE}(Test: $(printf "%.6f" $actual_test_time)s | Shell: $(printf "%.3f" $shell_overhead)s)${NC}"
            echo -e "  ${GREEN}✅ $test_name: All $file_total tests passed${NC}"
            PASSED_FILES=$((PASSED_FILES + 1))
            SUCCESSFULLY_RUN_FILES=$((SUCCESSFULLY_RUN_FILES + 1))
            ;;

        "TEST_FAILURE")
            # Extract timing if available (format: ⏱️  Total Test Time: 0.000155 seconds)
            actual_test_time=$(echo "$test_output" | grep -o "Total Test Time: [0-9]*\.[0-9]* seconds" | grep -o "[0-9]\+\.[0-9]\+" || echo "0.000000")
            # Ensure proper formatting with leading zero if needed
            if [[ "$actual_test_time" =~ ^\. ]]; then
                actual_test_time="0$actual_test_time"
            fi
            shell_overhead=$(echo "$process_elapsed - $actual_test_time" | bc -l)

            # Extract individual test counts
            counts=$(extract_test_counts "$test_output")
            read -r file_total file_passed file_failed <<< "$counts"

            # Add to global counters
            TOTAL_INDIVIDUAL_TESTS=$((TOTAL_INDIVIDUAL_TESTS + file_total))
            PASSED_INDIVIDUAL_TESTS=$((PASSED_INDIVIDUAL_TESTS + file_passed))
            FAILED_INDIVIDUAL_TESTS=$((FAILED_INDIVIDUAL_TESTS + file_failed))

            # Extract Suite XP from the progress bar line: "🎯 Suite XP [...] 928/2100 (Level 1)"
            session_xp=$(echo "$test_output" | grep -o "Suite XP \[.*\] [0-9]*/[0-9]*" | grep -o "[0-9]*/" | grep -o "[0-9]*" | head -1 || echo "0")
            # Extract XP breakdowns from: "💰 XP from tests: 935, XP from combos: 213, XP from achievements: 80"
            tests_xp=$(echo "$test_output" | grep -o "XP from tests: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")
            combos_xp=$(echo "$test_output" | grep -o "XP from combos: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")
            achievements_xp=$(echo "$test_output" | grep -o "XP from achievements: [0-9]*" | grep -o "[0-9]*" | tail -1 || echo "0")

            # Add to session totals
            TOTAL_SESSION_XP=$((TOTAL_SESSION_XP + session_xp))
            SESSION_XP_FROM_TESTS=$((SESSION_XP_FROM_TESTS + tests_xp))
            SESSION_XP_FROM_COMBOS=$((SESSION_XP_FROM_COMBOS + combos_xp))
            SESSION_XP_FROM_ACHIEVEMENTS=$((SESSION_XP_FROM_ACHIEVEMENTS + achievements_xp))

            TOTAL_TEST_TIME=$(echo "$TOTAL_TEST_TIME + $actual_test_time" | bc -l)
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $shell_overhead" | bc -l)

            echo -e " ${BOLD_WHITE}(Test: $(printf "%.6f" $actual_test_time)s | Shell: $(printf "%.3f" $shell_overhead)s)${NC}"
            echo -e "  ${YELLOW}⚠️  $test_name: $file_passed/$file_total tests passed, $file_failed failed${NC}"
            FAILED_FILES+=("$test_name:$make_target:TEST_FAILURE")
            SUCCESSFULLY_RUN_FILES=$((SUCCESSFULLY_RUN_FILES + 1))
            ;;

        "COMPILE_ERROR")
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $process_elapsed" | bc -l)
            echo -e " ${BOLD_WHITE}(Process: $(printf "%.6f" $process_elapsed)s)${NC}"
            echo -e "  ${RED}🔨 $test_name: Compilation error${NC}"
            COMPILE_ERROR_FILES+=("$test_name:$make_target")
            COMPILE_ERRORS=$((COMPILE_ERRORS + 1))
            ;;

        "RUNTIME_ERROR")
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $process_elapsed" | bc -l)
            echo -e " ${BOLD_WHITE}(Process: $(printf "%.6f" $process_elapsed)s)${NC}"
            echo -e "  ${ORANGE}💥 $test_name: Runtime error (segfault/crash)${NC}"
            RUNTIME_ERROR_FILES+=("$test_name:$make_target")
            RUNTIME_ERRORS=$((RUNTIME_ERRORS + 1))
            ;;
    esac
}

# Record overall start time
overall_start=$(date +%s.%N)

# Run all tests
echo ""
run_test "Item Creation & Destruction" "run-test-items-creation-destruction"
run_test "Item Type Checking & Access" "run-test-items-type-checking"
run_test "Material System" "run-test-items-material-system"
run_test "Item Properties & Stats" "run-test-items-properties"
run_test "Item Durability System" "run-test-items-durability"
run_test "Inventory Management" "run-test-items-inventory"
run_test "Item Usage & Effects" "run-test-items-usage"
run_test "Item Helper Functions" "run-test-items-helper-functions"
run_test "Item Integration Tests" "run-test-items-integration-tests"

# Calculate overall execution time
overall_end=$(date +%s.%N)
overall_time=$(echo "$overall_end - $overall_start" | bc -l)

# Calculate total file errors
TOTAL_FILE_ERRORS=$((COMPILE_ERRORS + RUNTIME_ERRORS + ${#FAILED_FILES[@]}))

# Ensure TOTAL_TEST_TIME is properly formatted
if [[ "$TOTAL_TEST_TIME" =~ ^\. ]]; then
    TOTAL_TEST_TIME="0$TOTAL_TEST_TIME"
fi

# Calculate efficiency ratio before calling show_progress
# Set bulk failures to total individual test failures across all test files
TOTAL_BULK_FAILURES=$FAILED_INDIVIDUAL_TESTS

# Calculate bulk failure penalty - exponential: -100, -200, -300, etc.
if [ "$TOTAL_BULK_FAILURES" -gt 0 ]; then
    for ((i=1; i<=TOTAL_BULK_FAILURES; i++)); do
        penalty=$((i * 100))
        BULK_FAILURE_PENALTY=$((BULK_FAILURE_PENALTY + penalty))
    done
fi

# Write stats first to update the legend file with all XP earned from tests
write_current_stats "$TOTAL_FILE_ERRORS" "$PASSED_INDIVIDUAL_TESTS" "$FAILED_INDIVIDUAL_TESTS" "$COMPILE_ERRORS" "$RUNTIME_ERRORS" "$EFFICIENCY_RATIO" "$TOTAL_TEST_TIME" "$IMPROVEMENT_STREAK" "$CURRENT_CHALLENGE"

# Now calculate actual session XP as the difference between ending and starting project XP
ENDING_LEGEND_LINE="$(read_legend_stats)"
IFS='|' read -r _end_runs _end_quests _end_hydra _end_blueprint _end_oracle _end_divine _end_longest _end_perfect _end_first_quest ENDING_PROJECT_XP _end_level <<< "$ENDING_LEGEND_LINE"
TOTAL_SESSION_XP=$((ENDING_PROJECT_XP - STARTING_PROJECT_XP))

# Store original XP totals for display (never modify the originals)
original_session_xp=$TOTAL_SESSION_XP

# Debug final totals before display
echo -e "${GRAY}[DEBUG] Final totals - Starting XP: $STARTING_PROJECT_XP, Ending XP: $ENDING_PROJECT_XP, Session XP: $TOTAL_SESSION_XP, Successfully run files: $SUCCESSFULLY_RUN_FILES, Failed tests: $FAILED_INDIVIDUAL_TESTS, Bulk penalty: $BULK_FAILURE_PENALTY${NC}" >&2

# Determine efficiency ratio only if we have test time
EFFICIENCY_RATIO=0
if (( $(echo "$TOTAL_TEST_TIME > 0" | bc -l) )); then
    EFFICIENCY_RATIO=$(echo "scale=0; $TOTAL_SHELL_OVERHEAD / $TOTAL_TEST_TIME" | bc -l)
fi
# Generate current challenge from failed tests
ALL_FAILED_TESTS=()
for failed in "${FAILED_FILES[@]}"; do
    ALL_FAILED_TESTS+=("$failed")
done
for compile_error in "${COMPILE_ERROR_FILES[@]}"; do
    ALL_FAILED_TESTS+=("$compile_error:COMPILE_ERROR")
done
for runtime_error in "${RUNTIME_ERROR_FILES[@]}"; do
    ALL_FAILED_TESTS+=("$runtime_error:RUNTIME_ERROR")
done

# Read previous challenge
stats_line="$(read_previous_stats)"
IFS='|' read -r prev_errors_temp prev_passes_temp prev_failures_temp prev_compile_errors_temp prev_runtime_errors_temp prev_efficiency_ratio_temp prev_pure_test_time_temp prev_streak_temp prev_challenge_temp prev_timestamp_temp <<< "$stats_line"

# Generate new challenge if needed or if previous challenge is resolved
CURRENT_CHALLENGE="$prev_challenge_temp"
if [ "$CURRENT_CHALLENGE" = "none" ] || [ ${#ALL_FAILED_TESTS[@]} -eq 0 ]; then
    CURRENT_CHALLENGE=$(generate_current_challenge "${ALL_FAILED_TESTS[@]}")
else
    # Check if current challenge is still valid (still in failed tests)
    challenge_still_valid=false
    for failed_test in "${ALL_FAILED_TESTS[@]}"; do
        if echo "$failed_test" | grep -q "$CURRENT_CHALLENGE"; then
            challenge_still_valid=true
            break
        fi
    done

    # If current challenge is resolved, generate new one
    if [ "$challenge_still_valid" = false ]; then
        CURRENT_CHALLENGE=$(generate_current_challenge "${ALL_FAILED_TESTS[@]}")
    fi
fi

# Calculate streak and challenge for current run
IMPROVEMENT_STREAK=0
CURRENT_CHALLENGE=""

# Check if we have archive or legacy file
if [ -d "$SISYPHUS_DIR" ] || [ -f "$SISYPHUS_FILE" ]; then
    prev_stats="$(read_previous_stats)"
    IFS='|' read -r prev_errors_temp prev_passes_temp prev_failures_temp prev_compile_errors_temp prev_runtime_errors_temp prev_efficiency_ratio_temp prev_pure_test_time_temp prev_streak_temp prev_challenge_temp prev_timestamp_temp <<< "$prev_stats"

    # Only calculate if we have valid previous data
    if [ "$prev_timestamp_temp" != "never" ]; then
        prev_total_issues=$((prev_errors_temp + prev_failures_temp + prev_compile_errors_temp + prev_runtime_errors_temp))
        curr_total_issues=$((TOTAL_FILE_ERRORS + FAILED_INDIVIDUAL_TESTS + COMPILE_ERRORS + RUNTIME_ERRORS))

        if [ "$curr_total_issues" -lt "$prev_total_issues" ]; then
            IMPROVEMENT_STREAK=$((prev_streak_temp + 1))
        elif [ "$curr_total_issues" -gt "$prev_total_issues" ]; then
            IMPROVEMENT_STREAK=0
        else
            IMPROVEMENT_STREAK=$prev_streak_temp
        fi
    fi
fi

# Set current challenge based on remaining issues (Epic Mythological Themes)
if [ "$COMPILE_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="🏗️ Daedalus' Workshop Crisis (${COMPILE_ERRORS} blueprint flaws - undefined symbols, missing headers, linker failures)"
elif [ "$RUNTIME_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="🐲 Hydra Beasts Rampage (${RUNTIME_ERRORS} runtime errors - segfaults, buffer overflows, null dereferences)"
elif [ ${#FAILED_FILES[@]} -gt 0 ]; then
    if [ ${#FAILED_FILES[@]} -eq 12 ]; then
        CURRENT_CHALLENGE="🌊 Poseidon's Twelve Trials (${#FAILED_FILES[@]} assertion storms - systematic logic validation required)"
    elif [ ${#FAILED_FILES[@]} -eq 1 ]; then
        CURRENT_CHALLENGE="👑 Minos' Final Requiem (${#FAILED_FILES[@]} labyrinth exit - one critical test failure blocking ascension)"
    else
        CURRENT_CHALLENGE="🏛️ Minotaur's Labyrinth (${#FAILED_FILES[@]} twisted corridors - failed assertions, logic errors, boundary conditions)"
    fi
elif [ "$TOTAL_FILE_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="✨ Zeus's Divine Judgment (${TOTAL_FILE_ERRORS} compilation warnings elevated to olympian standards)"
else
    CURRENT_CHALLENGE="🏛️ Mount Olympus Achieved - All tests pass, Daedalus' labyrinth conquered"
fi

# Show progress comparison before final summary
show_progress "$TOTAL_FILE_ERRORS" "$PASSED_INDIVIDUAL_TESTS" "$FAILED_INDIVIDUAL_TESTS" "$COMPILE_ERRORS" "$RUNTIME_ERRORS" "$EFFICIENCY_RATIO" "$TOTAL_TEST_TIME"

# Final Summary
SUCCESSFUL_FILES=$((PASSED_FILES))
FAILED_FILE_COUNT=$((${#FAILED_FILES[@]}))

echo ""
echo -e "${PURPLE}📊 Current Test Results Summary${NC}"
# Read current project level and XP for progress bar
legend_line="$(read_legend_stats)"
IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

# Calculate XP progress within current level using C header formula
# XP formula: 1000 + (level^3 * 200) + (level^2 * 600) + (level * 300)
get_xp_for_level() {
    local level=$1
    echo $(( 1000 + (level * level * level * 200) + (level * level * 600) + (level * 300) ))
}

get_current_level_from_xp() {
    local total_xp=$1
    local level=1
    local cumulative_xp=0

    while true; do
        local xp_needed=$(get_xp_for_level $level)
        if [ $((cumulative_xp + xp_needed)) -gt $total_xp ]; then
            break
        fi
        cumulative_xp=$((cumulative_xp + xp_needed))
        level=$((level + 1))
    done
    echo $level
}

get_xp_in_current_level() {
    local total_xp=$1
    local current_level=$2
    local cumulative_xp=0

    for ((i=1; i<current_level; i++)); do
        cumulative_xp=$((cumulative_xp + $(get_xp_for_level $i)))
    done
    echo $((total_xp - cumulative_xp))
}

# Recalculate project level based on total XP using correct formula
actual_project_level=$(get_current_level_from_xp $total_project_xp)
current_level_xp=$(get_xp_in_current_level $total_project_xp $actual_project_level)
xp_needed_for_next=$(get_xp_for_level $actual_project_level)

# Progress bar will be shown later as separator before mythological messages
echo -e "${CYAN}📁 Test File Summary:${NC}"
echo -e "Test Files Run:     ${YELLOW}$TOTAL_FILES${NC}"
echo -e "🟢 Files Passed:    ${GREEN}$SUCCESSFUL_FILES${NC}"
echo -e "🔴 Files w/ Failures: ${YELLOW}$FAILED_FILE_COUNT${NC}"
echo -e "🔨 Compile Errors:  ${RED}$COMPILE_ERRORS${NC}"
echo -e "💥 Runtime Errors:  ${ORANGE}$RUNTIME_ERRORS${NC}"
echo ""
echo -e "${CYAN}🧪 Individual Test Summary:${NC}"
echo -e "Total Tests:    ${YELLOW}$TOTAL_INDIVIDUAL_TESTS${NC}"
echo -e "🟢 Passed:      ${GREEN}$PASSED_INDIVIDUAL_TESTS${NC}"
echo -e "🔴 Failed:      ${RED}$FAILED_INDIVIDUAL_TESTS${NC}"

# Calculate success percentage
if [ "$TOTAL_INDIVIDUAL_TESTS" -gt 0 ]; then
    success_rate=$(echo "scale=1; $PASSED_INDIVIDUAL_TESTS * 100 / $TOTAL_INDIVIDUAL_TESTS" | bc -l)
    echo -e "📈 Success Rate: ${BOLD_WHITE}$(printf "%.1f" $success_rate)%${NC}"
fi

echo ""
echo -e "${CYAN}✨ Performance Analysis:${NC}"
if (( $(echo "$TOTAL_TEST_TIME > 0" | bc -l) )); then
    echo -e "${BOLD_WHITE}⏱️  Pure Test Time:     $(printf "%10.5f" $TOTAL_TEST_TIME) seconds${NC}"
    echo -e "${BOLD_WHITE}🐚 Shell Overhead:     $(printf "%10.3f" $TOTAL_SHELL_OVERHEAD) seconds${NC}"
    echo -e "${BOLD_WHITE}🕒 Total Runtime:      $(printf "%10.4f" $overall_time) seconds${NC}"
    echo -e "${BOLD_WHITE}🚀 Code Efficiency: Your Code is ${GREEN}${EFFICIENCY_RATIO}x${BOLD_WHITE} faster than the Sisyphus Framework!${NC}"
else
    echo -e "${BOLD_WHITE}💥 Runtime Crashes Detected: Tests failed to execute${NC}"
    echo -e "${BOLD_WHITE}🐚 Shell Overhead:     $(printf "%10.3f" $TOTAL_SHELL_OVERHEAD) seconds${NC}"
    echo -e "${BOLD_WHITE}🕒 Total Runtime:      $(printf "%10.4f" $overall_time) seconds${NC}"
    echo -e "${BOLD_WHITE}🔧 Fix segfaults to measure actual test performance${NC}"
fi



# Show detailed error breakdown (keeping the existing detailed error reporting)
if [ ${#COMPILE_ERROR_FILES[@]} -gt 0 ] || [ ${#RUNTIME_ERROR_FILES[@]} -gt 0 ] || [ ${#FAILED_FILES[@]} -gt 0 ]; then
    echo ""
    echo -e "${RED}💥 Issues Found:${NC}"

    if [ ${#COMPILE_ERROR_FILES[@]} -gt 0 ]; then
        echo ""
        echo -e "${RED}🏗️ DAEDALUS' WORKSHOP CRISIS - Blueprint Corruption Detected:${NC}"
        for error_file in "${COMPILE_ERROR_FILES[@]}"; do
            name=$(echo "$error_file" | cut -d':' -f1)
            target=$(echo "$error_file" | cut -d':' -f2)
            echo -e "  ${RED}• $name${NC} - Architectural flaws in the labyrinth design"
            echo -e "    ${GRAY}Common causes: missing headers, undefined symbols, linker dependencies${NC}"
            file_path=$(map_target_to_file_path "$target")
            echo -e "    Debug: ${YELLOW}make $target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
        done
    fi

    if [ ${#RUNTIME_ERROR_FILES[@]} -gt 0 ]; then
        echo ""
        echo -e "${ORANGE}🐲 MYTHICAL BEASTS RAMPAGE - Memory Corruption Unleashed:${NC}"
        for error_file in "${RUNTIME_ERROR_FILES[@]}"; do
            name=$(echo "$error_file" | cut -d':' -f1)
            target=$(echo "$error_file" | cut -d':' -f2)
            echo -e "  ${ORANGE}• $name${NC} - Hydra head strikes: segmentation fault, buffer overflow, or null pointer"
            echo -e "    ${GRAY}Memory beasts detected: check malloc/free pairs, array bounds, pointer initialization${NC}"
            file_path=$(map_target_to_file_path "$target")
            echo -e "    Debug: ${YELLOW}make $target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
        done
    fi

    if [ ${#FAILED_FILES[@]} -gt 0 ]; then
        echo ""
        echo -e "${YELLOW}🏛️  MINOTAUR'S LABYRINTH - Logic Paths Blocked:${NC}"
        for failed in "${FAILED_FILES[@]}"; do
            name=$(echo "$failed" | cut -d':' -f1)
            target=$(echo "$failed" | cut -d':' -f2)
            echo -e "  ${YELLOW}• $name${NC} - The corridor is blocked: logical fallacies, circular reasoning, or paradoxes"
            echo -e "    ${GRAY}Navigate maze: check expected vs actual values, boundary conditions, algorithm correctness${NC}"
            file_path=$(map_target_to_file_path "$target")
            echo -e "    Explore: ${YELLOW}make $target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
        done

    fi
fi

# Function to show motivational message based on progress
show_motivational_message() {
    local curr_errors="$1"
    local curr_passes="$2"
    local curr_failures="$3"
    local curr_compile_errors="$4"
    local curr_runtime_errors="$5"
    local curr_efficiency_ratio="$6"
    local curr_pure_test_time="$7"
    local curr_challenge="$8"

    # Read previous stats
    local stats_line="$(read_previous_stats)"
    IFS='|' read -r prev_errors prev_passes prev_failures prev_compile_errors prev_runtime_errors prev_efficiency_ratio prev_pure_test_time prev_improvement_streak prev_current_challenge prev_timestamp <<< "$stats_line"
    # Calculate changes with detailed context
    local total_curr_issues=$((curr_errors + curr_failures + curr_compile_errors + curr_runtime_errors))
    local total_prev_issues=$((prev_errors + prev_failures + prev_compile_errors + prev_runtime_errors))
    local total_change=$((total_curr_issues - total_prev_issues))
    local pass_change=$((curr_passes - prev_passes))
    local efficiency_change=$((curr_efficiency_ratio - prev_efficiency_ratio))
    local compile_change=$((prev_compile_errors - curr_compile_errors))
    local runtime_change=$((prev_runtime_errors - curr_runtime_errors))

    # Calculate improvement streak
    local current_streak=$prev_improvement_streak
    if [ "$total_change" -lt 0 ]; then
        current_streak=$((current_streak + 1))
    elif [ "$total_change" -gt 0 ]; then
        current_streak=0
    fi

    # Expanded mythological greetings with historical events and seasonal context
    local hour=$(date +%H)
    local month=$(date +%m)
    local day=$(date +%d)
    local greeting=""
    local greeting_context=""

    # Create arrays of greetings for different times and contexts
    declare -a dawn_greetings=(
        "🌅 DAWN GODDESS AURORA|Roman goddess of dawn (Greek Eos) painted the sky each morning"
        "🏺 EARLY POTTERY CRAFT|Athenian craftsmen began their work at first light - discipline builds mastery"
        "✨ ZEUS MORNING WATCH|King of Greek gods awakened on Mount Olympus as dawn broke"
        "🦉 NIGHT OWL WISDOM|Athena's owl sees clearly in darkness - late-night coding brings insight"
        "🌊 MORNING TIDE INSPIRATION|Poseidon, god of seas, brought fresh creative waves each dawn"
        "🔥 PROMETHEUS DAWN SPARK|Titan who stole fire from gods to help humanity - innovation at sunrise"
        "🏃 MARATHON DAWN RUN|Pheidippides ran 26 miles from Marathon to Athens with victory news"
        "🏛️ AGORA SUNRISE GATHERING|Athenian democracy began with citizens meeting at dawn in the marketplace"
    )

    declare -a morning_greetings=(
        "☀️ MORNING SUNRISE|Ancient Greeks believed Helios drove the sun across the sky each day"
        "🎭 DIONYSUS DREAMS|god of inspiration whispers through morning mist"
        "🏛️ DAWN ASSEMBLY|Like Athenian citizens gathering in the agora marketplace at sunrise"
        "📜 EARLY PHILOSOPHY|Morning study sessions - Socrates taught that wisdom begins at dawn"
        "✨ SPARTAN DAWN TRAINING|Ancient warriors started training at first light for discipline"
        "🏺 MORNING ORACLE|The Oracle at Delphi gave prophecies in the morning sacred hours"
        "🎯 OLYMPIC MORNING PREP|Ancient athletes trained at dawn for the Olympic Games"
        "🌿 APOLLO'S MORNING LIGHT|Greek god of music, healing, and knowledge - patron of morning wisdom"
        "⚖️ MORNING COURT SESSION|Athenian democracy - citizens debated justice at dawn assemblies"
        "🏛️ BUILDERS AT SUNRISE|Master craftsmen began work on the Parthenon at first light"
        "🌊 MORNING VOYAGE|Greek sailors departed at dawn to catch favorable winds across the Aegean"
    )

    declare -a afternoon_greetings=(
        "✨ MIDDAY POWER|Zeus ruled from Mount Olympus during the sun's peak hours"
        "🏃 AFTERNOON MARATHON|Pheidippides ran 26 miles from Marathon to Athens with victory news"
        "🎭 THEATER MATINEE|Greek dramas were performed in outdoor amphitheaters during daylight"
        "🔱 AFTERNOON SEAS|Poseidon controlled the Mediterranean during active sailing hours"
        "🏛️ GOLDEN AGE ATHENS|Pericles led Athens to greatness during the 5th century BCE"
        "📚 LIBRARY STUDY TIME|Alexandria's scholars researched during peak afternoon hours"
        "✨ TROJAN WAR BATTLES|Homer's Iliad - warriors fought during the heat of the day"
        "🎯 ARTEMIS AFTERNOON HUNT|Goddess of the hunt tracked prey through sacred forests"
        "🌋 FORGE WORK HOURS|Hephaestus crafted weapons and tools in his volcanic workshop"
        "🏺 SYMPOSIUM DISCUSSION|Greek philosophers debated over wine in afternoon gatherings"
        "🚢 ODYSSEY SAILING|Odysseus navigated home during favorable afternoon winds"
    )

    declare -a evening_greetings=(
        "🌆 EVENING HEARTH|Hestia tended the sacred fire as families gathered for dinner"
        "🌙 MOONRISE HUNT|Artemis hunted by moonlight - evening was her sacred time"
        "🎭 DIONYSUS EVENING RITES|Wine god's festivals celebrated harvest under starlight"
        "🔮 EVENING PROPHECY|Oracles gave their most powerful predictions by torchlight"
        "📜 HOMER'S EVENING TALES|Blind poet recited epic stories around evening fires"
        "🏛️ PLATO'S EVENING ACADEMY|Students discussed philosophy as the sun set over Athens"
        "🍇 HARVEST EVENING MYSTERIES|Secret religious ceremonies held after sunset"
        "🌟 NAVIGATION BY STARS|Ancient sailors used constellations to guide evening voyages"
        "🦉 ATHENA'S NIGHT WISDOM|Owl goddess saw truth most clearly in evening darkness"
        "🔥 SACRED EVENING FLAME|Temple fires burned brightest during twilight prayers"
    )

    declare -a late_night_greetings=(
        "🌙 MIDNIGHT GODDESS|Nyx ruled the night - ancient Greeks respected the power of darkness"
        "💫 SLEEP GOD'S REALM|Hypnos brought dreams and rest to tired minds after midnight"
        "🔮 CROSSROADS MAGIC|Hecate guided travelers at night crossroads with torchlight"
        "⭐ LATE NIGHT ORACLE|Apollo's prophecies were strongest during the deepest night hours"
        "🌌 ZEUS'S NIGHT WATCH|King of gods observed mortals from Mount Olympus after midnight"
        "🦉 NOCTURNAL WISDOM|Athena's owl represents wisdom that comes in quiet night hours"
        "🔥 ETERNAL NIGHT FLAME|Sacred fires burned continuously through the darkest hours"
        "💀 UNDERWORLD COUNSEL|Hades offered deep insights during the realm of sleep"
        "🌙 DREAMER'S INSPIRATION|Endymion received divine dreams during eternal slumber"
        "✨ MIDNIGHT THUNDER|Zeus sent his most powerful messages through night storms"
    )

    # Seasonal and date-based special greetings
    declare -a spring_greetings=(
        "🌸 PERSEPHONE RETURNS|queen of underworld brings spring renewal"
        "🌿 DEMETER'S BOUNTY|harvest goddess awakens the fertile earth"
        "🦋 PSYCHE'S TRANSFORMATION|soul undergoes metamorphosis like nature"
        "🌱 DIONYSUS REBIRTH|god of vegetation stirs from winter slumber"
    )

    declare -a summer_greetings=(
        "☀️ APOLLO'S DOMINION|sun god reigns supreme in golden splendor"
        "🏛️ PANATHENAIC FESTIVAL|Athens celebrates her patron goddess"
        "🏃 OLYMPIC FLAME|sacred games honor the gods with athletic glory"
        "🌊 AEGEAN ADVENTURES|perfect sailing weather calls to explorers"
    )

    declare -a autumn_greetings=(
        "🍇 HARVEST MYSTERIES|Demeter's gifts are gathered with gratitude"
        "🍂 PHILOSOPHIC SEASON|wisdom ripens like fruit on the vine"
        "🦉 ATHENA'S SEASON|goddess of wisdom strongest as days grow shorter"
        "📚 ACADEMIC GATHERING|students return to seats of learning"
    )

    declare -a winter_greetings=(
        "❄️ BOREAS WINDS|north wind brings clarity through bitter cold"
        "🔥 PROMETHEUS GIFT|fire-bringer's gift most precious in darkness"
        "📜 STORYTELLING SEASON|long nights perfect for epic recitations"
        "💎 CRYSTALLINE WISDOM|truth becomes clear like winter starlight"
    )

    # Select greeting based on time and season
    if [ "$hour" -lt 6 ]; then
        selected_greeting=$(echo "${late_night_greetings[RANDOM % ${#late_night_greetings[@]}]}")
    elif [ "$hour" -lt 9 ]; then
        selected_greeting=$(echo "${dawn_greetings[RANDOM % ${#dawn_greetings[@]}]}")
    elif [ "$hour" -lt 12 ]; then
        selected_greeting=$(echo "${morning_greetings[RANDOM % ${#morning_greetings[@]}]}")
    elif [ "$hour" -lt 18 ]; then
        selected_greeting=$(echo "${afternoon_greetings[RANDOM % ${#afternoon_greetings[@]}]}")
    else
        selected_greeting=$(echo "${evening_greetings[RANDOM % ${#evening_greetings[@]}]}")
    fi

    # Add seasonal flavor (20% chance to override with seasonal greeting)
    if [ $((RANDOM % 5)) -eq 0 ]; then
        if [ "$month" -ge 3 ] && [ "$month" -le 5 ]; then
            selected_greeting=$(echo "${spring_greetings[RANDOM % ${#spring_greetings[@]}]}")
        elif [ "$month" -ge 6 ] && [ "$month" -le 8 ]; then
            selected_greeting=$(echo "${summer_greetings[RANDOM % ${#summer_greetings[@]}]}")
        elif [ "$month" -ge 9 ] && [ "$month" -le 11 ]; then
            selected_greeting=$(echo "${autumn_greetings[RANDOM % ${#autumn_greetings[@]}]}")
        else
            selected_greeting=$(echo "${winter_greetings[RANDOM % ${#winter_greetings[@]}]}")
        fi
    fi

    # Add special historical date greetings (override everything else)
    if [ "$month" -eq 3 ] && [ "$day" -eq 15 ]; then
        selected_greeting="🗡️ IDES OF MARCH|'Beware the Ides!' - Caesar's fateful day teaches vigilance"
    elif [ "$month" -eq 4 ] && [ "$day" -eq 6 ]; then
        selected_greeting="🏃 MARATHON COMMEMORATION|Pheidippides' legendary run inspires endurance"
    elif [ "$month" -eq 8 ] && [ "$day" -eq 1 ]; then
        selected_greeting="🏛️ ATHENIAN DEMOCRACY|birth of democratic ideals shapes the world"
    elif [ "$month" -eq 9 ] && [ "$day" -eq 2 ]; then
        selected_greeting="✨ BATTLE OF ACTIUM|Octavian's victory reshapes the ancient world"
    elif [ "$month" -eq 10 ] && [ "$day" -eq 19 ]; then
        selected_greeting="🍇 OSCHOPHORIA FESTIVAL|Athenian harvest celebration honors Dionysus"
    elif [ "$month" -eq 11 ] && [ "$day" -eq 8 ]; then
        selected_greeting="🏛️ SOCRATES TRIAL|philosopher faces death for pursuing wisdom"
    elif [ "$month" -eq 12 ] && [ "$day" -eq 25 ]; then
        selected_greeting="🌟 WINTER SOLSTICE|ancient Greeks honored the return of light"
    fi

    # Parse the selected greeting
    IFS='|' read -r greeting greeting_context <<< "$selected_greeting"

    # Recalculate progress bar variables for THE FINAL DOPAMINE HIT!
    local legend_line="$(read_legend_stats)"
    IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

    # Calculate XP progress within current level using C header formula
    get_xp_for_level() {
        local level=$1
        echo $(( 1000 + (level * level * level * 200) + (level * level * 600) + (level * 300) ))
    }

    get_current_level_from_xp() {
        local total_xp=$1
        local level=1
        local cumulative_xp=0

        while true; do
            local xp_needed=$(get_xp_for_level $level)
            if [ $((cumulative_xp + xp_needed)) -gt $total_xp ]; then
                break
            fi
            cumulative_xp=$((cumulative_xp + xp_needed))
            level=$((level + 1))
        done
        echo $level
    }

    get_xp_in_current_level() {
        local total_xp=$1
        local current_level=$2
        local cumulative_xp=0

        for ((i=1; i<current_level; i++)); do
            cumulative_xp=$((cumulative_xp + $(get_xp_for_level $i)))
        done
        echo $((total_xp - cumulative_xp))
    }

    # Recalculate project level based on total XP using correct formula
    local actual_project_level=$(get_current_level_from_xp $total_project_xp)
    local current_level_xp=$(get_xp_in_current_level $total_project_xp $actual_project_level)
    local xp_needed_for_next=$(get_xp_for_level $actual_project_level)

    # Draw progress bar as separator before mythological messages - THE FINAL DOPAMINE HIT!
    local width=20
    local filled=$(( current_level_xp * width / (xp_needed_for_next > 0 ? xp_needed_for_next : 1) ))
    echo ""
    echo -n -e "${CYAN}🎯 ${BOLD_WHITE}$(format_number_with_commas $total_project_xp)${NC} ${CYAN}Project XP [${NC}"
    for ((i=0; i<width; i++)); do
        if [ $i -lt $filled ]; then
            echo -n "█"
        else
            echo -n "░"
        fi
    done
    echo -e "${CYAN}] $(format_number_with_commas $current_level_xp)/$(format_number_with_commas $xp_needed_for_next) ${BOLD_WHITE}(Level $actual_project_level)${NC}"
    # Apply bulk mode XP reduction (1/4 XP in bulk mode) BEFORE penalties
    local display_tests_xp=$SESSION_XP_FROM_TESTS
    local display_combos_xp=$SESSION_XP_FROM_COMBOS
    local display_achievements_xp=$SESSION_XP_FROM_ACHIEVEMENTS
    local display_bulk_penalty=$BULK_FAILURE_PENALTY
    local display_original_xp=$original_session_xp

    if [ "$SISYPHUS_FULL_TEST_MODE" = "1" ] && [ "$SUCCESSFULLY_RUN_FILES" -gt 0 ]; then
        display_tests_xp=$((SESSION_XP_FROM_TESTS / SUCCESSFULLY_RUN_FILES))
        display_combos_xp=$((SESSION_XP_FROM_COMBOS / SUCCESSFULLY_RUN_FILES))
        display_achievements_xp=$((SESSION_XP_FROM_ACHIEVEMENTS / SUCCESSFULLY_RUN_FILES))
        display_original_xp=$((original_session_xp / SUCCESSFULLY_RUN_FILES))
        # Penalty is NOT divided - it's applied after bulk reduction, making it harsh
    fi

    # Calculate final display XP after penalty
    local display_total_xp=$((display_original_xp > display_bulk_penalty ? display_original_xp - display_bulk_penalty : 0))

    # Always show XP breakdown with failures section
    if [ "$TOTAL_BULK_FAILURES" -gt 0 ]; then
        echo -e "💰 Session XP: ${YELLOW}$(format_number_with_commas $display_total_xp)${NC} | 🧪 From Tests: ${GREEN}$(format_number_with_commas $display_tests_xp)${NC} | 🔥 From Combos: ${ORANGE}$(format_number_with_commas $display_combos_xp)${NC} | 💀 From Failures: ${RED}-$(format_number_with_commas $display_bulk_penalty)${NC}"
        if [ "$display_original_xp" -gt 0 ] && [ "$display_bulk_penalty" -gt 0 ]; then
            echo -e "${GRAY}   Original XP: $(format_number_with_commas $display_original_xp) → Final XP: $(format_number_with_commas $display_total_xp) (${TOTAL_BULK_FAILURES} failures penalty)${NC}"
        fi
    else
        # No failures - show 0 penalty instead of achievements
        echo -e "💰 Session XP: ${YELLOW}$(format_number_with_commas $display_total_xp)${NC} | 🧪 From Tests: ${GREEN}$(format_number_with_commas $display_tests_xp)${NC} | 🔥 From Combos: ${ORANGE}$(format_number_with_commas $display_combos_xp)${NC} | 💀 From Failures: ${RED}-0${NC}"
    fi
    echo -e "${PURPLE}$greeting...${NC}"
    echo -e "    $greeting_context"

    # Analyze test results and provide technical feedback with thematic presentation
    if [ "$prev_timestamp" = "never" ]; then
        # First run - Initialize tracking
        echo -e "${CYAN}✨ TEST FRAMEWORK INITIALIZED${NC}"
        echo -e "    ${FADED}Sisyphus continuous improvement tracking started${NC}"

        if [ "$total_curr_issues" -eq 0 ]; then
            local badge=$(get_random_badge)
            echo -e "${GREEN}$badge PERFECT INITIALIZATION! All test suites passed on first run${NC}"
            echo -e "    ${FADED}Zero compilation errors, runtime crashes, or test failures detected${NC}"
        elif [ "$total_curr_issues" -le 5 ]; then
            echo -e "${PURPLE}💪 BASELINE ESTABLISHED! $total_curr_issues issues identified for improvement${NC}"
            echo -e "    ${FADED}Framework will track progress on: compilation, runtime, and test errors${NC}"
        else
            echo -e "${RED}🔥 SIGNIFICANT WORK AHEAD! $total_curr_issues issues detected across test suites${NC}"
            echo -e "    ${FADED}Focus areas: $COMPILE_ERRORS compile errors, $RUNTIME_ERRORS runtime crashes, ${#FAILED_FILES[@]} failing test files${NC}"
        fi
    elif [ "$total_change" -lt 0 ]; then
        # IMPROVEMENT DETECTED - Technical progress with gamification
        local improvement=$((total_prev_issues - total_curr_issues))
        local badge=$(get_random_badge)

        if [ "$improvement" -ge 10 ]; then
            echo -e "${GREEN}$badge LEGENDARY REFACTOR! Eliminated $improvement issues in single session${NC}"
            echo -e "    ${FADED}Major debugging breakthrough - systematic problem solving achieved${NC}"
            echo -e "${BOLD_WHITE}🎊 ACHIEVEMENT UNLOCKED: Master Debugger (10+ issues resolved)${NC}"
        elif [ "$improvement" -ge 5 ]; then
            echo -e "${GREEN}$badge EXCELLENT PROGRESS! Fixed $improvement critical issues${NC}"
            echo -e "    ${FADED}Strong debugging session - significant codebase health improvement${NC}"
            echo -e "${BOLD_WHITE}💥 ACHIEVEMENT UNLOCKED: Bug Slayer (5+ issues resolved)${NC}"
        elif [ "$improvement" -ge 2 ]; then
            echo -e "${GREEN}$badge SOLID DEBUGGING! Resolved $improvement issues${NC}"
            echo -e "    ${FADED}Consistent progress - methodical problem solving approach${NC}"
            echo -e "${BOLD_WHITE}🎯 STREAK BUILDING: Multiple issues resolved efficiently${NC}"
        else
            echo -e "${GREEN}✨ INCREMENTAL PROGRESS! Fixed $improvement issue${NC}"
            echo -e "    ${FADED}Every bug fixed improves code quality - systematic improvement${NC}"
        fi

        # Improvement streak tracking with technical context
        if [ "$current_streak" -ge 5 ]; then
            echo -e "${PURPLE}🔥 CONSISTENCY MASTER! $current_streak consecutive improvement sessions${NC}"
            echo -e "    ${FADED}Sustained debugging excellence - maintaining high development velocity${NC}"
        elif [ "$current_streak" -ge 3 ]; then
            echo -e "${CYAN}✨ MOMENTUM BUILDING! $current_streak improvements in a row${NC}"
            echo -e "    ${FADED}Strong problem-solving rhythm - effective debugging workflow established${NC}"
        elif [ "$current_streak" -ge 2 ]; then
            echo -e "${YELLOW}🎯 IMPROVEMENT STREAK! $current_streak consecutive fixes${NC}"
            echo -e "    ${FADED}Pattern of progress detected - methodical approach paying off${NC}"
        fi

        # Specific technical achievements with thematic presentation
        if [ "$compile_change" -gt 0 ]; then
            echo -e "${CYAN}🔨 SYNTAX MASTERY! Fixed $compile_change compilation error(s)${NC}"
            echo -e "    ${FADED}Code structure improved - compiler errors eliminated${NC}"
        fi
        if [ "$runtime_change" -gt 0 ]; then
            echo -e "${ORANGE}💥 MEMORY GUARDIAN! Eliminated $runtime_change segfault(s)${NC}"
            echo -e "    ${FADED}Runtime stability improved - memory safety enhanced${NC}"
        fi
        if [ "$pass_change" -gt 0 ]; then
            echo -e "${GREEN}🚀 TEST VICTORIES! +$pass_change additional tests passing${NC}"
            echo -e "    ${FADED}Code coverage improved - functionality validation enhanced${NC}"
        fi
    elif [ "$total_change" -gt 0 ]; then
        # REGRESSION DETECTED - Technical analysis with motivational framing
        local setback=$total_change
        echo -e "${YELLOW}⚠️  REGRESSION ANALYSIS: +$setback new issues introduced${NC}"

        if [ "$setback" -le 2 ]; then
            echo -e "${CYAN}🌊 MINOR SETBACK! $setback new issue(s) detected${NC}"
            echo -e "    ${FADED}Small regression - normal part of iterative development process${NC}"
            echo -e "${BOLD_WHITE}✨ DEBUGGING OPPORTUNITY: Quick fixes can restore stability${NC}"
        elif [ "$setback" -le 5 ]; then
            echo -e "${ORANGE}🐍 MODERATE REGRESSION! $setback new issues require attention${NC}"
            echo -e "    ${FADED}Code changes introduced complications - systematic debugging needed${NC}"
            echo -e "${BOLD_WHITE}🛡️ REFACTORING CHALLENGE: Time to strengthen code architecture${NC}"
        else
            echo -e "${RED}🔥 SIGNIFICANT REGRESSION! $setback new issues detected${NC}"
            echo -e "    ${FADED}Major instability introduced - comprehensive review recommended${NC}"
            echo -e "${BOLD_WHITE}👑 ARCHITECTURAL REVIEW: Consider rollback or staged fixes${NC}"
        fi

        # Technical progress despite regression
        if [ "$pass_change" -gt 0 ]; then
            echo -e "${GREEN}🌟 PARTIAL SUCCESS! +$pass_change tests improved despite regression${NC}"
            echo -e "    ${FADED}Some functionality enhanced - mixed development session${NC}"
        fi
        if [ "$efficiency_change" -gt 0 ]; then
            echo -e "${PURPLE}✨ PERFORMANCE GAIN! +${efficiency_change}x speed improvement${NC}"
            echo -e "    ${FADED}Execution efficiency improved - optimization work paying off${NC}"
        fi
    else
        # NO CHANGE - Mythological stability analysis with story progression
        echo -e "${CYAN}🏛️ ETERNAL VIGILANCE: The Oracle's vision remains unchanged${NC}"

        if [ "$total_curr_issues" -eq 0 ]; then
            # Perfect state - random celebration
            local perfect_messages=(
                "${GREEN}🏛️ MOUNT OLYMPUS ACHIEVED! Daedalus' labyrinth stands complete and flawless${NC}|Divine architecture perfected - gods themselves marvel at your craftsmanship"
                "${GREEN}👑 KING MINOS APPROVES! The royal decree: 'None shall challenge this perfect maze!'${NC}|Royal satisfaction achieved - labyrinth defenses are impenetrable"
                "${GREEN}🌟 OLYMPIAN PERFECTION! Zeus himself blesses your flawless construction${NC}|Divine harmony achieved - continue this blessed architectural state"
                "${GREEN}💎 MASTER CRAFTSMAN! Daedalus weeps with joy at your legendary skill${NC}|Legendary mastery achieved - your name echoes through mythological halls"
            )
            local selected_perfect="${perfect_messages[RANDOM % ${#perfect_messages[@]}]}"
            IFS='|' read -r message context <<< "$selected_perfect"
            echo -e "$message"
            echo -e "    ${FADED}$context${NC}"
        elif [ "$total_curr_issues" -eq 1 ]; then
            # Single issue - Minos' final decree
            if [ "$COMPILE_ERRORS" -gt 0 ]; then
                echo -e "${YELLOW}👑 MINOS' FINAL DECREE! One blueprint flaw blocks the royal seal${NC}"
                echo -e "    ${FADED}King Minos awaits: fix the final architectural error to complete the labyrinth${NC}"
            elif [ "$RUNTIME_ERRORS" -gt 0 ]; then
                echo -e "${YELLOW}👑 MINOS' FINAL DECREE! One beast prowls the completed corridors${NC}"
                echo -e "    ${FADED}King Minos commands: slay the final creature to secure the labyrinth${NC}"
            else
                echo -e "${YELLOW}👑 MINOS' FINAL DECREE! One corridor remains unexplored${NC}"
                echo -e "    ${FADED}King Minos demands: navigate the final passage to prove the maze${NC}"
            fi
        elif [ "$total_curr_issues" -le 3 ]; then
            # Few issues - near completion variety
            local near_completion_messages=(
                "${CYAN}🏗️ DAEDALUS INSPECTS! 'Only $total_curr_issues final touches remain on my masterpiece'${NC}|Master architect nods approvingly - the labyrinth nears completion"
                "${CYAN}🗡️ THESEUS APPROACHES! The hero senses only $total_curr_issues obstacles ahead${NC}|Legendary warrior prepares - victory lies just beyond these final trials"
                "${CYAN}⚖️ ATHENA'S WISDOM! Only $total_curr_issues minor trials separate you from glory${NC}|Goddess of wisdom whispers - near-divine mastery beckons heroically"
                "${CYAN}🏛️ TEMPLE GATES OPEN! $total_curr_issues sacred trials guard the inner sanctum${NC}|Divine threshold approaches - the gods prepare your final tests"
            )
            local selected_near="${near_completion_messages[RANDOM % ${#near_completion_messages[@]}]}"
            IFS='|' read -r message context <<< "$selected_near"
            echo -e "$message"
            echo -e "    ${FADED}$context${NC}"
        elif [ "$total_curr_issues" -le 6 ]; then
            # Moderate issues - active construction/exploration
            local moderate_messages=(
                "${PURPLE}🏗️ CONSTRUCTION CONTINUES! Daedalus works tirelessly on $total_curr_issues blueprint sections${NC}|Master architect's plans take shape - systematic building progresses steadily"
                "${PURPLE}🗡️ BEAST HUNTING PARTY! $total_curr_issues creatures roam the labyrinth depths${NC}|Brave warriors venture forth - each beast slain secures another corridor"
                "${PURPLE}🧭 CORRIDOR MAPPING! Explorers chart $total_curr_issues unexplored passages${NC}|Methodical exploration continues - each path mapped brings clarity to the maze"
                "${PURPLE}⚔️ TRIALS OF HEROES! $total_curr_issues challenges test your worthiness${NC}|Epic journey unfolds - heroes face trials that forge legends"
            )
            local selected_moderate="${moderate_messages[RANDOM % ${#moderate_messages[@]}]}"
            IFS='|' read -r message context <<< "$selected_moderate"
            echo -e "$message"
            echo -e "    ${FADED}$context${NC}"
        else
            # Many issues - epic quest beginning
            local epic_quest_messages=(
                "${RED}🏗️ DAEDALUS CALLS! 'The labyrinth blueprints need $total_curr_issues corrections, young architect!'${NC}|Master craftsman seeks your aid - great construction projects demand patience and skill"
                "${RED}🐲 MYTHICAL BEASTS ROAM! $total_curr_issues creatures have invaded the sacred grounds${NC}|Ancient monsters threaten the realm - heroes rise to face legendary challenges"
                "${RED}🌊 LABYRINTH DEPTHS! $total_curr_issues twisted passages confound even brave explorers${NC}|Maze complexity challenges all who enter - systematic exploration reveals hidden truths"
                "${RED}⚡ EPIC JOURNEY BEGINS! $total_curr_issues trials await the worthy challenger${NC}|Legendary journey starts - each obstacle overcome builds heroic strength"
                "${RED}🏛️ OLYMPIAN CHALLENGE! The gods test mortals with $total_curr_issues divine trials${NC}|Sacred tests measure your resolve - prove yourself worthy of mythological fame"
            )
            local selected_epic="${epic_quest_messages[RANDOM % ${#epic_quest_messages[@]}]}"
            IFS='|' read -r message context <<< "$selected_epic"
            echo -e "$message"
            echo -e "    ${FADED}$context${NC}"
        fi
    fi


    local final_messages=(
        "🏛️ The Fates weave victory into your thread of destiny!"
        "✨ Your code is your Sword, your logic your Shield!"
        "🌟 Each keystroke echoes through the halls of Olympus!"
        "💎 You forge digital ambrosia, food of the coding gods!"
        "🏆 Heroes are born in moments of impossible triumph!"
        "🔥 Prometheus gifted fire - you gift elegant solutions!"
        "🔥 Type with the fury of Zeus, debug with Athena's wisdom!"
        "📜 'Know thyself' - Socrates speaks through your structured code!"
        "🧘 'You have power over your mind' - Marcus Aurelius guides your focus!"
        "⚖️ 'The unexamined code is not worth running' - Socratic programming!"
        "🎯 'Excellence is never an accident' - Aristotle approves your discipline!"
        "📚 'The only true wisdom is knowing you know nothing' - Socratic humility!"
        "🏛️ 'We are what we repeatedly do' - Aristotelian habit formation!"
        "🌙 'In darkness, the wise owl sees most clearly' - Athena's night vision!"
        "🔥 'From small sparks, great fires kindle' - Heraclitean transformation!"
        "⚖️ 'Justice is the advantage of the stronger' - but your code protects the weak!"
        "🌊 'No man ever steps in the same river twice' - Heraclitean flow!"
        "💎 'Virtue is its own reward' - Stoic satisfaction in clean code!"
        "🎭 'The unexamined life is not worth living' - Socratic self-reflection!"
        "🏺 'The whole is greater than the sum of its parts' - Aristotelian architecture!"
        "✨ 'Think like a mountain' - Marcus Aurelius teaches patient debugging!"
        "🦉 'The fox knows many things, but the hedgehog knows one big thing' - Archilochus!"
        "🛡️ 'Fortune favors the bold' - but preparation favors the coder!"
        "🏛️ 'The measure of a man is what he does with power' - Plato's responsibility!"
        "🌿 'Every oak tree was once an acorn that held its ground' - Patience pays!"
    )

    # Final status assessment with technical recommendations
    if [ "$total_curr_issues" -eq 0 ]; then
        # Perfect state - epic victory declarations with new characters
        local victory_messages=(
            "${GREEN}🎉 MINOTAUR DEFEATED! The labyrinth guardian bows before your flawless code${NC}|All compilation successful, zero runtime crashes, perfect test results|${BOLD_WHITE}🌟 HEROIC LEGEND! Bards will sing of your debugging mastery!${NC}"
            "${GREEN}🏺 AMPHORA OVERFLOWS! Divine nectar of perfect code fills the sacred vessel${NC}|Immaculate architecture - every function, every pointer, every test pristine|${BOLD_WHITE}💎 CRYSTALLINE CODE! Your logic sparkles like Olympian gemstones!${NC}"
            "${GREEN}🦅 EAGLE SOARS! Zeus's messenger carries news of your triumph to all realms${NC}|Flawless execution achieved - gods study your techniques in wonder|${BOLD_WHITE}⚡ DIVINE SPARK! You've captured lightning in elegant algorithms!${NC}"
            "${GREEN}🌊 POSEIDON CALMS! The sea god stills all storms in honor of your mastery${NC}|Perfect stability - no segfaults disturb your computational seas|${BOLD_WHITE}🔱 TRIDENT WIELDER! Command over memory, logic, and compilation!${NC}"
        )
        local selected_victory="${victory_messages[RANDOM % ${#victory_messages[@]}]}"
        IFS='|' read -r message1 message2 message3 <<< "$selected_victory"
        echo -e "$message1"
        echo -e "    ${FADED}$message2${NC}"
        echo -e "$message3"
    elif [ "$total_curr_issues" -le 3 ]; then
        # Near completion - single line with Sisyphus-themed breakdown
        local final_breakdowns=(
            "$COMPILE_ERRORS blueprint flaws, $RUNTIME_ERRORS lurking beasts, ${#FAILED_FILES[@]} blocked passages"
            "$COMPILE_ERRORS structural cracks, $RUNTIME_ERRORS memory hydras, ${#FAILED_FILES[@]} maze dead-ends"
            "$COMPILE_ERRORS foundation gaps, $RUNTIME_ERRORS prowling minotaurs, ${#FAILED_FILES[@]} corridor puzzles"
            "$COMPILE_ERRORS architectural errors, $RUNTIME_ERRORS segfault serpents, ${#FAILED_FILES[@]} labyrinth riddles"
            "$COMPILE_ERRORS broken blueprints, $RUNTIME_ERRORS crash chimeras, ${#FAILED_FILES[@]} hidden doorways"
            "$COMPILE_ERRORS design flaws, $RUNTIME_ERRORS wild centaurs, ${#FAILED_FILES[@]} secret chambers"
            "$COMPILE_ERRORS cracked foundations, $RUNTIME_ERRORS buffer cyclops, ${#FAILED_FILES[@]} twisted pathways"
            "$COMPILE_ERRORS incomplete plans, $RUNTIME_ERRORS memory griffins, ${#FAILED_FILES[@]} unsolved mysteries"
            "$COMPILE_ERRORS build failures, $RUNTIME_ERRORS execution crashes, ${#FAILED_FILES[@]} test failures"
            "$COMPILE_ERRORS syntax errors, $RUNTIME_ERRORS memory errors, ${#FAILED_FILES[@]} logic errors"
            "$COMPILE_ERRORS linker issues, $RUNTIME_ERRORS runtime crashes, ${#FAILED_FILES[@]} assertion failures"
        )
        local selected_breakdown="${final_breakdowns[RANDOM % ${#final_breakdowns[@]}]}"
        colored_breakdown=$(echo "$selected_breakdown" | sed -E \
            -e "s/([0-9]+) (blueprint flaws|structural cracks|foundation gaps|architectural errors|broken blueprints|design flaws|cracked foundations|incomplete plans|build failures|syntax errors|linker issues)/\x1b[0;31m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (lurking beasts|memory hydras|prowling minotaurs|segfault serpents|crash chimeras|wild centaurs|buffer cyclops|memory griffins|execution crashes|memory errors|runtime crashes)/\x1b[0;93m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (blocked passages|maze dead-ends|corridor puzzles|labyrinth riddles|hidden doorways|secret chambers|twisted pathways|unsolved mysteries|test failures|logic errors|assertion failures)/\x1b[1;33m\1 \2\x1b[0m/g")
        echo -e "    ${FADED}Final debugging needed: $colored_breakdown${NC}"
    elif [ "$total_curr_issues" -le 5 ]; then
        # Moderate progress - single line with Sisyphus-themed breakdown
        local moderate_breakdowns=(
            "$COMPILE_ERRORS construction delays, $RUNTIME_ERRORS territorial beasts, ${#FAILED_FILES[@]} unexplored corridors"
            "$COMPILE_ERRORS workshop mishaps, $RUNTIME_ERRORS roaming monsters, ${#FAILED_FILES[@]} mapping challenges"
            "$COMPILE_ERRORS blueprint revisions, $RUNTIME_ERRORS beast encounters, ${#FAILED_FILES[@]} navigation puzzles"
            "$COMPILE_ERRORS engineering setbacks, $RUNTIME_ERRORS creature invasions, ${#FAILED_FILES[@]} path obstructions"
            "$COMPILE_ERRORS structural issues, $RUNTIME_ERRORS monster nests, ${#FAILED_FILES[@]} corridor blockages"
            "$COMPILE_ERRORS design challenges, $RUNTIME_ERRORS beast territories, ${#FAILED_FILES[@]} maze complications"
            "$COMPILE_ERRORS building problems, $RUNTIME_ERRORS lurking dangers, ${#FAILED_FILES[@]} exploration hurdles"
            "$COMPILE_ERRORS assembly errors, $RUNTIME_ERRORS beast patrols, ${#FAILED_FILES[@]} passage mysteries"
            "$COMPILE_ERRORS compilation errors, $RUNTIME_ERRORS segmentation faults, ${#FAILED_FILES[@]} failing tests"
            "$COMPILE_ERRORS dependency problems, $RUNTIME_ERRORS memory violations, ${#FAILED_FILES[@]} broken functions"
            "$COMPILE_ERRORS header conflicts, $RUNTIME_ERRORS access violations, ${#FAILED_FILES[@]} algorithm bugs"
        )
        local selected_breakdown="${moderate_breakdowns[RANDOM % ${#moderate_breakdowns[@]}]}"
        colored_breakdown=$(echo "$selected_breakdown" | sed -E \
            -e "s/([0-9]+) (construction delays|workshop mishaps|blueprint revisions|engineering setbacks|structural issues|design challenges|building problems|assembly errors|compilation errors|dependency problems|header conflicts)/\x1b[0;31m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (territorial beasts|roaming monsters|beast encounters|creature invasions|monster nests|beast territories|lurking dangers|beast patrols|segmentation faults|memory violations|access violations)/\x1b[0;93m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (unexplored corridors|mapping challenges|navigation puzzles|path obstructions|corridor blockages|maze complications|exploration hurdles|passage mysteries|failing tests|broken functions|algorithm bugs)/\x1b[1;33m\1 \2\x1b[0m/g")
        echo -e "    ${FADED}Development work ahead: $colored_breakdown${NC}"
    else
        # Many issues - single line with Sisyphus-themed breakdown
        local major_breakdowns=(
            "$COMPILE_ERRORS blueprint disasters, $RUNTIME_ERRORS beast invasions, ${#FAILED_FILES[@]} labyrinth mysteries"
            "$COMPILE_ERRORS foundation collapses, $RUNTIME_ERRORS monster hordes, ${#FAILED_FILES[@]} corridor chaos"
            "$COMPILE_ERRORS structural catastrophes, $RUNTIME_ERRORS creature stampedes, ${#FAILED_FILES[@]} maze confusion"
            "$COMPILE_ERRORS design meltdowns, $RUNTIME_ERRORS beast rampages, ${#FAILED_FILES[@]} passage pandemonium"
            "$COMPILE_ERRORS workshop explosions, $RUNTIME_ERRORS hydra attacks, ${#FAILED_FILES[@]} navigation nightmares"
            "$COMPILE_ERRORS architectural crises, $RUNTIME_ERRORS monster sieges, ${#FAILED_FILES[@]} corridor catastrophes"
            "$COMPILE_ERRORS blueprint chaos, $RUNTIME_ERRORS beast rebellions, ${#FAILED_FILES[@]} labyrinth upheaval"
            "$COMPILE_ERRORS construction disasters, $RUNTIME_ERRORS creature fury, ${#FAILED_FILES[@]} maze madness"
            "$COMPILE_ERRORS engineering failures, $RUNTIME_ERRORS beast riots, ${#FAILED_FILES[@]} passage turmoil"
            "$COMPILE_ERRORS foundational cracks, $RUNTIME_ERRORS monster mayhem, ${#FAILED_FILES[@]} exploration disasters"
            "$COMPILE_ERRORS build system failures, $RUNTIME_ERRORS program crashes, ${#FAILED_FILES[@]} test suite failures"
            "$COMPILE_ERRORS preprocessor errors, $RUNTIME_ERRORS stack overflows, ${#FAILED_FILES[@]} validation errors"
            "$COMPILE_ERRORS linking failures, $RUNTIME_ERRORS heap corruption, ${#FAILED_FILES[@]} regression bugs"
        )
        local selected_breakdown="${major_breakdowns[RANDOM % ${#major_breakdowns[@]}]}"
        colored_breakdown=$(echo "$selected_breakdown" | sed -E \
            -e "s/([0-9]+) (blueprint disasters|foundation collapses|structural catastrophes|design meltdowns|workshop explosions|architectural crises|blueprint chaos|construction disasters|engineering failures|foundational cracks|build system failures|preprocessor errors|linking failures)/\x1b[0;31m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (beast invasions|monster hordes|creature stampedes|beast rampages|hydra attacks|monster sieges|beast rebellions|creature fury|beast riots|monster mayhem|program crashes|stack overflows|heap corruption)/\x1b[0;93m\1 \2\x1b[0m/g" \
            -e "s/([0-9]+) (labyrinth mysteries|corridor chaos|maze confusion|passage pandemonium|navigation nightmares|corridor catastrophes|labyrinth upheaval|maze madness|passage turmoil|exploration disasters|test suite failures|validation errors|regression bugs)/\x1b[1;33m\1 \2\x1b[0m/g")
        echo -e "    ${FADED}Major debugging work: $colored_breakdown${NC}"
    fi

    # Display current development focus if there are issues
    if [ "$total_curr_issues" -gt 0 ] && [ -n "$curr_challenge" ]; then
        echo -e "${BOLD_WHITE}CURRENT QUEST:${NC}"

        if [[ "$curr_challenge" == *"Final Requiem"* ]] || [[ "$curr_challenge" == *"Mount Olympus"* ]]; then
            # Single issue remaining - final boss variety
            local final_quests=(
                "${YELLOW}👑 ROYAL SUMMONS: King Minos awaits your final demonstration of mastery${NC}|The throne room doors open only when perfection is achieved"
                "${YELLOW}🏛️ TEMPLE ASCENSION: Climb the final steps to the sacred coding sanctuary${NC}|Divine wisdom requires one last act of debugging devotion"
                "${YELLOW}⚡ LIGHTNING TRIAL: Zeus prepares to grant immortal status to worthy coders${NC}|Thunder rolls - prove yourself ready for Olympian programming powers"
                "${YELLOW}🌟 STELLAR ALIGNMENT: The stars align for your legendary coding achievement${NC}|Cosmic forces await your final keystroke to complete destiny"
            )
            local selected_final="${final_quests[RANDOM % ${#final_quests[@]}]}"
            IFS='|' read -r quest_msg quest_context <<< "$selected_final"
            echo -e "    $quest_msg"
            echo -e "    ${FADED}$quest_context${NC}"

        elif [[ "$curr_challenge" == *"Workshop"* ]] || [[ "$curr_challenge" == *"Daedalus"* ]]; then
            # Compile error quests - blueprint/architecture focus
            echo -e "    ${YELLOW}🏗️  REPAIR BLUEPRINTS: Daedalus needs help with ${RED}${#COMPILE_ERROR_FILES[@]} compilation error${YELLOW} files${NC}"

            # Randomly suggest a specific file to fix
            if [ ${#COMPILE_ERROR_FILES[@]} -gt 0 ]; then
                local random_compile_error="${COMPILE_ERROR_FILES[RANDOM % ${#COMPILE_ERROR_FILES[@]}]}"
                local suggested_file=$(echo "$random_compile_error" | cut -d':' -f1)
                local suggested_target=$(echo "$random_compile_error" | cut -d':' -f2)
                local file_path=$(map_target_to_file_path "$suggested_target")
                echo -e "    ${CYAN}📍 Focus on: ${BOLD_WHITE}$suggested_file${NC}"
                echo -e "    Debug: ${YELLOW}make $suggested_target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
            fi

            local blueprint_tips=(
                "Check #include statements - missing headers block construction"
                "Verify function declarations match implementations exactly"
                "Examine linker flags - libraries must be properly connected"
                "Review Makefile dependencies for correct build order"
                "Search for typos in function names and variable declarations"
                "Ensure all required libraries are installed and linked"
            )
            local selected_tip="${blueprint_tips[RANDOM % ${#blueprint_tips[@]}]}"
            echo -e "    ${FADED}Engineering wisdom: $selected_tip${NC}"

        elif [[ "$curr_challenge" == *"Hydra"* ]] || [[ "$curr_challenge" == *"Beasts"* ]] || [[ "$curr_challenge" == *"runtime"* ]]; then
            # Runtime error quests - monster hunting
            echo -e "    ${YELLOW}🗡️ SLAY MYTHICAL BEASTS: Clear ${ORANGE}${#RUNTIME_ERROR_FILES[@]} dangerous runtime error files${YELLOW} from the labyrinth depths${NC}"

            # Randomly suggest a specific file to fix
            if [ ${#RUNTIME_ERROR_FILES[@]} -gt 0 ]; then
                local random_runtime_error="${RUNTIME_ERROR_FILES[RANDOM % ${#RUNTIME_ERROR_FILES[@]}]}"
                local suggested_file=$(echo "$random_runtime_error" | cut -d':' -f1)
                local suggested_target=$(echo "$random_runtime_error" | cut -d':' -f2)
                local file_path=$(map_target_to_file_path "$suggested_target")
                echo -e "    ${CYAN}🎯 Hunt the beast in:${NC} ${YELLOW}make $suggested_target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
            fi

            local monster_tips=(
                "Run with valgrind to track memory corruption and leaks"
                "Initialize all pointers to NULL before use"
                "Check array bounds - ensure indices stay within allocated memory"
                "Verify malloc/free pairs - every allocation needs deallocation"
                "Use debugger breakpoints to trace execution before crashes"
                "Add printf statements to track variable values before segfaults"
                "Check for uninitialized variables causing random behavior"
            )
            local selected_tip="${monster_tips[RANDOM % ${#monster_tips[@]}]}"
            echo -e "    ${FADED}Monster hunting tip: $selected_tip${NC}"

        elif [[ "$curr_challenge" == *"Labyrinth"* ]] || [[ "$curr_challenge" == *"Minotaur"* ]]; then
            # Test failure quests - exploration/navigation focus
            echo -e "    ${YELLOW}🧭 EXPLORE LABYRINTH: Navigate ${YELLOW}${#FAILED_FILES[@]} test failure corridors${YELLOW} and solve their mysteries${NC}"

            # Randomly suggest a specific file to fix
            if [ ${#FAILED_FILES[@]} -gt 0 ]; then
                local random_failed_test="${FAILED_FILES[RANDOM % ${#FAILED_FILES[@]}]}"
                local suggested_file=$(echo "$random_failed_test" | cut -d':' -f1)
                local suggested_target=$(echo "$random_failed_test" | cut -d':' -f2)
                local file_path=$(map_target_to_file_path "$suggested_target")
                echo -e "    ${CYAN}🗺️ Start exploring: ${BOLD_WHITE}$suggested_file${NC}"
                echo -e "    Explore: ${YELLOW}make $suggested_target${NC} | ${CYAN}View Test:${NC} \e]8;;file://$(pwd)/$file_path\e\\Click Here\e]8;;\e\\"
            fi

            local exploration_tips=(
                "Compare expected vs actual outputs line by line"
                "Add debug prints to trace execution flow through your logic"
                "Test edge cases: empty inputs, boundary values, null data"
                "Verify loop conditions - check off-by-one errors"
                "Examine conditional statements for missing or wrong logic"
                "Use assertion messages to understand what the test expects"
                "Break complex functions into smaller, testable pieces"
            )
            local selected_exploration="${exploration_tips[RANDOM % ${#exploration_tips[@]}]}"
            echo -e "    ${FADED}Explorer's wisdom: $selected_exploration${NC}"

        else
            # Fallback for any other challenge types
            echo -e "    ${YELLOW}⚡ DIVINE TRIAL: The gods test your coding resolve with complex challenges${NC}"
            local general_tips=(
                "Debug systematically - isolate and fix one issue at a time"
                "Read error messages carefully - they contain valuable clues"
                "Use version control to track changes and revert if needed"
                "Take breaks when stuck - fresh perspective often reveals solutions"
            )
            local selected_tip="${general_tips[RANDOM % ${#general_tips[@]}]}"
            echo -e "    ${FADED}Ancient wisdom: $selected_tip${NC}"
        fi
    fi

    # Check for historical milestones
    local historical_analysis=$(analyze_historical_trends)
    if [ -n "$historical_analysis" ]; then
        check_milestones "$historical_analysis"
    fi

    # Show legend achievements
    show_legend_achievements

    local final_msg=$(get_random_message final_messages)
    echo -e "${BOLD_WHITE}$final_msg 🏛️${NC}"
}

echo ""
# Show motivational message
show_motivational_message "$TOTAL_FILE_ERRORS" "$PASSED_INDIVIDUAL_TESTS" "$FAILED_INDIVIDUAL_TESTS" "$COMPILE_ERRORS" "$RUNTIME_ERRORS" "$EFFICIENCY_RATIO" "$TOTAL_TEST_TIME" "$CURRENT_CHALLENGE"

echo ""
# Archive status message with historical context
if [ -d "$SISYPHUS_DIR" ]; then
    archive_count=$(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | wc -l)
    first_run_date=""

    # Get date of oldest run
    oldest_file=$(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | tail -1)
    if [ -n "$oldest_file" ]; then
        first_run_date=$(grep "TIMESTAMP=" "$oldest_file" | cut -d'=' -f2 | cut -d' ' -f1)
    fi



    # Intelligent trend analysis - show only the most interesting pattern
    if [ -n "$archive_count" ] && [ "$archive_count" -ge 3 ]; then
        recent_files=($(ls -1 "$SISYPHUS_DIR"/*.sp-stats 2>/dev/null | tail -5))

        # Collect data points for pattern analysis
        declare -a total_issues_timeline=()
        declare -a runtime_errors_timeline=()
        declare -a passes_timeline=()
        declare -a efficiency_timeline=()

        for file in "${recent_files[@]}"; do
            compile_errors=$(grep "COMPILE_ERRORS=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
            runtime_errors=$(grep "RUNTIME_ERRORS=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
            failures=$(grep "FAILURES=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
            passes=$(grep "PASSES=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')
            efficiency=$(grep "EFFICIENCY_RATIO=" "$file" | cut -d'=' -f2 | tr -d '\n\r' | tr -d ' ')

            if [[ "$compile_errors" =~ ^[0-9]+$ ]] && [[ "$runtime_errors" =~ ^[0-9]+$ ]] && [[ "$failures" =~ ^[0-9]+$ ]] && [[ "$passes" =~ ^[0-9]+$ ]]; then
                total_issues=$((compile_errors + runtime_errors + failures))
                total_issues_timeline+=("$total_issues")
                runtime_errors_timeline+=("$runtime_errors")
                passes_timeline+=("$passes")
                if [[ "$efficiency" =~ ^[0-9]+$ ]]; then
                    efficiency_timeline+=("$efficiency")
                fi
            fi
        done

        # Analyze patterns and show the most interesting one
        if [ ${#total_issues_timeline[@]} -ge 3 ]; then
            # Check for improvement streak (consecutive reductions from oldest to newest)
            streak_count=0
            for ((i=1; i<${#total_issues_timeline[@]}; i++)); do
                if [ "${total_issues_timeline[$i]}" -lt "${total_issues_timeline[$((i-1))]}" ]; then
                    streak_count=$((streak_count + 1))
                else
                    break
                fi
            done

            # Check for zero-error streak (from most recent backwards)
            zero_streak=0
            for ((i=${#total_issues_timeline[@]}-1; i>=0; i--)); do
                if [ "${total_issues_timeline[$i]}" -eq 0 ]; then
                    zero_streak=$((zero_streak + 1))
                else
                    break
                fi
            done

            # Check for consistent runtime beast slaying
            runtime_progress=0
            if [ ${#runtime_errors_timeline[@]} -ge 3 ]; then
                first_runtime="${runtime_errors_timeline[0]}"
                last_index=$((${#runtime_errors_timeline[@]} - 1))
                last_runtime="${runtime_errors_timeline[$last_index]}"
                if [ "$last_runtime" -lt "$first_runtime" ]; then
                    runtime_progress=$((first_runtime - last_runtime))
                fi
            fi

            # Check for test expansion (growing test suite)
            test_growth=0
            if [ ${#passes_timeline[@]} -ge 3 ]; then
                first_passes="${passes_timeline[0]}"
                last_passes_index=$((${#passes_timeline[@]} - 1))
                last_passes="${passes_timeline[$last_passes_index]}"
                if [ "$last_passes" -gt "$first_passes" ]; then
                    test_growth=$((last_passes - first_passes))
                fi
            fi



            # Show the most interesting trend (priority order)
            if [ "$zero_streak" -ge 2 ]; then
                echo -e "${GREEN}📈 Recent trend: 🏆 DIVINE PERFECTION - $zero_streak flawless runs in a row!${NC}"
            elif [ "$streak_count" -ge 1 ]; then
                echo -e "${GREEN}📈 Recent trend: 🚀 ASCENDING TRAJECTORY - $((streak_count + 1)) consecutive improvements!${NC}"
            elif [ "$runtime_progress" -ge 1 ]; then
                echo -e "${CYAN}📈 Recent trend: 🐲 HYDRA SLAYER - eliminated $runtime_progress segmentation faults & memory corruptions!${NC}"
            elif [ "$test_growth" -ge 3 ]; then
                echo -e "${YELLOW}📈 Recent trend: 🏗️ CODE ARCHITECT - expanded test suite by $test_growth tests!${NC}"
            else
                # Fallback to simple trend comparison
                first_total="${total_issues_timeline[0]}"
                last_total_index=$((${#total_issues_timeline[@]} - 1))
                last_total="${total_issues_timeline[$last_total_index]}"

                if [ "$first_total" -eq "$last_total" ]; then
                    echo -e "${PURPLE}📈 Recent trend: 🧘 STOIC CONSISTENCY - maintaining steady discipline!${NC}"
                elif [ "$last_total" -lt "$first_total" ]; then
                    echo -e "${CYAN}📈 Recent trend: 📉 GRADUAL MASTERY - slowly conquering challenges!${NC}"
                else
                    echo -e "${ORANGE}📈 Recent trend: ✨ PHOENIX RISING - transforming trials into wisdom!${NC}"
                fi
            fi
        fi
    fi
else
    echo -e "${CYAN}💡 Progress saved for next comparison${NC}"
fi

# Handle CLI commands for archive management
if [ "$1" = "history" ]; then
    view_sisyphus_history "${2:-10}"
    exit 0
elif [ "$1" = "stats" ]; then
    show_archive_stats
    exit 0
elif [ "$1" = "legend" ]; then
    show_legend_achievements
    exit 0
elif [ "$1" = "help" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo -e "${PURPLE}🏛️ Sisyphus Test Framework - Archive Commands${NC}"
    echo -e "${CYAN}Usage:${NC}"
    echo -e "  ./run_tests.sh           ${GRAY}# Run tests with progress tracking${NC}"
    echo -e "  ./run_tests.sh history   ${GRAY}# View last 10 runs${NC}"
    echo -e "  ./run_tests.sh history N ${GRAY}# View last N runs${NC}"
    echo -e "  ./run_tests.sh stats     ${GRAY}# Show legendary statistics${NC}"
    echo -e "  ./run_tests.sh legend    ${GRAY}# Show eternal legend status${NC}"
    echo -e "  ./run_tests.sh help      ${GRAY}# Show this help${NC}"
    echo ""
    echo -e "${YELLOW}🏺 Archive System:${NC}"
    echo -e "  Your progress is automatically saved in ${BOLD_WHITE}.sisyphus/${NC}"
    echo -e "  Each run creates a timestamped archive entry"
    echo -e "  Archive keeps last 30 runs for historical analysis"
    echo -e "  Milestone achievements unlock based on your journey"
    echo ""
    echo -e "${GREEN}✨ Pro Tip: Run tests regularly to build epic improvement streaks!${NC}"
    exit 0
fi

# Exit based on results
if [ "$COMPILE_ERRORS" -eq 0 ] && [ "$RUNTIME_ERRORS" -eq 0 ] && [ ${#FAILED_FILES[@]} -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed without errors! The boulder stays at the top!${NC}"
    echo ""
    echo -e "${GRAY}💡 Try './run_tests.sh history' to view your legendary journey${NC}"
    echo -e "${GRAY}💡 Or './run_tests.sh stats' to see your epic achievements${NC}"
    exit 0
else
    exit 1
fi
