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
| Characters | `Character = import "bind:game.character"` | move, jump, direction / setDirection, setLayer, playAnimation, place, remove, wander, patrol, addCharacter, follow |
| Maps | `Map = import "bind:game.map"` | teleport, blockX / blockY, blockHasProperty, shake, crumbling |
| Team | `Party = import "bind:game.party"` | addPokemon, heal, pokemonOut |
| Interface | `Interface = import "bind:game.interface"` | hideInterface, shop |

The script templates in `src/scripts` declare these functions, with comments. The functions that wait (`wait`, `move`, `message`, `choice`, `cinematic`) suspend the script until the action is over, without blocking the game.

Some features of the old scripts do not exist in this version of the game. Their functions only log a warning: `heal`, `shake`, `crumbling`, `follow`, `hideInterface`, `pokemonOut` and `shop`.

## Entities

Entities are `int` handles (`Engine.origin()`, `Engine.entity(name)`, `Character.addCharacter`...). A handle of a removed entity (ex: after a map change) is not valid anymore: using it stops the script with an error, even when the engine reuses the entity for another character. `Engine.exists(entity)` tests a handle. `-1` means "no entity".

## Engine events and engine calls

- Hooks: `Engine.on("event", function(entity: int) do ... end)` calls the function each time the game emits the event (it runs as a script: it can wait). `Engine.removeHook(id)` removes it. The game declares its events (`ScriptVM::declareEvent`): an unknown event is an error.
- The game can ask a script for a value (ex: a formula): `ScriptVM::call<long>(file, function, arguments)`. Such a function cannot wait.

## Shared script modules

- `Levels/dialog.miniska`: `Dialog.say(text)`. The player cannot move during the message, and only one dialog runs at a time.
- `Levels/story.miniska`: helpers for the story.
  - `Story.player()`
  - `Story.npc(number, index)`: the character with the sprite `pnj<number>`, named `npc-<number>_<index>` in the map events.
  - `Story.walk(entity, direction, blocks)`
  - Story steps, saved with the game: `Story.done(step)` / `Story.setDone(step)`.
- `Levels/surf.miniska`, `Levels/jump_wall.miniska`: water and ledges.

## Text

Messages are encoded in Latin-1 (the game text rendering), so script files containing accents are saved in Latin-1. In a message, `¤` (byte 0xA4) is a line break.
