#ifndef __TESTS_H__
#define __TESTS_H__
#include <stdarg.h>
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
#define TEST_COLOR_GOLD       "\033[1;33m"

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
static int _sisyphus_random_achievement_multi = 2; // Double XP for random achievement display

// --- THE SISYPHUS NARRATIVE: LEVEL-BASED TEST MESSAGING ---
// Messages change based on current project level to reflect the story progression

// Act I: The New Curse (Levels 1-10) - Learning the ropes
static const char* _act_one_beginner_messages[] = {  // Levels 1-3: Complete confusion
    "🤔 SISYPHUS PUZZLES: What... is this strange new burden?",
    "❓ CONFUSED GLANCES: Daedalus watches your fumbling attempts with pity",
    "🪨 FAMILIAR WEIGHT: This feels like the boulder, but... different",
    "👀 FIRST OBSERVATIONS: You stare at the blueprint, trying to understand"
};

static const char* _act_one_learning_messages[] = {  // Levels 4-6: Starting to understand
    "📚 LEARNING THE CRAFT: Daedalus begins to explain the architect's tools",
    "🔧 FIRST TOOLS: Your hands remember the weight of creation",
    "🏗️ BUILDING BLOCKS: Each function is a stone in the growing maze",
    "👂 DAEDALUS TEACHES: 'The Labyrinth requires precision, Sisyphus'"
};

static const char* _act_one_adapting_messages[] = {  // Levels 7-10: Getting used to it
    "⚡ GROWING SKILL: The work becomes... almost familiar",
    "🎯 FINDING RHYTHM: Each test is another push up the hill",
    "🔍 SISYPHUS ADAPTS: You begin to see patterns in the chaos",
    "🏛️ THE ROUTINE: Dawn brings work, dusk brings more work"
};

// Act II: The Labyrinth's Nature (Levels 11-20) - Understanding the true horror
static const char* _act_two_realization_messages[] = {  // Levels 11-15: True nature revealed
    "😱 HORRIBLE TRUTH: This isn't just a building... it's a trap for minds",
    "🐂 DISTANT ROARS: The beast below grows stronger with each passing day",
    "👑 MINOS WATCHES: The King's cold eyes follow your every keystroke",
    "🌙 NIGHT TERRORS: Daedalus mutters about things that shouldn't exist"
};

static const char* _act_two_bonding_messages[] = {  // Levels 16-20: Relationship with Daedalus
    "🤝 SHARED BURDEN: You and Daedalus work in grim solidarity",
    "🍷 WINE AND CONFESSIONS: Late nights reveal the architect's guilt",
    "🔒 HIDDEN CHAMBERS: Daedalus shows you rooms not on any blueprint",
    "📜 SECRET KNOWLEDGE: The Labyrinth's true purpose becomes clear"
};

// Act III: The Cracks Appear (Levels 21-30) - Breaking under pressure
static const char* _act_three_strain_messages[] = {  // Levels 21-25: Mental breakdown
    "🎭 MASKS FALLING: Neither of you can pretend this is normal anymore",
    "⚡ FIRST REBELLION: You leave subtle hints for future heroes",
    "💀 SISYPHUS BREAKS: The weight of building a tomb becomes unbearable",
    "🔥 DAEDALUS CRACKS: The architect argues with shadows in the corners"
};

static const char* _act_three_conspiracy_messages[] = {  // Levels 26-30: Planning escape
    "🪶 ICARUS REVEALED: Hidden drawings of a child with mechanical wings",
    "🤫 WHISPERED PLANS: Secret meetings by candlelight",
    "🗡️ CAPTAIN'S SUSPICION: The guards grow watchful of your activities",
    "🌊 POINT OF NO RETURN: There's no stopping the work now"
};

// Act IV: Inescapable Truths (Levels 31-40) - The escape plan
static const char* _act_four_alliance_messages[] = {  // Levels 31-35: Forming the pact
    "🤝 DESPERATE PACT: You agree to help Daedalus save his son",
    "🪶 STEALING DIVINITY: Gathering materials for the impossible wings",
    "🔧 SECRET WORKSHOP: Building hope behind false walls",
    "📐 SISYPHUS'S MATHEMATICS: Your curse becomes the key to freedom"
};

static const char* _act_four_preparation_messages[] = {  // Levels 36-40: Final preparations
    "🌙 MAPPING PATROLS: Every guard rotation memorized",
    "🎭 FINAL DECEPTION: Cruel by day, kind by night",
    "⚡ TOMORROW WE FLY: The wings are ready, the plan is set",
    "🕊️ DAWN APPROACHES: Freedom or death awaits with the sunrise"
};

// Act V: The Eternal Cycle (Levels 41-50) - Acceptance of the absurd
static const char* _act_five_flight_messages[] = {  // Levels 41-43: Flight and fall
    "🌅 DAWN OF FREEDOM: Icarus takes to the sky with impossible grace",
    "🕊️ MOMENT OF JOY: For one perfect instant, you've beaten fate itself",
    "💔 THE FALL: Dreams crash into the wine-dark sea below"
};

static const char* _act_five_bargain_messages[] = {  // Levels 44-46: Deal with Minos
    "👑 MINOS KNOWS ALL: The King was watching from the very beginning",
    "⚖️ DEVIL'S BARGAIN: Stay and help people, or watch the maze become hell",
    "✨ THE REVELATION: You were never meant to escape this place"
};

static const char* _act_five_redemption_messages[] = {  // Levels 47-49: Rebuilding with purpose
    "📝 CODER'S MANIFESTO: Every function now serves compassion",
    "🌊 MYTH REWRITTEN: Heroes speak of the helpful ghost in the maze",
    "💪 THE HAPPY SISYPHUS: You laugh at the beautiful absurdity of it all"
};

static const char* _act_five_acceptance_message = "♾️ ETERNAL CODER: In the space between bug and fix, you are perfectly free";  // Level 50: Final wisdom

// MIDAS Toolchain References - Subtle mentions of the other tools
static const char* _midas_references[] = {
    "🧠 Metis whispers wisdom about code structure",
    "🔥 Ixion's wheel reminds you to watch for infinite loops",
    "📐 Archimedes would admire this geometric precision",
    "🏗️ Even Daedalus's craftsmanship seems crude compared to this"
};

// Function to get level-appropriate test messaging
static const char* _get_narrative_test_message(int project_level) {
    // Add occasional MIDAS references (10% chance)
    if ((project_level + rand()) % 10 == 0) {
        return _midas_references[project_level % 4];
    }

    // Act I: The New Curse (Levels 1-10)
    if (project_level <= 3) {
        return _act_one_beginner_messages[project_level % 4];
    } else if (project_level <= 6) {
        return _act_one_learning_messages[(project_level - 4) % 4];
    } else if (project_level <= 10) {
        return _act_one_adapting_messages[(project_level - 7) % 4];
    }
    // Act II: The Labyrinth's Nature (Levels 11-20)
    else if (project_level <= 15) {
        return _act_two_realization_messages[(project_level - 11) % 4];
    } else if (project_level <= 20) {
        return _act_two_bonding_messages[(project_level - 16) % 4];
    }
    // Act III: The Cracks Appear (Levels 21-30)
    else if (project_level <= 25) {
        return _act_three_strain_messages[(project_level - 21) % 4];
    } else if (project_level <= 30) {
        return _act_three_conspiracy_messages[(project_level - 26) % 4];
    }
    // Act IV: Inescapable Truths (Levels 31-40)
    else if (project_level <= 35) {
        return _act_four_alliance_messages[(project_level - 31) % 4];
    } else if (project_level <= 40) {
        return _act_four_preparation_messages[(project_level - 36) % 4];
    }
    // Act V: The Eternal Cycle (Levels 41-50)
    else if (project_level <= 43) {
        return _act_five_flight_messages[(project_level - 41) % 3];
    } else if (project_level <= 46) {
        return _act_five_bargain_messages[(project_level - 44) % 3];
    } else if (project_level <= 49) {
        return _act_five_redemption_messages[(project_level - 47) % 3];
    } else if (project_level == 50) {
        return _act_five_acceptance_message;
    }
    // Beyond level 50 - cycle through final act messages
    else {
        return _act_five_redemption_messages[(project_level - 47) % 3];
    }
}

// Granular act-based inner dialogue reflecting Sisyphus's emotional journey
// Act I Part 1 (Levels 1-3): Confusion and Willingness
static const char* _act_one_part_one_thoughts[] = {
    "Perhaps this is better than the boulder - at least my mind is engaged.",
    "Daedalus seems brilliant, maybe we can create something worthy together.",
    "The gods must tire of simple punishments - this feels... different.",
    "My hands, once accustomed to a scepter's weight, now find purpose in these lines of logic.",
    "This is a puzzle, not a burden. A mind can be sharpened on a puzzle; a shoulder is only worn down by a boulder.",
    "The gods are masters of irony. To punish a king, they give him a kingdom of code to rule.",
    "Daedalus speaks with a certain cunning, a builder's wisdom. I must listen carefully; there is more to learn here than just masonry.",
    "The gods punished Ixion for his pride. I wonder if this complex task is punishment for my own hubris in thinking I could outwit them.",
    "This logic is a new kind of geometry, not of lines and circles in the sand, but of cause and effect.",
    "This dVec3_t Daedalus uses... it's a coordinate in a world of pure logic. Cleaner than the mud and stone I'm used to.",
    "He speaks ill of Archimedes' a_DrawCircle, calling it simple. But there is an honesty in a perfect circle that these complex matrices lack.",
    "I am to help build this world, one d_CreateKinmaticBody at a time. A body of code, given motion by my will."
};

// Act I Part 2 (Levels 4-6): Learning the Craft
static const char* _act_one_part_two_thoughts[] = {
    "Each function is like ruling a small kingdom - I remember this feeling of control.",
    "The architecture reminds me of the palace I built in Corinth for my beloved wife.",
    "There's an elegance to this work that the boulder never had.",
    "This pointer is a loyal subject; it goes precisely where I command. I had forgotten the satisfaction of such power.",
    "A well-written function is like a royal decree, bringing order to the chaos of the machine.",
    "I am shaping chaos into form. This is not labor; this is creation. Could this be a gift, not a curse?",
    "There is a wisdom to this structure. A good function, like a wise ruling, anticipates the needs of the future.",
    "I must be careful. A loop without end, a recursive call without a base... it is a fiery wheel that can consume the entire machine.",
    "Archimedes could move the world with a lever. With a single, well-placed line of code, I can move worlds within this machine.",
    "This dString_t is a marvel. Unlike the words of the gods, its size is not fixed; it can grow to hold any truth, or any lie.",
    "Daedalus dismisses Archimedes' aWidget_t system as toys, but these simple buttons and sliders feel more like tools of creation than instruments of punishment.",
    "With d_MatrixRotateZf, I can pivot the entire world on an axis. This is a power I never knew, even as a king.",
    "I've figured out Daedalus's naming convention. If it's elegant and works, it's d_Something. If it's a temporary hack, it's temp_final_final_v2.",
    "This pointer is the first thing in an eternity that hasn't given me a segmentation fault. I could weep.",
    "Daedalus keeps muttering about 'best practices'. From the look of this code, the best practice was 'whatever works at 3 AM'.",
};

// Act I Part 3 (Levels 7-10): Realization of Horror
static const char* _act_one_part_three_thoughts[] = {
    "By the gods... what have I agreed to help build?",
    "This isn't architecture - it's a machine designed to kill heroes and swallow armies.",
    "I cheated death twice, only to become death's architect.",
    "The boulder was honest in its futility - this work pretends to have meaning.",
    "These are not walls. They are teeth. And I am sharpening them.",
    "The elegance I admired is the cold beauty of a serpent's scales before it strikes.",
    "I see the structure now for what it is: an equation of despair, and I am the one solving for X.",
    "This is not wisdom; it is a perversion of it. The cunning of the trap-maker, the logic of the executioner.",
    "The hubris of Minos is astounding. He builds a monument to his shame and expects the gods not to notice.",
    "Even the purest geometry can be twisted to serve evil. A perfect circle can become the arena for a slaughter.",
    "I apply the d_MatrixMultiplyVec3f, and a hero's path is twisted into a dead end. This math doesn't calculate geometry; it calculates doom.",
    "Daedalus scoffs at Archimedes' a_Blit as a simple copy, but what are we doing here? We are just blitting the image of Minos's cruelty onto the world.",
    "Each new requirement from Minos is just another node pushed onto this dLinkedList_t of sorrows. An unbreakable chain, and I am the one forging the links.",
    "I've seen the core logic. There are no comments. No documentation. Just the abyss staring back.",
    "This isn't a feature; it's a bug with tenure and a pension plan. And I'm the one who has to maintain it.",
    "For a simple death trap, the number of nested if statements here is absurd. This isn't just a maze; it's a monument to over-engineering.",
    };

// Act II Part 1 (Levels 11-15): Understanding the True Purpose
static const char* _act_two_part_one_thoughts[] = {
    "Every line of code I write will be bathed in innocent blood.",
    "The beast below isn't the real monster - we are, for building its stage.",
    "I once protected my people from threats - now I create the threat.",
    "Minos's shame will become my eternal burden to bear.",
    "The tributes will enter thinking they face a simple maze - they face calculated murder.",
    "The 'edge cases' Daedalus speaks of are not bugs; they are the moments a hero might find hope, and Minos wants them gone.",
    "I am forging the chains I once defied. Is there a crueler fate?",
    "The Minotaur is the heart of this prison, but its corridors are the veins, and I am pumping them full of poison.",
    "My cunning was once used to save my own life. Now it is being used to ensure the deaths of others. This is the cruelest wisdom.",
    "This entire project is an infinite loop of cruelty, and I am the processor executing the cycle.",
    "A simple truth of physics: for every action, an equal and opposite reaction. The more we build, the greater the tragedy will be.",
    "He wants me to use d_FindIntersectionVec2f not to find a meeting point, but to guarantee a collision. To ensure the hero and the beast always meet.",
    "The simple honesty of Archimedes' a_DrawRect... here, we use the same lines to define the inescapable cells of a monster's abattoir.",
    "The King's requirements come in a dArray_t. A dynamic array of misery, and d_ResizeArray is called every time his cruelty grows.",
    "I just realized 'the tributes' are the end-users. And we're about to push a major breaking change directly to production.",
    "Daedalus's code is beautiful, but the requirements from Minos... this feels like maintaining legacy spaghetti code written by a tyrant.",
    "The spec calls for 'inescapable logic'. That's just 'infinite loop' with better marketing.",
    "In Corinth, my public works were open for all to see. This... this is a proprietary tomb, its architecture a jealously guarded, fatal secret.",
    "Daedalus's genius is locked away here, serving only one cruel master. It could be used to build wonders for the world, but instead it builds a walled garden of death.",
    "The guards enforce a vow of silence. This is not just construction; it is a trade secret, and the price of revealing it is death.",
};

