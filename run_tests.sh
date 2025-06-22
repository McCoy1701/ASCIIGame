#!/bin/bash
# File: run_tests.sh - Enhanced Test Runner with Sisyphus Progress Tracking
# This testing framework is designed for Dopamine-driven development
# It should Gamify, and help coders with ADHD feel a sense of accomplishment.
# It should also provide a thematic experience within the world of Greek mythology.
# The Sisyphus legend serves as a metaphor for perseverance and the pursuit of perfection.

# Set environment variable to indicate this is a full test run (1/4 XP, no achievements)
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

# Greek Mythology-themed Sisyphus messages for dopamine-driven development
LEGENDARY_MESSAGES=(
    "⚡ ZEUS HIMSELF APPLAUDS! Your boulder has reached Olympus!"
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
    "⚔️ SPARTAN SPIRIT! This code shall not pass without a fight!"
    "🏛️ EPICTETUS REMINDS! 'It's not what happens, but how you react!'"
    "📚 SENECA'S WISDOM! 'Every new beginning comes from some other beginning's end!'"
    "⚡ MARCUS AURELIUS! 'The impediment to action advances action. What stands in the way becomes the way!'"
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
    "🏺 AMPHORA" "⚡ THUNDERBOLT" "🏛️ TEMPLE" "👑 LAUREL" "🔱 TRIDENT"
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

    # Read current legend stats
    local legend_line="$(read_legend_stats)"
    IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

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
        local xp_needed=$(( 200 + (correct_project_level * correct_project_level * correct_project_level * 50) + (correct_project_level * correct_project_level * 150) ))
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
        echo -e "${RED}❌ No Sisyphus archive found${NC}"
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
        echo -e "  ⚡ Peak Efficiency: ${max_efficiency}x"
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
    update_legend_stats "$RUNTIME_ERRORS" "$COMPILE_ERRORS" "$FAILED_INDIVIDUAL_TESTS" "$improvement_streak" "$is_perfect_run" "$CURRENT_CHALLENGE"
}

