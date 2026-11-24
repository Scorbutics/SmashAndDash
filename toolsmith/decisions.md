# Toolsmith decisions — Smash & Dash

One dated line each, with the map it came from (`docs/agent-map.md`). The engine's decisions: `external/ska/toolsmith/decisions.md`.

- 2026-11-23 — `Data/Saves/save1` is a dev fixture on purpose: it stays the default save; named states go to `Data/Saves/fixtures/<name>`; an agent run never writes into either (it works on a copy). (map grill 1)
- 2026-11-23 — The web track (`sad-web`) goes now, independent of the native console. (map grill 2)
- 2026-11-23 — Go for every milestone of the map (M1–M4); the deferred cards stay deferred. (review)
- 2026-11-24 — The hand-made maps, the story and the characters are gone: `save1` (and `new_game`) start in the generated world (`tools/worldgen`, seed 7, `w_1_1`); `story-step` became `variables`; the doctor warps to `w_room_0 11 8`. (human: "Yes to delete all the old stuff")
- 2026-11-24 — The world is 8 x 8 chunks with climate biomes: `save1` starts on `w_3_4` (seed 7); the doctor's map name check follows it.
- 2026-11-24 — Mountains (relief): the village moved, `save1` starts on `w_5_4` (seed 7); the doctor's map name check follows it.
- 2026-11-24 — A world per save: worlds in `Data/Worlds/<name>` (seed7 for `new_game`/`save1`, seed42 for `save2`), named by the save (`[World] directory`); `Levels/` keeps only the shared scripts.