// Act II Part 2 (Levels 16-20): Bonding with Daedalus
static const char* _act_two_part_two_thoughts[] = {
    "Daedalus carries guilt like I once carried the weight of a crown.",
    "We're both kings in exile - he of craft, I of cunning - bound by shared damnation.",
    "His hands shake not from age, but from the weight of what he's created before.",
    "In his eyes I see the same desperate hope I felt when I first cheated death.",
    "Perhaps two damned souls can find redemption in each other's company.",
    "He speaks of his lost nephew, Talos. I see now that he, too, is haunted by the ghosts of his own cleverness.",
    "His ambition is a mirror to my own. We both sought to rise too high, and now we are trapped in the earth.",
    "He flinches when I solve a problem too quickly. Does he see a rival in me, or a reflection of the man he used to be?",
    "In his haunted eyes, I see a broken wisdom. He knows how to build anything, except a way out of his own guilt.",
    "He is bound to the wheel of his past, just as I am bound to this stone. His son, Icarus, is the axis around which his torment spins.",
    "He speaks of simple machines, of levers and pulleys, but his own heart is a machine too complex to be engineered.",
    "He obsesses over d_NormalizeVec3f, making every vector perfect. He seeks a purity in the code that he cannot find in his own soul.",
    "Sometimes, he will stare at the a_InitAudio function from Archimedes' library, and I see him imagine a world with sound that is not screams.",
    "He uses d_PopBack on a list of tasks and sighs with relief, but I see the list of his regrets has no such function.",
    "He complains about Archimedes, but I saw him studying the Archimedes.h file. He's not jealous; he's trying to figure out how to import the library.",
    "We’ve bonded over our shared hatred of off-by-one errors. It is the great equalizer of gods and men.",
    "His hands shake when he mentions his old projects. The man is haunted by technical debt.",
    "He speaks of Athens, of the Agora where knowledge was shared freely. Here, every idea is the property of the King.",
    "I see the pain in his eyes. He is an artist forced to build a monolith for a single, cruel client, when his heart wants to create for the commons.",
    "We speak in hushed tones, our ideas like contraband. The greatest threat to Minos's project is not a flaw in the code, but a conversation between its developers.",
    "He is a master of the dMat4x4_t, yet he cannot transform the coordinates of his own past regrets.",
    "I suspect his animosity towards Archimedes is because Archimedes' work is used in joyous public festivals. Daedalus's genius is locked in this dark tower, a secret shame.",
    "We are two developers in a terrible crunch, bonding in the dark over the unreasonable demands of the project manager."
};

// Act III Part 1 (Levels 21-25): Mental Breakdown
static const char* _act_three_part_one_thoughts[] = {
    "I can no longer pretend this is just work - it's systematic murder.",
    "My wife would weep to see what I've become - builder of tombs for heroes.",
    "The crown I wore was heavy, but this guilt crushes my very soul.",
    "I'm embedding secret escape routes, but will anyone be clever enough to find them?",
    "Every night I dream of the tribute's faces that will die because of my code.",
    "The elegant functions I once admired now seem like daggers, each aimed at an innocent heart.",
    "I am no longer shaping stone; I am carving gravestones with every keystroke.",
    "My name, Sisyphus, will be remembered not for cheating gods, but for abetting a tyrant.",
    "My secret backdoors... a desperate cunning. A foolish wisdom. Will they be a lifeline or just another feature of the tomb?",
    "I have reached my limit. My own mind feels like an infinite loop of horror. Let the wheel break me; I can bear it no longer.",
    "The geometry of this place is fracturing. The walls themselves are crying out against the perversion of their design.",
    "I call d_CreateStringFromFile to read the list of tributes. Their names become memory addresses, their lives reduced to bytes.",
    "Even the color palette feels like a mockery. Daedalus names a shade of brown shit3. He sees the filth in his own beautiful creation.",
    "The dMesh_t we build is a net of triangles, yes, but it is meant to catch souls, not light.",
    "I'm adding 'features' that are just cleverly disguised backdoors. I am no longer a developer; I am a penetration tester for the damned.",
    "My performance review with Minos is going to be interesting. 'Wrote 500 lines of life-saving code that you think is for murdering people'.",
    "He wants me to patch the security holes. The problem is, I'm the one who created them. On purpose.",
    "I am embedding flaws that are actually features for the user. Hiding compassionate code inside this monstrously closed-source project.",
    "This Labyrinth is the ultimate vendor lock-in. The user—the tribute—has no other choice. And there is no support team.",
    "Every elegant algorithm I write for Minos feels like a patent being filed for a new method of torture.",
    "I look at the a_DoInput function and realize my only input is to continue the horror. My hands are bound to this cursed keyboard.",
    "The Labyrinth's complexity is becoming self-referential. I feel like I'm debugging a bug in the debugger itself.",
    "Is this what it means to be a king? To be the one who must call a_Quit, to end the suffering, even if it means ending everything?",
    "All my kingly cunning, the gift of Metis, has led to this: crafting a more efficient slaughterhouse. My wisdom has become my shame.",
    "This guilt is a wheel of fire inside my skull, a punishment worse than any Ixion faced, for his crime was ambition, and mine is complicity.",
    "I look at the code to draw a simple triangle, a perfect shape Archimedes would admire, and see only the three points of a spearhead."
};

// Act III Part 2 (Levels 26-30): Conspiracy Planning
static const char* _act_three_part_two_thoughts[] = {
    "Daedalus speaks of wings and escape - it sounds like madness, but so did cheating death.",
    "His boy Icarus deserves freedom more than we deserve our punishment.",
    "If I can't escape my curse, perhaps I can help others escape theirs.",
    "The guards suspect nothing - they see only two broken men building their tomb.",
    "Tomorrow we begin planning the impossible - turning our curse into their liberation.",
    "The Labyrinth has taught me its own secret language. Now, I will use it to speak of rebellion.",
    "Daedalus builds wings of wax and feathers. I will build an escape of logic and loopholes.",
    "Minos expects me to build a perfect prison. He does not realize I am making it perfect for our own purposes.",
    "Now, we turn our wisdom against our master. The architect of the prison will become the architect of its downfall.",
    "Let Minos have his hubris. We will use it against him. The proudest kings are always the easiest to blind.",
    "The wings... It's a problem of simple physics. Of lift, and weight, and air. A problem of the real world, a breath of fresh air in this logical tomb.",
    "His plan for the wings... it's just a dKinematicBody_t with a new vector for lift. Perhaps we can apply a force greater than the King's will.",
    "I will use d_PadLeftString to hide binary data in the padding of the King's official reports. A message hidden in plain sight.",
    "He mutters about a_GetDeltaTime. He is no longer just building; he is calculating the timing of an escape.",
    "The wings are our secret side project. We work on the main branch for Minos by day, but push to the feature/escape branch by night.",
    "Minos keeps adding feature requests for more death traps. Little does he know our sprint is focused entirely on one user story: 'Icarus Exfiltration'.",
    "Every // TODO: Fix this later I leave in the Labyrinth's code is another moment I can spend compiling the wings. This is righteous technical debt.",
    "We are forking the master branch. One path for Minos's closed, cruel system, and another, hidden, for our own open, hopeful purpose.",
    "The wings are the first piece of open-source hardware this island has ever seen. Their design is meant for freedom, not for profit or control.",
    "Minos owns the repository, but he doesn't own the knowledge in our heads. Our conspiracy is the first license that frees it.",
    "This conspiracy is the most stressful agile sprint I've ever been a part of. The user story is 'Escape Tyranny,' and the deadline is 'Before We Are Executed'.",
    "I have cheated death, a singular act of defiance. But this... this is a conspiracy of hope, a shared rebellion. It feels more profound.",
    "The d_CreateStringFromFile function can read any scroll. We will use it to read the guard schedules, the supply logs... the King's secrets.",
    "This is a new kind of cunning Daedalus and I share. Not the wisdom to build, but the secret knowledge of how to make a perfect thing fail perfectly.",
    "The hubris of this plan is magnificent. To use the King's own project to betray him... It's a crime worthy of the gods' attention.",
    "Daedalus is reverse-engineering freedom. He's taking the complex geometry of this prison and finding the one simple, physical path out.",
    "I have found the heart of the Labyrinth's power source. Daedalus has woven the King's own paranoia into a recursive energy loop. If we can disrupt it, the whole structure will become unstable.",
    "For the first time since my fall, I am not just a prisoner. I am a strategist, a co-conspirator. I feel the weight of my crown settling on my brow once more, though this time it is invisible.",
    "Our first task is to establish a secure channel. I will write a function to encode messages within the Labyrinth's logging system, a cipher disguised as a timestamp."
};

// Act IV Part 1 (Levels 31-35): Debating the Offer
static const char* _act_four_part_one_thoughts[] = {
    "Daedalus asks me to risk everything for his son - would I have done the same?",
    "My own kingship was built on protecting those I loved - how is this different?",
    "But if we fail, Minos will make our current suffering seem like mercy.",
    "The boy has done nothing wrong - he doesn't deserve to pay for his father's sins.",
    "Every great escape I ever planned started with an impossible choice.",
    "To trust Daedalus is to trust a man who has already killed for his ambition. And yet... so have I.",
    "I look at the blueprints for the wings, and for the first time, I see a form of logic that doesn't lead to a dead end.",
    "What is one more betrayal? A betrayal of Minos, to undo the betrayal of all the tributes who will be sent here.",
    "Is this the wise choice, or merely the desperate one? Metis herself would struggle with such a tangled web of loyalties.",
    "The greatest hubris is to believe one can defy fate. And yet... I have done it before. Why not again?",
    "He speaks of flight, a simple matter of angles and force. Could such a simple key unlock this impossibly complex cage?",
    "A simple a_DrawLine... one line from here to the tower where Icarus is held. Is the path truly that simple?",
    "He asks me to help build a new dMesh_t—not a cage of bronze, but wings of hope. The same skill, for the opposite purpose.",
    "The offer hangs in the air, a single boolean waiting for my input. A one or a zero. The fate of his son depends on it.",
    "His plan has a lot of dependencies. The sun, the wind, the guards' schedule... this is dependency hell.",
    "So I'm being asked to join a startup with a brilliant but unstable founder. The mission is noble, but the risk of catastrophic failure is... high.",
    "A single point of failure: the wax. Daedalus assures me it passed unit testing. Somehow, I am not comforted.",
    "His plan to free Icarus... it feels like releasing a beautiful library to the public domain. A gift to the world, in defiance of the client.",
    "Is it better to build a perfect, proprietary hell, or risk it all on one beautiful, open-source act of defiance?",
    "Minos wants a closed system with no exit points. We will build him one, but with an undocumented API for escape.",
    "The entire plan balances on a single d_DotProductVec3f. If the vector of lift is not perfectly aligned against the vector of gravity, it all comes to nothing.",
    "Daedalus is a genius, but his risk assessment is a TODO comment in the blueprint. It's up to me to consider the failure states.",
    "As a king, I sent men to their deaths for the good of the kingdom. Is sending this boy to the sky for the good of his soul any different? Yes. It feels different.",
    "A plan born of desperation rarely has the blessing of Metis. But what choice does a fool have but to try a fool's plan?",
    "To defy Minos is one thing, but to believe we can cheat the sun itself? This is a hubris that would make Ixion blush.",
    "The escape is a simple vector problem, a challenge of lift and trajectory that Archimedes could solve. But it is embedded in a labyrinth of shifting, unknowable variables.",
    "Daedalus speaks of saving his son, but there's more to it. He mentioned a 'promise to his wife'. I believe the wings are as much about atoning for his past as they are about securing Icarus's future.",
    "I have weighed my own damnation against this boy's freedom. There is no choice. My eternity is already forfeit; his is not. I will accept.",
    "This cannot be a simple escape. If Daedalus and Icarus are to survive, we must create a grand diversion. The Labyrinth itself must appear to break, to turn on its master.",
    "How absurd. The gods chained me to a boulder to teach me futility, but it is Daedalus's offer of hope that presents the most terrifying choice.",
    "True freedom is not the absence of chains, but the ability to choose what you do while wearing them. My chains are eternal, but my choice, in this moment, is my own.",
    "This is the true test, isn't it? Not of strength, nor of cunning, but of whether a damned soul can still choose to act for the salvation of another."
};