# Function to show legend achievements
show_legend_achievements() {
    local legend_line="$(read_legend_stats)"
    IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

    # Only show if we have significant achievements
    if [ "$total_quests" -ge 5 ] || [ "$perfect_ascensions" -ge 1 ] || [ "$longest_streak" -ge 3 ]; then
        echo ""
        echo -e "${PURPLE}⚔️ ETERNAL LEGEND STATUS${NC}"
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

        if [ "$labyrinth_corridors_explored" -gt 0 ]; then
            echo -e "${BLUE}🔮 Labyrinth Corridors Explored: $labyrinth_corridors_explored${NC}"
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
    echo -e "${RED}❌ Failed to build test dependencies${NC}"
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

# Function to extract test counts from output
extract_test_counts() {
    local output="$1"

    # Extract counts using grep and sed
    local total=$(echo "$output" | grep -o "Total Tests: [0-9]*" | grep -o "[0-9]*" || echo "0")
    local passed=$(echo "$output" | grep -o "✅ Passed: [0-9]*" | grep -o "[0-9]*" || echo "0")
    local failed=$(echo "$output" | grep -o "❌ Failed: [0-9]*" | grep -o "[0-9]*" || echo "0")

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

    # Check for runtime errors
    if echo "$output" | grep -q -E "(Segmentation fault|core dumped|Aborted|Bus error|Floating point exception)"; then
        echo "RUNTIME_ERROR"
        return
    fi

    # Check for test failures (tests ran but some failed)
    if echo "$output" | grep -q "❌ Failed: [1-9]"; then
        echo "TEST_FAILURE"
        return
    fi

    # Check if it looks like successful execution but exit code indicates failure
    if [ "$exit_code" -ne 0 ] && echo "$output" | grep -q "Total Tests:"; then
        echo "TEST_FAILURE"
        return
    fi

    # If exit code is non-zero but we can't classify it, it's probably a runtime error
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

            TOTAL_TEST_TIME=$(echo "$TOTAL_TEST_TIME + $actual_test_time" | bc -l)
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $shell_overhead" | bc -l)

            echo -e " ${BOLD_WHITE}(Test: $(printf "%.6f" $actual_test_time)s | Shell: $(printf "%.3f" $shell_overhead)s)${NC}"
            echo -e "  ${GREEN}✅ $test_name: All $file_total tests passed${NC}"
            PASSED_FILES=$((PASSED_FILES + 1))
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

            TOTAL_TEST_TIME=$(echo "$TOTAL_TEST_TIME + $actual_test_time" | bc -l)
            TOTAL_SHELL_OVERHEAD=$(echo "$TOTAL_SHELL_OVERHEAD + $shell_overhead" | bc -l)

            echo -e " ${BOLD_WHITE}(Test: $(printf "%.6f" $actual_test_time)s | Shell: $(printf "%.3f" $shell_overhead)s)${NC}"
            echo -e "  ${YELLOW}⚠️  $test_name: $file_passed/$file_total tests passed, $file_failed failed${NC}"
            FAILED_FILES+=("$test_name:$make_target:TEST_FAILURE")
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

# Set current challenge based on remaining issues
if [ "$COMPILE_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="🔨 Compilation Errors (${COMPILE_ERRORS} syntax/build issues to fix)"
elif [ "$RUNTIME_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="💥 Runtime Crashes (${RUNTIME_ERRORS} segfaults/memory errors to debug)"
elif [ ${#FAILED_FILES[@]} -gt 0 ]; then
    if [ ${#FAILED_FILES[@]} -eq 12 ]; then
        CURRENT_CHALLENGE="🎯 Major Refactor Needed (${#FAILED_FILES[@]} test suites failing)"
    elif [ ${#FAILED_FILES[@]} -eq 1 ]; then
        CURRENT_CHALLENGE="🔧 Final Fix Required (${#FAILED_FILES[@]} remaining test failure)"
    else
        CURRENT_CHALLENGE="🐛 Multiple Test Failures (${#FAILED_FILES[@]} test suites need debugging)"
    fi
elif [ "$TOTAL_FILE_ERRORS" -gt 0 ]; then
    CURRENT_CHALLENGE="⚠️ Mixed Issues (${TOTAL_FILE_ERRORS} various problems detected)"
else
    CURRENT_CHALLENGE="✅ All Tests Passing (code quality achieved)"
fi

# Show progress comparison before final summary
show_progress "$TOTAL_FILE_ERRORS" "$PASSED_INDIVIDUAL_TESTS" "$FAILED_INDIVIDUAL_TESTS" "$COMPILE_ERRORS" "$RUNTIME_ERRORS" "$EFFICIENCY_RATIO" "$TOTAL_TEST_TIME"

# Write current stats to .sisyphus file
write_current_stats "$TOTAL_FILE_ERRORS" "$PASSED_INDIVIDUAL_TESTS" "$FAILED_INDIVIDUAL_TESTS" "$COMPILE_ERRORS" "$RUNTIME_ERRORS" "$EFFICIENCY_RATIO" "$TOTAL_TEST_TIME" "$IMPROVEMENT_STREAK" "$CURRENT_CHALLENGE"

# Final Summary
SUCCESSFUL_FILES=$((PASSED_FILES))
FAILED_FILE_COUNT=$((${#FAILED_FILES[@]}))

echo ""
echo -e "${PURPLE}📊 Current Test Results Summary${NC}"
# Read current project level and XP for progress bar
legend_line="$(read_legend_stats)"
IFS='|' read -r total_runs total_quests hydra_beasts blueprint_flaws oracle_mysteries divine_streaks longest_streak perfect_ascensions first_quest total_project_xp project_level <<< "$legend_line"

# Calculate XP progress within current level using C header formula
# XP formula: 200 + (level^3 * 50) + (level^2 * 150)
get_xp_for_level() {
    local level=$1
    echo $(( 200 + (level * level * level * 50) + (level * level * 150) ))
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
echo -e "${CYAN}⚡ Performance Analysis:${NC}"
echo -e "${BOLD_WHITE}⏱️  Pure Test Time:     $(printf "%10.5f" $TOTAL_TEST_TIME) seconds${NC}"
echo -e "${BOLD_WHITE}🐚 Shell Overhead:     $(printf "%10.3f" $TOTAL_SHELL_OVERHEAD) seconds${NC}"
echo -e "${BOLD_WHITE}🕒 Total Runtime:      $(printf "%10.4f" $overall_time) seconds${NC}"

# Calculate and display code efficiency
if (( $(echo "$TOTAL_TEST_TIME > 0" | bc -l) )); then
    echo -e "${BOLD_WHITE}🚀 Code Efficiency: Your Code is ${GREEN}${EFFICIENCY_RATIO}x${BOLD_WHITE} faster than the Sisyphus Framework!${NC}"
fi



# Show detailed error breakdown (keeping the existing detailed error reporting)
if [ ${#COMPILE_ERROR_FILES[@]} -gt 0 ] || [ ${#RUNTIME_ERROR_FILES[@]} -gt 0 ] || [ ${#FAILED_FILES[@]} -gt 0 ]; then
    echo ""
    echo -e "${RED}💥 Issues Found:${NC}"

    if [ ${#COMPILE_ERROR_FILES[@]} -gt 0 ]; then
        echo ""
        echo -e "${RED}🔨 Compilation Errors:${NC}"
        for error_file in "${COMPILE_ERROR_FILES[@]}"; do
            name=$(echo "$error_file" | cut -d':' -f1)
            target=$(echo "$error_file" | cut -d':' -f2)
            echo -e "  ${RED}• $name${NC} - Fix compilation issues"
            echo -e "    Run: ${YELLOW}make $target${NC} to see compiler errors"
        done
    fi

    if [ ${#RUNTIME_ERROR_FILES[@]} -gt 0 ]; then
        echo -e "${ORANGE}💥 Runtime Errors (Segfaults/Crashes):${NC}"
        for error_file in "${RUNTIME_ERROR_FILES[@]}"; do
            name=$(echo "$error_file" | cut -d':' -f1)
            target=$(echo "$error_file" | cut -d':' -f2)
            echo -e "  ${ORANGE}• $name${NC} - Segfault or crash during execution"
            echo -e "    Run: ${YELLOW}make $target${NC} to see crash details"
        done
    fi

    if [ ${#FAILED_FILES[@]} -gt 0 ]; then
        echo ""
        echo -e "${YELLOW}⚠️  Test Failures (Code ran but tests failed):${NC}"
        for failed in "${FAILED_FILES[@]}"; do
            name=$(echo "$failed" | cut -d':' -f1)
            target=$(echo "$failed" | cut -d':' -f2)
            echo -e "  ${YELLOW}• $name${NC} - Tests executed but some failed"
            echo -e "    Run: ${YELLOW}make $target${NC} to see detailed failures"
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
        "🌅 AURORA'S CHARIOT|goddess of dawn guides early risers to victory"
        "🏺 AMPHORA FILLING|like Athenian potters beginning their daily craft"
        "⚡ ZEUS AWAKENS|king of gods stirs from Mount Olympus slumber"
        "🦉 ATHENA'S WISDOM|owl of wisdom hoots - knowledge comes to night owls"
        "🌊 POSEIDON'S TIDE|god of seas brings fresh waves of inspiration"
        "🔥 PROMETHEUS SPARK|fire-bringer ignites the forge of creation"
        "🏃 MARATHON RUNNER|like Pheidippides racing from Marathon to Athens"
        "🎭 DIONYSUS DREAMS|god of inspiration whispers through morning mist"
    )

    declare -a morning_greetings=(
        "☀️ HELIOS ASCENDANT|sun god drives his golden chariot across the sky"
        "🏛️ AGORA GATHERING|citizens assemble in the marketplace of ideas"
        "📜 SOCRATIC CIRCLE|philosophers debate in the Academy gardens"
        "⚔️ SPARTAN TRAINING|warriors hone their skills at dawn's first light"
        "🏺 DELPHIC ORACLE|pythia breathes sacred vapors of prophecy"
        "🎯 OLYMPIC GAMES|athletes prepare for contests of divine skill"
        "🌿 APOLLO'S LAUREL|god of music and prophecy crowns the worthy"
        "⚖️ ATHENIAN COURT|justice flows like honey from democratic debate"
        "🏛️ PARTHENON RISING|master builders craft monuments to eternity"
        "🌊 AEGEAN VOYAGES|ships set sail for distant shores of discovery"
    )

    declare -a afternoon_greetings=(
        "⚡ ZEUS COMMANDS|king of gods surveys his domain from Olympus peak"
        "🏃 PHILIPPIDES RUN|messenger races with news of victory at Marathon"
        "🎭 THEATER CHORUS|tragic and comic masks dance in amphitheater"
        "🔱 POSEIDON'S REALM|earth-shaker rules the wine-dark sea"
        "💪 HERACLES LABORS|hero tackles twelve impossible tasks"
        "🏛️ PERICLES GOLDEN AGE|Athens shines in her greatest glory"
        "📚 LIBRARY OF ALEXANDRIA|scholars gather infinite scrolls of wisdom"
        "⚔️ TROJAN SIEGE|heroes battle for ten legendary years"
        "🎯 ARTEMIS HUNT|goddess of the hunt tracks through sacred groves"
        "🌋 HEPHAESTUS FORGE|god of fire shapes metal in volcanic workshop"
        "🏺 SYMPOSIUM FEAST|philosophers drink wine and debate eternal truths"
        "🚢 ODYSSEY VOYAGE|hero navigates treacherous waters toward home"
    )

    declare -a evening_greetings=(
        "🌆 HESTIA'S HEARTH|goddess of home tends the sacred fire"
        "🌙 ARTEMIS SILVER BOW|moon goddess hunts through starlit forests"
        "🎭 DIONYSUS FESTIVAL|god of wine celebrates harvest mysteries"
        "🔮 SIBYL'S PROPHECY|oracle speaks riddles by flickering torchlight"
        "📜 HOMER'S RECITAL|blind bard sings tales of gods and heroes"
        "🏛️ ACADEMY EVENING|Plato's students contemplate forms and shadows"
        "🍇 DIONYSIAC RITES|sacred mysteries celebrated under cover of darkness"
        "🌟 NAVIGATOR'S STARS|sailors read celestial maps across the Mediterranean"
        "🦉 ATHENA'S NIGHT WATCH|wisdom goddess protects the learned city"
        "🔥 SACRED FLAME|eternal fire burns in temple of Vesta"
    )

    declare -a late_night_greetings=(
        "🌙 NYX'S DOMINION|primordial goddess of night embraces the world"
        "💫 HYPNOS REALM|god of sleep brings dreams to weary minds"
        "🔮 HECATE'S CROSSROADS|triple goddess guides through dark passages"
        "⭐ CASSANDRA'S VISIONS|prophetic dreams reveal hidden truths"
        "🌌 ORPHEUS DESCENT|poet-musician journeys to underworld's depths"
        "🦉 NOCTURNAL WISDOM|Athena's owl sees clearly in darkness"
        "🔥 ETERNAL FLAME|sacred fire of knowledge burns through the night"
        "💀 HADES COUNSEL|lord of underworld offers profound insights"
        "🌙 ENDYMION'S SLEEP|eternal dreamer receives lunar inspiration"
        "⚡ MIDNIGHT THUNDER|Zeus sends divine signals through darkness"
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
        selected_greeting="⚔️ BATTLE OF ACTIUM|Octavian's victory reshapes the ancient world"
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
        echo $(( 200 + (level * level * level * 50) + (level * level * 150) ))
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
    echo -n -e "${CYAN}🎯 Project XP [${NC}"
    for ((i=0; i<width; i++)); do
        if [ $i -lt $filled ]; then
            echo -n "█"
        else
            echo -n "░"
        fi
    done
    echo -e "${CYAN}] $current_level_xp/$xp_needed_for_next ${BOLD_WHITE}(Level $actual_project_level)${NC}"
    echo -e "${PURPLE}$greeting...${NC}"
    echo -e "    $greeting_context"

    # Analyze test results and provide technical feedback with thematic presentation
    if [ "$prev_timestamp" = "never" ]; then
        # First run - Initialize tracking
        echo -e "${CYAN}⚔️  TEST FRAMEWORK INITIALIZED${NC}"
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
            echo -e "${CYAN}⚡ MOMENTUM BUILDING! $current_streak improvements in a row${NC}"
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
            echo -e "${BOLD_WHITE}⚡ DEBUGGING OPPORTUNITY: Quick fixes can restore stability${NC}"
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
            echo -e "${PURPLE}⚡ PERFORMANCE GAIN! +${efficiency_change}x speed improvement${NC}"
            echo -e "    ${FADED}Execution efficiency improved - optimization work paying off${NC}"
        fi
    else
        # NO CHANGE - Stability analysis
        echo -e "${CYAN}📊 STABLE STATE: No change in issue count detected${NC}"

        if [ "$total_curr_issues" -eq 0 ]; then
            echo -e "${GREEN}👑 PERFECT STABILITY! All tests passing, zero issues detected${NC}"
            echo -e "    ${FADED}Excellent code quality maintained - no action required${NC}"
        elif [ "$total_curr_issues" -le 3 ]; then
            echo -e "${BLUE}🎯 CONTROLLED STATE! Only $total_curr_issues issues remaining${NC}"
            echo -e "    ${FADED}Near-perfect stability - minor cleanup opportunities available${NC}"
        else
            echo -e "${PURPLE}🧠 ANALYSIS PHASE! $total_curr_issues issues unchanged${NC}"
            echo -e "    ${FADED}No progress detected - may need different debugging approach${NC}"
        fi
    fi

    # Performance analysis with technical metrics
    if [ "$efficiency_change" -gt 5000 ]; then
        echo -e "${GREEN}🚀 EXTREME OPTIMIZATION! +${efficiency_change}x execution speed improvement${NC}"
        echo -e "    ${FADED}Massive performance gain - algorithmic optimization breakthrough${NC}"
    elif [ "$efficiency_change" -gt 1000 ]; then
        echo -e "${GREEN}⚡ MAJOR SPEEDUP! +${efficiency_change}x faster execution detected${NC}"
        echo -e "    ${FADED}Significant performance improvement - code efficiency enhanced${NC}"
    elif [ "$efficiency_change" -gt 100 ]; then
        echo -e "${GREEN}🔥 PERFORMANCE BOOST! +${efficiency_change}x speed increase${NC}"
        echo -e "    ${FADED}Notable efficiency gain - optimization work successful${NC}"
    fi

    # Final mythological inspiration
    local final_messages=(
        "🏛️ The Fates weave victory into your thread of destiny!"
        "⚔️ Your code is your Sword, your logic your Shield!"
        "🌟 Each keystroke echoes through the halls of Olympus!"
        "💎 You forge digital ambrosia, food of the coding gods!"
        "🏆 Heroes are born in moments of impossible triumph!"
        "🔥 Prometheus gifted fire - you gift elegant solutions!"
        "⚡ Type with the fury of Zeus, debug with Athena's wisdom!"
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
        "🏺 'The whole is greater than the sum of its parts' - Aristotelian architecture!"
        "⚡ 'Think like a mountain' - Marcus Aurelius teaches patient debugging!"
        "🦉 'The fox knows many things, but the hedgehog knows one big thing' - Archilochus!"
        "🛡️ 'Fortune favors the bold' - but preparation favors the coder!"
        "🏛️ 'The measure of a man is what he does with power' - Plato's responsibility!"
        "🌿 'Every oak tree was once an acorn that held its ground' - Patience pays!"
    )

    # Final status assessment with technical recommendations
    if [ "$total_curr_issues" -eq 0 ]; then
        echo -e "${GREEN}🎉 PERFECT TEST SUITE! All tests passing, zero issues detected${NC}"
        echo -e "    ${FADED}Excellent code quality - ready for production deployment${NC}"
        echo -e "${BOLD_WHITE}👑 ACHIEVEMENT UNLOCKED: Code Quality Master${NC}"
    elif [ "$total_curr_issues" -le 3 ]; then
        echo -e "${CYAN}🏔️ NEAR PERFECTION! Only $total_curr_issues issue(s) remaining${NC}"
        echo -e "    ${FADED}Excellent stability - minor cleanup will achieve perfection${NC}"
    elif [ "$total_curr_issues" -le 5 ]; then
        echo -e "${PURPLE}💪 GOOD PROGRESS! $total_curr_issues issues need attention${NC}"
        echo -e "    ${FADED}Manageable issue count - focused debugging session recommended${NC}"
    else
        echo -e "${RED}🔥 WORK REQUIRED! $total_curr_issues issues detected across test suites${NC}"
        echo -e "    ${FADED}Significant debugging needed - systematic approach recommended${NC}"
    fi

    # Display current development focus if there are issues
    if [ "$total_curr_issues" -gt 0 ] && [ -n "$curr_challenge" ]; then
        echo -e "${YELLOW}🎯 PRIORITY TARGET: $curr_challenge${NC}"
        echo -e "    ${FADED}Recommended next debugging focus for maximum impact${NC}"
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
                echo -e "${CYAN}📈 Recent trend: ⚔️ HYDRA SLAYER - eliminated $runtime_progress runtime beasts!${NC}"
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
                    echo -e "${ORANGE}📈 Recent trend: ⚡ PHOENIX RISING - transforming trials into wisdom!${NC}"
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
    echo -e "${GREEN}⚡ Pro Tip: Run tests regularly to build epic improvement streaks!${NC}"
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
