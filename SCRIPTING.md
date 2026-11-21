# Scripting Smash & Dash

The game is scripted with [skalang](https://github.com/Scorbutics/skalang) (`.miniska` scripts), run by the ska engine (see its README, "Scripting").

## Where scripts are started

- **Map events** (`Levels/<map>/<map>E.txt`): `x y trigger script, arguments`. The trigger is `0` when the map is loaded, `1` for the action key, and `2` when the player walks onto the block.
  - Characters are added when the map is loaded, with `Levels/add_character.miniska, sprite, x, y, name, movement, script, arguments...`.
  - The movement is `fixed`, `wander`, or a path of directions walked again and again: `b` down, `d` right, `h` up, `g` left.
  - The script runs when the player talks to the character.
- **Tiles**: `Resources/Chipsets/<chipset>/Scripts/<tile x>_<tile y>_<trigger>.miniska`. The trigger is `1` for action, `2` for walked on, `3` for walked out and `4` for touched. The arguments are the previous block (x, y), the new block (x, y) and the direction.
- **Scripts**: `Engine.script(file, arguments)` starts another script.

A script declares `run = function(args: string[]) do ... end`. A script started by a trigger does not start again while it is still running.

The scripts are checked during the build (`smash_and_dash_scripts_check` in `src/CMakeLists.txt`): they are compiled without running them, with the templates of the engine and game functions. A syntax error, a type error or a wrong call fails the build, with the file and the line. A script only imported by other scripts (no `run` function) is listed in `GAME_SCRIPT_MODULES`.

## Functions given to the scripts

| Module | Import | Functions |
| --- | --- | --- |
| Engine (ska) | `Engine = import "bind:ska.engine"` | wait, log, random, origin (the entity that triggered the script), entity (by name), disableComponent / enableComponent, variable / setVariable (saved game variables), script |
| Dialogs | `GameDialog = import "bind:game.dialog"` | message, choice (yes / no), cinematic |
| Characters | `Character = import "bind:game.character"` | move, jump, direction / setDirection, setLayer, playAnimation, place, remove, wander, patrol, addCharacter, follow (walks after another character, until `follow(entity, -1)`) |
| Maps | `Map = import "bind:game.map"` | teleport, blockX / blockY, blockHasProperty, shake (camera), crumbling |
| Team | `Party = import "bind:game.party"` | addPokemon, size, hp, level, heal, pokemonOut |
| Interface | `Interface = import "bind:game.interface"` | hideInterface (buttons and windows, not the messages), shop |
| Battles | `Battle = import "bind:game.battle"` | start, battles, lastWon |
| Saving | `Save = import "bind:game.save"` | save |

The script templates in `src/scripts` declare these functions, with comments. The functions that wait (`wait`, `move`, `message`, `choice`, `cinematic`) suspend the script until the action is over, without blocking the game.

Some features of the old scripts do not exist in this version of the game. Their functions only log a warning: `crumbling`, `pokemonOut` and `shop` (no inventory in this version).

## Saving

- `Save.save()` saves the map and the position of the player, and the game variables (`Engine.setVariable`). The nurse of the Pokemon center saves the game after healing.
- The team is saved as it changes (`Data/Saves/<save>/Team/<index>.ini`): a new Pokemon, the hit points and the experience after a battle, the healing.
- The game variables are in `Data/Saves/<save>/variables.ini`, `[Variable]` section, `name=value`.

## Entities

Entities are `int` handles (`Engine.origin()`, `Engine.entity(name)`, `Character.addCharacter`...). A handle of a removed entity (ex: after a map change) is not valid anymore: using it stops the script with an error, even when the engine reuses the entity for another character. `Engine.exists(entity)` tests a handle. `-1` means "no entity".

## Engine events and engine calls

- Hooks: `Engine.on("event", function(entity: int) do ... end)` calls the function each time the game emits the event (it runs as a script: it can wait). `Engine.removeHook(id)` removes it. The game declares its events (`ScriptVM::declareEvent`): an unknown event is an error.
- The game can ask a script for a value (ex: a formula): `ScriptVM::call<long>(file, function, arguments)`. Such a function cannot wait.

## Battles

Battles are real time (C++): the Pokemon of the player uses its skills with the keys 1 to 4, a skill can be used again after its cooldown. The scripts give the rules and the behaviours:

- `Data/Battle/rules.miniska`: statistics from the base statistics and the level (`stat`, `maxHp`), level from the experience (`level`), damage of a hit (`damage`).
- `Data/Battle/ai.miniska`: the skill used by a wild Pokemon (`decide`, asked every 500 ms with the skills it can use now: ready and in range).
- These functions are called by the battle and cannot wait. When one fails, the error is logged and a default value is used.
- `Battle = import "bind:game.battle"`: `Battle.start(monster, level)` starts a battle against a wild Pokemon next to the player, with the first Pokemon of the team able to fight, and waits for its end (back on the map): `true` when the player won, `false` right away when no Pokemon can fight.
- After a battle, the Pokemon of the player keeps its hit points left and wins experience (`experience` rule). A Pokemon with no hit points left does not fight until it is healed (`Party.heal`).
- Hooks: `Engine.on("battle.won", ...)` and `Engine.on("battle.lost", ...)` (with the player) at the end of each battle, including the random ones. They start during the end of the battle: what they do after a wait happens back on the map.
- Every Pokemon has the basic attack (`Data/Skills/autoattack.ini`) in its first slot without a projectile skill.

## Shared script modules

- `Levels/dialog.miniska`: `Dialog.say(text)`. The player cannot move during the message, and only one dialog runs at a time.
- `Levels/story.miniska`: helpers for the story.
  - `Story.player()`
  - `Story.npc(number, index)`: the character with the sprite `pnj<number>`, named `npc-<number>_<index>` in the map events.
  - `Story.walk(entity, direction, blocks)`
  - Story steps, saved with the game: `Story.done(step)` / `Story.setDone(step)`.
- `Levels/surf.miniska`, `Levels/jump_wall.miniska`: water and ledges.

## Tests

`tests/*_test.miniska` are run by `ctest` (in the build directory), without the game: the engine and game functions are their templates (default bodies), or the fakes of `tests/fakes`. A test declares `run`, and checks with `Test = import "bind:std.native.test"` (`check`, `equal`, `fail`).

## Text

Messages are encoded in Latin-1 (the game text rendering), so script files containing accents are saved in Latin-1. In a message, `¤` (byte 0xA4) is a line break.