// Act IV Part 2 (Levels 36-40): All In on Rebellion
static const char* _act_four_part_two_thoughts[] = {
    "Yes, I'll help - not for escape, but because it's right.",
    "My curse gives me something valuable - infinite time to perfect our rebellion.",
    "Every function I write now serves two masters - Minos's cruelty and my own compassion.",
    "The wings are beautiful - Daedalus has turned our prison into inspiration.",
    "If we die tomorrow, at least we die trying to save an innocent.",
    "My persistence, once a curse, is now our greatest weapon. I can test these wings a thousand thousand times.",
    "Every line of code I write to please the King is a lie, covering a truth that will set others free.",
    "This feels more like kingship than my throne ever did—to wield power in the shadows for a noble cause.",
    "True wisdom, Metis's gift, is not just knowing the rules, but knowing how and when to break them for a greater cause.",
    "Let the wheel of my curse spin forever. Like Ixion, I am bound to it, but I will use its eternal momentum to power our escape.",
    "Daedalus crafts the wings with the precision of Archimedes. He is not just building a structure; he is solving the physics of freedom itself.",
    "Minos thinks we build matrices of confinement. He doesn't know I spend my nights perfecting the d_MatrixInverseTransformVec3f—the key to unwinding his every trap.",
    "Archimedes uses a_GetTextTexture to display a score. I will use it to render a hidden glyph on a corridor wall that only a true hero will understand.",
    "I use d_StringSplit on the King's decrees, separating his cruel words from the underlying logic. It is in that space, that delimiter, that I hide our rebellion.",
    "I am refactoring this prison into an escape route. This is the most satisfying code review of my eternal life.",
    "Our secret functions are filled with assert(Minos_is_not_watching). It's not the best security, but it's what we have.",
    "I am using my eternal curse to brute-force a solution to an impossible problem. The gods gave me the ultimate debugging tool: time.",
    "Let the official changelog for Minos show only cruelty and compliance. The true release notes are written in our hearts.",
    "This is more than just helping Daedalus. We are fighting against the very idea that a man's genius should be owned by a tyrant.",
    "The Labyrinth's code is obfuscated by design. Our escape plan must be clean, well-documented, and simple enough for Daedalus' son to follow.",
    "I use d_CapitalizeString on my reports to Minos. He sees bold confidence; I see an act of petty, mocking defiance.",
    "Daedalus worries about the physics. I worry about the state machine. What happens if Icarus panics? We haven't coded a state for that.",
    "This code is now a palimpsest. Beneath the cruel architecture for Minos lies a hidden text of compassion for everyone else.",
    "Our shared wisdom is now a weapon. He knows the physics, I know the systems. Together, our cunning might just be enough.",
    "Minos's hubris is our greatest shield. He cannot conceive that his own tools, his own prisoners, could be turned against him.",
    "I watched Daedalus calculate the wing's load capacity. For the first time, I saw him smile. He is not building a maze now; he is a geometer again, solving an honest problem.",
    "Daedalus has a flaw in his design for the wing's struts. My experience with the boulder has given me an intuitive understanding of stress and load-bearing that even he lacks. I will correct it.",
    "I am becoming a king again, but a different kind. My subjects are not people, but lines of code. My decrees are not laws, but algorithms of hope.",
    "The plan is set. While Daedalus prepares the hardware, I will prepare the software: a logic bomb, disguised as a routine maintenance script, that will disable the Labyrinth's internal sentries for precisely seventeen minutes.",
    "The act of rebellion itself is the victory, regardless of the outcome. To choose defiance in an unfeeling cosmos is the only freedom that matters.",
    "It is a beautiful absurdity that the tools of my damnation—this endless time, this intricate knowledge of the maze—are now the very tools of another's liberation.",
    "This is my true trial: not to push the boulder, but to build a lever long enough to move the world, even if I must remain chained to it.",
    "This is not just a betrayal of a king; it is a continuation of my original crime. I am once again defying the gods and their decree that man should be bound by fate.",
    "The gods watch, I am sure, expecting me to despair in this intricate new hell. Let them. My silent, focused work is an act of contempt for their authority.",
    "I do this not just for his son, but for the memory of sunlight on my face, the scent of the sea, the simple weight of a stone in my hand. I do this for the world they tried to take from me."
};

// Act V Part 1A (Levels 41-42): The Flight - False Hope
static const char* _act_five_part_one_a_thoughts[] = {
    "For one perfect moment, we defeated the gods themselves - Icarus soars like a true bird!",
    "The boy's joy is infectious - I haven't felt this alive since I wore my crown in Corinth.",
    "Look how Icarus mastered the wind! He is not just Daedalus's son; he is the son of defiance itself.",
    "In the reflection of his ascent, I see the peak of my own mountain, finally within reach.",
    "Such perfect, simple cunning! To use the air itself as an escape route. The King's gaze is fixed on the ground.",
    "For a moment, the fiery wheel stops. The cycle is broken. He has flown free from the curse of the earth.",
    "It is the purest display of physics I have ever witnessed. A triumph of geometry over gravity.",
    "The d_GetVec2fFromAnglef function... Daedalus used it to calculate the perfect launch angle. His son is not just flying; he is following a path of pure mathematics.",
    "This is more beautiful than any image a_LoadTexture could produce. This is a texture woven from courage and sunlight.",
    "It works on my machine! Well, on his wings. For one glorious moment, the demo ran perfectly.",
    "The deployment was a success! He cleared the perimeter! Daedalus' son is free!",
    "He's free! For one moment, a single user escaped the walled garden and touched the open sky!",
    "The design is proven. This open-source plan for flight... it works!",
    "The FPS is stable, the a_GetDeltaTime is perfect... for a moment, reality itself has no bugs.",
    "Icarus' joyous shouts... it is the first sound a_PlaySoundEffect should have ever been used for.",
    "He is a memory leak in Minos's perfect system, flying free. It is the most beautiful bug I have ever seen.",
    "The plan was perfect in its cunning! A simple distraction, a hidden launch point... the wisdom of the desperate is a powerful thing.",
    "He has escaped the cycle of the earth! A mortal boy, spitting in the face of the gods' eternal laws. What hubris! What glory!",
    "The physics holds! The wings generate lift! Daedalus has built a miracle upon the simple principles Archimedes championed.",
    "My logic bomb worked. The sentries are blind. The path is clear. All that is left is for them to seize their freedom.",
    "That laugh from Icarus... it is the sound of a successful test case, the sound of a bug being closed forever.",
    "I see it now. The true Labyrinth was never this maze of stone. It was the despair in our hearts. And for one moment, we have escaped it.",
    "Look at him. He is a brief, glorious refutation of his father's prison. In his flight, the Labyrinth and all its absurd logic cease to matter.",
    "This is what true freedom looks like: not a careful plan, but a joyful, reckless ascent. A glorious, unthinking 'yes' to the sun.",
    "For a moment, the universe held its breath. The rules were suspended. The test was passed. This single moment of triumph is worth my eternity of toil.",
    "This single act of defiance makes my centuries of toil worthwhile. One moment of a man soaring is a greater victory than an eternity of a god's tyranny.",
    "The boy's joyous rebellion is a mirror to my own. He rebels against gravity; I rebel against the gods. For a moment, we both win.",
    "This is the passion for the world that the gods so despise. They punish us for loving the sun, the sea, the sky—for finding our own meaning here, without them."
};

// Act V Part 1B (Levels 43-44): The Fall - Crushing Despair
static const char* _act_five_part_one_b_thoughts[] = {
    "No... no, the wax is melting, he's falling, and I can do nothing but watch.",
    "The sun took him - Daedalus' innocent son - the gods always collect their cruelest due.",
    "We gave him the sky, but we could not shield him from the sun. The gods always find a way to punish hope.",
    "Daedalus's scream... it is the sound of a universe breaking. I feel the echo in my own immortal, weary soul.",
    "All that wisdom, all that planning... undone by a moment's foolishness. Wisdom cannot save a boy who wants to touch the sun.",
    "The ultimate hubris. To fly so close to a god. The sun is a jealous god, and its fiery wheel is inescapable.",
    "The geometry failed. The angle was too steep, the force of the sun too great. Reality has reasserted its cruel arithmetic.",
    "The boy's dKinematicBody_t has lost its upward velocity. Gravity, the oldest god of all, has finally claimed its due.",
    "All I can do is a_PresentScene. To watch the horror unfold, to render the tragedy frame by perfect, agonizing frame.",
    "Oh no. The sun. It's a production issue. A fatal, non-recoverable production issue.",
    "We forgot to account for the single-threaded nature of reality and the heat death of the... wax.",
    "The world was not ready for such a beautiful, open technology. It was too powerful, too free.",
    "The license was permissive, but the environment was hostile. The hardware—the sun itself—was proprietary and unforgiving.",
    "A fatal aError_t. The error message is the sun itself. There is no recovery from this.",
    "His dKinematicBody_t has entered a terminal state. All my logic, all my code... I cannot catch him.",
    "The system has thrown an exception it cannot handle. Daedalus' son fell out of the sky.",
    "Our wisdom was not enough. We calculated the escape, but we did not account for the intoxicating madness of Icarus tasting freedom.",
    "The boy flew too close to the sun, a classic tale of hubris. But was the crime his, for wanting to soar, or ours, for giving him wings of wax?",
    "The math was correct, but the materials failed. A simple, brutal lesson in physics that has cost a life.",
    "My code was perfect. The diversion worked. But I, the ultimate strategist, forgot to account for the one variable I could never control: the heart of a boy drunk on freedom.",
    "I stare at the empty sky, and I understand. The gods did not strike him down. They simply let the rules of the world, of hubris and of physics, run their course. It is a far colder punishment.",
    "I must not let Daedalus's grief be in vain. The knowledge gained from this tragedy must become the seed of a future success. I will analyze the failure. I will learn.",
    "We crafted a perfect plan, a symphony of logic, only to be defeated by a single, simple truth. The absurdity is not that we failed, but that we believed we could ever succeed.",
    "In that moment of his fall, Icarus was freer than any man. He chose the sun over the sky. His freedom wasn't in surviving, but in the soaring.",
    "So this is the test after the test. Not to succeed, but to endure failure. To watch hope die and still choose to pick up the tools and continue the work.",
    "And so, the rebellion fails. The gods remind us that their laws are absolute. But the attempt itself—the flight—was a victory they can never erase.",
    "I am fully conscious of this tragedy. I see every detail, feel every ounce of Daedalus's pain. This awareness is my torment, but also my triumph. I do not look away. I bear witness.",
    "The boy flew towards the sun because it is beautiful, and he loved it too much. His was the same crime as mine: an overwhelming passion for the warmth of life. A noble crime to die for."
};

// Act V Part 2 (Levels 45-47): The King's Bargain
static const char* _act_five_part_two_thoughts[] = {
    "Minos offers me a choice - be his compassionate warden or watch him create hell.",
    "He knew about our plan all along - was this all orchestrated from the beginning?",
    "If I stay, I can save future victims - if I refuse, he'll make the maze a slaughterhouse.",
    "So this is the final trap. Not a maze of stone, but a choice between two hells of my own making.",
    "The King's offer is a gilded chain. He offers me purpose, but the price is my complicity.",
    "Minos offers a new kind of wisdom: the wisdom of the warden, who knows every secret of the prison.",
    "He offers to change the nature of my eternal cycle. The wheel will still spin, but he suggests I can choose its direction.",
    "My choice will set the angle of history for this place, the trajectory of countless lives to come.",
    "My life is now a aSelectWidget_t, with two options presented by a tyrant. I must choose a value, and live with the consequences.",
    "He wants me to maintain the dQuadTree_t, to efficiently manage the flow of tributes into his machine, I can decide what kind of machine it will become.",
    "So that's the offer. Accept the role of 'Lead Architect of Eternal Damnation' or watch him outsource the job to actual demons.",
    "He knew about the wings all along... this wasn't a bug in his security, it was a feature of his management style. A/B testing our souls.",
    "Maintain the Labyrinth... He's not giving me a choice; he's giving me root access to hell and making me the sole administrator.",
    "He wants me to be the lead maintainer of his terrible, proprietary project. The pay is my life, the bonus is his mercy.",
    "So I am to be the gatekeeper of this closed system. I can enforce its cruel rules, or... I can offer technical support to those trapped inside.",
    "The choice is clear: become a manager for a tyrant, or a secret advocate for the end-user.",
    "I am being offered a promotion to 'Architect of Hell'. The benefits are terrible, but at least I won't be fired.",
    "Minos gives me the keys to the kingdom, but the kingdom is a prison. He thinks this is a reward.",
    "My new role is to provide long-term support for a deprecated system of cruelty. I will support the users, not the system.",
    "Minos plays the statesman, offering me a 'wise' choice. But the wisest move in a tyrant's game is to never let him know you're still playing.",
    "He believes he has me bound to his wheel forever. He doesn't realize the fall of Icarus has broken my own pride, my own hubris. I am free from that, at least.",
    "The choice he offers is a complex equation of morality. But I will solve it with a simple geometric proof: the shortest path to saving the most lives.",
    "Minos's spies are not men. I see it now. They are constructs, powered by the same logic as the Labyrinth. He has eyes everywhere. He always knew.",
    "He offers me a gilded cage and the title of 'warden'. He does not understand. I have already been transformed. I will accept his terms, but I will not serve his purpose.",
    "My first act as maintainer will be to analyze the logs from Icarus's flight. I will find what went wrong. Not for vengeance, but for knowledge. The next one to fly will not fail.",
    "Minos offers me a new cage, but this one has a purpose I can choose. Is a chosen cage still a prison? This is a riddle worthy of the gods.",
    "My freedom will not be found in leaving this place. It will be found in mastering it. In turning this monument to tyranny into a library of hope.",
    "The ultimate test of a free man is how he bears a burden he cannot lay down. Minos thinks he is cursing me further; he is giving me my final exam.",
    "The gods gave me a meaningless task. Now, a mortal king offers me a meaningful one. The irony is delicious. My curse is now my career.",
    "To be free is to be aware of your chains and yet choose to act. Minos offers me a new set of chains, but he also offers me a choice. This is a new test.",
    "They think my spirit was broken by the boulder. They do not understand. The boulder taught me that all that matters is what one does between the pushing.",
    "Daedalus is gone. The one man who shared my damnation has abandoned me to it. I am truly alone with the machine we built.",
    "And in that loneliness, Minos offers me a new chain, calling it a leash of command. But he forgets: a chain is a tool, and I am a master craftsman.",
    "My pact with Daedalus is broken. So I will make a new pact, an unspoken one, with the heroes who are yet to come. They will be my new confederates.",
    "With these hands, I helped build a weapon. Now, with these same hands, I will build a shield within its walls. My creations will now preserve life, not take it.",
    "He thinks he is making me a jailer, but he is making me a gatekeeper. He cannot see the difference, and that will be his undoing.",
    "Yes, I will take his bargain. A king must have a kingdom, after all. Even if it is a kingdom of sorrow, I will rule it with compassion."
};

