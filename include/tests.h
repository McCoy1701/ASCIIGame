#ifndef __TESTS_H__
#define __TESTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Use high-resolution timing if available
#ifdef _POSIX_C_SOURCE
#include <sys/time.h>
#define USE_HIGH_RES_TIMER 1
#else
#define USE_HIGH_RES_TIMER 0
#endif

// Epic test color codes for mythological visibility
#define TEST_COLOR_RESET      "\033[0m"
#define TEST_COLOR_GREEN      "\033[32m"
#define TEST_COLOR_RED        "\033[31m"
#define TEST_COLOR_BLUE       "\033[34m"
#define TEST_COLOR_YELLOW     "\033[33m"
#define TEST_COLOR_BOLD_WHITE "\033[1;37m"
#define TEST_COLOR_PURPLE     "\033[35m"
#define TEST_COLOR_CYAN       "\033[36m"
#define TEST_COLOR_ORANGE     "\033[93m"
#define TEST_COLOR_GRAY       "\033[2m"

// Global test counters and timing (to be defined in each test file)
extern int total_tests;
extern int tests_passed;
extern int tests_failed;

// Global timing variables for suite-level timing
static double _test_suite_start_time;
static double _total_test_time = 0.0;

// Sisyphus gamification system
static int _sisyphus_beasts_slayed = 0;
static int _sisyphus_trials_conquered = 0;
static int _sisyphus_perfect_streak = 0;
static int _sisyphus_xp_earned = 0;
static int _sisyphus_combo_multiplier = 1;
static int _sisyphus_current_level = 1;
static char _sisyphus_suite_name[256] = "";
static int _sisyphus_achievements_unlocked = 0;
static int _sisyphus_total_expected_tests = 0;
static int _sisyphus_total_project_xp = 0;
static int _sisyphus_project_level = 1;
static int _sisyphus_is_full_test_mode = 0;
static int _sisyphus_xp_from_tests = 0;
static int _sisyphus_xp_from_combos = 0;
static int _sisyphus_xp_from_achievements = 0;

// Achievement flags
static int _achievement_first_blood = 0;
static int _achievement_combo_master = 0;
static int _achievement_speed_demon = 0;
static int _achievement_perfectionist = 0;
// Additional achievement flags
static int _achievement_lightning_fast = 0;      // Complete all tests under 1ms total
static int _achievement_atlas_endurance = 0;    // Complete 50+ tests in one suite
static int _achievement_hermes_messenger = 0;   // Complete suite in under 0.001 seconds
static int _achievement_hydra_slayer = 0;       // Overcome failures and still win
static int _achievement_midas_touch = 0;        // Get 100+ XP in single suite
static int _achievement_odyssey_complete = 0;   // Complete suite with perfect accuracy
static int _achievement_spartan_warrior = 0;   // Win with exactly 300 XP (reference!)
static int _achievement_trojan_horse = 0;       // Unexpected comeback (fail first, then all pass)
static int _achievement_phoenix_rising = 0;     // Achieve max combo after breaking streak
static int _achievement_golden_fleece = 0;      // Find the fastest possible time (under 10µs)
static int _achievement_pandoras_box = 0;       // Unlock 5+ achievements in one run
static int _achievement_oracle_wisdom = 0;     // Achieve level 7+ in single suite
static int _achievement_titan_strength = 0;     // Earn 1000+ total project XP
static int _achievement_nectar_gods = 0;        // Perfect run with 10x combo multiplier
static int _achievement_sisyphus_persistence = 0; // Come back from defeat to victory

// Progress tracking
static double _sisyphus_fastest_test = 999.0;
static int _sisyphus_max_combo = 0;

// Basic utility functions first
static int _get_xp_for_level(int level) {
    // Battle Arena XP Requirements: Level 1=500, 2=1200, 3=2500, 4=4800, 5=8500, etc.
    // Each level requires exponentially more battles to master
    return 200 + (level * level * level * 50) + (level * level * 150);
}

