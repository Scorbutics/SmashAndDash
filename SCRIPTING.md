# Scripting Smash & Dash

The game is scripted with [skalang](https://github.com/Scorbutics/skalang) (`.miniska` scripts), run by the ska engine (see its README, "Scripting").

## Where scripts are started

- **Map events** (`<map>E.txt` in the folder of the map: `Data/Worlds/<world>/<map>/`): `x y trigger script, arguments`. The trigger is `0` when the map is loaded, `1` for the action key, and `2` when the player walks onto the block. The maps are generated (see "The world"): their events are the teleports of the chunk edges and the doors (`Levels/teleport.miniska, map, x, y`) and the healing counters (`Levels/pokescript.miniska`, action key).
- **Tiles**: `Resources/Chipsets/<chipset>/Scripts/<tile x>_<tile y>_<trigger>.miniska`, listed in `Resources/Chipsets/<chipset>/scripts.txt`. The trigger is `1` for action, `2` for walked on, `3` for walked out and `4` for touched. The arguments are the previous block (x, y), the new block (x, y) and the direction. The world generator copies them from `Resources/Chipsets/tile_scripts` (`water`: starts surfing; `ledge_left`, `ledge_down`, `ledge_right`: the jumps over a ledge, not placed by the generator yet).
- **Scripts**: `Engine.script(file, arguments)` starts another script.

A script declares `run = function(args: string[]) do ... end`. A script started by a trigger does not start again while it is still running.

The scripts are checked during the build (`smash_and_dash_scripts_check` in `src/CMakeLists.txt`): they are compiled without running them, with the templates of the engine and game functions. A syntax error, a type error or a wrong call fails the build, with the file and the line. A script only imported by other scripts (no `run` function) is listed in `GAME_SCRIPT_MODULES`.

## Functions given to the scripts

| Module | Import | Functions |
| --- | --- | --- |
| Engine (ska) | `Engine = import "bind:ska.engine"` | wait, log, random, origin (the entity that triggered the script), entity (by name), disableComponent / enableComponent, variable / setVariable (saved game variables), script |
| Dialogs | `GameDialog = import "bind:game.dialog"` | message, choice (yes / no), cinematic |
| Characters | `Character = import "bind:game.character"` | setMovement / movement (`walk`, `run`, `locked` for the player), move, step (whole blocks, through the obstacles), jumpOver (step in the air : a ledge), jump, direction / setDirection, setLayer, playAnimation, setSprite (another sheet, ex : surfing), place, remove, wander, patrol, addCharacter, follow (walks after another character, until `follow(entity, -1)`) |
| Maps | `Map = import "bind:game.map"` | teleport, blockX / blockY, blockHasProperty, blockFree (no wall, no character), shake (camera), crumbling |
| Team | `Party = import "bind:game.party"` | addPokemon, size, hp, level, heal, pokemonOut |
| Interface | `Interface = import "bind:game.interface"` | hideInterface (buttons and windows, not the messages), shop |
| Battles | `Battle = import "bind:game.battle"` | start, startWith, battles, lastWon, lastFled |
| Saving | `Save = import "bind:game.save"` | save |
| Bag | `Bag = import "bind:game.inventory"` | count, items, add, remove, money, earn, pay, name, price, use |

The script templates in `src/scripts` declare these functions, with comments. The functions that wait (`wait`, `move`, `step`, `jumpOver`, `message`, `choice`, `cinematic`) suspend the script until the action is over, without blocking the game. The functions that use the battle rules (`Party.addPokemon`, `Party.level`, `Party.heal`, `Bag.use`) wait for the next update: the rules are scripts, and no script can be called during the step of another one.

Some features of the old scripts do not exist in this version of the game. Their functions only log a warning: `crumbling`, `pokemonOut` and `Interface.shop` (the shops are scripts: `Levels/shop.miniska`).

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

Battles are real time (C++), played like a fighting game: the bars of the two fighters are at the top of the screen (the player on the left, the wild Pokemon on the right), the skills of the player at the bottom (keys 1 to 4, a skill can be used again after its cooldown). The fight takes place in an arena (an overlay on the map): walking out of it for 1 second flees the wild Pokemon (a Pokemon pushed out by a hit, the player not walking, goes back inside). The camera zooms in on the arena and moves as in the anime: it follows the Pokemon of the player a little more than its opponents while it shows them all, pushes in on the hits, looks at a Pokemon preparing a long attack (a startup of 400 ms or more), closes in on a K.O. The Pokemon walk at a constant speed and stop at once (no running in a battle). An attack has a startup (the Pokemon prepares it), then it goes, then a recovery: the Pokemon cannot move during all of it and can be hit (a hit during the startup cancels the attack). A skill key pressed while the Pokemon is busy is kept 200 ms. A hit stops everything a moment, the Pokemon hit flashes, is stunned (it blinks) and pushed back; a K.O. ends in slow motion. The jump key (Space) dodges: a short dash, it cannot be hit at its start (a projectile goes through), then it cannot dodge again for a moment. A "!" shows over an opponent preparing an attack. Sounds: `Sounds/battle_*.wav` (an attack, a hit, a strong hit, a guard, a status, a dodge, a K.O.). The scripts give the rules and the behaviours:

- `Data/Battle/rules.miniska`: statistics from the base statistics and the level (`stat`, `maxHp`), level from the experience (`level`), the type chart (`effectiveness(skillType, type1, type2)`: a percentage, 0 when it does not affect the target), damage of a hit (`damage(power, attack, defense, level, effectiveness, stab)`: `stab` when the skill has a type of its Pokemon). The battle shows "SUPER EFFICACE" and "PEU EFFICACE".
- `Data/Battle/ai.miniska`: the wild Pokemon, asked every 300 ms: the skill it uses (`decide`, with the skills it can use now: ready and in range) and how it moves (`move`: hold, approach, retreat, sidestep, from the distance to its opponent and the reach of its ready skills), and whether it dodges an attack prepared against it (`dodge`, with the time left before the attack goes).
- `SAD_BATTLE_AUTOPLAY=1` (environment): the AI also plays the Pokemon of the player, to watch a battle between two AI (the debug log traces it).
- `SAD_BATTLE_PASSIVE=1` (environment): the wild Pokemon do nothing (they do not move nor attack), to test a battle (moving around, the drawing).
- On the web, the parameters of the page named `SAD_...` are the environment of the game: `index.html?SAD_BATTLE_PASSIVE=1`.
- These functions are called by the battle and cannot wait. When one fails, the error is logged and a default value is used.
- `Battle = import "bind:game.battle"`: `Battle.start(monster, level)` starts a battle against a wild Pokemon next to the player, with the first Pokemon of the team able to fight, and waits for its end (back on the map): `true` when the player won (`false`: lost or fled, `Battle.lastFled()` tells which), `false` right away when no Pokemon can fight. `Battle.startWith(monsters, levels, canFlee)` starts a battle against a group of wild Pokemon (one level for each); `canFlee` false: a story battle, the arena cannot be left. Each fighter fights the nearest one of the other side; a K.O. Pokemon leaves the fight, the battle ends when a side has no Pokemon standing (the experience of each wild Pokemon K.O. is won).
- After a battle, the Pokemon of the player keeps its hit points left and wins experience (`experience` rule). A Pokemon with no hit points left does not fight until it is healed (`Party.heal`).
- Hooks: `Engine.on("battle.won", ...)`, `Engine.on("battle.lost", ...)` and `Engine.on("battle.fled", ...)` (with the player) at the end of each battle, including the random ones. They start during the end of the battle: what they do after a wait happens back on the map.
- Every Pokemon has Charge (`Data/Skills/charge.ini`) in its first slot without a usable skill.
- Skills (`Data/Skills/*.ini`, Latin-1): `style_2=Projectile` throws `[Particle] number` particles towards the opponent; `style_2=Contact` (Charge, Griffe) lunges and hits the opponent when it is in its range (`[Stats] blocks_range`) in front of it, showing the sprite of the skill where it hits (`[Particle] show_sprite=0`: none); `style_2=Area` touches every opponent in its range around the Pokemon (Rugissement); `style_2=Self` acts on the Pokemon itself (Repli, Abri). `[Stats] cooldown`, `startup` and `recovery` (milliseconds), `[Particle] damage` (power, 0: no damage) and `knockback`. A Pokemon has the last 4 skills it learned at its level (`[Description] level`).
- Effects of a skill, `[Effects]` (all optional):
  - `target_attack`, `target_defense`, `target_speed` (on the Pokemon touched) and `self_attack`, `self_defense`, `self_speed` (on the Pokemon using it): stages, -6 to 6 (a stage multiplies the statistic by (2 + stage) / 2, or 2 / (2 - stage) below 0). They last `duration` milliseconds (6000 by default).
  - `status`: `poison`, `burn` (damage over time; a burn halves the physical damage), `paralysis` (slower, 1 attack in 4 fails), `sleep` (no action, a hit wakes it up), `leech` (damage over time healing the Pokemon that used it), `flinch` (a short stun), `guard` (on itself: blocks the hits). `status_chance` (percentage, 100 by default), for `duration` milliseconds.
  - `fixed_damage`: damage that does not depend on the statistics (Draco-Rage).

## The world

Each save has its world: `tools/worldgen/worldgen.py --seed N --save NAME` (Python, Pillow) writes the world `Data/Worlds/seed<N>/` and the save `Data/Saves/NAME` starting in it (its `trainer.ini` names the world: `[World] directory=./Data/Worlds/seed<N>/`; a save without it uses `Levels/`). `save1` and the new game: seed 7; `save2`: seed 42. A world, with the tiles of `tools/tilegen`: an island of 8 x 8 chunks of 40 x 40 blocks (`w_<x>_<y>`, edges linked by teleports), a village, forests, cliffs and stairs, water, and the inside of the buildings (`w_room_<n>`, from the templates of `tools/worldgen/rooms`). Each map has a light map (`<map>L.png`). The biomes (meadow, autumn, snow, desert: `tools/tilegen` `BIOMES`, `tools/worldgen` `NATURE`, `SPAWNS`, `BIOME_LIFT`) come from a climate, as in Minecraft: a temperature and a humidity noise spanning about 150 blocks, and a table (`climate_biome`: cold snow, hot desert, temperate and wet autumn, else meadow); a region under 500 blocks joins its neighbour. A biome has its colours, its tree, its vegetation, its wild Pokemon (those of the biome of most of the chunk) and raises the land a little (dry deserts). The relief is a variable of the climate apart from the biome (a third noise, `RELIEF_FROM`, `RELIEF_FULL`): any biome can be mountains, terraces up to 4 levels high whose rock faces are about 1.5 blocks tall; every terrace has stairs down to a lower one. The first village grows in the meadow nearest the middle of the world, where the game starts; others grow in each biome (`VILLAGE_STYLE`: roofs, snow on the roofs, adobe or log walls, names), each with a healing centre, paved streets with curbs, a fountain or a painted emblem on the plaza, benches and a bin, sandy yards, street lamps (their lanterns flicker), mailboxes and pots of flowers by the doors, flower beds that sway, butterflies, bushes, a well, barrels (desert, autumn), a fenced vegetable garden, broadleaf trees, sometimes a pond with a stone rim and water lilies (`Village`, `World.decorate`), linked by roads (A* around the water, cutting passes through the mountains). Signs at the village exits (`Levels/sign.miniska, <text>`, action key) give their names. Beside the roads, places to find (`POI_KINDS`): camps (a tent, a campfire, an item), ruins (stone pillars around an item), rest houses (a cabin whose bed heals the team: `Levels/rest.miniska`). Items to find also lie on open ground off the roads, rather on the mountains and in the forests (`Levels/item.miniska`: an item ball, a character added when the map is loaded unless its game variable `item_<map>_<x>_<y>` is set; talking to it puts the item in the bag, once). The borders of the biomes blend over about 10 blocks: each block corner has its mix of biomes, and a tile is drawn with the mix of its corners blended across it. Each map has its own chipset with the tiles it uses (`Data/Worlds/<world>/Chipsets/chipset_<map>.png`, as high as needed; `chipset_rooms` for the rooms); `Resources/Chipsets/corr.png` names 30 x 64 cells. The new game (`Data/Saves/new_game`) and `save1` start in the village. There is no story and no character but the player.

## Wild Pokemon

The wild Pokemon of a map are `<map folder>/Monsters/0.ini` (written by the generator for the biome of the chunk: `SPAWNS` in `tools/worldgen/worldgen.py`). The game logs each one: `Wild pokemon <id> level <l> at block (x, y)`, `1.ini`... (`[Data] id`, `level_min`, `level_max`, `path_type`: 1 wanders block by block, 2 stays; `[Spawn] probability_percent`, `on_blockid_<i>`: blocks where it does not appear). They appear around the player (out of sight, never in a wall). A battle starts sometimes when one is close to the player; 1 encounter in 5 is a group of two Pokemon of the same species.

## Shared script modules

- `Levels/dialog.miniska`: `Dialog.say(text)`. The player cannot move during the message, and only one dialog runs at a time.
- `Levels/surf.miniska`, `Levels/jump_wall.miniska`: water and ledges.
- `Levels/shop.miniska` (with the seller `Levels/shopscript.miniska`), `Levels/pokescript.miniska` (heals the team and saves), `Levels/teleport.miniska`.

The player is `Engine.entity("trainer")`.

## Keys and movement

- On the map, the player moves block by block (`GridMovementSystem`): holding a direction walks one block after another (0.25 s per block, 0.13 s running: Shift, or the `"run"` movement mode), a short press only turns. A step starts only when the next block is free: walls and characters block it (nothing is pushed). Walking against a wall touches it: the "touched" tile scripts run (ex: the water starts surfing). The characters given `wander` or a path move block by block too, with the same rules (they wait while the next block is not free). In battles, the movement stays free (real time).
- The movement itself is done by the game, at each frame. The scripts choose how the player moves: `Character.setMovement(player, "locked")` during a cutscene (no movement, no jump), `"run"` (faster), `"walk"` (back to normal). `Character.move` moves a character by pixels (the cutscenes): move the player by whole blocks (48 pixels) so it stays on the blocks. `Character.step(player, direction, blocks)` walks a character block by block, through the obstacles (ex: the jump over a ledge, surfing): it always stays on the blocks. `Character.jumpOver(player, direction, blocks)` does the same in the air (the jump over a ledge). The jump key hops where the player stands (not the physics jump).
- Coming through a door or a map edge (`Map.teleport`), the player appears one block behind the position (on the door) and walks to it while the screen lightens, as in the classic games: the block behind must be free, else the player appears on the position.
- Hooks of the keys on the map, with the player: `Engine.on("input.action", function(player: int) do ... end)` (action key), `"input.jump"` (jump key). They run as scripts (they can wait). The action key also starts the scripts of the characters and blocks in front of the player, and closes the messages.

## Bag and shops

- The bag (items, money) is saved as it changes, in the trainer file of the save (`[Items]`, `[Trainer] money`). The items are `Data/Items/<item>.ini` (name, `[Shop] price`, `[Buffs] hp` of a healing item), in Latin-1.
- `Bag.use(item, pokemon)` uses a healing item on a Pokemon of the team (false when it has no effect: the item is kept).
- On the map, the bag button shows the bag, and the keys Z, X, C, V, B, N use the items at these places in the bag on the first Pokemon they heal.
- `Levels/shop.miniska`: `Shop.sell(Shop.catalogue([0, 1, 2]))` offers the items one by one (yes / no), the catalogue is a map (item -> price).

## Tests

`tests/*_test.miniska` are run by `ctest` (in the build directory), without the game: the engine and game functions are their templates (default bodies), or the fakes of `tests/fakes`. A test declares `run`, and checks with `Test = import "bind:std.native.test"` (`check`, `equal`, `fail`).

## Text

Messages are encoded in Latin-1 (the game text rendering), so script files containing accents are saved in Latin-1. In a message, `¤` (byte 0xA4) is a line break.