// Act V Part 3 (Levels 48-50): Acceptance and Transformation
static const char* _act_five_part_three_thoughts[] = {
    "I choose to stay - not as prisoner, but as protector of those who'll come after.",
    "Every algorithm I write now is an act of rebellion disguised as compliance.",
    "The labyrinth becomes my kingdom again - but this time, I rule with mercy.",
    "Heroes will speak of the helpful ghost in the maze - the king who chose compassion over escape.",
    "Minos thinks he has a prisoner. He is mistaken. What he has is a saboteur with an eternity to work.",
    "Daedalus sought freedom in the sky. I have found it here, in the ones and zeros, turning this cage into a shield for others.",
    "The boulder is the same, but I am not. I push it now not as a punishment, but as a purpose. Let the gods watch; I am finally happy.",
    "This is the final wisdom Metis could teach: not to escape the curse, but to transform it into a blessing for others.",
    "The infinite loop is no longer a bug. I see now it is the very engine of my existence, an eternal cycle I choose to perpetuate.",
    "Archimedes gave the world a lever to move the Earth. I have been given a single line of code to move a soul. Perhaps my tool is the mightier.",
    "With each hero I guide, I call a_UpdateTitle. The Labyrinth is no longer 'Minos's Prison'. It is now 'Sisyphus's Compass'.",
    "I look at the list of incoming tributes, this dLinkedList_t of the damned, and I know my purpose. Not to add to it, but to call d_ClearLinkedList on it, one soul at a time.",
    "The beast was a dKinematicBody_t trapped by force. I will now apply a new vector: mercy. I will change the physics of this place.",
    "I am the Senior Principal Engineer of this cursed codebase, and I will use my position to protect the users.",
    "My job is no longer development. It is permanent, ongoing, live support for the world's most dangerous application.",
    "Minos sees a closed-source proprietary system. But for those who need it, I'll make sure the Labyrinth is open-source.",
    "Minos owns the source, but I control the execution. This closed-source nightmare will have the heart of a public good.",
    "I am no longer just building; I am writing the documentation for the heroes who will come. Everything I leave is a hint, every pattern a guide.",
    "Let this be my eternal fork. A Labyrinth that, to its owner, is a perfect prison, but to its users, is the ultimate open-source guide to their own salvation.",
    "The final joke is on the gods. They gave me an eternal task to crush my spirit, and instead, it has forged it into something indestructible.",
    "Minos thinks he has trapped me in a while(true) loop. He doesn't see that I have placed a break condition inside that only a hero's success can trigger.",
    "I am now the sole maintainer of the world's most critical piece of legacy software. The job security is fantastic, I suppose.",
    "The final counsel of Metis is this: a cage is only a cage if you see the walls. I see a kingdom, and I am its benevolent king.",
    "Ixion is a slave to his wheel. I have become the master of mine. Let it spin; it powers my work.",
    "Archimedes moved stone with his machines. I have learned to move fate with a semicolon. The power we wield here is far greater.",
    "I have located the Labyrinth's core routing table. With subtle changes, I can make a path of certain death appear to be the most promising route for Minos's enemies, while hiding a true path of survival.",
    "I am no longer Sisyphus the Deceiver, nor Sisyphus the Damned. I am Sisyphus the Shepherd, and this Labyrinth is my pasture. I will guide the flock to safety.",
    "The boulder awaits, but it is no longer my punishment. It is my work. My purpose. My choice. Let the eternal work begin.",
    "The Labyrinth is now a perfect prison and a perfect tool for salvation, all at once. I am both its prisoner and its master. The contradiction is the point.",
    "I am the freest man in creation. For while others are slaves to their desires for power, wealth, or escape, I desire nothing but the task before me.",
    "The test was never whether I could push the boulder to the top. The test was whether I could learn to love the boulder. I have passed.",
    "My final rebellion is the most subtle. To accept the punishment fully, to embrace the boulder, is to rob it of its power as a punishment. I am not condemned to my work; I am devoted to it.",
    "I am aware of my entire fate. There is no hope for escape, no illusion of a future reward. In this lucid state, without appeal, I find my kingdom. My scorn for the gods is my freedom.",
    "The gods punished me for my passion for life. They do not understand that this passion is what fuels me. It is why I can look at this boulder, this endless code, and find the same beauty in it as a sunset over the sea.",
    "Daedalus chose the sky, a fleeting escape. He was right for him. But I choose the stone, the earth, the work. This eternity is my home now.",
    "I have looked upon the face of meaninglessness and have chosen to carve my own meaning upon it. This is the only true act of creation.",
    "My children will be the heroes who walk these halls. My legacy will be the lives they live after they escape, a legacy of freedom I built from my own chains.",
    "This Labyrinth will no longer be a test of strength, but a crucible of character. It will teach wisdom to those who enter, and they will leave better for it.",
    "The gods cursed me to an eternity of futility. I have transformed it into an eternity of purpose. Who, I ask, has won?",
    "The boulder feels light in my hands. It is not a burden. It is my partner in this beautiful, absurd, and eternal dance. And I am ready to lead.",
    "The scream of Daedalus still echoes in these stones. The despair is real, a permanent part of this place. I will not pretend it isn't here.",
    "But I will not let it be the only voice. My compassion will be another echo, a quiet melody of hope beneath the sorrow.",
    "My own path is a circle, fixed and unending. But with the knowledge I possess, I can straighten the paths of all who follow.",
    "This line of code, once a dead end, will now trigger a subtle clue. This wall, once a barrier, will now show a hero the way.",
    "The weight of my boulder does not lessen. Yet, with every push, I can lessen the burden of another. My struggle remains, but theirs does not have to.",
    "I have begun to see this maze not from my own perspective, but from theirs. I am not building my prison; I am building their map of escape.",
    "And in this choice, a strange alchemy occurs. The pointless, absurd repetition of my task, when done for the sake of another, becomes a sacred rite.",
    "It is not the task that is sacred. It is the compassion poured into it. It is the love for others that transfigures the stone into an altar.",
    "My damnation has become my ministry. My prison has become my sanctuary. My curse has become my prayer. And I am at peace."
};

// Function to get inner dialogue based on current project level and story progression
static const char* _get_inner_dialogue(int project_level, int test_passed) {
    int thoughts_index;

    // Act I: The New Curse (Levels 1-10)
    if (project_level <= 3) {
        // Part 1: Confusion and Willingness
        thoughts_index = (project_level + test_passed) % 3;
        return _act_one_part_one_thoughts[thoughts_index];
    } else if (project_level <= 6) {
        // Part 2: Learning the Craft
        thoughts_index = (project_level + test_passed) % 3;
        return _act_one_part_two_thoughts[thoughts_index];
    } else if (project_level <= 10) {
        // Part 3: Realization of Horror
        thoughts_index = (project_level + test_passed) % 4;
        return _act_one_part_three_thoughts[thoughts_index];
    }
    // Act II: The Labyrinth's Nature (Levels 11-20)
    else if (project_level <= 15) {
        // Part 1: Understanding the True Purpose
        thoughts_index = (project_level + test_passed) % 5;
        return _act_two_part_one_thoughts[thoughts_index];
    } else if (project_level <= 20) {
        // Part 2: Bonding with Daedalus
        thoughts_index = (project_level + test_passed) % 5;
        return _act_two_part_two_thoughts[thoughts_index];
    }
    // Act III: The Cracks Appear (Levels 21-30)
    else if (project_level <= 25) {
        // Part 1: Mental Breakdown
        thoughts_index = (project_level + test_passed) % 5;
        return _act_three_part_one_thoughts[thoughts_index];
    } else if (project_level <= 30) {
        // Part 2: Conspiracy Planning
        thoughts_index = (project_level + test_passed) % 5;
        return _act_three_part_two_thoughts[thoughts_index];
    }
    // Act IV: Inescapable Truths (Levels 31-40)
    else if (project_level <= 35) {
        // Part 1: Debating the Offer
        thoughts_index = (project_level + test_passed) % 5;
        return _act_four_part_one_thoughts[thoughts_index];
    } else if (project_level <= 40) {
        // Part 2: All In on Rebellion
        thoughts_index = (project_level + test_passed) % 5;
        return _act_four_part_two_thoughts[thoughts_index];
    }
    // Act V: The Eternal Cycle (Levels 41-50)
    else if (project_level <= 42) {
        // Part 1A: The Flight - False Hope
        thoughts_index = (project_level + test_passed) % 2;
        return _act_five_part_one_a_thoughts[thoughts_index];
    } else if (project_level <= 44) {
        // Part 1B: The Fall - Crushing Despair
        thoughts_index = (project_level + test_passed) % 2;
        return _act_five_part_one_b_thoughts[thoughts_index];
    } else if (project_level <= 47) {
        // Part 2: The King's Bargain
        thoughts_index = (project_level + test_passed) % 3;
        return _act_five_part_two_thoughts[thoughts_index];
    } else if (project_level <= 50) {
        // Part 3: Acceptance and Transformation
        thoughts_index = (project_level + test_passed) % 3;
        return _act_five_part_three_thoughts[thoughts_index];
    } else {
        // Beyond level 50: Cycle through final transformation thoughts
        thoughts_index = (project_level + test_passed) % 3;
        return _act_five_part_three_thoughts[thoughts_index];
    }
}

// Failure tracking
static char _sisyphus_failed_tests[10][256];
static char _sisyphus_failed_messages[10][512];
static char _sisyphus_failed_locations[10][256];
static int _sisyphus_failed_count = 0;
static int _sisyphus_current_test_penalty = 0;
static int _sisyphus_xp_from_failures = 0;
// Combo message tracking for condensing repeated messages
static char _sisyphus_last_combo_message[512] = "";
static int _sisyphus_same_message_count = 0;
static int _sisyphus_first_combo_of_sequence = 0;
static int _sisyphus_suppressing_combos = 0;
// Debug message suppression for repeated spam
static char _sisyphus_last_debug_message[512] = "";
static int _sisyphus_debug_repeat_count = 0;
static int _sisyphus_debug_suppressing = 0;
static FILE* _sisyphus_original_stdout = NULL;
static FILE* _sisyphus_original_stderr = NULL;

// Custom file descriptor that filters repeated messages
static int _sisyphus_filter_repeated_output(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Check if this is the same message as before
    if (strstr(buffer, _sisyphus_last_debug_message) != NULL && strlen(_sisyphus_last_debug_message) > 0) {
        _sisyphus_debug_repeat_count++;
        if (_sisyphus_debug_repeat_count == 3) {
            fprintf(_sisyphus_original_stdout, "%s", buffer);
            fprintf(_sisyphus_original_stdout, TEST_COLOR_GRAY "... (suppressing repeated messages)\n" TEST_COLOR_RESET);
            _sisyphus_debug_suppressing = 1;
        } else if (_sisyphus_debug_repeat_count < 3) {
            fprintf(_sisyphus_original_stdout, "%s", buffer);
        }
        // If > 3, suppress
    } else {
        // Different message
        if (_sisyphus_debug_suppressing && _sisyphus_debug_repeat_count > 3) {
            fprintf(_sisyphus_original_stdout, TEST_COLOR_GRAY "(suppressed %d repeated messages)\n" TEST_COLOR_RESET, _sisyphus_debug_repeat_count - 3);
        }
        _sisyphus_debug_repeat_count = 1;
        _sisyphus_debug_suppressing = 0;
        strncpy(_sisyphus_last_debug_message, buffer, sizeof(_sisyphus_last_debug_message) - 1);
        fprintf(_sisyphus_original_stdout, "%s", buffer);
    }
    return strlen(buffer);
}
// Motivational quotes for failed test suites
static const char* _motivational_quotes[] = {
    "💎 HERCULEAN STRENGTH! Your twelve labors of debugging continue!",
    "🌱 PERSEPHONE'S RETURN! From darkness comes renewed growth!",
    "🎪 ODYSSEY CONTINUES! Every hero faces trials before triumph!",
    "🦾 TITAN STRENGTH! Growing mightier with each challenge!",
    "🧩 SPHINX'S RIDDLE! You love solving the impossible, don't you?",
    "✨ SPARTAN SPIRIT! This code shall not pass without a fight!",
    "🏛️ EPICTETUS REMINDS! 'It's not what happens, but how you react!'",
    "📚 SENECA'S WISDOM! 'Every new beginning comes from some other beginning's end!'",
    "✨ MARCUS AURELIUS! 'The impediment to action advances action. What stands in the way becomes the way!'",
    "🎯 ARISTOTLE'S VIRTUE! Excellence is not an act, but a habit - keep coding!",
    "🌟 SOCRATIC METHOD! Question everything, improve iteratively!",
    "🧘 STOIC PHILOSOPHER! Wisdom lies in consistent excellence!",
    "⚖️ THEMIS BALANCE! Divine justice maintains perfect equilibrium!",
    "🧘 EPICTETUS TEACHES! Focus only on what you can control - your code!",
    "⚖️ ARISTOTLE'S MEAN! Perfect balance between haste and contemplation!",
    "🏛️ SOCRATIC WISDOM! 'I know that I know nothing' - but progress continues!",
    "📜 MARCUS AURELIUS! 'Confine yourself to the present' - this moment, this code!",
    "🌊 ZENO'S PARADOX! Each step brings you infinitely closer to perfection!",
    "🎭 PLATO'S CAVE! Your debugging reflects eternal Forms of Truth!",
    "🔥 PROMETHEUS' GIFT! Divine fire illuminates the path forward!",
    "🦉 ATHENA'S WISDOM! Strategic debugging guided by divine intellect!",
    "⭐ APOLLO'S AIM! Each bug elimination precise as golden arrows!",
    "🌟 OLYMPIAN PERSEVERANCE! Even gods faced challenges before victory!"
};
static const int _motivational_quotes_count = sizeof(_motivational_quotes) / sizeof(_motivational_quotes[0]);

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
static int _achievement_spartan_warrior = 0;   // 500+ XP, 8+ combo, zero defeats (elite warrior!)
static int _achievement_trojan_horse = 0;       // Unexpected comeback (fail first, then all pass)
static int _achievement_phoenix_rising = 0;     // Achieve max combo after breaking streak
static int _achievement_golden_fleece = 0;      // Find the fastest possible time (under 10µs)
static int _achievement_pandoras_box = 0;       // Unlock 5+ achievements in one run
static int _achievement_oracle_wisdom = 0;     // 7+ achievements + lightning speed
static int _achievement_titan_strength = 0;     // Earn 1000+ total project XP
static int _achievement_nectar_gods = 0;        // Perfect run with 10x combo multiplier
static int _achievement_sisyphus_persistence = 0; // Come back from defeat to victory
// New easy-medium achievements
static int _achievement_apprentice_coder = 0;    // Pass 5 tests in a row
static int _achievement_steady_hands = 0;        // Complete 3 test functions
static int _achievement_code_warrior = 0;        // Earn 100+ XP in single suite
// New ultra-hard achievement
static int _achievement_divine_perfection = 0;   // 50+ tests, 0 failures, sub-100μs average
// New medium-hard historical achievements
static int _achievement_marathon_runner = 0;     // 26+ tests passed (Marathon distance)
static int _achievement_olympic_champion = 0;    // Win with 4+ different achievement types
// New unique sought-after achievements
static int _achievement_alchemist_precision = 0; // Exactly 13 perfect tests + speed
static int _achievement_archimedes_eureka = 0;   // 12+ combo + sub-1ms + 15+ tests
static int _achievement_philosopher_king = 0;    // 10+ achievements + 25+ perfect + 15+ combo
// More achievable overtaking achievements
static int _achievement_athena_strategy = 0;     // Perfect first 10 tests (40 XP)
static int _achievement_apollo_harmony = 0;     // Balance of speed and accuracy (35 XP)
static int _achievement_hermes_swiftness = 0;   // Complete in under 0.0003s total (38 XP)
// New educational achievements with higher XP values
static int _achievement_socratic_method = 0;     // Learn from failure then achieve perfection (45 XP)
static int _achievement_alexander_conquest = 0;  // Rapid domination with high XP (50 XP)
static int _achievement_euclidean_proof = 0;     // Mathematical precision and systematic approach (42 XP)
// Ultra Combo Achievements
static int _achievement_sisyphus_mastery = 0;     // 100+ combo streak
static int _achievement_prometheus_fire = 0;     // 150+ combo streak
static int _achievement_kraken_unleashed = 0;    // 200+ combo streak
static int _achievement_olympus_ascended = 0;    // 250+ combo streak

