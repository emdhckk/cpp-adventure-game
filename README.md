# Element Island — Infinite Procedural 2D Adventure Game

**Element Island** is an isometric exploration game written in C++ and inspired by the Avatar universe. The player controls a hero mastering the four elements (Fire, Water, Air, Earth) and navigates procedurally generated thematic islands.

The primary objective is to complete four elemental quests: converse with island guardians, fulfill their trials, and recover ancient relics to achieve victory. Dynamic enemies spawn at regular intervals and scale in difficulty over time.

---

## Authors / Team
Academic project developed for the **Object-Oriented Programming** course (INFO-H2001) at **Université libre de Bruxelles (ULB)** (Academic Year 2025–2026).

* Emma Dehaeck
* Ranya Jabrazko
* Vladimir Tranca
* Gio Bao

---

## Features

### Core Implementation
- **Class Hierarchy & Polymorphism:** Abstract base class `Entity` inherited by `Player`, `Enemy`, and `Neutral`. Entities are stored within a `std::vector<std::unique_ptr<Entity>>` and managed via runtime polymorphism.
- **2D Isometric Engine:** Custom isometric rendering using SFML 3.0.
- **Infinite World & Dynamic Chunks:** 32×32 tile chunks loaded and unloaded dynamically based on player proximity to optimize memory usage.
- **Procedural Generation:** Dual Voronoi noise generation (`shapeNoise` and `biomeNoise`) via FastNoiseLite to synthesize oceans, coastlines, and elemental biomes. Deterministic relic placement implemented using chunk-coordinate hashing.
- **State Persistence:** World modifications (e.g., extinguished volcanoes, constructed bridges) are recorded in a `TileChange` structure and re-applied when chunks reload.
- **Serialization:** Full game state (player position, HP, stats, abilities, quests, boat status, modified tiles) saved to JSON format via `nlohmann/json` (manual trigger with `F5` or automatic on exit).
- **Interactive Environment:** Impassable water tiles, interactable/destructible elements (volcanoes extinguished with Water powers, collectible fragments, relic pick-ups).
- **Autonomous Behaviors:** Dynamic pathfinding/pursuit AI for enemies; autonomous wandering routines for Neutral NPCs.
- **Progression System:** Upgradable elemental powers (`firePower`, `waterPower`, `airPower`, `earthPower`), progressive difficulty scaling (`enemyPower`), and multi-stage quest progression.

### Additional Highlights
- Directional animated spritesheets for characters and creatures.
- Interactive NPC dialogue tree and distinct quest storylines per biome.
- Navigable boat mechanics with treasure maps for exploration.
- Real-time compass tracking unvisited NPCs.
- Procedural title screen with coded elemental rendering and looping audio.
- Persistent high-score tracker (kills and quest completion time).

---

## Technical Design & Architecture

* **Procedural Generation:** Uses FastNoiseLite Voronoi algorithms. `shapeNoise` determines elevation and ocean boundaries; `biomeNoise` partitions regions into elemental archetypes.
* **Chunk & Memory Management:** Chunks are kept lightweight; only visible chunks reside in active memory. Player-induced environmental changes are decoupled into a lightweight delta-log (`TileChange`) to preserve determinism while allowing dynamic persistence.
* **Polymorphic Architecture:** Unified entity management loop operating on `std::unique_ptr<Entity>`, abstracting drawing, physics, and state updates.
* **Optimized Isometric Rendering:** Chunk terrain is batched into a single `sf::VertexArray` constructed directly from tile spritesheets using projection offsets ($X_{\text{step}} = 26.0\text{px}$, $Y_{\text{step}} = 13.0\text{px}$).

```text
src/
├── main.cpp              — Main game loop, event handling, rendering pipeline
├── Entity.h/cpp          — Abstract base entity (position, HP, draw, update)
├── Player.h/cpp          — Player entity: input handling, combat, HUD, animations
├── Enemy.h/cpp           — Autonomous enemy AI and health systems
├── Neutral.h/cpp         — Guardian NPC: idle AI and dialogue system
├── World.h/cpp           — World management, chunk streaming, collision pipeline
├── Chunk.h/cpp           — Procedural chunk generation, isometric vertex arrays
├── Tile.h/cpp            — Individual tile definitions
├── TileRegistry.h/cpp    — Metadata registry (walkable, interactable)
├── TileChange.h          — Delta tracking structure for modified tiles
├── LoadedChunk.h         — Chunk coordinate mapping
├── NoiseGen.h/cpp        — FastNoiseLite wrapper (shape and biome sampling)
├── FastNoiseLite.h       — Noise generation library
├── Island.h/cpp          — Island clustering structure
├── QuestManager.h/cpp    — Quest states, dialogues, and relic HUD
├── Boat.h/cpp            — Boat navigation, fragments, and treasure map mechanics
├── RelicRender.h/cpp     — Relic layer rendering
├── TileFinder.h/cpp      — Spatial lookup utility for chunks
├── Compass.h/cpp         — Directional compass pointing to objectives
├── ScoreSave.h/cpp       — High-score JSON persistence
├── TitleScreen.h/cpp     — Animated main menu
└── GameOver.h/cpp        — Game over / victory screen

assets/
├── textures/             — Spritesheets (characters, enemies, boat, terrain)
├── fonts/                — Typography assets
└── island_0.ogg          — Soundtrack

savefiles/
├── savefile.json         — Active game state
└── highscore.json        — High-score records
