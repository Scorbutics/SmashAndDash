# Smash & Dash

Pokémon fan game with real time battles, built on the ska engine (`external/ska`, with skalang in `external/ska/external/skalang`). The game is a library (`libGame`) loaded by the engine launcher `ska_main`.

## Build and run

```sh
cmake -S . -B build && cmake --build build -j8
LD_LIBRARY_PATH=build/lib build/bin/ska_main   # from the repository root (the game data and the "wd:" scripts are relative to it)
```

The build compiles every script (`smash_and_dash_scripts_check`): a script error fails the build, with its file and line.

Battle simulation: `SAD_BATTLE_AUTOPLAY=1` makes the battle AI play the Pokemon of the player too; the debug log traces the battle (`Battle t=...`: decisions, skills used, hits, misses).

Script tests, headless (no window): `cd build && ctest`. They are `tests/*_test.miniska` (assertions: `Test = import "bind:std.native.test"`), run with the templates of the engine and game functions (default bodies) and the fakes of `tests/fakes` (ex: the engine keeps the game variables, `setNextRandom` chooses the random values). Add a test with a script change when the script has logic (rules, AI, helpers).

## Scripts

Read [SCRIPTING.md](SCRIPTING.md) (where scripts start, the functions given to them) and `external/ska/external/skalang/doc/LANGUAGE.md` (the language) before writing a script. The syntax differs from Ruby, JavaScript and Python. Keep skalang's rules: strong static typing, no implicit conversion, no null value.

- Map events: `Levels/<map>/<map>E.txt`. Map scripts: `Levels/<map>/*.miniska`. Shared helpers: `Levels/story.miniska`, `Levels/dialog.miniska`. Tile scripts: `Resources/Chipsets/<chipset>/Scripts/`.
- The functions the game gives to the scripts are C++ modules in `src/src/Game/Script` (`game.dialog`, `game.character`, `game.map`, `game.party`, `game.interface`, `game.battle`), each declared by a template in `src/scripts/game.<name>.miniska`. Change the template with the C++ binding.
- A script only imported by other scripts (no `run` function) is listed in `GAME_SCRIPT_MODULES` (`src/CMakeLists.txt`).
- Script files with accents are Latin-1 (the text rendering). `¤` is a line break in a message.

## Boundaries

C++ (ECS systems) owns the per frame work: the player and the characters walking around (`wander`, `patrol`) move block by block on the map (`Gameplay/Control/GridMovementSystem`, walls from the physics shapes), freely in battles; the drawing shows the entities between their last two logic states (`ska::FrameClock`); the real time battle simulation (`Gameplay/Fight`, a fighting game: `BattleStage` shares the time with its hit stops, the arena and the feedbacks between `BattleDirectorSystem`, `BattleSystem`, `FighterSystem`, the skill systems and `GUIBattle`): skills, movements, hits, end of the battle. The battle rules (statistics, damage) and the AI of the wild Pokemon are scripts (`Data/Battle`, called through `BattleRules`), the battle flow is scripted with `Battle.start` and the `battle.won` / `battle.lost` hooks. Scripts own the content and the sequencing: they call verbs and wait for their completion. A missing feature for scripts is a new verb in a game module (C++ and template), not polling from the script.