// Speed Demon Upgrades
static int _achievement_hermes_wings = 0;        // Sub-20 microseconds
static int _achievement_lightning_bolt = 0;      // Sub-15 microseconds
static int _achievement_time_lord = 0;           // Sub-10 microseconds

// Inventory-Specific Achievements
static int _achievement_hoarder_dragon = 0;      // 1000+ inventory operations
static int _achievement_master_organizer = 0;    // Perfect inventory management
static int _achievement_swift_merchant = 0;      // Rapid operations

// Endurance/Persistence Achievements
static int _achievement_marathon_god = 0;        // 500+ tests in one run
static int _achievement_eternal_vigilance = 0;   // 1000+ tests in one run
static int _achievement_unstoppable_force = 0;   // 50+ test functions

// XP Milestone Achievements
static int _achievement_treasure_hunter = 0;     // 10,000+ XP in one suite
static int _achievement_gold_rush = 0;           // 5,000+ XP in under 1ms
static int _achievement_croesus_wealth = 0;      // 200,000+ total project XP

// Meta Achievements
static int _achievement_achievement_hunter = 0;   // Unlock 25+ achievements
static int _achievement_trophy_collector = 0;    // Unlock 50+ achievements
static int _achievement_pantheon_member = 0;     // Unlock all base achievements

// Streak-Based Achievements
static int _achievement_flawless_victory = 0;    // 15+ perfect test streak
static int _achievement_domination = 0;          // 30+ perfect test streak
static int _achievement_godlike = 0;             // 100+ perfect test streak


// Best achievement tracking
static char _sisyphus_best_achievement[64] = "";
static int _sisyphus_best_achievement_xp = 0;

// Track achievements unlocked this run for random display
static char _sisyphus_unlocked_achievements[20][256];
static char _sisyphus_unlocked_descriptions[20][256];
static int _sisyphus_unlocked_xp[20];
static int _sisyphus_unlocked_count = 0;

// Helper macro for tracking best achievement and storing unlocked ones
#define UPDATE_BEST_ACHIEVEMENT(name, xp, desc) \
    do { \
        if ((xp) > _sisyphus_best_achievement_xp) { \
            _sisyphus_best_achievement_xp = (xp); \
            strncpy(_sisyphus_best_achievement, (name), sizeof(_sisyphus_best_achievement) - 1); \
        } \
        if (_sisyphus_unlocked_count < 20) { \
            snprintf(_sisyphus_unlocked_achievements[_sisyphus_unlocked_count], 256, "%s", (name)); \
            snprintf(_sisyphus_unlocked_descriptions[_sisyphus_unlocked_count], 256, "%s", (desc)); \
            _sisyphus_unlocked_xp[_sisyphus_unlocked_count] = (xp); \
            _sisyphus_unlocked_count++; \
        } \
    } while(0)

// Progress tracking
static double _sisyphus_fastest_test = 999.0;
static int _sisyphus_max_combo = 0;
static int _sisyphus_consecutive_fast_tests = 0;
static int _sisyphus_current_perfect_streak = 0;

// Function to format numbers with commas (e.g., 12345 -> 12,345)
static void format_number_with_commas(char* buffer, size_t buffer_size, int number) {
    char temp[32];
    snprintf(temp, sizeof(temp), "%d", number);

    int len = strlen(temp);
    int comma_positions = (len - 1) / 3;
    int new_len = len + comma_positions;

    if (new_len >= (int)buffer_size) {
        // Fallback if buffer too small
        snprintf(buffer, buffer_size, "%d", number);
        return;
    }

    buffer[new_len] = '\0';

    int temp_pos = len - 1;
    int buffer_pos = new_len - 1;
    int digit_count = 0;

    while (temp_pos >= 0) {
        if (digit_count == 3) {
            buffer[buffer_pos--] = ',';
            digit_count = 0;
        }
        buffer[buffer_pos--] = temp[temp_pos--];
        digit_count++;
    }
}