static int _get_current_level_from_xp(int total_xp) {
    int level = 1;
    int cumulative_xp = 0;
    while (cumulative_xp + _get_xp_for_level(level) <= total_xp) {
        cumulative_xp += _get_xp_for_level(level);
        level++;
    }
    return level;
}

static int _get_xp_in_current_level(int total_xp, int current_level) {
    int cumulative_xp = 0;
    for (int i = 1; i < current_level; i++) {
        cumulative_xp += _get_xp_for_level(i);
    }
    return total_xp - cumulative_xp;
}

// Count total expected tests dynamically
static int _count_expected_tests(void) {
    // Count RUN_TEST calls in current function by parsing source (simplified)
    // For now, use a reasonable default and auto-adjust
    return 8; // Will be updated dynamically as tests run
}

static void _display_progress_bar(int current, int total, const char* label, const char* color) {
    int width = 20;
    int filled = (current * width) / (total > 0 ? total : 1);
    printf("%s🎯 %s [", color, label);
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %d/%d" TEST_COLOR_RESET "\n", current, total);
}

static void _display_progress_bar_with_level(int current, int total, const char* label, const char* color, int level) {
    int width = 20;
    int filled = (current * width) / (total > 0 ? total : 1);
    printf("%s🎯 %s [", color, label);
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %d/%d " TEST_COLOR_BOLD_WHITE "(Level %d)" TEST_COLOR_RESET "\n", current, total, level);
}

// Read existing project-wide XP from sisyphus.legend
static void _read_project_xp(void) {
    FILE *fp = fopen("sisyphus.legend", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "TOTAL_PROJECT_XP=", 17) == 0) {
                _sisyphus_total_project_xp = atoi(line + 17);
            } else if (strncmp(line, "PROJECT_LEVEL=", 14) == 0) {
                _sisyphus_project_level = atoi(line + 14);
            }
        }
        fclose(fp);
    }
    // Set current level to match project level to prevent resets
    _sisyphus_current_level = _sisyphus_project_level;
}

// Update sisyphus.legend with battle spoils (preserve existing XP)
static void _update_project_xp(void) {
    // Only add earned XP, don't reset existing progress
    int new_total_xp = _sisyphus_total_project_xp + _sisyphus_xp_earned;
    int new_level = _get_current_level_from_xp(new_total_xp);

    // Read existing legend file
    FILE *read_fp = fopen("sisyphus.legend", "r");
    FILE *write_fp = fopen("sisyphus.legend.tmp", "w");
    if (!write_fp) return;

    char line[256];
    int found_xp = 0, found_level = 0;

    if (read_fp) {
        while (fgets(line, sizeof(line), read_fp)) {
            if (strncmp(line, "TOTAL_PROJECT_XP=", 17) == 0) {
                fprintf(write_fp, "TOTAL_PROJECT_XP=%d\n", new_total_xp);
                found_xp = 1;
            } else if (strncmp(line, "PROJECT_LEVEL=", 14) == 0) {
                fprintf(write_fp, "PROJECT_LEVEL=%d\n", new_level);
                found_level = 1;
            } else {
                fputs(line, write_fp);
            }
        }
        fclose(read_fp);
    }

    // Add missing fields
    if (!found_xp) fprintf(write_fp, "TOTAL_PROJECT_XP=%d\n", new_total_xp);
    if (!found_level) fprintf(write_fp, "PROJECT_LEVEL=%d\n", new_level);

    fclose(write_fp);
    rename("sisyphus.legend.tmp", "sisyphus.legend");

    // Update global variables
    _sisyphus_total_project_xp = new_total_xp;
    _sisyphus_project_level = new_level;
}

