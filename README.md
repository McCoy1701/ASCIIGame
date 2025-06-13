# ASCIIGame

A medieval fantasy ASCII-based RPG with political intrigue, magic, and progression mechanics inspired by Dwarf Fortress, RuneScape, and Minecraft. Experience a rich world in crisis as you rise from a humble peasant to influence the fate of a kingdom torn by civil war and foreign invasion.

## Game Overview

### The Story

In the ancient kingdom of ?-?, the 500+ year reign of King ?-? the Eternal has suddenly ended under mysterious circumstances. His unexpected death has shattered centuries of stability, plunging the realm into chaos as three powerful factions vie for control:

- **The Royal Loyalists** - Nobles who seek to maintain the old order and crown a rightful heir
- **The People's Coalition** - Commoners and merchants who demand democratic representation
- **The Arcane Order** - Powerful mages who believe magical bloodlines should rule

But while the kingdom tears itself apart, a foreign armada from the unexplored continent of ?-? has begun their invasion, bringing unknown technologies and dark magic that threaten to consume everything.

You begin as a lowly peasant caught in this maelstrom. Through quests, skill development, and strategic choices, you'll forge your own path - will you unite the fractured kingdom, or watch it burn as you carve out your own destiny?

### Core Gameplay

- **ASCII-based graphics** with rich world simulation
- **Deep progression system** with skills, crafting, and character development
- **Political intrigue** - align with factions or play them against each other
- **Complex item system** with materials, enchantments, and durability
- **World exploration** across regions with unique biomes and secrets
- **Combat system** featuring weapons, armor, magic, and tactical positioning

## Technical Architecture

### Modern Web Technology Stack
- **Backend**: C with SDL2 for graphics and audio
- **Frontend**: Modern HTML5 + JavaScript (MJS modules)
- **Compilation**: Emscripten to WebAssembly for browser deployment
- **Native Support**: Compiles to native binaries for development and testing

### Project Structure

```
ASCIIGame/
├── src/                   # Core game engine
│   ├── main.c             # Application entry point
│   ├── game.c             # Main game loop and logic
│   └── items.c            # Comprehensive item system
├── include/               # Header files
│   ├── structs.h          # Game data structures
│   ├── items.h            # Item system API
│   ├── game.h             # Game engine interface
│   └── defs.h             # Constants and definitions
├── tests/                 # Comprehensive test suite
│   └── items/             # Item system tests
├── editor/                # World and content editors
├── resources/             # Game assets
│   ├── assets/            # Graphics and sprites
│   ├── audio/             # Sound effects and music
│   ├── fonts/             # Text rendering fonts
│   └── world/             # World data files
├── htmlTemplate/          # Web deployment template
└── lib/                   # External libraries
```

## Getting Started

### Prerequisites

- **GCC** - GNU Compiler Collection
- **Emscripten** - For WebAssembly compilation
- **SDL2** libraries: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer
- **Make** - Build automation
- **Archimedes** - For running the game natively

### Building the Game

#### Web Version (Recommended)

```bash
# Build for web deployment
make all

python3 -m http.server 8000
# The compiled game will be in index/index.html
```

#### Native Version (Development)

```bash
# Build native binary
make native

# Run the game
./bin/native
```

#### World Editor

```bash
# Build the world editor
make editor

# Run the editor
./bin/editor
```

### Development Commands

```bash
# Clean build artifacts
make clean

# Run comprehensive test suite
make test

# Run specific test categories
make run-test-items-creation-destruction
make run-test-items-type-checking
make run-test-items-material-system
make run-test-items-properties

# View all test options
make test-help
```

## 🎮 Game Systems

### Item System

The game features a sophisticated item system with:
- **Multiple item types**: Weapons, armor, keys, consumables, ammunition
- **Material properties**: Weight, durability, damage modifiers, enchantability
- **Dynamic stats**: Materials affect base item properties
- **Inventory management**: Stacking, equipment slots, weight limits
- **Crafting integration**: Materials can be combined and modified

### World System

- **Hierarchical world structure**: World → Regions → Local cells → Tiles
- **Procedural generation**: Biomes, elevation, temperature modeling
- **Persistent world state**: Changes saved across sessions
- **Region-based factors**: Environmental effects on gameplay

### Character Progression

- **Skill-based advancement**: No traditional leveling, grow through use
- **Multiple progression paths**: Combat, magic, crafting, social, stealth
- **Faction reputation**: Your actions affect standing with different groups
- **Quest system**: Dynamic quests based on world state and faction conflicts

## Testing & Quality Assurance

The project includes a comprehensive testing framework:
- **Unit tests** for all major systems
- **Integration tests** for complex interactions
- **Material system validation** ensuring proper stat calculations
- **Memory safety tests** preventing leaks and corruption
- **Cross-platform compatibility** testing

Run the full test suite with: `make test`

---

## Vision

ASCIIGame aims to create a rich, immersive RPG experience that combines the depth of classic roguelikes with modern web technology. By leveraging WebAssembly, we're making complex game simulations accessible to anyone with a web browser while maintaining the performance and flexibility of native code.

The political intrigue system and faction dynamics create emergent storytelling opportunities where player choices genuinely matter and reshape the world. Whether you choose to unite the kingdom, exploit the chaos for personal gain, or forge entirely new alliances, your journey through ?-? will be uniquely yours.