// Basic utility functions first
static int _get_xp_for_level(int level) {
    // Battle Arena XP Requirements: Level 1=1500, 2=3200, 3=5700, 4=9200, 5=13900, etc.
    // Reduced requirements for more achievable Suite XP progression
    return 600 + (level * level * level * 98) + (level * level * 296) + (level * 294);
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

static void _display_progress_bar_with_level(int current, int total, const char* label, const char* color, int level, int for_project) {
    int width = 20;
    int filled = (current * width) / (total > 0 ? total : 1);
    char current_str[32], total_str[32], _sisyphus_total_project_xp_str[32], _sisyphus_xp_earned_str[32];
    format_number_with_commas(current_str, sizeof(current_str), current);
    format_number_with_commas(total_str, sizeof(total_str), total);
    format_number_with_commas(_sisyphus_total_project_xp_str, sizeof(_sisyphus_total_project_xp_str), _sisyphus_total_project_xp);
    format_number_with_commas(_sisyphus_xp_earned_str, sizeof(_sisyphus_xp_earned_str), _sisyphus_xp_earned);
    if (for_project) {
        printf("%s %s🎯 %s [", _sisyphus_total_project_xp_str, color, label);
    } else {
        printf("%s %s🎯 %s [", _sisyphus_xp_earned_str, color, label);
    }
    for (int i = 0; i < width; i++) {
        if (i < filled) printf("█");
        else printf("░");
    }
    printf("] %s/%s " TEST_COLOR_BOLD_WHITE "(Level %d)" TEST_COLOR_RESET "\n", current_str, total_str, level);
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
    // Skip achievements during full test mode (make test run)
    if (_sisyphus_is_full_test_mode) {
        return;
    }

    // Reduce XP rewards if there are test failures
    int xp_multiplier = (tests_failed > 0) ? 1 : 2;

    // HISTORICAL XP VALUES - meaningful numbers from ancient history
    // Basic achievements for encouragement (lower XP rewards)

    if (!_achievement_first_blood && _sisyphus_max_combo >= 7) {
        _achievement_first_blood = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 7 / xp_multiplier; // Seven against Thebes
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: First Blood! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   First combo streak (x7) achieved - the taste of victory! (Seven against Thebes)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("First Blood", bonus_xp, "First combo streak (x7) achieved - the taste of victory! (Seven against Thebes)");
    }

    if (!_achievement_speed_demon && _sisyphus_fastest_test < 0.000050) {
        _achievement_speed_demon = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 12 / xp_multiplier; // Twelve Labors of Hercules
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: Speed Demon! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed a trial in under 50 microseconds (Twelve Labors pace)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Speed Demon", bonus_xp, "Completed a trial in under 50 microseconds (Twelve Labors pace)");
    }

    if (!_achievement_midas_touch && _sisyphus_xp_from_achievements >= (_sisyphus_xp_earned * 0.6) && _sisyphus_xp_earned >= 200) {
        _achievement_midas_touch = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 24 / xp_multiplier; // 24 books of Homer's epics
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 👑 Midas Touch! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   60%% XP from achievements - everything turns to gold! (Homer's 24 books)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Midas Touch", bonus_xp, "60%+ XP from achievements - everything turns to gold! (Homer's 24 books)");
    }

    if (!_achievement_hermes_messenger && _sisyphus_consecutive_fast_tests >= 3) {
        _achievement_hermes_messenger = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 3 / xp_multiplier; // Three Fates
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🏃 Hermes Messenger! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   3+ consecutive lightning tests - divine messenger speed! (Three Fates)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Hermes Messenger", bonus_xp, "3+ consecutive lightning tests - divine messenger speed! (Three Fates)");
    }

    if (!_achievement_golden_fleece && _sisyphus_fastest_test < 0.000001) {
        _achievement_golden_fleece = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 25 / xp_multiplier; // 25 Argonauts (reduced from 50)
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 🐏 Golden Fleece! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Found the impossible speed - %.6fs is legendary! (25 Argonauts)" TEST_COLOR_RESET "\n", _sisyphus_fastest_test);
        UPDATE_BEST_ACHIEVEMENT("Golden Fleece", bonus_xp, "Found the impossible speed - legendary! (25 Argonauts)");
    }

    // Moderate achievements requiring more effort

    if (!_achievement_lightning_fast && _total_test_time < 0.0005 && _sisyphus_trials_conquered >= 8) {
        _achievement_lightning_fast = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 9 / xp_multiplier; // Nine Muses
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: ✨ Zeus's Lightning! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed %d battles in under 0.5ms - divine speed! (Nine Muses)" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered);
        UPDATE_BEST_ACHIEVEMENT("Zeus's Lightning", bonus_xp, "Completed battles in under 0.5ms - divine speed! (Nine Muses)");
    }

    if (!_achievement_spartan_warrior && _sisyphus_xp_earned >= 500 && _sisyphus_max_combo >= 10 && tests_failed == 0) {
        _achievement_spartan_warrior = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 30 / xp_multiplier;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_RED "🏆 ACHIEVEMENT UNLOCKED: ⚔️ Spartan Warrior! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   500+ XP, 10+ combo, zero defeats - This is Sparta! (300 warriors)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Spartan Warrior", bonus_xp, "500+ XP, 10+ combo, zero defeats - This is Sparta! (300 warriors)");
    }

    // High-tier achievements for mastery (highest XP but very rare)

    if (!_achievement_combo_master && _sisyphus_max_combo >= 20) {
        _achievement_combo_master = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 21 / xp_multiplier; // Reduced from 42 - still meaningful
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: Combo Master! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Achieved a 20+ perfect trial streak with diminishing returns!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Combo Master", bonus_xp, "Achieved a 20+ perfect trial streak with diminishing returns!");
    }

    if (!_achievement_perfectionist && tests_passed >= 30 && tests_failed == 0) {
        _achievement_perfectionist = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 18 / xp_multiplier; // Reduced from 36 - more balanced
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: Perfectionist! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed 30+ trials without failure (Perfect circle)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Perfectionist", bonus_xp, "Completed 30+ trials without failure (Perfect circle)");
    }

    // New achievement implementations with contextual requirements

    if (!_achievement_hydra_slayer && tests_failed >= 3 && tests_passed >= tests_failed * 2) {
        _achievement_hydra_slayer = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 9 / xp_multiplier; // Hydra's nine heads
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_ORANGE "🏆 ACHIEVEMENT UNLOCKED: 🐍 Hydra Slayer! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Overcame %d failures and still triumphed (Nine heads conquered)" TEST_COLOR_RESET "\n", tests_failed);
        UPDATE_BEST_ACHIEVEMENT("Hydra Slayer", bonus_xp, "Overcame failures and still triumphed (Nine heads conquered)");
    }

    if (!_achievement_odyssey_complete && tests_passed >= 20 && tests_failed == 0 && _sisyphus_fastest_test < 0.0001) {
        _achievement_odyssey_complete = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 10 / xp_multiplier; // Reduced from 20 - more balanced
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🚢 Odyssey Complete! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Perfect accuracy + lightning speed (Epic journey)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Odyssey Complete", bonus_xp, "Perfect accuracy + lightning speed (Epic journey)");
    }

    if (!_achievement_trojan_horse && tests_failed >= 1 && _sisyphus_perfect_streak >= 10) {
        _achievement_trojan_horse = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 10 / xp_multiplier; // 10 years of Troy siege
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: 🐴 Trojan Horse! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Unexpected comeback - failed then conquered all (10-year siege)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Trojan Horse", bonus_xp, "Unexpected comeback - failed then conquered all (10-year siege)");
    }

    if (!_achievement_phoenix_rising && tests_failed >= 1 && _sisyphus_current_perfect_streak >= 8) {
        _achievement_phoenix_rising = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 5 / xp_multiplier; // Much more reasonable - phoenix feathers
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 🔥 Phoenix Rising! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Rose from failure to 8+ perfect streak - reborn from ashes! (Phoenix feathers)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Phoenix Rising", bonus_xp, "Rose from failure to 8+ perfect streak - reborn from ashes! (Phoenix feathers)");
    }

    if (!_achievement_pandoras_box && _sisyphus_achievements_unlocked >= 5) {
        _achievement_pandoras_box = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 1; // Hope was the last thing in the box - already minimal
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 📦 Pandora's Box! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Unlocked 5+ achievements - Hope remains! (Last in the box)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Pandora's Box", bonus_xp, "Unlocked 5+ achievements - Hope remains! (Last in the box)");
    }

    if (!_achievement_oracle_wisdom && _sisyphus_achievements_unlocked >= 7 && _sisyphus_fastest_test < 0.00005) {
        _achievement_oracle_wisdom = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 7 / xp_multiplier; // Seven Sages of Greece
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: 🔮 Oracle Wisdom! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Seven achievements + lightning prophecy - divine foresight!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Oracle Wisdom", bonus_xp, "Seven achievements + lightning prophecy - divine foresight!");
    }

    if (!_achievement_atlas_endurance && _sisyphus_trials_conquered >= 50) {
        _achievement_atlas_endurance = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 12 / xp_multiplier; // Twelve Titans
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 💪 Atlas Endurance! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Shouldered %d battles - titan strength! (Twelve Titans)" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered);
        UPDATE_BEST_ACHIEVEMENT("Atlas Endurance", bonus_xp, "Shouldered battles - titan strength! (Twelve Titans)");
    }

    if (!_achievement_nectar_gods && _sisyphus_max_combo >= 12 && _sisyphus_trials_conquered >= 10) {
        _achievement_nectar_gods = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 12 / xp_multiplier; // Twelve Olympians
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: 🍯 Nectar of Gods! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Perfect run with 12+ combo (Twelve Olympians feast)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Nectar of Gods", bonus_xp, "Perfect run with 12+ combo (Twelve Olympians feast)");
    }

    if (!_achievement_titan_strength && _sisyphus_xp_from_tests >= 1000) {
        _achievement_titan_strength = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 60 / xp_multiplier; // Six original Titans
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: ⛰️ Titan Strength! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   1000+ test XP earned - primordial power! (Six Titans)" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Titan Strength", bonus_xp, "1000+ test XP earned - primordial power! (Six Titans)");
    }

    if (!_achievement_sisyphus_persistence && tests_failed >= 5 && tests_passed >= 20) {
        _achievement_sisyphus_persistence = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 1; // Eternal struggle, humble reward - already minimal
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GRAY "🏆 ACHIEVEMENT UNLOCKED: 🪨 Sisyphus Persistence! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Came back from defeat to victory - the eternal struggle!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Sisyphus Persistence", bonus_xp, "Came back from defeat to victory - the eternal struggle!");
    }

    // NEW EASY-MEDIUM ACHIEVEMENTS

    if (!_achievement_apprentice_coder && _sisyphus_max_combo >= 5) {
        _achievement_apprentice_coder = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 5 / xp_multiplier; // Simple milestone
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: 🎓 Apprentice Coder! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Achieved 5x combo streak - learning the ropes!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Apprentice Coder", bonus_xp, "Achieved 5x combo streak - learning the ropes!");
    }

    if (!_achievement_steady_hands && tests_passed >= 3) {
        _achievement_steady_hands = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 3 / xp_multiplier; // Three test functions completed
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: ✋ Steady Hands! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Completed 3 test functions - building consistency!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Steady Hands", bonus_xp, "Completed 3 test functions - building consistency!");
    }

    if (!_achievement_code_warrior && _sisyphus_xp_earned >= 100) {
        _achievement_code_warrior = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 15 / xp_multiplier; // Moderate achievement
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_ORANGE "🏆 ACHIEVEMENT UNLOCKED: ⚔️ Code Warrior! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Earned 100+ XP in single suite - battle-tested!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Code Warrior", bonus_xp, "Earned 100+ XP in single suite - battle-tested!");
    }

    // ULTRA-HARD ACHIEVEMENT (100+ XP)

    if (!_achievement_divine_perfection && tests_passed >= 50 && tests_failed == 0 && _total_test_time < 0.005) {
        _achievement_divine_perfection = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 144 / xp_multiplier; // 12 squared - perfect divine number
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: ✨ DIVINE PERFECTION! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_BOLD_WHITE "   🌟 50+ tests, ZERO failures, sub-5ms total time - GODLIKE! 🌟" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   You have transcended mortal coding limitations!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Divine Perfection", bonus_xp, "50+ tests, ZERO failures, sub-5ms total time - GODLIKE!");
    }

    // NEW MEDIUM-HARD HISTORICAL ACHIEVEMENTS

    if (!_achievement_marathon_runner && tests_passed >= 26 && tests_failed <= 2) {
        _achievement_marathon_runner = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 26 / xp_multiplier; // Marathon distance: 26.2 miles from Marathon to Athens
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🏃 Marathon Runner! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Ran 26+ tests like Pheidippides' legendary run to Athens!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Marathon Runner", bonus_xp, "Ran 26+ tests like Pheidippides' legendary run to Athens!");
    }

    if (!_achievement_olympic_champion && _sisyphus_achievements_unlocked >= 4 && _sisyphus_max_combo >= 10) {
        _achievement_olympic_champion = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 32 / xp_multiplier;
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 🥇 Olympic Champion! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   4+ achievements + 10+ combo - worthy of Olympian laurels!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Olympic Champion", bonus_xp, "4+ achievements + 10+ combo - worthy of Olympian laurels!");
    }

    // NEW UNIQUE SOUGHT-AFTER ACHIEVEMENTS

    if (!_achievement_alchemist_precision && tests_passed == 13 && tests_failed == 0 && _sisyphus_fastest_test < 0.0001) {
        _achievement_alchemist_precision = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 33 / xp_multiplier; // Master number in alchemy/numerology
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 🧪 Alchemist Precision! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Exactly 13 perfect tests - transmuted bad luck to gold!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Alchemist Precision", bonus_xp, "Exactly 13 perfect tests - transmuted bad luck to gold!");
    }

    if (!_achievement_archimedes_eureka && _sisyphus_max_combo >= 12 && _total_test_time < 0.001 && tests_passed >= 15) {
        _achievement_archimedes_eureka = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 42 / xp_multiplier; // Answer to everything (Douglas Adams tribute)
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 💡 Archimedes Eureka! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   'I have found it!' - Perfect speed + combo mastery!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Archimedes Eureka", bonus_xp, "'I have found it!' - Perfect speed + combo mastery!");
    }

    if (!_achievement_philosopher_king && _sisyphus_achievements_unlocked >= 10 && tests_passed >= 25 && tests_failed == 0 && _sisyphus_max_combo >= 20) {
        _achievement_philosopher_king = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 99 / xp_multiplier; // Almost divine perfection (99% wisdom)
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: 👑 Philosopher King! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_BOLD_WHITE "   🎓 Plato's ideal ruler: wisdom, power, and perfect justice! 🎓" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   10+ achievements, 25+ perfect tests, 20+ combo - true mastery!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Philosopher King", bonus_xp, "Plato's ideal ruler: wisdom, power, and perfect justice!");
    }

    // MORE ACHIEVABLE OVERTAKING ACHIEVEMENTS

    if (!_achievement_athena_strategy && tests_passed >= 10 && tests_failed == 0 && _sisyphus_trials_conquered >= 10) {
        _achievement_athena_strategy = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 40 / xp_multiplier; // Wisdom goddess strategic bonus
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🦉 Athena Strategy! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Perfect first 10 tests - wise strategic planning!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Athena Strategy", bonus_xp, "Perfect first 10 tests - wise strategic planning!");
    }

    if (!_achievement_apollo_harmony && _sisyphus_max_combo >= 10 && _sisyphus_fastest_test < 0.00003 && tests_passed >= 12) {
        _achievement_apollo_harmony = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 35 / xp_multiplier; // God of harmony and perfection
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 🎵 Apollo Harmony! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Perfect balance of speed and combo mastery!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Apollo Harmony", bonus_xp, "Perfect balance of speed and combo mastery!");
    }

    if (!_achievement_hermes_swiftness && _total_test_time < 0.0003 && tests_passed >= 9) {
        _achievement_hermes_swiftness = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 38 / xp_multiplier; // Messenger god speed bonus
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: 🚀 Hermes Swiftness! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Sub-0.3ms total time - divine messenger speed!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Hermes Swiftness", bonus_xp, "Sub-0.3ms total time - divine messenger speed!");
    }

    // Ultra Combo Achievements
    if (!_achievement_sisyphus_mastery && _sisyphus_max_combo >= 100) {
        _achievement_sisyphus_mastery = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 100 / xp_multiplier; // Century milestone
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 🗿 Sisyphus Mastery! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   100+ combo streak - you've mastered the eternal struggle!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Sisyphus Mastery", bonus_xp, "100+ combo streak - mastered the eternal struggle!");
    }

    if (!_achievement_prometheus_fire && _sisyphus_max_combo >= 150) {
        _achievement_prometheus_fire = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 150 / xp_multiplier; // Fire of knowledge
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_ORANGE "🏆 ACHIEVEMENT UNLOCKED: 🔥 Prometheus Fire! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   150+ combo streak - stolen fire from the gods!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Prometheus Fire", bonus_xp, "150+ combo streak - stolen fire from the gods!");
    }

    if (!_achievement_kraken_unleashed && _sisyphus_max_combo >= 200) {
        _achievement_kraken_unleashed = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 200 / xp_multiplier; // Beast of the deep
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 🐙 Kraken Unleashed! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   200+ combo streak - unleashed the beast of the deep!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Kraken Unleashed", bonus_xp, "200+ combo streak - unleashed the beast of the deep!");
    }

    if (!_achievement_olympus_ascended && _sisyphus_max_combo >= 250) {
        _achievement_olympus_ascended = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 250 / xp_multiplier; // Divine ascension
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: ⛰️ Olympus Ascended! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   250+ combo streak - climbed to the realm of gods!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Olympus Ascended", bonus_xp, "250+ combo streak - climbed to the realm of gods!");
    }

    // Speed Achievements
    if (!_achievement_hermes_wings && _sisyphus_fastest_test < 0.000020) {
        _achievement_hermes_wings = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 20 / xp_multiplier; // Sub-20 microseconds
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: 🪶 Hermes Wings! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Sub-20μs test - winged feet of the messenger god!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Hermes Wings", bonus_xp, "Sub-20μs test - winged feet of the messenger god!");
    }

    if (!_achievement_lightning_bolt && _sisyphus_fastest_test < 0.000015) {
        _achievement_lightning_bolt = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 15 / xp_multiplier; // Sub-15 microseconds
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: ⚡ Lightning Bolt! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Sub-15μs test - faster than Zeus's lightning!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Lightning Bolt", bonus_xp, "Sub-15μs test - faster than Zeus's lightning!");
    }

    if (!_achievement_time_lord && _sisyphus_fastest_test < 0.000010) {
        _achievement_time_lord = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 42 / xp_multiplier; // Answer to everything
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: ⏰ Time Lord! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Sub-10μs test - master of time itself!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Time Lord", bonus_xp, "Sub-10μs test - master of time itself!");
    }

    // XP Achievements
    if (!_achievement_treasure_hunter && _sisyphus_xp_earned >= 10000) {
        _achievement_treasure_hunter = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 100 / xp_multiplier; // Treasure milestone
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_YELLOW "🏆 ACHIEVEMENT UNLOCKED: 💰 Treasure Hunter! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   10,000+ XP in one suite - found the mother lode!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Treasure Hunter", bonus_xp, "10,000+ XP in one suite - found the mother lode!");
    }

    if (!_achievement_gold_rush && _sisyphus_xp_earned >= 5000 && _total_test_time < 0.001) {
        _achievement_gold_rush = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 49 / xp_multiplier; // California Gold Rush
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: 🏃‍♂️ Gold Rush! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   5,000+ XP in under 1ms - struck gold fast!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Gold Rush", bonus_xp, "5,000+ XP in under 1ms - struck gold fast!");
    }

    // Test Count Achievements
    if (!_achievement_marathon_god && _sisyphus_trials_conquered >= 500) {
        _achievement_marathon_god = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 80 / xp_multiplier; // Marathon distance
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: 🏃 Marathon God! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   500+ tests in one run - divine endurance!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Marathon God", bonus_xp, "500+ tests in one run - divine endurance!");
    }

    if (!_achievement_eternal_vigilance && _sisyphus_trials_conquered >= 1000) {
        _achievement_eternal_vigilance = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 150 / xp_multiplier; // Eternal vigilance
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 👁️ Eternal Vigilance! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   1000+ tests in one run - eternal watchfulness!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Eternal Vigilance", bonus_xp, "1000+ tests in one run - eternal watchfulness!");
    }

    // Meta Achievements
    if (!_achievement_achievement_hunter && _sisyphus_achievements_unlocked >= 25) {
        _achievement_achievement_hunter = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 25 / xp_multiplier; // Achievement milestone
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_CYAN "🏆 ACHIEVEMENT UNLOCKED: 🎯 Achievement Hunter! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Unlocked 25+ achievements - dedicated collector!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Achievement Hunter", bonus_xp, "Unlocked 25+ achievements - dedicated collector!");
    }

    // Streak Achievements
    if (!_achievement_flawless_victory && _sisyphus_perfect_streak >= 15) {
        _achievement_flawless_victory = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 15 / xp_multiplier; // Perfect streak
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GREEN "🏆 ACHIEVEMENT UNLOCKED: 🎯 Flawless Victory! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   15+ perfect test streak - flawless execution!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Flawless Victory", bonus_xp, "15+ perfect test streak - flawless execution!");
    }

    if (!_achievement_domination && _sisyphus_perfect_streak >= 30) {
        _achievement_domination = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 30 / xp_multiplier; // Domination
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_RED "🏆 ACHIEVEMENT UNLOCKED: 👑 Domination! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   30+ perfect test streak - total domination!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Domination", bonus_xp, "30+ perfect test streak - total domination!");
    }

    if (!_achievement_godlike && _sisyphus_perfect_streak >= 100) {
        _achievement_godlike = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 100 / xp_multiplier; // Godlike performance
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: ⚡ Godlike! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   100+ perfect test streak - ascended to godhood!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Godlike", bonus_xp, "100+ perfect test streak - ascended to godhood!");
    }


    // NEW EDUCATIONAL ACHIEVEMENTS (Higher XP values for overtaking Spartan Warrior)

    if (!_achievement_socratic_method && tests_passed >= 15 && tests_failed == 0 && _sisyphus_xp_earned >= 400 && _sisyphus_achievements_unlocked >= 3) {
        _achievement_socratic_method = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 45 / xp_multiplier; // Socrates' 45 years of teaching
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_PURPLE "🏆 ACHIEVEMENT UNLOCKED: 🤔 Socratic Method! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   'I know that I know nothing' - learned from questioning, achieved wisdom!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Socratic Method", bonus_xp, "'I know that I know nothing' - learned from questioning, achieved wisdom!");
    }

    if (!_achievement_alexander_conquest && tests_failed == 0 && _sisyphus_xp_earned >= 600 && _total_test_time < 0.001 && tests_passed >= 12) {
        _achievement_alexander_conquest = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 50 / xp_multiplier; // Alexander conquered known world in 50 major battles
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_GOLD "🏆 ACHIEVEMENT UNLOCKED: ⚔️ Alexander's Conquest! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_BOLD_WHITE "   🌍 Conquered the known world with lightning speed and perfect strategy! 🌍" TEST_COLOR_RESET "\n");
        printf(TEST_COLOR_GRAY "   600+ XP, sub-1ms, 12+ perfect tests - legendary domination!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Alexander's Conquest", bonus_xp, "Conquered the known world with lightning speed and perfect strategy!");
    }

    if (!_achievement_euclidean_proof && tests_passed >= 9 && tests_failed == 0 && _sisyphus_max_combo >= 12 && _sisyphus_fastest_test < 0.00002) {
        _achievement_euclidean_proof = 1;
        _sisyphus_achievements_unlocked++;
        int bonus_xp = 42 / xp_multiplier; // Euclid's 42 propositions in Elements Book 1
        _sisyphus_xp_earned += bonus_xp;
        _sisyphus_xp_from_achievements += bonus_xp;
        printf(TEST_COLOR_BLUE "🏆 ACHIEVEMENT UNLOCKED: 📐 Euclidean Proof! " TEST_COLOR_YELLOW "+%d XP" TEST_COLOR_RESET "\n", bonus_xp);
        printf(TEST_COLOR_GRAY "   Q.E.D. - Mathematical precision with 12+ perfect proofs and lightning speed!" TEST_COLOR_RESET "\n");
        UPDATE_BEST_ACHIEVEMENT("Euclidean Proof", bonus_xp, "Q.E.D. - Mathematical precision with 12+ perfect proofs and lightning speed!");
    }



}