// Sisyphus file integration functions
static void _write_sisyphus_mini_stats(void) {
    // Create .sisyphus directory if it doesn't exist
    system("mkdir -p .sisyphus");

    FILE *fp = fopen(".sisyphus/mini_stats.tmp", "w");
    if (fp) {
        fprintf(fp, "SUITE_XP=%d\n", _sisyphus_xp_earned);
        fprintf(fp, "SUITE_LEVEL=%d\n", _get_current_level_from_xp(_sisyphus_xp_earned));
        fprintf(fp, "SUITE_COMBO=%d\n", _sisyphus_max_combo);
        fprintf(fp, "SUITE_SPEED=%.6f\n", _sisyphus_fastest_test);
        fprintf(fp, "SUITE_ACHIEVEMENTS=%d\n", _sisyphus_achievements_unlocked);
        fprintf(fp, "SUITE_NAME=%s\n", _sisyphus_suite_name);
        fprintf(fp, "SUITE_TESTS_PASSED=%d\n", _sisyphus_trials_conquered);
        fprintf(fp, "SUITE_STREAK=%d\n", _sisyphus_perfect_streak);
        fprintf(fp, "SUITE_XP_FROM_TESTS=%d\n", _sisyphus_xp_from_tests);
        fprintf(fp, "SUITE_XP_FROM_COMBOS=%d\n", _sisyphus_xp_from_combos);
        fprintf(fp, "SUITE_XP_FROM_ACHIEVEMENTS=%d\n", _sisyphus_xp_from_achievements);
        fclose(fp);
    }
}

static void _check_achievements(void) {
    if (!_achievement_first_blood && _sisyphus_trials_conquered >= 1) {
        _achievement_first_blood = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 50;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: First Blood! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Your first victory tastes the sweetest" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_combo_master && _sisyphus_max_combo >= 10) {
        _achievement_combo_master = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 200;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: Combo Master! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Achieved a 10+ perfect trial streak" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_speed_demon && _sisyphus_fastest_test < 0.000050) {
        _achievement_speed_demon = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 150;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: Speed Demon! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed a trial in under 50 microseconds" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_perfectionist && tests_passed >= 20 && tests_failed == 0) {
        _achievement_perfectionist = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 300;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: Perfectionist! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed 20+ trials without a single failure" TEST_COLOR_RESET "\n");
    }

    // Enhanced achievement system for battle arena
    if (!_achievement_lightning_fast && _total_test_time < 0.001 && _sisyphus_trials_conquered >= 5) {
        _achievement_lightning_fast = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 500;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: ⚡ Zeus's Lightning! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed %d battles in under 1ms - divine speed!" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered);
    }

    if (!_achievement_atlas_endurance && _sisyphus_trials_conquered >= 50) {
        _achievement_atlas_endurance = 1;
        _sisyphus_achievements_unlocked++;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 💪 Atlas Endurance!" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   Shouldered %d battles - you carry the weight of the world!" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered);
    }

    if (!_achievement_hermes_messenger && _total_test_time < 0.0001 && _sisyphus_trials_conquered >= 3) {
        _achievement_hermes_messenger = 1;
        _sisyphus_achievements_unlocked++;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🏃 Hermes Messenger!" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   Delivered results faster than divine messages!" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_midas_touch && _sisyphus_xp_earned >= 100) {
        _achievement_midas_touch = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 100;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 👑 Midas Touch! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Everything you code turns to gold - earned before bonus!" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_spartan_warrior && _sisyphus_xp_earned == 300) {
        _achievement_spartan_warrior = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 30;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_RED "🏆 ACHIEVEMENT UNLOCKED: ⚔️ Spartan Warrior!" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   Earned exactly 300 XP - This is your coding Thermopylae!" TEST_COLOR_RESET "\n");
    }

    if (!_achievement_golden_fleece && _sisyphus_fastest_test < 0.00001) {
        _achievement_golden_fleece = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 6;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 🐏 Golden Fleece!" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   Found the impossible speed - %.6fs is legendary!" TEST_COLOR_RESET "\n", _sisyphus_fastest_test);
    }

    if (!_achievement_titan_strength && _sisyphus_xp_from_tests >= 1000) {
        _achievement_titan_strength = 1;
        _sisyphus_achievements_unlocked++;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: ⛰️ Titan Strength!" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   %d total battle XP - you've overthrown the old gods!" TEST_COLOR_RESET "\n", _sisyphus_total_project_xp);
    }
}

