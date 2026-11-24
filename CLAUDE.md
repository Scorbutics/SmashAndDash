# Smash & Dash

How to reach the running game (launch, remote eval in skalang, screenshots, replays): @AGENTS.md

Pokémon fan game with real time battles, built on the ska engine (`external/ska`, with skalang in `external/ska/external/skalang`). The game is a library (`libGame`) loaded by the engine launcher `ska_main`.

## Build and run

```sh
cmake -S . -B build && cmake --build build -j8
LD_LIBRARY_PATH=build/lib build/bin/ska_main   # from the repository root (the game data and the "wd:" scripts are relative to it)
```

The build compiles every script (`smash_and_dash_scripts_check`): a script error fails the build, with its file and line.

Battle simulation: `SAD_BATTLE_AUTOPLAY=1` makes the battle AI play the Pokemon of the player too; the debug log traces the battle (`Battle t=...`: decisions, skills used, hits, misses).

Script tests, headless (no window): `cd build && ctest`. They are `tests/*_test.miniska` (assertions: `Test = import "bind:std.native.test"`), run with the templates of the engine and game functions (default bodies) and the fakes of `tests/fakes` (ex: the engine keeps the game variables, `setNextRandom` chooses the random values). Add a test with a script change when the script has logic (rules, AI, helpers).

Web (browser, WebAssembly): `tools/web/build.sh [build-web]` (needs the Emscripten SDK: `source <emsdk>/emsdk_env.sh`, or `EMSDK=<emsdk>`; the first build builds the SDL3 libraries for the web), then `tools/web/serve.sh` serves `build-web/game/bin` to the local network (`http://<address>:8000`). The page is `src/web/shell.html`: it keeps the saves (`Data/Saves`) in the browser (IndexedDB). On the web: no thread, the browser calls each frame (`GameCore::run`), the renderer is WebGL (`opengles2`: no shaders), the game files are packed in `index.data` (the directories listed in `src/CMakeLists.txt`). The engine side is `external/ska/cmake/SkaWeb.cmake`.

Linux CI: `.github/workflows/linux.yml` (Debug and Release builds and the script tests, in an Ubuntu 26.04 container for its SDL3 packages).

Shaders: GLSL fragment shaders in `src/shaders/*.frag`, compiled during the build to `Shaders/<name>.frag.spv` (ska `ska_compile_shaders`, needs `glslangValidator`), loaded with `ska::Shader{ "./Shaders/<name>" }` (ska README "Shaders"). They need the SDL3 `gpu` renderer: without it the game draws without them. Ex: `battle.frag`, the post-processing of the battles (`GUIBattle`).

## Scripts

Read [SCRIPTING.md](SCRIPTING.md) (where scripts start, the functions given to them) and `external/ska/external/skalang/doc/LANGUAGE.md` (the language) before writing a script. The syntax differs from Ruby, JavaScript and Python. Keep skalang's rules: strong static typing, no implicit conversion, no null value.

- The generator is being ported to C++ (`src/src/Game/Gameplay/World/Generation`, plain C++ : it will make the world of a new game in the game, native and web), stage by stage, checked bit for bit against the Python one: `tools/worldgen/golden.py > tests/worldgen/golden.txt` (after a change of the Python generator), `ctest -R sad_worldgen`. Done: the noise, the climate (biomes, relief), the height before the villages; the villages (sites, growth, names), the distance to them, the biome weights and the mixes of the corners (`WorldPlan`); the roads (A*), the terrain (grounds, terraces, cleaned), the stairs, the places (`Terrain`); the tile of each block of the three layers (a tile is the text of its Python key), the blocks walked on, the villages decorated, the trees, the signs, the items, the rooms, the events, the light (`Layers`: the shapes of the drawn structures come from `Resources/Worldgen/shapes.txt`, `tools/worldgen/shapes.py`; the room templates are `Resources/Worldgen/rooms`). the chipsets (`Chipset`: the tiles drawn in each biome by Python, `Resources/Worldgen/tiles.png` at one pixel per voxel, made by `tools/worldgen/atlas.py` after a change of the drawings or of the keys; C++ blends them per corner as PIL does and fills the cells; `Png`: a PNG reader and a BMP writer, plain C++). Next: the files of a world, a world made at the new game.
- The maps are generated, a world per save: `tools/worldgen/worldgen.py --seed 7 --save new_game --save save1` writes the world `Data/Worlds/seed7/` (8 x 8 chunks of 40 x 40 `w_*`, rooms `w_room_*`, a chipset per map in `Chipsets/`, biomes from a climate as in Minecraft, blended at their borders) and the saves that start in it (their `trainer.ini` names it: `[World] directory`; the game reads its maps there, `MapDirectory` in `SavePaths.h`). `--seed 42 --save save2`: another save, another world (SCRIPTING.md, "The world"). Change the generator, not its output. No story, no characters: only the capability scripts (`Levels/*.miniska`: surf, ledges, teleport, healing, shop, dialog, signs, items to find, rest beds).
- Map events: `Data/Worlds/<world>/<map>/<map>E.txt`; the scripts they start stay in `Levels/`. Tile scripts: `Resources/Chipsets/tile_scripts/` (copied into the `Scripts/` of each generated chipset by the generator).
- The functions the game gives to the scripts are C++ modules in `src/src/Game/Script` (`game.dialog`, `game.character`, `game.map`, `game.party`, `game.interface`, `game.battle`), each declared by a template in `src/scripts/game.<name>.miniska`. Change the template with the C++ binding.
- A script only imported by other scripts (no `run` function) is listed in `GAME_SCRIPT_MODULES` (`src/CMakeLists.txt`).
- Script files with accents are Latin-1 (the text rendering). `¤` is a line break in a message.

## Boundaries

C++ (ECS systems) owns the per frame work: the player and the characters walking around (`wander`, `patrol`) move block by block on the map (`Gameplay/Control/GridMovementSystem`, walls from the physics shapes), freely in battles; the drawing shows the entities between their last two logic states (`ska::FrameClock`); the real time battle simulation (`Gameplay/Fight`, a fighting game: each fighter has a `FighterComponent` (its side, its current opponent: the nearest one standing, who plays it, its attack, stun, K.O., cooldowns) iterated by `BattleDirectorSystem`, `BattleSystem`, `FighterSystem` and the skill systems; `BattleStage` shares the battle-wide state (time with its hit stops, arena, feedbacks, sounds, which fighter is the player's) with them and `GUIBattle`; `BattleHit` applies a hit (damage, `BattleEffects`: stat stages and statuses); `BattleCameraSystem` moves and zooms the camera (the world is drawn zoomed below `WorldInterfacePriority`, the interface is not: `DrawableContainer::setWorldZoom`)): skills, movements, hits, end of the battle. The battle rules and the AI of the wild Pokemon are scripts (`Data/Battle`, called through `BattleRules`, each with a C++ fallback: `rules` statistics, damage, types; `status`; `stages`; `encounters`; `skills` known; `rewards`; `ai`) : pure functions asked at events, never every frame (what applies every frame is asked when it changes and cached); the battle rolls the dice and gives them, so a seeded replay stays the same, the battle flow is scripted with `Battle.start`, the `battle.won` / `battle.lost` hooks and the steps of each battle (`Data/Battle/steps.miniska`: the director starts it at each step and waits for it; its banners, its dialogs). The battle interface (`GUIBattle`) draws with the theme of the GUI (`Theme::panel`, the images of `Menu/default_theme` made by `tools/tilegen/ui.py`). Scripts own the content and the sequencing: they call verbs and wait for their completion. A missing feature for scripts is a new verb in a game module (C++ and template), not polling from the script.