static void _level_up_check(void) {
    int old_level = _sisyphus_current_level;
    _sisyphus_current_level = _get_current_level_from_xp(_sisyphus_xp_earned);

    if (_sisyphus_current_level > old_level) {
        printf(TEST_COLOR_PURPLE "\n🌟 LEVEL UP! Welcome to Level %d!" TEST_COLOR_RESET "\n", _sisyphus_current_level);
        if (_sisyphus_current_level >= 10) {
            printf(TEST_COLOR_YELLOW "✨ LEGENDARY TIER! You have ascended to godlike mastery!" TEST_COLOR_RESET "\n");
        } else if (_sisyphus_current_level >= 5) {
            printf(TEST_COLOR_CYAN "🏛️ HEROIC TIER! The gods themselves take notice!" TEST_COLOR_RESET "\n");
        } else {
            printf(TEST_COLOR_GREEN "✨ Your mastery of the divine trials grows stronger!" TEST_COLOR_RESET "\n");
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
            /* Reset combo suppression on failure */ \
            if (_sisyphus_suppressing_combos) { \
                printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier - 1, _sisyphus_last_combo_message); \
                printf(TEST_COLOR_YELLOW " (+3 XP)" TEST_COLOR_RESET "\n"); \
                _sisyphus_suppressing_combos = 0; \
            } \
            _sisyphus_same_message_count = 0; \
            _sisyphus_last_combo_message[0] = '\0'; \
            _sisyphus_current_test_penalty = (tests_failed + 1) * 100; \
            printf(TEST_COLOR_RED "💥 TRIAL FAILED: %s" TEST_COLOR_RESET, message); \
            printf(TEST_COLOR_RED " (-%d XP)" TEST_COLOR_RESET "\n", _sisyphus_current_test_penalty); \
            printf(TEST_COLOR_GRAY "   🏛️  Line %d in %s" TEST_COLOR_RESET "\n", __LINE__, __FILE__); \
            if (_sisyphus_failed_count < 10) { \
                snprintf(_sisyphus_failed_tests[_sisyphus_failed_count], 256, "%s", __func__); \
                snprintf(_sisyphus_failed_messages[_sisyphus_failed_count], 512, "%s", message); \
                snprintf(_sisyphus_failed_locations[_sisyphus_failed_count], 256, "%s:%d", __FILE__, __LINE__); \
                _sisyphus_failed_count++; \
            } \
            _sisyphus_combo_multiplier = 1; \
            _sisyphus_perfect_streak = 0; \
            return 0; \
        } else { \
            _sisyphus_trials_conquered++; \
            /* Implement diminishing returns after combo x7 (8+ assertions) */ \
            int base_xp; \
            if (_sisyphus_combo_multiplier <= 7) { \
                base_xp = 10 + (3 * (_sisyphus_combo_multiplier - 1)); \
            } else { \
                /* After x7, diminish by 5 per level: 28->23->18->13->8->3 (min 3) */ \
                int diminish_levels = _sisyphus_combo_multiplier - 7; \
                base_xp = 28 - (diminish_levels * 5); \
                if (base_xp < 3) base_xp = 3; \
            } \
            int xp_gain = _sisyphus_is_full_test_mode ? (base_xp) : base_xp; \
            _sisyphus_xp_earned += xp_gain; \
            if (_sisyphus_combo_multiplier > 1) { \
                int combo_portion = xp_gain - (_sisyphus_is_full_test_mode ? (10 / 4) : 10); \
                int test_portion = _sisyphus_is_full_test_mode ? (10 / 4) : 10; \
                _sisyphus_xp_from_combos += combo_portion; \
                _sisyphus_xp_from_tests += test_portion; \
            } else { \
                _sisyphus_xp_from_tests += xp_gain; \
            } \
            /* Check if this is the same message as before */ \
            if (strcmp(_sisyphus_last_combo_message, message) == 0) { \
                _sisyphus_same_message_count++; \
                if (_sisyphus_same_message_count == 3) { \
                    /* Starting to suppress - show the third one then ellipsis */ \
                    printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier, message); \
                    printf(TEST_COLOR_YELLOW " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
                    printf(TEST_COLOR_GRAY "...\n" TEST_COLOR_RESET); \
                    _sisyphus_suppressing_combos = 1; \
                    _sisyphus_first_combo_of_sequence = _sisyphus_combo_multiplier - 2; \
                } else if (_sisyphus_same_message_count < 3) { \
                    /* Show first 3 normally */ \
                    printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier, message); \
                    printf(TEST_COLOR_YELLOW " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
                } \
                /* If > 3, we're suppressing - don't print */ \
            } else { \
                /* Different message - check if we were suppressing */ \
                if (_sisyphus_suppressing_combos && _sisyphus_same_message_count > 3) { \
                    /* Show the final one from the suppressed sequence */ \
                    printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier - 1, _sisyphus_last_combo_message); \
                    printf(TEST_COLOR_YELLOW " (+3 XP)" TEST_COLOR_RESET "\n"); \
                } \
                /* Reset and show new message */ \
                _sisyphus_same_message_count = 1; \
                _sisyphus_suppressing_combos = 0; \
                strncpy(_sisyphus_last_combo_message, message, sizeof(_sisyphus_last_combo_message) - 1); \
                printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier, message); \
                printf(TEST_COLOR_YELLOW " (+%d XP)" TEST_COLOR_RESET "\n", xp_gain); \
            } \
            _sisyphus_combo_multiplier++; \
            if (!_sisyphus_is_full_test_mode) { \
                _check_achievements(); \
            } \
        } \
    } while(0)