static void _level_up_check(void) {
    int old_level = _sisyphus_current_level;
    _sisyphus_current_level = _get_current_level_from_xp(_sisyphus_xp_earned);

    if (_sisyphus_current_level > old_level) {
        printf(TEST_COLOR_PURPLE "\n🌟 LEVEL UP! Welcome to Level %d!" TEST_COLOR_RESET "\n", _sisyphus_current_level);
        if (_sisyphus_current_level >= 10) {
            printf(TEST_COLOR_YELLOW "⚡ LEGENDARY TIER! You have ascended to godlike mastery!" TEST_COLOR_RESET "\n");
        } else if (_sisyphus_current_level >= 5) {
            printf(TEST_COLOR_CYAN "🏛️ HEROIC TIER! The gods themselves take notice!" TEST_COLOR_RESET "\n");
        } else {
            printf(TEST_COLOR_GREEN "⚡ Your mastery of the divine trials grows stronger!" TEST_COLOR_RESET "\n");
        }
    }
}

// High-resolution timer function
static double get_time_precise(void) {
#if USE_HIGH_RES_TIMER
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

// Epic Sisyphus gamified test assertion macro
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(TEST_COLOR_RED "💥 TRIAL FAILED: %s" TEST_COLOR_RESET "\n", message); \
            printf(TEST_COLOR_GRAY "   🏛️  Line %d in %s" TEST_COLOR_RESET "\n", __LINE__, __FILE__); \
            _sisyphus_combo_multiplier = 1; \
            _sisyphus_perfect_streak = 0; \
            return 0; \
        } else { \
            _sisyphus_trials_conquered++; \
            int xp_gain = 10 * _sisyphus_combo_multiplier; \
            _sisyphus_xp_earned += xp_gain; \
            if (_sisyphus_combo_multiplier > 1) { \
                _sisyphus_xp_from_combos += (xp_gain - 10); \
                _sisyphus_xp_from_tests += 10; \
                printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier, message); \
                printf(TEST_COLOR_YELLOW " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
            } else { \
                _sisyphus_xp_from_tests += xp_gain; \
                printf(TEST_COLOR_CYAN "✅ TRIAL PASSED: %s" TEST_COLOR_RESET, message); \
                printf(TEST_COLOR_CYAN " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
            } \
            _sisyphus_combo_multiplier = (_sisyphus_combo_multiplier < 5) ? _sisyphus_combo_multiplier + 1 : 5; \
            _check_achievements(); \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        if (!_sisyphus_is_full_test_mode) { \
            printf(TEST_COLOR_CYAN "\n🏛️ COLOSSEUM BOUT: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_GRAY " [Lv.%d]" TEST_COLOR_RESET "\n", _sisyphus_current_level); \
            _display_progress_bar(tests_passed, _sisyphus_total_expected_tests, "Arena Progress", TEST_COLOR_BLUE); \
        } else { \
            printf(TEST_COLOR_GRAY "•"); \
        } \
        double start_time = get_time_precise(); \
        int test_result = test_func(); \
        double end_time = get_time_precise(); \
        double elapsed_time = end_time - start_time; \
        _total_test_time += elapsed_time; \
        if (elapsed_time < _sisyphus_fastest_test) _sisyphus_fastest_test = elapsed_time; \
        if (test_result) { \
            tests_passed++; \
            _sisyphus_perfect_streak++; \
            if (_sisyphus_perfect_streak > _sisyphus_max_combo) _sisyphus_max_combo = _sisyphus_perfect_streak; \
            int battle_xp = _sisyphus_trials_conquered * 5; \
            int final_xp = _sisyphus_is_full_test_mode ? (battle_xp / 4) : battle_xp; \
            _sisyphus_xp_earned += final_xp; \
            _sisyphus_xp_from_tests += final_xp; \
            printf(TEST_COLOR_GREEN "🎉 CHAMPION VICTORIOUS: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET, elapsed_time); \
            if (_sisyphus_perfect_streak >= 3) { \
                printf(TEST_COLOR_PURPLE " 🔥STREAK x%d!" TEST_COLOR_RESET, _sisyphus_perfect_streak); \
            } \
            printf("\n"); \
            if (_sisyphus_trials_conquered > 0 && !_sisyphus_is_full_test_mode) { \
                printf(TEST_COLOR_PURPLE "   🏛️ Won %d gladiator bouts! " TEST_COLOR_RESET, _sisyphus_trials_conquered); \
                printf(TEST_COLOR_YELLOW "+%d Arena XP" TEST_COLOR_RESET "\n", final_xp); \
            } \
            if (!_sisyphus_is_full_test_mode) { \
                _level_up_check(); \
            } \
        } else { \
            tests_failed++; \
            _sisyphus_beasts_slayed += _sisyphus_trials_conquered; \
            printf(TEST_COLOR_RED "💀 HEROIC DEFEAT: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET "\n", elapsed_time); \
            if (_sisyphus_trials_conquered > 0) { \
                printf(TEST_COLOR_ORANGE "   ⚔️  But you slayed %d beasts before falling!" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered); \
            } \
            _sisyphus_perfect_streak = 0; \
            _sisyphus_combo_multiplier = 1; \
        } \
        _sisyphus_trials_conquered = 0; \
        total_tests++; \
        if (!_sisyphus_is_full_test_mode) { \
            _check_achievements(); \
        } \
    } while(0)

// Epic Sisyphus gamified test suite header macro
#define TEST_SUITE_START(suite_name) \
    _test_suite_start_time = get_time_precise(); \
    _total_test_time = 0.0; \
    _sisyphus_beasts_slayed = 0; \
    _sisyphus_trials_conquered = 0; \
    _sisyphus_perfect_streak = 0; \
    _sisyphus_xp_earned = 0; \
    _sisyphus_combo_multiplier = 1; \
    _sisyphus_achievements_unlocked = 0; \
    _sisyphus_fastest_test = 999.0; \
    _sisyphus_max_combo = 0; \
    _sisyphus_total_expected_tests = _count_expected_tests(); \
    _sisyphus_is_full_test_mode = (getenv("SISYPHUS_FULL_TEST_MODE") != NULL); \
    _sisyphus_xp_from_tests = 0; \
    _sisyphus_xp_from_combos = 0; \
    _sisyphus_xp_from_achievements = 0; \
    _read_project_xp(); \
    strncpy(_sisyphus_suite_name, suite_name, sizeof(_sisyphus_suite_name) - 1); \
    if (!_sisyphus_is_full_test_mode) { \
        printf(TEST_COLOR_PURPLE "⚔️  ENTERING THE BATTLE ARENA: %s" TEST_COLOR_RESET "\n", suite_name); \
        printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_YELLOW "🔥 Prepare for combat! Each test is a battle for glory!" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_GRAY "💀 Slay beasts • Chain victories • Unlock battle achievements!" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_CYAN "🏆 Gladiator Level: %d | Total Battle XP: %d | Next Level: %d XP" TEST_COLOR_RESET "\n", _sisyphus_project_level, _sisyphus_total_project_xp, _get_xp_for_level(_sisyphus_project_level + 1)); \
    } else { \
        printf(TEST_COLOR_GRAY "🏛️  Colosseum Suite: %s" TEST_COLOR_RESET " ", suite_name); \
    }

#define TEST_SUITE_END() \
    double _suite_end_time = get_time_precise(); \
    double _total_suite_time = _suite_end_time - _test_suite_start_time; \
    _update_project_xp(); \
    _write_sisyphus_mini_stats(); \
    printf(TEST_COLOR_CYAN "\n════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_PURPLE "🎮 FINAL GAME STATS - ARENA MASTERY REPORT" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    _display_progress_bar(tests_passed, total_tests, "Victory Rate", TEST_COLOR_GREEN); \
    int current_level_xp = _get_xp_in_current_level(_sisyphus_xp_earned, _sisyphus_current_level); \
    int xp_needed_for_next = _get_xp_for_level(_sisyphus_current_level); \
    _display_progress_bar_with_level(current_level_xp, xp_needed_for_next, "Suite XP", TEST_COLOR_YELLOW, _sisyphus_current_level); \
    int project_level_xp = _get_xp_in_current_level(_sisyphus_total_project_xp, _sisyphus_project_level); \
    int project_xp_needed = _get_xp_for_level(_sisyphus_project_level); \
    _display_progress_bar_with_level(project_level_xp, project_xp_needed, "Project XP", TEST_COLOR_PURPLE, _sisyphus_project_level); \
    printf(TEST_COLOR_BOLD_WHITE "⚔️  Divine Trials Faced: %d" TEST_COLOR_RESET " | ", total_tests); \
    printf(TEST_COLOR_GREEN "🏆 Won: %d" TEST_COLOR_RESET " | ", tests_passed); \
    printf(TEST_COLOR_RED "💀 Lost: %d" TEST_COLOR_RESET "\n", tests_failed); \
    printf(TEST_COLOR_YELLOW "💰 Suite XP: %d" TEST_COLOR_RESET " | ", _sisyphus_xp_earned); \
    printf(TEST_COLOR_PURPLE "📊 Suite Lv: %d" TEST_COLOR_RESET " | ", _sisyphus_current_level); \
    printf(TEST_COLOR_CYAN "🔥 Max Combo: %d" TEST_COLOR_RESET, _sisyphus_max_combo); \
    if (_sisyphus_is_full_test_mode) { \
        printf(TEST_COLOR_GRAY " (¼ XP Mode)" TEST_COLOR_RESET); \
    } \
    printf("\n"); \
    printf(TEST_COLOR_BOLD_WHITE "🏛️  Project Total XP: %d" TEST_COLOR_RESET " | ", _sisyphus_total_project_xp); \
    printf(TEST_COLOR_BOLD_WHITE "🌟 Project Level: %d" TEST_COLOR_RESET " | ", _sisyphus_project_level); \
    printf(TEST_COLOR_GRAY "📈 XP Gained This Session: +%d" TEST_COLOR_RESET "\n", _sisyphus_xp_earned); \
    printf(TEST_COLOR_ORANGE "⚡ Beasts Slayed: %d" TEST_COLOR_RESET " | ", _sisyphus_beasts_slayed + (tests_failed == 0 ? _sisyphus_trials_conquered : 0)); \
    printf(TEST_COLOR_BLUE "⏱️  Fastest Trial: %.6fs" TEST_COLOR_RESET " | ", _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    printf(TEST_COLOR_GREEN "🏆 Achievements: %d" TEST_COLOR_RESET "\n", _sisyphus_achievements_unlocked); \
    printf(TEST_COLOR_GRAY "🕒 Arena Time: %.6fs | ⚔️  Combat: %.6fs | 📊 Overhead: %.6fs" TEST_COLOR_RESET "\n", \
           _total_suite_time, _total_test_time, _total_suite_time - _total_test_time); \
    if (tests_failed == 0) { \
        if (!_sisyphus_is_full_test_mode) { \
            if (_sisyphus_xp_earned >= 500) { \
                printf(TEST_COLOR_PURPLE "\n🌟 LEGENDARY GLADIATOR! Champion of the Colosseum!" TEST_COLOR_RESET "\n"); \
                printf(TEST_COLOR_YELLOW "⚡ %d XP earned - The crowd roars your name!" TEST_COLOR_RESET "\n", _sisyphus_xp_earned); \
            } else if (_sisyphus_max_combo >= 5) { \
                printf(TEST_COLOR_GREEN "\n🔥 COMBO GLADIATOR! Your battle rhythm is flawless!" TEST_COLOR_RESET "\n"); \
                printf(TEST_COLOR_CYAN "🎯 %dx combo achieved - True warrior precision!" TEST_COLOR_RESET "\n", _sisyphus_max_combo); \
            } else { \
                printf(TEST_COLOR_GREEN "\n🎉 COLOSSEUM CONQUERED! The arena bows to you!" TEST_COLOR_RESET "\n"); \
            } \
        } else { \
            printf(TEST_COLOR_GRAY "\n✅ Suite completed in full test mode" TEST_COLOR_RESET "\n"); \
        } \
        printf(TEST_COLOR_CYAN "💰 XP from tests: %d, " TEST_COLOR_RED "XP from combos: %d," TEST_COLOR_YELLOW " XP from achievements: %d" TEST_COLOR_RESET "\n", _sisyphus_xp_from_tests, _sisyphus_xp_from_combos, _sisyphus_xp_from_achievements); \
        return 0; \
    } else { \
        printf(TEST_COLOR_ORANGE "\n⚔️  HONORABLE DEFEAT! Warriors learn from every battle!" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_YELLOW "🎮 Level %d achieved with %d XP - Progress never stops!" TEST_COLOR_RESET "\n", _sisyphus_current_level, _sisyphus_xp_earned); \
        printf(TEST_COLOR_GRAY "📈 'Every master was once a disaster' - Keep fighting!" TEST_COLOR_RESET "\n"); \
        return 1; \
    }

// Compact gamified version for when running multiple test files in the great test runner
#define TEST_SUITE_END_QUIET() \
    double _suite_end_time = get_time_precise(); \
    double _total_suite_time = _suite_end_time - _test_suite_start_time; \
    _update_project_xp(); \
    _write_sisyphus_mini_stats(); \
    printf(TEST_COLOR_BOLD_WHITE "⚔️  Trials: %d" TEST_COLOR_RESET " | ", total_tests); \
    printf(TEST_COLOR_GREEN "🏆 Won: %d" TEST_COLOR_RESET " | ", tests_passed); \
    printf(TEST_COLOR_RED "💀 Lost: %d" TEST_COLOR_RESET " | ", tests_failed); \
    printf(TEST_COLOR_YELLOW "💰 +%dXP" TEST_COLOR_RESET " | ", _sisyphus_xp_earned); \
    printf(TEST_COLOR_PURPLE "Lv.%d" TEST_COLOR_RESET " | ", _sisyphus_current_level); \
    printf(TEST_COLOR_CYAN "🔥 %dx" TEST_COLOR_RESET " | ", _sisyphus_max_combo); \
    printf(TEST_COLOR_BOLD_WHITE "🌟 ProjLv.%d" TEST_COLOR_RESET "\n", _sisyphus_project_level); \
    printf(TEST_COLOR_GRAY "⏱️  %.6fs | 🏆 %d achievements | ⚡ %.6fs fastest" TEST_COLOR_RESET "\n", \
           _total_test_time, _sisyphus_achievements_unlocked, _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    if (tests_failed == 0) { \
        printf(TEST_COLOR_PURPLE "🎮 Perfect run! +%d XP integrated to Project Level %d!" TEST_COLOR_RESET "\n", _sisyphus_xp_earned, _sisyphus_project_level); \
        return 0; \
    } else { \
        printf(TEST_COLOR_ORANGE "⚔️  +%d XP gained! Project Level %d progress saved!" TEST_COLOR_RESET "\n", _sisyphus_xp_earned, _sisyphus_project_level); \
        return 1; \
    }

#endif