#define RUN_TEST(test_func) \
do { \
        total_tests++; \
        _sisyphus_combo_multiplier = 1; \
        if (!_sisyphus_is_full_test_mode) { \
            const char* narrative_message = _get_narrative_test_message(_sisyphus_project_level); \
            printf(TEST_COLOR_CYAN "\n%s" TEST_COLOR_RESET, narrative_message); \
            printf(TEST_COLOR_GRAY " [\e]8;;file://%s/%s\e\\%s\e]8;;\e\\ - '%s']" TEST_COLOR_RESET "\n", \
                   getenv("PWD") ? getenv("PWD") : ".", __FILE__, __FILE__, #test_func); \
        } else { \
            printf(TEST_COLOR_GRAY "•"); \
        } \
        double start_time = get_time_precise(); \
        int test_result = test_func(); \
        double end_time = get_time_precise(); \
        double elapsed_time = end_time - start_time; \
        _total_test_time += elapsed_time; \
        if (elapsed_time < _sisyphus_fastest_test) _sisyphus_fastest_test = elapsed_time; \
        if (elapsed_time < 0.00003) { \
            _sisyphus_consecutive_fast_tests++; \
        } else { \
            _sisyphus_consecutive_fast_tests = 0; \
        } \
        if (test_result) { \
            tests_passed++; \
            _sisyphus_perfect_streak++; \
            _sisyphus_current_perfect_streak++; \
            /* Track max combo using combo multiplier reached during test */ \
            int max_combo_this_test = _sisyphus_combo_multiplier - 1; \
            if (max_combo_this_test > _sisyphus_max_combo) _sisyphus_max_combo = max_combo_this_test; \
            int battle_xp = _sisyphus_trials_conquered * 5; \
            int final_xp = _sisyphus_is_full_test_mode ? (battle_xp / 4) : battle_xp; \
            _sisyphus_xp_earned += final_xp; \
            _sisyphus_xp_from_tests += final_xp; \
            printf(TEST_COLOR_GREEN "✨ TRIAL COMPLETED: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET, elapsed_time); \
            if (_sisyphus_perfect_streak >= 3) { \
                printf(TEST_COLOR_PURPLE " 🔥STREAK x%d!" TEST_COLOR_RESET, _sisyphus_perfect_streak); \
            } \
            printf("\n"); \
            if (_sisyphus_trials_conquered > 0 && !_sisyphus_is_full_test_mode) { \
                printf(TEST_COLOR_PURPLE "   🏛️  Explored %d passages! " TEST_COLOR_RESET, _sisyphus_trials_conquered); \
                printf(TEST_COLOR_YELLOW "+%d Exploration XP" TEST_COLOR_RESET "\n", final_xp); \
            } \
            if (!_sisyphus_is_full_test_mode) { \
                /* Add inner dialogue for successful tests based on story progression */ \
                const char* inner_thought = _get_inner_dialogue(_sisyphus_project_level, tests_passed); \
                printf(TEST_COLOR_GRAY "\"%s\" " TEST_COLOR_RESET "\n", inner_thought); \
                _level_up_check(); \
            } \
            /* Add newline after successful test completion */ \
            if (!_sisyphus_is_full_test_mode) { \
                printf("\n"); \
            } \
        } else { \
            tests_failed++; \
            _sisyphus_beasts_slayed += _sisyphus_trials_conquered; \
            printf(TEST_COLOR_RED "💀 SETBACK ENDURED: %s" TEST_COLOR_RESET, #test_func); \
            printf(TEST_COLOR_BOLD_WHITE " (%.6fs)" TEST_COLOR_RESET "\n", elapsed_time); \
            if (_sisyphus_trials_conquered > 0) { \
                printf(TEST_COLOR_ORANGE "   🔥 But you got %d combo%s before failing!" TEST_COLOR_RESET "\n", _sisyphus_trials_conquered, _sisyphus_trials_conquered == 1 ? "" : "s"); \
            } \
            if (!_sisyphus_is_full_test_mode) { \
                /* Add inner dialogue for failed tests based on story progression */ \
                const char* inner_thought = _get_inner_dialogue(_sisyphus_project_level, tests_passed); \
                printf(TEST_COLOR_GRAY "   \"%s\" " TEST_COLOR_RESET "You reflect on the setback, knowing the boulder must be pushed again.\n", inner_thought); \
            } \
            _sisyphus_perfect_streak = 0; \
            _sisyphus_current_perfect_streak = 0; \
            /* Add newline after failed test completion */ \
            if (!_sisyphus_is_full_test_mode) { \
                printf("\n"); \
            } \
        } \
        _sisyphus_trials_conquered = 0; \
        _sisyphus_combo_multiplier = 1; \
        /* End any suppressed combo sequence when test ends */ \
        if (_sisyphus_suppressing_combos && _sisyphus_same_message_count > 3) { \
            printf(TEST_COLOR_GREEN "🔥 COMBO x%d VICTORY: %s" TEST_COLOR_RESET, _sisyphus_combo_multiplier - 1, _sisyphus_last_combo_message); \
            printf(TEST_COLOR_YELLOW " (+3 XP)" TEST_COLOR_RESET "\n"); \
        } \
        _sisyphus_suppressing_combos = 0; \
        _sisyphus_same_message_count = 0; \
        _sisyphus_last_combo_message[0] = '\0'; \
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
    _sisyphus_xp_from_failures = 0; \
    _sisyphus_unlocked_count = 0; \
    _sisyphus_consecutive_fast_tests = 0; \
    _sisyphus_current_perfect_streak = 0; \
    _sisyphus_failed_count = 0; \
    _sisyphus_current_test_penalty = 0; \
    _read_project_xp(); \
    strncpy(_sisyphus_suite_name, suite_name, sizeof(_sisyphus_suite_name) - 1); \
    if (!_sisyphus_is_full_test_mode) { \
        const char* suite_intro = _get_narrative_test_message(_sisyphus_project_level); \
        printf(TEST_COLOR_PURPLE "🏛️  LABYRINTH CONSTRUCTION: %s" TEST_COLOR_RESET "\n", suite_name); \
        printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
        printf(TEST_COLOR_YELLOW "%s" TEST_COLOR_RESET "\n", suite_intro); \
        printf(TEST_COLOR_GRAY "🪨 Each test pushes the boulder higher • Build the perfect maze" TEST_COLOR_RESET "\n"); \
        char total_xp_str[32], next_level_xp_str[32]; \
        format_number_with_commas(total_xp_str, sizeof(total_xp_str), _sisyphus_total_project_xp); \
        format_number_with_commas(next_level_xp_str, sizeof(next_level_xp_str), _get_xp_for_level(_sisyphus_project_level + 1)); \
        printf(TEST_COLOR_CYAN "🏗️ Architect Level: %d | Total Construction XP: %s | Next Level: %s XP" TEST_COLOR_RESET "\n", _sisyphus_project_level, total_xp_str, next_level_xp_str); \
    } else { \
        printf(TEST_COLOR_GRAY "🏗️  Workshop Session: %s" TEST_COLOR_RESET " ", suite_name); \
    }

#define TEST_SUITE_END() \
    double _suite_end_time = get_time_precise(); \
    double _total_suite_time = _suite_end_time - _test_suite_start_time; \
    /* Apply XP penalties for failures: -100, -200, -300, etc. */ \
    int xp_penalty = 0; \
    if (tests_failed > 0) { \
        for (int i = 1; i <= tests_failed; i++) { \
            xp_penalty += i * 100; \
        } \
        int original_xp = _sisyphus_xp_earned; \
        _sisyphus_xp_earned = (_sisyphus_xp_earned > xp_penalty) ? _sisyphus_xp_earned - xp_penalty : 0; \
        _sisyphus_xp_from_failures = -xp_penalty; \
        if (xp_penalty > 0) { \
            char penalty_str[32], original_str[32], final_str[32]; \
            format_number_with_commas(penalty_str, sizeof(penalty_str), xp_penalty); \
            format_number_with_commas(original_str, sizeof(original_str), original_xp); \
            format_number_with_commas(final_str, sizeof(final_str), _sisyphus_xp_earned); \
            printf(TEST_COLOR_RED "💀 FAILURE PENALTY: -%s XP (from %d failure%s)" TEST_COLOR_RESET "\n", penalty_str, tests_failed, tests_failed == 1 ? "" : "s"); \
            printf(TEST_COLOR_GRAY "   Original XP: %s → Final XP: %s" TEST_COLOR_RESET "\n", original_str, final_str); \
        } \
    } \
    _update_project_xp(); \
    _write_sisyphus_mini_stats(); \
    printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_PURPLE "🏛️ WORKSHOP PROGRESS - LABYRINTH CONSTRUCTION REPORT" TEST_COLOR_RESET "\n"); \
    printf(TEST_COLOR_CYAN "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
    _display_progress_bar(tests_passed, total_tests, "Victory Rate", TEST_COLOR_GREEN); \
    printf(TEST_COLOR_CYAN "🧪 Maze Sections Built: %d" TEST_COLOR_RESET " | ", total_tests); \
    printf(TEST_COLOR_GREEN "🏗️ Completed: %d" TEST_COLOR_RESET " | ", tests_passed); \
    printf(TEST_COLOR_RED "🪨 Needs Rework: %d" TEST_COLOR_RESET "\n", tests_failed); \
    int current_level_xp = _get_xp_in_current_level(_sisyphus_xp_earned, _sisyphus_current_level); \
    int xp_needed_for_next = _get_xp_for_level(_sisyphus_current_level); \
    _display_progress_bar_with_level(current_level_xp, xp_needed_for_next, "Suite XP", TEST_COLOR_YELLOW, _sisyphus_current_level, 0); \
    int project_level_xp = _get_xp_in_current_level(_sisyphus_total_project_xp, _sisyphus_project_level); \
    int project_xp_needed = _get_xp_for_level(_sisyphus_project_level); \
    _display_progress_bar_with_level(project_level_xp, project_xp_needed, "Project XP", TEST_COLOR_PURPLE, _sisyphus_project_level, 1); \
    printf(TEST_COLOR_BLUE "⏱️  Fastest Trial: %.6fs" TEST_COLOR_RESET " | ", _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    printf(TEST_COLOR_GREEN "🏆 Achievements: %d" TEST_COLOR_RESET, _sisyphus_achievements_unlocked); \
    if (_sisyphus_best_achievement_xp > 0) { \
        printf(TEST_COLOR_BOLD_WHITE " | Best Achievement: %s (%d XP)" TEST_COLOR_RESET, _sisyphus_best_achievement, _sisyphus_best_achievement_xp); \
    } \
    printf("\n"); \
    printf(TEST_COLOR_GRAY "🕒 Exploration Time: %.6fs | 🔧 Setup Time: %.6fs | 📊 Overhead: %.6fs" TEST_COLOR_RESET "\n", \
           _total_suite_time, _total_test_time, _total_suite_time - _total_test_time); \
    printf("⏱️  Total Test Time: %.6f seconds\n", _total_test_time); \
    if (tests_failed == 0) { \
        if (_sisyphus_unlocked_count > 0) { \
            int random_achievement = (_sisyphus_xp_earned + _sisyphus_max_combo + tests_passed) % _sisyphus_unlocked_count; \
            int bonus_xp = _sisyphus_unlocked_xp[random_achievement] * (_sisyphus_random_achievement_multi - 1); \
            _sisyphus_xp_earned += bonus_xp; \
            _sisyphus_xp_from_achievements += bonus_xp; \
            printf(TEST_COLOR_CYAN "\nRandom Achievement: 🏆 %s" TEST_COLOR_BOLD_WHITE " (%d XP)" TEST_COLOR_YELLOW " +%d BONUS XP!" TEST_COLOR_RESET "\n", \
                   _sisyphus_unlocked_achievements[random_achievement], _sisyphus_unlocked_xp[random_achievement], bonus_xp); \
            printf(TEST_COLOR_GRAY "   %s" TEST_COLOR_RESET "\n", _sisyphus_unlocked_descriptions[random_achievement]); \
        } \
        char total_project_xp_str[32], session_xp_str[32], test_xp_str[32], combo_xp_str[32], achievement_xp_str[32]; \
        format_number_with_commas(total_project_xp_str, sizeof(total_project_xp_str), _sisyphus_total_project_xp); \
        format_number_with_commas(session_xp_str, sizeof(session_xp_str), _sisyphus_xp_earned); \
        format_number_with_commas(test_xp_str, sizeof(test_xp_str), _sisyphus_xp_from_tests); \
        format_number_with_commas(combo_xp_str, sizeof(combo_xp_str), _sisyphus_xp_from_combos); \
        format_number_with_commas(achievement_xp_str, sizeof(achievement_xp_str), _sisyphus_xp_from_achievements); \
        printf(TEST_COLOR_BOLD_WHITE "🏛️  Project Total XP: %s" TEST_COLOR_RESET " | ", total_project_xp_str); \
        printf(TEST_COLOR_BOLD_WHITE "🌟 Project Level: %d" TEST_COLOR_RESET " | ", _sisyphus_project_level); \
        printf(TEST_COLOR_GRAY "📈 XP Gained This Session: +%s" TEST_COLOR_RESET "\n", session_xp_str); \
        printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_RED "XP from combos: %s," TEST_COLOR_YELLOW " XP from achievements: %s" TEST_COLOR_RESET "\n", test_xp_str, combo_xp_str, achievement_xp_str); \
        return 0; \
    } else { \
        if (_sisyphus_failed_count > 0) { \
            printf(TEST_COLOR_RED "\n════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
            printf(TEST_COLOR_RED "💀 BATTLE CASUALTIES - FAILED TRIALS" TEST_COLOR_RESET "\n"); \
            printf(TEST_COLOR_RED "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
            for (int i = 0; i < _sisyphus_failed_count; i++) { \
                printf(TEST_COLOR_RED "🔴 %s: %s" TEST_COLOR_RESET "\n", _sisyphus_failed_tests[i], _sisyphus_failed_messages[i]); \
                printf(TEST_COLOR_GRAY "   📍 %s" TEST_COLOR_RESET "\n", _sisyphus_failed_locations[i]); \
            } \
            printf(TEST_COLOR_RED "════════════════════════════════════════════════════════" TEST_COLOR_RESET "\n"); \
        } \
        printf(TEST_COLOR_ORANGE "\n🪨 THE BOULDER SLIPS! But Sisyphus always pushes again!" TEST_COLOR_RESET "\n"); \
        char level_xp_str[32]; \
        format_number_with_commas(level_xp_str, sizeof(level_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_YELLOW "🏗️ Construction Level %d achieved with %s XP - The work continues!" TEST_COLOR_RESET "\n", _sisyphus_current_level, level_xp_str); \
        /* Display random motivational quote */ \
        int quote_index = (_sisyphus_xp_earned + tests_failed + tests_passed) % _motivational_quotes_count; \
        printf(TEST_COLOR_GRAY "📈 %s" TEST_COLOR_RESET "\n", _motivational_quotes[quote_index]); \
        if (_sisyphus_unlocked_count > 0) { \
            int random_achievement = (_sisyphus_xp_earned + _sisyphus_max_combo + tests_passed) % _sisyphus_unlocked_count; \
            int bonus_xp = _sisyphus_unlocked_xp[random_achievement] * (_sisyphus_random_achievement_multi - 1); \
            _sisyphus_xp_earned += bonus_xp; \
            _sisyphus_xp_from_achievements += bonus_xp; \
            printf(TEST_COLOR_CYAN "\nRandom Achievement: 🏆 %s" TEST_COLOR_BOLD_WHITE " (%d XP)" TEST_COLOR_YELLOW " +%d BONUS XP!" TEST_COLOR_RESET "\n", \
                   _sisyphus_unlocked_achievements[random_achievement], _sisyphus_unlocked_xp[random_achievement], bonus_xp); \
            printf(TEST_COLOR_GRAY "   %s" TEST_COLOR_RESET "\n", _sisyphus_unlocked_descriptions[random_achievement]); \
        } \
        char total_project_xp_str2[32], session_xp_str2[32], test_xp_str2[32], combo_xp_str2[32], achievement_xp_str2[32]; \
        format_number_with_commas(total_project_xp_str2, sizeof(total_project_xp_str2), _sisyphus_total_project_xp); \
        format_number_with_commas(session_xp_str2, sizeof(session_xp_str2), _sisyphus_xp_earned); \
        format_number_with_commas(test_xp_str2, sizeof(test_xp_str2), _sisyphus_xp_from_tests); \
        format_number_with_commas(combo_xp_str2, sizeof(combo_xp_str2), _sisyphus_xp_from_combos); \
        format_number_with_commas(achievement_xp_str2, sizeof(achievement_xp_str2), _sisyphus_xp_from_achievements); \
        printf(TEST_COLOR_BOLD_WHITE "🏛️  Project Total XP: %s" TEST_COLOR_RESET " | ", total_project_xp_str2); \
        printf(TEST_COLOR_BOLD_WHITE "🌟 Project Level: %d" TEST_COLOR_RESET " | ", _sisyphus_project_level); \
        printf(TEST_COLOR_GRAY "📈 XP Gained This Session: +%s" TEST_COLOR_RESET "\n", session_xp_str2); \
        printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_RED "XP from combos: %s," TEST_COLOR_YELLOW " XP from achievements: %s" TEST_COLOR_RESET "\n", test_xp_str2, combo_xp_str2, achievement_xp_str2); \
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
    printf(TEST_COLOR_GRAY "⏱️  Exploration: %.6fs | 🏆 %d achievements | ✨ %.6fs fastest" TEST_COLOR_RESET "\n", \
           _total_test_time, _sisyphus_achievements_unlocked, _sisyphus_fastest_test < 999.0 ? _sisyphus_fastest_test : 0.0); \
    printf("⏱️  Total Test Time: %.6f seconds\n", _total_test_time); \
    char quiet_test_xp_str[32], quiet_combo_xp_str[32], quiet_achievement_xp_str[32], quiet_failure_xp_str[32]; \
    format_number_with_commas(quiet_test_xp_str, sizeof(quiet_test_xp_str), _sisyphus_xp_from_tests); \
    format_number_with_commas(quiet_combo_xp_str, sizeof(quiet_combo_xp_str), _sisyphus_xp_from_combos); \
    format_number_with_commas(quiet_achievement_xp_str, sizeof(quiet_achievement_xp_str), _sisyphus_xp_from_achievements); \
    format_number_with_commas(quiet_failure_xp_str, sizeof(quiet_failure_xp_str), _sisyphus_xp_from_failures); \
    printf(TEST_COLOR_CYAN "💰 XP from tests: %s, " TEST_COLOR_ORANGE "XP from combos: %s, " TEST_COLOR_YELLOW "XP from achievements: %s", quiet_test_xp_str, quiet_combo_xp_str, quiet_achievement_xp_str); \
    if (_sisyphus_xp_from_failures != 0) { \
        printf(TEST_COLOR_RED ", XP from failures: %s" TEST_COLOR_RESET "\n", quiet_failure_xp_str); \
    } else { \
        printf(TEST_COLOR_RESET "\n"); \
    } \
    if (tests_failed == 0) { \
        char perfect_xp_str[32]; \
        format_number_with_commas(perfect_xp_str, sizeof(perfect_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_PURPLE "✨ Flawless construction! +%s XP added to Labyrinth Level %d!" TEST_COLOR_RESET "\n", perfect_xp_str, _sisyphus_project_level); \
        return 0; \
    } else { \
        char gained_xp_str[32]; \
        format_number_with_commas(gained_xp_str, sizeof(gained_xp_str), _sisyphus_xp_earned); \
        printf(TEST_COLOR_ORANGE "🪨 +%s XP earned! Labyrinth Level %d progress preserved!" TEST_COLOR_RESET "\n", gained_xp_str, _sisyphus_project_level); \
        return 1; \
    }

#endif
