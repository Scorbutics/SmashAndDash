#!/usr/bin/env python3
"""worldgen : a procedural world for Smash & Dash, from a seed.

The world is a grid of chunks ; each chunk is an ordinary map of 40 x 40 blocks (Data/Worlds/<world>/w_<cx>_<cy> : the 3 layers,
the .ini, the events, the wild Pokemon), linked to its neighbours by teleports on its edges. The terrain comes from
one continuous noise over the whole world, so the chunks match on their borders. The tiles are drawn by
tools/tilegen (same style) : each combination of neighbours (shores, sand and path edges, cliff faces) gets a tile of
its own. Each chunk has a biome (BIOMES : meadow, autumn, snow, desert) : its colours and its tree (tilegen.BIOMES),
its vegetation (NATURE), its wild Pokemon (SPAWNS). The village is in the meadow ; the north is colder. The borders of
the biomes wander and blend : a tile is drawn with the biomes of its 4 corners blended across it. Each map gets a
chipset of the tiles it uses (Data/Worlds/<world>/Chipsets/chipset_<map>.png ; chipset_rooms : the rooms).

  tools/worldgen/worldgen.py [--seed N] [--chunks 8x8] [--world NAME] [--root .] [--save NAME]... [--preview out.png]

  The world goes in Data/Worlds/<world> (default seed<N> ; not kept in git : the reference of the C++ port, a look at
  it) ; each --save NAME (repeatable) is a save starting in its village (Data/Saves/NAME, made from new_game), whose
  world the game generates from the seed ([World] generate, seed : the C++ port of this generator). Ex : --seed 7
  --save save1 ; --seed 42 --save save2
  --preview     the whole world in one image (the 3 layers, the colour key transparent)

In the chipsets : the water at (1, 4) (animated, its touch script starts surfing ; the game makes x < 6, y < 4 and
x < 3, y < 8 water in every chipset), the other animated tiles (flowers, tall grass) 4 cells high in ANIMATED_SLOTS.
"""
import argparse
import heapq
import bisect
import math
import os
import shutil
import sys
from PIL import Image

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "tilegen"))
import tilegen as g  # noqa: E402
from tilegen import Scene, N, T, V, KEY, h01  # noqa: E402
import rooms  # noqa: E402

CHUNK = 40
DATA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")   # the wild Pokemon, the cell properties
ROOM_MAP = (25, 15)                                  # the smallest map of a room, in blocks (the screen : 23 x 13)
# The animated tiles (4 frames, stacked in the chipset) : ids of the tiles, like the others
WATER_CELL, FLOWER_CELL, TALL_CELL = "water", "flower", "tall"
CAMPFIRE_CELL = "campfire"
ANIMATED = {WATER_CELL: g.water, FLOWER_CELL: g.flowers, TALL_CELL: g.tall_grass, CAMPFIRE_CELL: g.campfire}
ANIMATED_WALK = {WATER_CELL: False, FLOWER_CELL: True, TALL_CELL: True, CAMPFIRE_CELL: False}
# The life of the villages : the lanterns of the lamps flicker, the fountains splash, the flower beds sway, butterflies
FLOWERBED_COLOURS = ["roofr", "flower", "roofb", "orange"]
ANIMATED["lamphead"], ANIMATED_WALK["lamphead"] = g.lamp_head, True
ANIMATED["butterfly"], ANIMATED_WALK["butterfly"] = g.butterfly, True
for _c in range(2):
    for _r in range(2):
        ANIMATED[f"fountain_{_c}_{_r}"] = (lambda c, r: lambda f: g.fountain(c, r, f))(_c, _r)
        ANIMATED_WALK[f"fountain_{_c}_{_r}"] = False
for _colour in FLOWERBED_COLOURS:
    ANIMATED[f"flowerbed_{_colour}"] = (lambda col: lambda f: g.flowerbed_anim(col, f))(_colour)
    ANIMATED_WALK[f"flowerbed_{_colour}"] = True
# The cells with a touch script (Resources/Chipsets/tile_scripts/<name>.miniska ; also there : ledge_left, ledge_down,
# ledge_right, the jumps over a ledge, for when the world has ledges)
TILE_SCRIPTS = {WATER_CELL: "water"}
# The game makes these cells water in every chipset (WorldState BuildTileset) : the water goes there, nothing else
WATER_REGION = {(x, y) for x in range(6) for y in range(4)} | {(x, y) for x in range(3) for y in range(4, 8)}
WATER_SLOTS = [(1, 4)] + [(x, 0) for x in range(6)] + [(0, 4), (2, 4)]
# The chipsets : 30 cells wide, as high as a map needs (Resources/Chipsets/corr.png names CHIP_ROWS rows of cells)
CHIP_COLS, CHIP_ROWS = 30, 64
# Where the other animated tiles go (their 4 frames down from there)
ANIMATED_SLOTS = [(x, 4) for x in range(3, 30)] + [(x, 0) for x in range(6, 30)] \
    + [(x, y) for y in range(8, CHIP_ROWS, 4) for x in range(30)]
# The biomes of a block corner : how many MIX_STEPS of each biome (BIOMES order). A tile is drawn with the mix of its 4
# corners, blended across it : two neighbouring blocks meet on the same colour, the borders of the biomes are gradients
MIX_STEPS = 6
# Biomes (tilegen.BIOMES : their colours, their tree) : one per chunk, the village's is the meadow. Their borders
# wander away from the chunk edges and blend over about 10 blocks : colours (gradients), vegetation, trees.
BIOMES = list(g.BIOMES)
# The climate, as in Minecraft : two noises over the world, the temperature and the humidity, slow (a biome spans
# about CLIMATE_SCALE blocks), ranked over the world (0 the coldest / driest .. 1), then a table : cold -> snow,
# hot -> desert, temperate and wet -> autumn forest, temperate -> meadow. A region smaller than BIOME_MIN blocks joins
# the biome around it (the smoothing of the Minecraft biome layers)
CLIMATE_SCALE = 150.0
BIOME_MIN = 500
# The relief, a variable of the climate apart from the biome (any biome can be mountains, the sea cannot) : a third
# slow noise, ranked over the world ; over RELIEF_FROM the land rises in terraces (cliffs, up to MAX_LEVEL levels),
# fully from RELIEF_FULL
RELIEF_FROM, RELIEF_FULL, MAX_LEVEL = 0.55, 0.8, 4
# How much each biome raises the land (fewer lakes)
BIOME_LIFT = {"meadow": 0.0, "autumn": 0.015, "snow": 0.0, "desert": 0.05}


def climate_biome(t, h):
    if t < 0.28:
        return "snow"
    if t > 0.72:
        return "desert"
    return "autumn" if h > 0.55 else "meadow"
# Vegetation : the forests (noise over), the groves, the lone trees (chance), the tall grass (noise over), the boulders
NATURE = {
    "meadow": dict(forest=0.52, grove=0.64, lone=0.015, tall=0.60, boulder=0.010),
    "autumn": dict(forest=0.47, grove=0.60, lone=0.025, tall=0.58, boulder=0.008),
    "snow": dict(forest=0.50, grove=0.62, lone=0.020, tall=0.64, boulder=0.012),
    "desert": dict(forest=9.00, grove=9.00, lone=0.035, tall=0.66, boulder=0.025),
}
# The wild Pokemon of a biome : (id, level min, level max, path type (1 wanders, 2 stays), chance %, blocks excluded :
# the game numbers a block x + y * 48, see WorldState::spawnMob)
SPAWNS = {
    "meadow": [(16, 2, 5, 2, 100, []), (19, 2, 4, 1, 75, []), (25, 2, 4, 1, 35, []), (1, 2, 4, 1, 15, [])],
    "autumn": [(10, 3, 6, 1, 80, []), (16, 3, 6, 2, 100, []), (1, 3, 5, 1, 30, []), (19, 3, 5, 1, 40, [])],
    "snow": [(7, 4, 7, 1, 60, []), (19, 4, 6, 1, 50, []), (25, 4, 7, 1, 25, [])],
    "desert": [(4, 4, 7, 1, 60, []), (19, 4, 6, 1, 60, []), (16, 4, 6, 2, 50, [])],
}
WALK, BLOCK = (255, 255, 255), (0, 0, 0)
WALK_ANIMATED, BLOCK_ANIMATED = (0xAA, 0xAA, 0xAA), (0x46, 0x46, 0x46)
EMPTY = (255, 255, 255)
# 8 neighbours : N E S W, then NE SE SW NW
D8 = [(0, -1), (1, 0), (0, 1), (-1, 0), (1, -1), (1, 1), (-1, 1), (-1, -1)]
# The grounds over one another : a higher one reaches into the cells of a lower one (the paving : straight curbs)
PRIORITY = {"water": 0, "pave": 0.5, "sand": 1, "path": 2, "grass": 3}
# The trees : their width, and their rows (dy from the block of the canopy bottom, row of the image, kind : "top" over
# the characters, "canopy" blocks and covers them, "trunk" blocks)
TREE_SHAPES = {
    "round": (3, [(-2, 0, "top"), (-1, 1, "top"), (0, 2, "canopy"), (1, 3, "trunk")]),
    "pine": (2, [(-1, 0, "top"), (0, 1, "canopy"), (1, 3, "trunk")]),
    "snowpine": (2, [(-1, 0, "top"), (0, 1, "canopy"), (1, 3, "trunk")]),
    "cactus": (2, [(-1, 0, "top"), (0, 1, "canopy"), (1, 3, "trunk")]),
}


# ---------------------------------------------------------------- noise (value noise, seeded, continuous everywhere)

def lattice(ix, iy, seed):
    return h01(ix, iy, seed * 7919 + 13)


def smooth(t):
    return t * t * (3 - 2 * t)


def vnoise(x, y, seed):
    ix, iy = math.floor(x), math.floor(y)
    fx, fy = smooth(x - ix), smooth(y - iy)
    a, b = lattice(ix, iy, seed), lattice(ix + 1, iy, seed)
    c, d = lattice(ix, iy + 1, seed), lattice(ix + 1, iy + 1, seed)
    return (a + (b - a) * fx) * (1 - fy) + (c + (d - c) * fx) * fy


def fbm(x, y, seed, octaves=4):
    v, amp, total, f = 0.0, 1.0, 0.0, 1.0
    for o in range(octaves):
        v += amp * vnoise(x * f, y * f, seed + o * 101)
        total += amp
        amp *= 0.5
        f *= 2.0
    return v / total


def mask8(cell_is, x, y):
    """The neighbours of (x, y) for which cell_is(nx, ny) : 8 bits (N E S W NE SE SW NW). A diagonal counts only when
    its two sides do not (else it changes nothing) : at most 47 different masks."""
    bits = [cell_is(x + dx, y + dy) for dx, dy in D8]
    for k, (a, b) in zip(range(4, 8), ((0, 1), (2, 1), (2, 3), (0, 3))):
        if bits[a] or bits[b]:
            bits[k] = False
    return tuple(bits)


# ---------------------------------------------------------------- tiles (drawn with the tilegen style)

def wobble(t, phase):
    """A depth (voxels) along an edge, periodic on a tile : two tiles side by side meet on the same depth."""
    return 4.2 + 0.9 * math.sin(2 * math.pi * t / 8 + phase) + 0.5 * math.sin(2 * math.pi * t / 16 * 3 + 2 * phase)


def intrusion(mask, straight=False):
    """The voxels of a tile taken by the neighbours of the mask (rounded, wavy edges ; straight : the edges of a
    paving, square corners)."""
    inside = [[False] * N for _ in range(N)]
    for y in range(N):
        for x in range(N):
            cx, cy = x + 0.5, y + 0.5
            if straight:
                d = 3
                inside[y][x] = (mask[0] and cy < d) or (mask[1] and N - cx < d) or (mask[2] and N - cy < d) \
                    or (mask[3] and cx < d) or (mask[4] and cy < d and N - cx < d) \
                    or (mask[5] and N - cy < d and N - cx < d) or (mask[6] and N - cy < d and cx < d) \
                    or (mask[7] and cy < d and cx < d)
                continue
            hit = False
            if mask[0] and cy < wobble(cx, 0.0):
                hit = True
            if mask[1] and N - cx < wobble(cy, 1.3):
                hit = True
            if mask[2] and N - cy < wobble(cx, 2.1):
                hit = True
            if mask[3] and cx < wobble(cy, 0.7):
                hit = True
            for k, (px, py) in zip(range(4, 8), ((N, 0), (N, N), (0, N), (0, 0))):
                if mask[k] and math.hypot(cx - px, cy - py) < 5.0:
                    hit = True
            inside[y][x] = hit
    return inside


def fill(s, mat, seed, speckles=4):
    for y in range(N):
        for x in range(N):
            s.put(x, y, mat, 0, 1, 2)
    for i in range(speckles):
        x, y = int(h01(i, 1, 70 + seed) * N), int(h01(i, 2, 70 + seed) * N)
        s.put(x, y, mat, 0, 1, 3 if i % 2 else 1)


def ground(mat, over_mat=None, mask=None, shade_north=False, variant=0):
    """A ground tile : its material, the neighbours of another material reaching into it, the shadow of a cliff."""
    if mat == "grass" and over_mat is None and not shade_north:
        return g.grass(variant)
    s = Scene(N, N)
    if mat == "grass":
        g.grass_field(s, variant)
    elif mat == "pave":                              # slabs in staggered rows, their joints
        for y in range(N):
            for x in range(N):
                joint = y % 8 == 7 or (x + (4 if y // 8 % 2 else 0)) % 8 == 7
                s.put(x, y, "pave", 0, 1, 1.35 if joint else 2.35 if y % 8 == 0 else 2.0)
    else:
        fill(s, mat, 30 + variant, 6)
        if mat == "path":
            for i in range(3):                      # pebbles
                s.put(int(h01(i, 5, 41) * N), int(h01(i, 6, 41) * N), "stone", 0, 1, 2)
    if over_mat:
        inside = intrusion(mask, straight=mat == "pave")
        over = Scene(N, N)
        if over_mat == "grass":
            g.grass_field(over, variant)
        for y in range(N):
            for x in range(N):
                if inside[y][x]:
                    edge = any(0 <= x + dx < N and 0 <= y + dy < N and not inside[y + dy][x + dx]
                               for dx, dy in D8[:4])
                    t = over.tone[y][x] if over_mat == "grass" and not edge else 1 if edge else 2
                    s.put(x, y, over_mat, 0, 1, t)
                elif any(0 <= x + dx < N and 0 <= y + dy < N and inside[y + dy][x + dx] for dx, dy in D8[:4]):
                    s.tone[y][x] = 0.9 if mat == "pave" else 1   # the curb of a paving, the shadow of the edge
    if shade_north:                                  # the foot of a cliff face, to the north
        for y in range(3):
            for x in range(N):
                s.tone[y][x] = 0 if y == 0 else 1 if y == 1 else min(2, s.tone[y][x] or 2)
    return s.render()


def water_overlay(over_mat, mask):
    """Over the water (the animated cell under it) : the bank reaching into the water, a foam line along it."""
    s = Scene(N, N)
    inside = intrusion(mask, straight=over_mat == "pave")
    for y in range(N):
        for x in range(N):
            if inside[y][x]:
                edge = any(0 <= x + dx < N and 0 <= y + dy < N and not inside[y + dy][x + dx] for dx, dy in D8[:4])
                s.put(x, y, over_mat, 0, 1, 1 if edge else 2.4 if over_mat == "pave" else 2)
            elif any(0 <= x + dx < N and 0 <= y + dy < N and inside[y + dy][x + dx] for dx, dy in D8[:4]):
                s.put(x, y, "water", 0, 1, 3)       # foam
    return s.render()


def cliff(low, stairs=False, foot=False):
    """A cell of a plateau next to lower ground (low : 8 bits, N E S W NE SE SW NW). A low south : a rock face in
    the lower part of the cell (the plateau seen from the front) ; a low west or east : the rock side ; a low north :
    the rim. Stairs : steps cut in the face."""
    s = Scene(N, N)
    g.grass_field(s, 2)
    if low[0]:                                       # the back of the plateau : a rock rim, its top in the light
        for x in range(N):
            s.put(x, 0, "rock", 0, 1, 1)
            s.put(x, 1, "rock", 0, 1, 3)
            s.put(x, 2, "rock", 0, 1, 2)
            s.put(x, 3, "grass", 0, 1, 1)
    side = 6                                         # the rock of the sides : half the cell, in layers
    for y in range(N):
        layer = 1 if y % 4 == 3 else 0
        if low[3]:                                   # the west side, in the light
            for x in range(side):
                s.put(x, y, "rock", 0, 1, (2 if x == 0 else 3) - layer)
            s.put(side, y, "grass", 0, 1, 1)
        if low[1]:                                   # the east side, in the shade
            for x in range(N - side, N):
                s.put(x, y, "rock", 0, 1, max(0, (0 if x == N - 1 else 1) - layer))
            s.put(N - side - 1, y, "grass", 0, 1, 1)
    for k, (px, py) in zip(range(4, 8), ((N, 0), (N, N), (0, N), (0, 0))):
        if low[k]:
            for y in range(N):
                for x in range(N):
                    d = math.hypot(x + 0.5 - px, y + 0.5 - py)
                    if d < 3.2:
                        s.put(x, y, "rock", 0, 1, 1 if d < 2.2 else 2)
    if low[2]:
        top = 6
        for y in range(top, N):
            for x in range(N):
                band = (y - top) % 4
                t = 3 if y == top else 0 if y == N - 1 and not foot else 1 if band == 3 else 2
                if h01(x // 3, y // 4, 77) < 0.15 and y not in (top, N - 1):
                    t = 1                            # a crack
                if low[3] and x < 3 and y > top:
                    t = min(3, t + 1)                # the west end of the face, in the light
                if low[1] and x > N - 4 and y > top:
                    t = max(0, t - 1)                # the east end, in the shade
                s.put(x, y, "rock", 0, 1, t)
        for x in range(N):
            s.put(x, top - 1, "grass", 0, 1, 1)      # the lip of the grass over the face
        if stairs:
            for y in range(top - 1, N):
                for x in range(4, 12):
                    step = (y - top + 1) % 3
                    s.put(x, y, "stone", 0, 1, 3 if step == 0 else 2 if step == 1 else 1)
                s.put(3, y, "rock", 0, 1, 0)
                s.put(12, y, "rock", 0, 1, 0)
    return s.render()


def cliff_foot(mat, west, east, stairs=False):
    """The lower part of a rock face, on the block under it (a face is about 1.5 blocks high) : the rock bands go
    on from the face above, then the ground at its foot, in the shade. west, east : the face goes on that side (else
    its end, in the light to the west, in the shade to the east). Stairs : the steps go on down to the ground."""
    s = Scene(N, N)
    if mat == "grass":
        g.grass_field(s, 2)
    else:
        fill(s, mat, 31, 4)
    bottom = 11                                      # the face ends here, the ground below
    for y in range(bottom):
        band = (y + N - 6) % 4
        for x in range(N):
            t = 1 if band == 3 else 2
            if h01(x // 3, (y + 16) // 4, 77) < 0.15:
                t = 1                                # a crack
            if not west and x < 3:
                t = min(3, t + 1)
            if not east and x > N - 4:
                t = max(0, t - 1)
            s.put(x, y, "rock", 0, 1, 0 if y == bottom - 1 else t)
    for y in range(bottom, bottom + 2):              # the shadow at the foot of the face
        for x in range(N):
            s.tone[y][x] = 0 if y == bottom else 1
    if stairs:
        for y in range(0, N):
            for x in range(4, 12):
                step = (y + N - 5) % 3
                s.put(x, y, "stone", 0, 1, 3 if step == 0 else 2 if step == 1 else 1)
            if y < bottom:
                s.put(3, y, "rock", 0, 1, 0)
                s.put(12, y, "rock", 0, 1, 0)
    return s.render()


# ---------------------------------------------------------------- the tiles of the world

def pure(biome):
    """A corner all in one biome."""
    return tuple(MIX_STEPS if b == biome else 0 for b in BIOMES)


def pure_mix(biome):
    """A block all in one biome : its 4 corners."""
    return (pure(biome),) * 4


MEADOW = pure_mix("meadow")
_MASKS = {}


def blend_mask(weights):
    """A mask (L, a tile) : the weights of the 4 corners (NW NE SW SE, 0 .. 1) blended across the tile, per voxel."""
    if weights not in _MASKS:
        nw, ne, sw, se = weights
        mask = Image.new("L", (T, T))
        px = mask.load()
        for vy in range(N):
            v = (vy + 0.5) / N
            for vx in range(N):
                u = (vx + 0.5) / N
                w = (nw * (1 - u) + ne * u) * (1 - v) + (sw * (1 - u) + se * u) * v
                c = int(round(255 * w))
                for j in range(V):
                    for i in range(V):
                        px[vx * V + i, vy * V + j] = c
        _MASKS[weights] = mask
    return _MASKS[weights]


class Tiles:
    """The tiles of the world : an id per tile, drawn on demand in each palette it is used with. The chipsets of the
    maps are made from them (Chipset)."""

    def __init__(self):
        self.ids, self.draws, self.walkable, self.base, self.flags = {}, {}, {}, {}, {}
        for tid in ANIMATED:
            self.walkable[tid] = ANIMATED_WALK[tid]
            self.base[tid], self.flags[tid] = tile_base(tid), tile_flags(tid)
        self.cache, self.pure = {}, {}

    def cell(self, key, walkable):
        """The id of a tile (its key : what it shows, its image drawn by draw_key in the current palette)."""
        full = (key, walkable)
        if full not in self.ids:
            tid = len(self.ids)
            self.ids[full], self.walkable[tid], self.base[tid], self.flags[tid] = tid, walkable, tile_base(key), \
                tile_flags(key)
            self.draws[tid] = lambda: draw_key(key, walkable)
        return self.ids[full]

    def pure_frames(self, tid, biome):
        """The frames of a tile in the colours of one biome (4 for an animated tile, else 1)."""
        if (tid, biome) not in self.pure:
            g.use_biome(biome)
            images = [ANIMATED[tid](f) for f in range(4)] if tid in ANIMATED else [self.draws[tid]()]
            self.pure[(tid, biome)] = [im.convert("RGB") for im in images]
            g.use_biome("meadow")
        return self.pure[(tid, biome)]

    def frames(self, tid, mix):
        """The frames of a tile for the biomes of its corners (mix : 4 corners, see MIX_STEPS). The geometry of a tile
        does not change with the colours : a mix is the blend, pixel by pixel, of the tile in each biome."""
        if (tid, mix) not in self.cache:
            out, total = None, [0.0] * 4
            for k, biome in enumerate(BIOMES):
                w = [corner[k] / MIX_STEPS for corner in mix]
                if not any(w):
                    continue
                images = self.pure_frames(tid, biome)
                if out is None:
                    out, total = list(images), w
                    continue
                total = [a + b for a, b in zip(total, w)]
                mask = blend_mask(tuple(b / a if a else 0.0 for a, b in zip(total, w)))
                out = [Image.composite(im, o, mask) for im, o in zip(images, out)]
            self.cache[(tid, mix)] = out
        return self.cache[(tid, mix)]


class Chipset:
    """The chipset of a map : the tiles it uses, in their palettes (the same images share a cell)."""

    def __init__(self, tiles, used):
        self.image = Image.new("RGB", (CHIP_COLS * T, CHIP_ROWS * T), KEY)
        self.col = Image.new("RGB", (CHIP_COLS, CHIP_ROWS), BLOCK)
        # red : the properties of a tile (tile_flags) ; green : its base + 1 (0 flat)
        self.prop = Image.new("RGB", (CHIP_COLS, CHIP_ROWS), (0, 0, 0))
        self.cells, self.water = {}, []
        taken = set(WATER_REGION)
        by_image = {}
        water_slots, anim_slots = list(WATER_SLOTS), list(ANIMATED_SLOTS)
        order = sorted(used, key=lambda u: (str(u[0]), u[1]))
        for tid, pal in order:                       # the animated tiles first : 4 cells in a column
            if tid not in ANIMATED:
                continue
            frames = tiles.frames(tid, pal)
            sig = (tid, tuple(f.tobytes() for f in frames))
            if sig not in by_image:
                if tid == WATER_CELL:
                    if not water_slots:
                        raise SystemExit("worldgen : too many kinds of water in one map")
                    slot = water_slots.pop(0)
                    self.water.append(slot)
                else:
                    slot = next((c for c in anim_slots if all((c[0], c[1] + f) not in taken for f in range(4))), None)
                    if slot is None:
                        raise SystemExit("worldgen : no room for the animated tiles of a map")
                    anim_slots.remove(slot)
                for f, im in enumerate(frames):
                    self.image.paste(im, (slot[0] * T, (slot[1] + f) * T))
                    taken.add((slot[0], slot[1] + f))
                self.col.putpixel(slot, WALK_ANIMATED if tiles.walkable[tid] else BLOCK_ANIMATED)
                self.prop.putpixel(slot, (tiles.flags[tid], tiles.base[tid] + 1, 0))
                by_image[sig] = slot
            self.cells[(tid, pal)] = by_image[sig]
        free = [(x, y) for y in range(CHIP_ROWS) for x in range(CHIP_COLS) if (x, y) not in taken]
        for tid, pal in order:
            if tid in ANIMATED:
                continue
            im = tiles.frames(tid, pal)[0]
            sig = (tiles.walkable[tid], tiles.base[tid], tiles.flags[tid], im.tobytes())
            if sig not in by_image:
                if not free:
                    raise SystemExit(f"worldgen : the chipset of a map is full ({len(used)} tiles, {CHIP_ROWS} rows)")
                c = free.pop(0)
                taken.add(c)
                self.image.paste(im, (c[0] * T, c[1] * T))
                self.col.putpixel(c, WALK if tiles.walkable[tid] else BLOCK)
                self.prop.putpixel(c, (tiles.flags[tid], tiles.base[tid] + 1, 0))
                by_image[sig] = c
            self.cells[(tid, pal)] = by_image[sig]
        # As high as needed (a multiple of 4 rows, 16 at least)
        rows = max([16] + [y + 1 for (x, y) in taken if (x, y) not in WATER_REGION])
        self.rows = (rows + 3) // 4 * 4
        self.image = self.image.crop((0, 0, CHIP_COLS * T, self.rows * T))
        self.col = self.col.crop((0, 0, CHIP_COLS, self.rows))
        self.prop = self.prop.crop((0, 0, CHIP_COLS, self.rows))
        self.used = len(taken - WATER_REGION) + len(self.water) * 4

    def save(self, chips, name, sources):
        """Writes the chipset (its image, collisions, properties, touch scripts copied from sources) in chips."""
        base = f"{chips}/{name}"
        self.image.save(f"{base}.png")
        self.col.save(f"{base}.png.col", format="PNG")
        self.prop.save(f"{base}.png.prop", format="PNG")       # the properties : the depth of the tiles
        # The touch scripts of the cells (Resources/Chipsets/tile_scripts, checked by the build) : the water surfs
        os.makedirs(f"{base}/Scripts")
        scripts = [(x, y, TILE_SCRIPTS[WATER_CELL]) for x, y in self.water]
        with open(f"{base}/scripts.txt", "w") as f:
            f.write("".join(f"{x}_{y}\n" for x, y, _ in scripts))
        for x, y, script in scripts:
            shutil.copyfile(f"{sources}/{script}.miniska", f"{base}/Scripts/{x}_{y}_4.miniska")


_TREES = {}
_STAMPS = {}


def stamp_makers():
    """The structures stamped on the world (World.stamp) : name -> its image, drawn in the current palette."""
    out = {}
    for b in BIOMES:
        out[f"center_{b}"] = lambda b=b: g.styled(g.healing_center(), b).render()
        for roof in ROOFS:
            out[f"house_{roof}_{b}"] = lambda b=b, rf=roof: g.styled(g.house(rf), b).render()
    for canvas in ["roofo", "roofg", "roofb", "roofr"]:
        out[f"tent_{canvas}"] = lambda cv=canvas: g.tent(cv)
    for broken in (False, True):
        out[f"pillar_{broken}"] = lambda br=broken: g.pillar(br)
    return out


def crop(img, c, r):
    return img.crop((c * T, r * T, c * T + T, r * T + T))


def draw_key(key, walkable):
    """The image of a tile (not animated) from its key, in the current palette : the keys of World.build and of
    rooms.build_room."""
    kind = key[0]
    if kind == "cliff":
        return cliff(*key[1:])
    if kind == "bank":
        return water_overlay(*key[1:])
    if kind == "foot":
        return cliff_foot(*key[1:])
    if kind == "ground":
        return ground(key[1], None, None, key[4]) if len(key) == 6 else ground(*key[1:])
    if kind == "grass":
        return g.grass(key[1])
    if kind == "stamp":
        name, c, r = key[1:]
        tile = crop(stamp_image(name, STAMP_MAKERS[name]), c, r)
        return tile if r == 0 else g.over(tile, g.grass(0))
    if kind == "tree":
        style, row, c = key[1:]
        return tree_part(style, row, c) if walkable else g.over(tree_part(style, row, c), g.grass(0))
    if kind == "emblem":
        return g.emblem(*key[1:])
    if kind == "lamp":
        return g.lamp().crop((0, T, T, 2 * T))
    if kind == "fence":
        return g.fence_post(key[1]) if key[1] else g.fence()
    if kind == "flowerpot":
        return g.flowerpot(key[1])
    simple = {"bench": g.bench, "bin": g.bin_, "mailbox": g.mailbox, "barrel": g.barrel, "bush": g.bush,
              "crops": g.crops, "well": g.well, "lily": g.lily, "boulder": g.boulder, "log seat": g.log_seat,
              "sign": g.sign}
    if kind in simple:
        return simple[kind]()
    return rooms.draw_key(key)


# The depth of the tiles (the property "base" of a tileset, written in the .prop of each chipset, read by the game :
# ska TileProperties) : FLAT, a tile on the ground (drawn under the characters) ; n >= 0, a part of an object standing n
# blocks lower (drawn over the characters behind that base line, under the ones in front of it)
FLAT = -1
# The objects standing on their block (the posts, the furniture of the villages, the rocks)
STANDING = {"lamp", "fence", "flowerpot", "bench", "bin", "mailbox", "barrel", "bush", "well", "boulder", "log seat",
            "sign"}
ANIMATED_BASE = {"lamphead": 1, CAMPFIRE_CELL: 0, "fountain_0_0": 1, "fountain_1_0": 1, "fountain_0_1": 0,
                 "fountain_1_1": 0}
# The properties of the tiles for the game (the bits of the red of the .prop of the chipsets : ska TileProperties
# bitMask, 1 is the water of the engine) : HIDEOUT, a wild Pokemon may come out of it (a bin, a bush, a trunk... : the
# scripts read it, Data/Battle/arrivals.miniska) ; GRASS, tall grass (the legs of a character in it are not drawn :
# TallGrassSystem)
HIDEOUT = 2
GRASS = 4
HIDEOUTS = {"bin", "barrel", "log seat", "bush"}
_STAMP_ROWS = {}


def tile_flags(key):
    """The properties of a tile (its key, or the name of an animated tile) for the game : bits (HIDEOUT, GRASS)."""
    if isinstance(key, str):
        return GRASS if key == TALL_CELL else 0
    if key[0] == "tree":
        style, row, c = key[1:]
        return HIDEOUT if any(part == "trunk" and part_row == row for _, part_row, part in TREE_SHAPES[style][1]) else 0
    return HIDEOUT if key[0] in HIDEOUTS else 0


def tile_base(key):
    """The base of a tile (its key, or the name of an animated tile) : FLAT, or how many blocks lower its object
    stands (a roof : the bottom row of its building ; a tree top : its trunk)."""
    if isinstance(key, str):
        return ANIMATED_BASE.get(key, FLAT)
    kind = key[0]
    if kind == "stamp":
        name, c, r = key[1:]
        if name not in _STAMP_ROWS:
            _STAMP_ROWS[name] = stamp_image(name, STAMP_MAKERS[name]).height // T
        return _STAMP_ROWS[name] - 1 - r
    if kind == "tree":
        style, row, c = key[1:]
        dy = next(dy for dy, part_row, _ in TREE_SHAPES[style][1] if part_row == row)
        trunk = next(dy for dy, _, part in TREE_SHAPES[style][1] if part == "trunk")
        return trunk - dy
    if kind == "room furniture":
        if key[1] == "H":
            return 0
        char, rw, th, extra, c, r = key[1:]
        return th - 1 - r
    return 0 if kind in STANDING else FLAT


def stamp_image(name, make):
    """The image of a structure in the current palette (drawn once per palette)."""
    key = (name, tuple(map(tuple, g.PAL["path"])), tuple(map(tuple, g.PAL["grass"])))
    if key not in _STAMPS:
        _STAMPS[key] = make()
    return _STAMPS[key]


def tree_part(style, row, col):
    """A tile of a tree (tilegen.tree_image), in the current palette."""
    key = (style, tuple(map(tuple, g.PAL["leaf"])), tuple(map(tuple, g.PAL["grass"])))
    if key not in _TREES:
        img = g.tree_image(style)
        _TREES[key] = {r: [img.crop((c * T, r * T, c * T + T, r * T + T)) for c in range(img.width // T)]
                       for r in range(img.height // T)}
    return _TREES[key][row][col]


# ---------------------------------------------------------------- the world

class Rng:
    """A seeded random sequence (the same plan for the same seed)."""

    def __init__(self, seed):
        self.seed, self.n = seed, 0

    def random(self):
        self.n += 1
        return h01(self.n, self.seed, 4242)

    def randint(self, a, b):
        return a + int(self.random() * (b - a + 1))

    def choice(self, items):
        return items[min(len(items) - 1, int(self.random() * len(items)))]


ROOFS = ["roofr", "roofb", "roofg", "roofo"]
STAMP_MAKERS = stamp_makers()
# The villages of each biome : their roofs, their names (a word and a form), the line of their sign
VILLAGE_STYLE = {
    "meadow": dict(roofs=ROOFS, words=["Trèfle", "Verdure", "Prairie", "Bourgeon", "Coquelicot"],
                   line="Un village paisible au milieu des prés."),
    "autumn": dict(roofs=["roofr", "roofo", "roofg"], words=["Châtaigne", "Noisette", "Feuillage", "Brume", "Rousse"],
                   line="Les feuilles y tombent toute l'année."),
    "snow": dict(roofs=["roofb", "roofr", "roofg"], words=["Givre", "Frimas", "Flocon", "Glace", "Neige"],
                 line="L'hiver ne finit jamais ici."),
    "desert": dict(roofs=["roofo", "roofr"], words=["Dune", "Sable", "Ocre", "Mirage", "Soleil"],
                   line="La chaleur ne fait pas peur à ses habitants."),
}
NAME_FORMS = ["Bourg-{}", "{}ville", "Village {}", "Hameau {}"]
VILLAGE_SPACING = 90                                # blocks between two villages at least
# Items to find : (item id of Data/Items, chance) ; how many per chunk at most
ITEMS = [(0, 0.5), (1, 0.25), (2, 0.25)]
ITEMS_PER_CHUNK = 2
BALL_SPRITE = 900
# Places between the villages, beside the roads (a short path to them) : a camp (a tent, a campfire, an item), a ruin
# (stone pillars around an item), a rest house (its bed heals the team). Their size in blocks ; blocks between them
POI_KINDS = ["camp", "ruins", "rest"]
POI_SIZE = {"camp": (6, 5), "ruins": (7, 7), "rest": (6, 6)}
POI_SPACING = 35                                   # Sprites/Charsets/pnj900.png (Levels/item.miniska)


class Village:
    """A village grown like the villages of Minecraft : roads grow from a small plaza, turn and branch at random, and
    the buildings line the roads, their doors on a road. Planned before the terrain : the land under it is levelled."""

    def __init__(self, cx, cy, rng, roofs=ROOFS, houses=(5, 8)):
        self.center = (cx, cy)
        self.road = set()
        self.buildings = []                          # (kind, roof, x0, y0, w, h, door column, door row)
        self.footprint = set()
        # The plaza : a rough disc
        for y in range(cy - 3, cy + 4):
            for x in range(cx - 3, cx + 4):
                if math.hypot(x - cx, y - cy) < 2.2 + rng.random() * 1.3:
                    self.road.add((x, y))
        # Roads : a few walks from the plaza, 2 blocks wide, each segment straight then a turn, sometimes a branch
        dirs = [(1, 0), (-1, 0), (0, 1), (0, -1)]
        starts = [(cx, cy, d) for d in dirs if rng.random() < 0.8] or [(cx, cy, (1, 0))]
        queue = [(x, y, d, 0) for x, y, d in starts]
        while queue:
            x, y, (dx, dy), depth = queue.pop(0)
            for _ in range(rng.randint(1, 3) if depth == 0 else rng.randint(1, 2)):
                for _ in range(rng.randint(4, 9)):
                    x, y = x + dx, y + dy
                    if abs(x - cx) > 17 or abs(y - cy) > 15:
                        break
                    self.road.add((x, y))
                    self.road.add((x + abs(dy), y + abs(dx)))       # the second lane
                    if depth < 2 and rng.random() < 0.07:
                        side = rng.choice([(dy, dx), (-dy, -dx)])
                        queue.append((x, y, side, depth + 1))
                if rng.random() < 0.5:
                    dx, dy = rng.choice([(dy, dx), (-dy, -dx)])  # a turn
        # Buildings : along the roads, set back from them, a short path to their door (the fronts face south).
        # North of an east-west road : the path goes down to it ; beside a north-south road : the path goes along
        # the front of the building to it
        kinds = [("center", None, 5, 5, 2)] + [("house", rng.choice(roofs), 4, 4, 2)
                                               for _ in range(rng.randint(*houses))]
        spots = []
        for rx, ry in self.road:
            if (rx, ry - 1) not in self.road:
                spots.append(("north", rx, ry))
            if (rx - 1, ry) not in self.road and (rx, ry - 1) in self.road:
                spots.append(("west", rx, ry))
            if (rx + 1, ry) not in self.road and (rx, ry - 1) in self.road:
                spots.append(("east", rx, ry))
        # (the ties broken by the side and the block : no order of a set in the result, the C++ port gives the same)
        spots.sort(key=lambda c: (h01(c[1] * 3 + len(c[0]), c[2], rng.seed + 17), c[0], c[1], c[2]))
        for kind, roof, w, h, door in kinds:
            for side, rx, ry in spots:
                gap = rng.randint(0, 2)
                if side == "north":
                    x0, y0 = rx - door, ry - gap - h
                    path = {(rx, y) for y in range(ry - gap, ry)}
                elif side == "west":
                    x0, y0 = rx - 1 - gap - w, ry - h
                    path = {(x, ry) for x in range(x0 + door, rx)}
                else:
                    x0, y0 = rx + 2 + gap, ry - h
                    path = {(x, ry) for x in range(rx + 1, x0 + door + 1)}
                body = {(x, y) for x in range(x0 - 1, x0 + w + 1) for y in range(y0, y0 + h)}
                if body & self.road or body & self.footprint or path & self.footprint or path & body:
                    continue
                if abs(x0 - cx) > 19 or abs(y0 - cy) > 17:
                    continue
                self.buildings.append((kind, roof, x0, y0, w, h, door, h - 1))
                self.footprint |= {(x, y) for x in range(x0 - 1, x0 + w + 1) for y in range(y0 - 1, y0 + h)}
                self.road |= path
                break
        # Gardens : flowers along some buildings
        self.flowers = set()
        for kind, roof, x0, y0, w, h, door, _ in self.buildings:
            if kind == "house" and rng.random() < 0.6:
                side = x0 - 1 if rng.random() < 0.5 else x0 + w
                for y in range(y0 + 1, y0 + h):
                    if (side, y) not in self.road:
                        self.flowers.add((side, y))
        # Yards : sand in front of the buildings (the row under them, but their path)
        self.yard = set()
        for kind, roof, x0, y0, w, h, door, _ in self.buildings:
            for x in range(x0, x0 + w):
                if (x, y0 + h) not in self.road and (x, y0 + h) not in self.footprint:
                    self.yard.add((x, y0 + h))
        # A pond near the plaza (half of the villages) : 4 x 3 water in a ring of paving
        self.pond, self.pond_ring = set(), set()
        if rng.random() < 0.6:
            taken = self.road | self.footprint | self.flowers | self.yard
            for _ in range(40):
                px, py = cx + rng.randint(-12, 8), cy + rng.randint(-10, 7)
                ring = {(x, y) for x in range(px - 1, px + 5) for y in range(py - 1, py + 4)}
                if ring & taken or any((x + dx, y + dy) in taken for x, y in ring for dx, dy in D8):
                    continue
                self.pond = {(x, y) for x in range(px, px + 4) for y in range(py, py + 3)}
                self.pond_ring = ring - self.pond
                break
        self.cells = self.road | self.footprint | self.flowers | self.yard | self.pond | self.pond_ring


class World:
    def __init__(self, seed, cw, ch):
        self.seed, self.cw, self.ch = seed, cw, ch
        self.W, self.H = cw * CHUNK, ch * CHUNK
        self.near_village = {}
        self.climate()
        self.town = self.village_site()
        self.plan_villages()
        self.near_village = self.distance_to(set().union(*(v.cells for v in self.villages)), 8)
        self.biome_field()
        self.plan_routes()
        self.layers = {l: [[None] * self.W for _ in range(self.H)] for l in ("", "M", "T")}
        self.walk = [[True] * self.W for _ in range(self.H)]
        self.taken = set()
        self.objects = {}                            # block -> height of what stands on it (light map)
        self.chip = Tiles()
        self.village_ground = set().union(*(v.yard | v.pond | v.pond_ring for v in self.villages))
        self.terrain = [[self.classify(x, y) for x in range(self.W)] for y in range(self.H)]
        for _ in range(8):                           # until every terrace has its stairs
            self.clean()
            if not self.place_stairs():
                break
        self.plan_pois()

    def climate(self):
        """The biome of each block from its climate (CLIMATE_SCALE, climate_biome), its borders warped (ragged at two
        scales) ; self.label. The biome of a chunk (its wild Pokemon) : the one most of its blocks have ; self.biome."""
        W, H = self.W, self.H
        ox, oy = h01(self.seed, 1, 301) * 500, h01(self.seed, 2, 301) * 500    # off the lattice of the noise

        def field(seed):
            return [[fbm((x + ox) / CLIMATE_SCALE, (y + oy) / CLIMATE_SCALE, seed, 3) for x in range(0, W, 4)]
                    for y in range(0, H, 4)]

        def ranks(grid):                             # value -> its rank over the world (0 .. 1)
            values = sorted(v for line in grid for v in line)
            return lambda v: bisect.bisect_left(values, v) / len(values)

        temp, hum, rel = field(self.seed + 300), field(self.seed + 310), field(self.seed + 320)
        rank_t, rank_h, rank_r = ranks(temp), ranks(hum), ranks(rel)
        # The relief : 0 flat .. 1 mountains (no warp : the mountains cross the biome borders)
        self.relief = [[smooth(max(0.0, min(1.0, (rank_r(fbm((x + ox) / CLIMATE_SCALE, (y + oy) / CLIMATE_SCALE,
                                                              self.seed + 320, 3)) - RELIEF_FROM)
                                             / (RELIEF_FULL - RELIEF_FROM))))
                        for x in range(W)] for y in range(H)]
        self.label = [[None] * W for _ in range(H)]
        for y in range(H):
            for x in range(W):
                wx = x + 12 * (fbm(x / 30.0, y / 30.0, self.seed + 404, 2) - 0.5) / 0.25 \
                    + 4 * (fbm(x / 9.0, y / 9.0, self.seed + 400, 3) - 0.5) / 0.25
                wy = y + 12 * (fbm(x / 30.0, y / 30.0, self.seed + 405, 2) - 0.5) / 0.25 \
                    + 4 * (fbm(x / 9.0, y / 9.0, self.seed + 401, 3) - 0.5) / 0.25
                t = fbm((wx + ox) / CLIMATE_SCALE, (wy + oy) / CLIMATE_SCALE, self.seed + 300, 3)
                h = fbm((wx + ox) / CLIMATE_SCALE, (wy + oy) / CLIMATE_SCALE, self.seed + 310, 3)
                self.label[y][x] = climate_biome(rank_t(t), rank_h(h))
        self.merge_small_biomes()
        self.biome = {}
        for cy in range(self.ch):
            for cx in range(self.cw):
                count = {}
                for y in range(cy * CHUNK, cy * CHUNK + CHUNK):
                    for x in range(cx * CHUNK, cx * CHUNK + CHUNK):
                        count[self.label[y][x]] = count.get(self.label[y][x], 0) + 1
                self.biome[(cx, cy)] = max(sorted(count), key=lambda b: count[b])

    def merge_small_biomes(self):
        """A region of a biome smaller than BIOME_MIN blocks takes the biome most of its neighbours have."""
        W, H = self.W, self.H
        for _ in range(3):
            seen = [[False] * W for _ in range(H)]
            changed = False
            for y0 in range(H):
                for x0 in range(W):
                    if seen[y0][x0]:
                        continue
                    b = self.label[y0][x0]
                    region, front, around = [(x0, y0)], [(x0, y0)], {}
                    seen[y0][x0] = True
                    while front:
                        x, y = front.pop()
                        for dx, dy in D8[:4]:
                            nx, ny = x + dx, y + dy
                            if not (0 <= nx < W and 0 <= ny < H):
                                continue
                            nb = self.label[ny][nx]
                            if nb != b:
                                around[nb] = around.get(nb, 0) + 1
                            elif not seen[ny][nx]:
                                seen[ny][nx] = True
                                region.append((nx, ny))
                                front.append((nx, ny))
                    if len(region) < BIOME_MIN and around:
                        to = max(sorted(around), key=lambda k: around[k])
                        for x, y in region:
                            self.label[y][x] = to
                        changed = True
            if not changed:
                break

    def village_site(self):
        """Where the village grows (and the game starts), as Minecraft chooses its spawn : on land, in the meadow,
        near the middle of the world. The places near the middle of a chunk (the start far from its edges) are scored :
        the share of meadow around (the village stays in the meadow anyway), minus the distance to the middle."""
        cx, cy = self.W // 2, self.H // 2
        best, best_score = (cx // CHUNK * CHUNK + CHUNK // 2, cy // CHUNK * CHUNK + CHUNK // 2), None
        for y in range(25, self.H - 25, 2):
            for x in range(25, self.W - 25, 2):
                if abs(x % CHUNK - CHUNK // 2) > 6 or abs(y % CHUNK - CHUNK // 2) > 6 or self.height(x, y) < 0.45:
                    continue
                box = [self.label[y + dy][x + dx] == "meadow" for dx in range(-20, 21, 4) for dy in range(-18, 19, 4)]
                score = sum(box) / len(box) - math.hypot(x - cx, y - cy) / (self.W + self.H) \
                    - 2 * self.relief[y][x]
                if best_score is None or score > best_score:
                    best, best_score = (x, y), score
        return best

    def plan_villages(self):
        """The villages : the first one where the game starts (village_site), then one in each biome while there is
        room (VILLAGE_SPACING apart, on land, not on the highest mountains (a village levels its ground), in that
        biome all around, near the middle of a chunk),
        each in the style of its biome (VILLAGE_STYLE). The blocks around a village keep its biome."""
        sites = [(self.town, "meadow")]
        for biome in ["desert", "snow", "autumn", "meadow", "desert", "snow", "autumn"]:
            best, best_score = None, None
            for cy in range(self.ch):
                for cx in range(self.cw):
                    for y in range(cy * CHUNK + CHUNK // 2 - 6, cy * CHUNK + CHUNK // 2 + 7, 2):
                        for x in range(cx * CHUNK + CHUNK // 2 - 6, cx * CHUNK + CHUNK // 2 + 7, 2):
                            if not (25 <= x < self.W - 25 and 25 <= y < self.H - 25):
                                continue
                            if self.relief[y][x] > 0.55 or self.height(x, y) < 0.45:
                                continue
                            if any(math.hypot(x - sx, y - sy) < VILLAGE_SPACING for (sx, sy), _ in sites):
                                continue
                            box = [self.label[y + dy][x + dx] == biome for dx in range(-20, 21, 5)
                                   for dy in range(-18, 19, 6)]
                            share = sum(box) / len(box)
                            if share < 0.7:
                                continue
                            score = share - self.relief[y][x] + 0.05 * h01(x, y, self.seed + 7)
                            if best_score is None or score > best_score:
                                best, best_score = (x, y), score
            if best:
                sites.append((best, biome))
        self.villages, used = [], set()
        for i, ((x, y), biome) in enumerate(sites):
            rng = Rng(self.seed + 1000 * i)
            style = VILLAGE_STYLE[biome]
            v = Village(x, y, rng, style["roofs"], (5, 8) if i == 0 else (3, 6))
            v.biome = biome
            words = [w for w in style["words"] if w not in used] or style["words"]
            word = rng.choice(words)
            used.add(word)
            v.name = rng.choice(NAME_FORMS).format(word)
            self.villages.append(v)
        self.village = self.villages[0]
        self.village_biome = {}
        for v in self.villages:
            for c in self.distance_to(v.cells, 12):
                self.village_biome[c] = v.biome

    def plan_routes(self):
        """Roads between the villages (a minimum spanning tree) : A* over the land, around the water and the
        buildings, avoiding the mountains (it cuts a pass when it has to : the land along a road stays low)."""
        W, H = self.W, self.H
        blocked = set().union(*(v.footprint for v in self.villages))
        cost = [[None] * W for _ in range(H)]
        for y in range(1, H - 1):
            for x in range(1, W - 1):
                if (x, y) in blocked or self.height(x, y) < 0.41:
                    continue
                # a noise : the roads wind, as old paths do
                cost[y][x] = 1.0 + 10.0 * self.relief[y][x] + (0.0 if (x, y) in self.near_village else 0.5) \
                    + 3.0 * fbm(x / 6.0, y / 6.0, self.seed + 500, 2)
        centers = [v.center for v in self.villages]
        edges, linked = [], {0}
        while len(linked) < len(centers):           # Prim : the shortest link from the linked villages each time
            a, b = min(((a, b) for a in linked for b in range(len(centers)) if b not in linked),
                       key=lambda e: math.dist(centers[e[0]], centers[e[1]]))
            edges.append((a, b))
            linked.add(b)
        self.routes, self.route_paths = set(), []
        for a, b in edges:
            path = self.astar(cost, centers[a], centers[b])
            if path is None:
                continue
            self.route_paths.append((a, b, path))
            for i, (x, y) in enumerate(path):
                self.routes.add((x, y))
                nx, ny = path[min(i + 1, len(path) - 1)]
                side = (x + 1, y) if nx == x else (x, y + 1)          # two blocks wide
                if cost[side[1]][side[0]] is not None:
                    self.routes.add(side)
        self.routes -= blocked
        self.near_route = self.distance_to(self.routes, 3)

    def astar(self, cost, start, goal):
        W, H = self.W, self.H
        best = {start: 0.0}
        came = {}
        heap = [(0.0, start)]
        while heap:
            _, cur = heapq.heappop(heap)
            if cur == goal:
                path = [cur]
                while cur in came:
                    cur = came[cur]
                    path.append(cur)
                return path[::-1]
            x, y = cur
            for dx, dy in D8[:4]:
                nx, ny = x + dx, y + dy
                if not (0 <= nx < W and 0 <= ny < H) or cost[ny][nx] is None:
                    continue
                g_ = best[cur] + cost[ny][nx]
                if g_ < best.get((nx, ny), 1e18):
                    best[(nx, ny)] = g_
                    came[(nx, ny)] = cur
                    heapq.heappush(heap, (g_ + abs(nx - goal[0]) + abs(ny - goal[1]), (nx, ny)))
        return None

    def biome_field(self):
        """The biomes of each block : its climate (self.label ; the village stays in the meadow), blurred into weights,
        so that two biomes blend over about 10 blocks. Each block gets the biomes of its 4 corners (self.pal), blended across its tiles."""
        W, H = self.W, self.H
        label = [[None] * W for _ in range(H)]
        for y in range(H):
            for x in range(W):
                label[y][x] = self.village_biome.get((x, y), self.label[y][x])
        r = 5
        self.weights = [[{} for _ in range(W)] for _ in range(H)]
        for b in BIOMES:
            grid = [[1.0 if label[y][x] == b else 0.0 for x in range(W)] for y in range(H)]
            for _ in range(2):                       # two box blurs : a smooth ramp
                rows = []
                for y in range(H):
                    acc, line = 0.0, []
                    pre = [0.0]
                    for x in range(W):
                        pre.append(pre[-1] + grid[y][x])
                    for x in range(W):
                        lo, hi = max(0, x - r), min(W, x + r + 1)
                        line.append((pre[hi] - pre[lo]) / (hi - lo))
                    rows.append(line)
                grid = [[0.0] * W for _ in range(H)]
                for x in range(W):
                    pre = [0.0]
                    for y in range(H):
                        pre.append(pre[-1] + rows[y][x])
                    for y in range(H):
                        lo, hi = max(0, y - r), min(H, y + r + 1)
                        grid[y][x] = (pre[hi] - pre[lo]) / (hi - lo)
            for y in range(H):
                for x in range(W):
                    if grid[y][x] > 0.001:
                        self.weights[y][x][b] = grid[y][x]
        # The corners : the mean of their 4 blocks, in MIX_STEPS (the largest remainders round up)
        corner = [[None] * (W + 1) for _ in range(H + 1)]
        for j in range(H + 1):
            for i in range(W + 1):
                acc = [0.0] * len(BIOMES)
                for dx in (-1, 0):
                    for dy in (-1, 0):
                        wts = self.weights[min(H - 1, max(0, j + dy))][min(W - 1, max(0, i + dx))]
                        for k, b in enumerate(BIOMES):
                            acc[k] += wts.get(b, 0.0)
                total = sum(acc)
                raw = [a / total * MIX_STEPS for a in acc]
                q = [int(v) for v in raw]
                for k in sorted(range(len(BIOMES)), key=lambda k: -(raw[k] - q[k]))[:MIX_STEPS - sum(q)]:
                    q[k] += 1
                corner[j][i] = tuple(q)
        self.pal = [[(corner[y][x], corner[y][x + 1], corner[y + 1][x], corner[y + 1][x + 1]) for x in range(W)]
                    for y in range(H)]

    def nature(self, x, y):
        """The vegetation of a block : the settings of its biomes, by their weights."""
        w = self.weights[y][x]
        total = sum(w.values())
        return {k: sum(NATURE[b][k] * v for b, v in w.items()) / total for k in NATURE["meadow"]}

    def tree_style(self, x, y):
        """The tree of a block : one of its biomes, at random by their weights."""
        w = sorted(self.weights[y][x].items())
        r = h01(x, y, self.seed + 403) * sum(v for _, v in w)
        style = g.BIOMES[w[-1][0]]["tree"]
        for b, v in w:
            r -= v
            if r <= 0:
                style = g.BIOMES[b]["tree"]
                break
        if style == "round" and h01(x, y, self.seed + 404) < 0.15:
            style = "pine"                           # a few pines among the broadleaf trees
        return style

    def clean(self):
        """Small patches read as noise : a terrace under 16 blocks goes down a level, sand or water under 5 take the
        land around ; a terrace one block wide goes down (a cliff needs two blocks)."""
        for _ in range(MAX_LEVEL + 2):
            changed = False
            for y in range(self.H):                  # one block wide : lower on both sides of an axis
                for x in range(self.W):
                    m, lv = self.terrain[y][x]
                    if lv > 0 and ((self.level(x, y - 1) < lv and self.level(x, y + 1) < lv)
                                   or (self.level(x - 1, y) < lv and self.level(x + 1, y) < lv)):
                        self.terrain[y][x] = (m, lv - 1)
                        changed = True
                    elif lv > 0 and self.level(x, y - 1) > lv and any(self.low_mask(x, y)[:4]):
                        # under a face (its foot) and a cliff itself : no room for both, it goes down
                        self.terrain[y][x] = (m, lv - 1)
                        changed = True
            seen = set()
            for y in range(self.H):
                for x in range(self.W):
                    if (x, y) in seen:
                        continue
                    kind = self.terrain[y][x]
                    patch, front = [(x, y)], [(x, y)]
                    seen.add((x, y))
                    while front:
                        a, b = front.pop()
                        for dx, dy in D8[:4]:
                            c = (a + dx, b + dy)
                            if 0 <= c[0] < self.W and 0 <= c[1] < self.H and c not in seen \
                                    and self.terrain[c[1]][c[0]] == kind:
                                seen.add(c)
                                patch.append(c)
                                front.append(c)
                    if any(c in self.village_ground for c in patch):
                        continue                     # the yards and ponds of the villages stay
                    if kind[0] == "grass" and kind[1] > 0 and len(patch) < 16:
                        to = ("grass", kind[1] - 1)
                    elif kind[0] in ("sand", "water") and len(patch) < 5:
                        to = ("grass", 0) if kind[0] != "water" else ("sand", 0)
                    else:
                        continue
                    for a, b in patch:
                        self.terrain[b][a] = to
                    changed = True
            if not changed:
                break

    def low_mask(self, x, y):
        lv = self.level(x, y)
        return mask8(lambda a, b: self.level(a, b) < lv, x, y) if lv > 0 else (False,) * 8

    def is_foot(self, x, y):
        """The block under a rock face : the lower part of the face (not on the water)."""
        return 0 <= x < self.W and 0 <= y < self.H and self.mat(x, y) != "water" \
            and self.level(x, y - 1) > self.level(x, y)

    def flat(self, x, y):
        """A block one walks on without a cliff in the way : on the map, not water, no cliff face or side, not the
        foot of a face."""
        return 0 <= x < self.W and 0 <= y < self.H and self.mat(x, y) != "water" \
            and not any(self.low_mask(x, y)[:4]) and not self.is_foot(x, y)

    def stair_ok(self, x, y):
        """Stairs in the south face of a terrace : the face only to the south, the same level left and right, flat
        ground above them, down through the foot of the face to flat ground."""
        lv = self.level(x, y)
        low = self.low_mask(x, y)
        return lv > 0 and low[2] and not low[0] and not low[1] and not low[3] and self.level(x - 1, y) == lv \
            and self.level(x + 1, y) == lv and self.flat(x, y - 1) and self.is_foot(x, y + 1) \
            and self.flat(x, y + 2)

    def poi_block_ok(self, x, y):
        """A block for a place or its path : on the map, away from the edges of the chunks (their teleports), low flat
        grass, not a road, not a village, no stairs."""
        return 0 <= x < self.W and 0 <= y < self.H and 1 < x % CHUNK < CHUNK - 2 and 1 < y % CHUNK < CHUNK - 2 \
            and self.terrain[y][x] == ("grass", 0) and self.flat(x, y) and (x, y) not in self.routes \
            and self.near_village.get((x, y), 99) > 6 and (x, y) not in self.stairs

    def plan_pois(self):
        """The places between the villages (POI_KINDS, in turn) : every 12 blocks of road, an area on one side of it,
        3 to 5 blocks away, all on low flat grass, POI_SPACING from the others and from the villages ; a path from the
        road to it. A rest house faces the road (its area north of a road going east or west)."""
        self.pois = []
        n = 0
        for a, b, path in self.route_paths:
            for i in range(10, len(path) - 10, 12):
                (x, y), (nx, ny) = path[i], path[i + 1]
                if any(math.dist((x, y), (p["x"], p["y"])) < POI_SPACING for p in self.pois) \
                        or any(math.dist((x, y), v.center) < POI_SPACING for v in self.villages):
                    continue
                found = None
                for kind in POI_KINDS[n % len(POI_KINDS):] + POI_KINDS[:n % len(POI_KINDS)]:
                    found = self.poi_area(kind, x, y, nx, ny)
                    if found:
                        break
                if not found:
                    continue
                ax, ay, spur = found
                w, h = POI_SIZE[kind]
                if kind == "rest" and (ax + 3, ay + h - 1) not in spur:   # the path goes on to the door
                    spur = spur + [(ax + 3, ay + h - 1)]
                for cx, cy in spur:
                    self.terrain[cy][cx] = ("path", 0)
                self.pois.append({"kind": kind, "x": ax, "y": ay, "w": w, "h": h, "spur": spur,
                                  "biome": self.label[ay][ax]})
                n += 1

    def poi_area(self, kind, x, y, nx, ny):
        """The area of a place beside the road block (x, y) (the road goes on to (nx, ny)) : (x, y, path) or None."""
        w, h = POI_SIZE[kind]
        sides = [(ny - y, nx - x), (y - ny, x - nx)]          # the two sides of the road
        if kind == "rest":                           # its door faces south : not south of the road
            sides = [d for d in sides if d != (0, 1)]
        for px, py in sides:
            for dist in (3, 4, 5):
                if px == 0:                          # north or south of the road
                    ax = x - w // 2
                    ay = y + dist if py > 0 else y - dist - h + 1
                elif kind == "rest":                 # beside the road : its last row level with the road
                    ay = y - h + 1
                    ax = x + dist if px > 0 else x - dist - w + 1
                else:
                    ay = y - h // 2
                    ax = x + dist if px > 0 else x - dist - w + 1
                area = [(ax + i, ay + j) for j in range(h) for i in range(w)]
                spur = [(x + px * k, y + py * k) for k in range(1, dist)]
                if (x + px, y + py) in self.routes:  # the road is two blocks wide
                    spur = spur[1:]
                if kind == "rest" and px != 0:       # on along the last row to the door
                    spur += [(ax + i, ay + h - 1) for i in (range(3, w) if px < 0 else range(0, 4))]
                if all(self.poi_block_ok(*c) for c in area + spur):
                    return ax, ay, spur
        return None

    def place_stairs(self):
        """Stairs : some along the long faces, and at least one for each terrace region, down to a lower one (every
        level can be reached). A region with no room for stairs goes down a level ; returns whether one did."""
        self.stairs = {(x, y) for y in range(self.H) for x in range(self.W)
                       if x % 9 == 4 and h01(x // 9, y, self.seed + 5) < 0.5 and self.stair_ok(x, y)}
        seen, lowered = set(), False
        for y0 in range(self.H):
            for x0 in range(self.W):
                lv = self.level(x0, y0)
                if lv == 0 or (x0, y0) in seen:
                    continue
                region, front = [(x0, y0)], [(x0, y0)]
                seen.add((x0, y0))
                while front:
                    a, b = front.pop()
                    for dx, dy in D8[:4]:
                        c = (a + dx, b + dy)
                        if 0 <= c[0] < self.W and 0 <= c[1] < self.H and c not in seen and self.level(*c) == lv:
                            seen.add(c)
                            region.append(c)
                            front.append(c)
                if not any(c in self.stairs for c in region):
                    ok = sorted(c for c in region if self.stair_ok(*c))
                    if ok:
                        self.stairs.add(ok[len(ok) // 2])
                    else:
                        for a, b in region:
                            self.terrain[b][a] = (self.terrain[b][a][0], lv - 1)
                        lowered = True
        return lowered

    def distance_to(self, cells, limit):
        """The distance (blocks, 4 neighbours) to the nearest of the cells, up to limit."""
        dist = {c: 0 for c in cells}
        front = list(cells)
        for d in range(1, limit + 1):
            nxt = []
            for x, y in front:
                for dx, dy in D8[:4]:
                    c = (x + dx, y + dy)
                    if c not in dist:
                        dist[c] = d
                        nxt.append(c)
            front = nxt
        return dist

    # Terrain ---------------------------------------------------------------------------------------------
    def height(self, x, y):
        # A broad landmass, smaller hills and lakes on it (features every 10 to 20 blocks)
        h = 0.6 * fbm(x / 30.0, y / 30.0, self.seed) + 0.4 * fbm(x / 11.0, y / 11.0, self.seed + 50, 3)
        # The biomes raise the land a little : the deserts are dry (fewer lakes), blended at their borders
        weights = getattr(self, "weights", None)
        if weights is not None:
            w = weights[y][x]
            h += sum(BIOME_LIFT[b] * v for b, v in w.items()) / sum(w.values())
        # The mountains raise the land (fewer lakes up there)
        relief = getattr(self, "relief", None)
        if relief is not None:
            h += 0.06 * relief[y][x]
        # An island : the sea all around the world
        border = min(x, y, self.W - 1 - x, self.H - 1 - y)
        h -= 0.45 * smooth(max(0.0, 1 - border / 10.0))
        # The village : levelled meadow, its edge following the village (not a circle)
        d = self.near_village.get((x, y))
        w = 0.0 if d is None else smooth(max(0.0, min(1.0, (7 - d) / 5)))
        return h * (1 - w) + 0.52 * w

    def classify(self, x, y):
        for v in self.villages:                      # the villages : paved streets, sand yards, ponds
            if (x, y) in v.road or (x, y) in v.pond_ring:
                return ("pave", 0)
            if (x, y) in v.pond:
                return ("water", 0)
            if (x, y) in v.yard:
                return ("sand", 0)
        if (x, y) in self.routes:
            return ("path", 0)
        h = self.height(x, y)
        if h < 0.40:
            return ("water", 0)
        if h < 0.425:
            return ("sand", 0)
        if self.in_town(x, y, 6) or self.near_route.get((x, y), 9) <= 2:
            return ("grass", 0)                      # the villages and the roads stay low
        # Plateaus : their own smooth noise, away from the shores and the village
        plateau = fbm(x / 15.0, y / 15.0, self.seed + 60, 3) > 0.6 and h > 0.47
        # Mountains : the height cut in terraces, as many as the relief allows, ragged by a small noise
        a = self.relief[y][x]
        m = (h - 0.43) * 14 * a + 1.2 * a * (fbm(x / 7.0, y / 7.0, self.seed + 330, 2) - 0.5)
        return ("grass", max(1 if plateau else 0, min(MAX_LEVEL, int(max(0.0, m)))))

    def mat(self, x, y):
        if not (0 <= x < self.W and 0 <= y < self.H):
            return "water"
        return self.terrain[y][x][0]

    def level(self, x, y):
        if not (0 <= x < self.W and 0 <= y < self.H):
            return 0
        return self.terrain[y][x][1]

    def in_town(self, x, y, r=3):
        d = self.near_village.get((x, y))
        return d is not None and d <= r

    def plain(self, x, y):
        """Grass with nothing around that changes its tile : trees, tall grass, flowers go there."""
        if self.mat(x, y) != "grass":
            return False
        lv = self.level(x, y)
        return all(self.mat(x + dx, y + dy) == "grass" and self.level(x + dx, y + dy) == lv for dx, dy in D8)

    # Building the layers -------------------------------------------------------------------------------
    def set(self, layer, x, y, cell):
        if 0 <= x < self.W and 0 <= y < self.H:
            self.layers[layer][y][x] = cell

    def build(self):
        self.ground()
        self.place_town()
        self.lone_buildings = []
        self.events = {}
        self.place_pois()
        for x, y in self.stairs:                     # nothing in the way of the stairs (trees, boulders)
            self.taken |= {(x, y - 2), (x, y - 1), (x, y + 2), (x, y + 3)}
        self.place_nature()
        self.place_signs()
        self.place_items()
        self.place_rooms()

    def ground(self):
        """The tiles of the terrain : grounds, cliffs and their feet, the water and its banks (tools/worldgen/tileroom.py
        uses it alone)."""
        chip = self.chip
        for y in range(self.H):
            for x in range(self.W):
                m, lv = self.terrain[y][x]
                low = mask8(lambda a, b: self.level(a, b) < lv, x, y) if lv > 0 else None
                if low and any(low):
                    stairs = (x, y) in self.stairs
                    foot = low[2] and self.is_foot(x, y + 1)
                    walk = stairs or not any(low[:4])
                    self.set("", x, y, chip.cell(("cliff", low, stairs, foot), walk))
                    self.walk[y][x] = walk
                    continue
                if m == "water":
                    self.set("", x, y, WATER_CELL)
                    self.walk[y][x] = False
                    land = lambda a, b: self.mat(a, b) != "water"
                    mk = mask8(land, x, y)
                    if any(mk):
                        over = max((self.mat(x + dx, y + dy) for (dx, dy), b in zip(D8, mk) if b),
                                   key=lambda k: PRIORITY[k])
                        over = "sand" if over == "path" else over
                        self.set("M", x, y, chip.cell(("bank", over, mk), True))
                    continue
                if self.is_foot(x, y):
                    stairs = (x, y - 1) in self.stairs
                    west, east = self.is_foot(x - 1, y), self.is_foot(x + 1, y)
                    self.set("", x, y, chip.cell(("foot", m, west, east, stairs), stairs))
                    self.walk[y][x] = stairs
                    self.objects[(x, y)] = 0.5
                    continue
                shade = self.level(x, y - 1) > lv
                higher = [self.mat(x + dx, y + dy) for dx, dy in D8 if self.mat(x + dx, y + dy) != "water"
                          and PRIORITY[self.mat(x + dx, y + dy)] > PRIORITY[m] and self.level(x + dx, y + dy) == lv]
                if higher:
                    over = max(higher, key=lambda k: PRIORITY[k])
                    mk = mask8(lambda a, b: self.mat(a, b) == over and self.level(a, b) == lv, x, y)
                    self.set("", x, y, chip.cell(("ground", m, over, mk, shade), True))
                    continue
                if m == "grass" and not shade:
                    r = h01(x, y, self.seed + 97)
                    if r < 0.012 and not self.in_town(x, y, 4):
                        self.set("", x, y, FLOWER_CELL)
                        continue
                    v = 0 if r < 0.5 else 1 if r < 0.75 else 3 if r < 0.92 else 2
                    self.set("", x, y, chip.cell(("grass", v), True))
                else:
                    self.set("", x, y, chip.cell(("ground", m, None, None, shade, 0), True))

    def stamp(self, name, x0, y0, door=None):
        """A structure : its first row over the characters, the other rows on the ground (blocked, but the door).
        Its image : STAMP_MAKERS (each biome draws it in its colours, on its ground)."""
        img = STAMP_MAKERS[name]()
        tw, th = img.width // T, img.height // T
        for r in range(th):
            for c in range(tw):
                if g.blank(img.crop((c * T, r * T, c * T + T, r * T + T))):
                    continue
                x, y = x0 + c, y0 + r
                key = ("stamp", name, c, r)
                self.taken.add((x, y))
                self.objects[(x, y)] = 1.3
                if r == 0:
                    self.set("T", x, y, self.chip.cell(key, True))
                else:
                    walk = door == (c, r)
                    self.set("", x, y, self.chip.cell(key, walk))
                    self.walk[y][x] = walk
                    self.set("M", x, y, None)

    def place_town(self):
        for v in self.villages:
            for kind, roof, x0, y0, w, h, dc, dr in v.buildings:
                name = f"{kind}_{v.biome}" if kind == "center" else f"house_{roof}_{v.biome}"
                self.stamp(name, x0, y0, door=(dc, dr))
            colour = FLOWERBED_COLOURS[int(h01(*v.center, 9) * len(FLOWERBED_COLOURS))]
            for x, y in v.flowers:                   # flower beds along some houses (they sway)
                if self.mat(x, y) == "grass" and (x, y) not in self.taken:
                    self.set("M", x, y, f"flowerbed_{colour}")
                    self.taken.add((x, y))
            self.taken |= v.cells
            self.decorate(v)
        self.taken |= self.routes

    def deco_ok(self, x, y):
        """A block for a decoration of a village : free, low flat ground (grass or sand), nothing on it."""
        return 0 <= x < self.W and 0 <= y < self.H and (x, y) not in self.taken and self.walk[y][x] \
            and self.terrain[y][x] in (("grass", 0), ("sand", 0)) and self.flat(x, y) \
            and self.layers["M"][y][x] is None and 1 < x % CHUNK < CHUNK - 2 and 1 < y % CHUNK < CHUNK - 2

    def decorate(self, v):
        """The details of a village, as in the towns of the old games : a painted emblem on the plaza, street lamps
        along the streets, a mailbox by each house door, bushes along the houses, a fenced vegetable garden, broadleaf
        trees around."""
        rng = Rng(self.seed + 77 + v.center[0])
        cx, cy = v.center
        if all((x, y) in v.road for x in (cx - 1, cx) for y in (cy - 1, cy)):
            fountain = rng.random() < 0.5            # a fountain on the plaza, else a painted emblem
            for c in range(2):
                for r in range(2):
                    x, y = cx - 1 + c, cy - 1 + r
                    if fountain:
                        self.set("M", x, y, f"fountain_{c}_{r}")
                        self.walk[y][x] = False
                        self.taken.add((x, y))
                        self.objects[(x, y)] = 0.4
                    else:
                        self.set("M", x, y, self.chip.cell(("emblem", c, r), True))
        lamps = []
        for x, y in sorted(v.road, key=lambda c: (h01(c[0], c[1], self.seed + 61), c[0], c[1])):
            if any(abs(x - a) + abs(y - b) < 7 for a, b in lamps) or len(lamps) > 8:
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                lx, ly = x + dx, y + dy
                if (lx, ly) not in v.road and self.deco_ok(lx, ly) and self.layers["T"][ly - 1][lx] is None \
                        and (lx, ly - 1) not in v.footprint:
                    self.lamp(lx, ly)
                    lamps.append((lx, ly))
                    break
        # Benches around the plaza, a bin by one of them
        benches = 0
        for _ in range(60):
            if benches >= 2:
                break
            bx, by = cx + rng.randint(-5, 5), cy + rng.randint(-4, 4)
            if (bx, by) in v.road or not self.deco_ok(bx, by) \
                    or not any((bx + dx, by + dy) in v.road for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                continue
            self.put_object("M", bx, by, ("bench",), 0.3)
            benches += 1
            if benches == 1 and self.deco_ok(bx + 1, by) and (bx + 1, by) not in v.road:
                self.put_object("M", bx + 1, by, ("bin",), 0.4)
        pot = FLOWERBED_COLOURS[int(h01(cx, cy, 31) * len(FLOWERBED_COLOURS))]
        for kind, roof, x0, y0, w, h, dc, dr in v.buildings:
            if kind == "house":                      # the mailbox, beside the path to the door ; a pot of flowers
                mx, my = x0 + dc + 1, y0 + h
                if self.deco_ok(mx, my):
                    self.set("M", mx, my, self.chip.cell(("mailbox",), False))
                    self.walk[my][mx] = False
                    self.taken.add((mx, my))
                if self.deco_ok(x0 + dc - 1, my):
                    self.put_object("M", x0 + dc - 1, my, ("flowerpot", pot), 0.4)
            if v.biome in ("desert", "autumn") and rng.random() < 0.6 and self.deco_ok(x0 + w, y0 + h - 1):
                self.put_object("M", x0 + w, y0 + h - 1, ("barrel",), 0.5)
            if rng.random() < 0.6:                   # bushes along a side
                side = x0 - 1 if rng.random() < 0.5 else x0 + w
                for y in range(y0 + 1, y0 + h):
                    if self.deco_ok(side, y):
                        self.set("M", side, y, self.chip.cell(("bush",), False))
                        self.walk[y][side] = False
                        self.taken.add((side, y))
                        self.objects[(side, y)] = 0.5
        # A vegetable garden : 4 x 2 rows of crops, fenced on its north and sides (open to the south), butterflies
        garden = None
        for _ in range(80):
            gx, gy = cx + rng.randint(-16, 12), cy + rng.randint(-14, 12)
            area = [(gx + i, gy + j) for j in range(3) for i in range(6)]
            if all(self.deco_ok(x, y) and self.terrain[y][x] == ("grass", 0) for x, y in area) \
                    and all(self.deco_ok(x, gy + 3) for x in range(gx + 1, gx + 5)):
                for i in range(6):
                    kind = "tl" if i == 0 else "tr" if i == 5 else None
                    cell = self.chip.cell(("fence", kind), False)
                    self.set("M", gx + i, gy, cell)
                    self.walk[gy][gx + i] = False
                for j in (1, 2):
                    for i in (0, 5):
                        self.set("M", gx + i, gy + j, self.chip.cell(("fence", "v"), False))
                        self.walk[gy + j][gx + i] = False
                    for i in range(1, 5):
                        self.set("M", gx + i, gy + j, self.chip.cell(("crops",), True))
                self.taken |= set(area)
                garden = (gx, gy)
                break
        # A well
        for _ in range(60):
            wx, wy = cx + rng.randint(-12, 12), cy + rng.randint(-10, 10)
            if all(self.deco_ok(wx + dx, wy + dy) for dx in (-1, 0, 1) for dy in (-1, 0, 1)):
                self.put_object("M", wx, wy, ("well",), 0.6)
                break
        # Butterflies over the grass, near the flowers and the garden
        near = sorted(v.flowers) + ([(garden[0] + 2, garden[1] + 3)] if garden else [])
        flown = 0
        for _ in range(60):
            if flown >= 3 or not near:
                break
            fx, fy = rng.choice(near)
            bx, by = fx + rng.randint(-3, 3), fy + rng.randint(-2, 3)
            if self.deco_ok(bx, by) and self.terrain[by][bx] == ("grass", 0) and self.plain(bx, by) \
                    and self.layers["T"][by][bx] is None:
                self.set("", bx, by, "butterfly")
                self.taken.add((bx, by))
                flown += 1
        # Water lilies on the pond
        lilies = [c for c in sorted(v.pond) if self.layers["M"][c[1]][c[0]] is None]
        for x, y in lilies[:2]:
            self.set("M", x, y, self.chip.cell(("lily",), False))
        # Broadleaf trees between the houses
        planted = 0
        for _ in range(80):
            if planted >= 5:
                break
            tx, ty = cx + rng.randint(-18, 15), cy + rng.randint(-15, 15)
            if self.near_village.get((tx, ty), 99) < 1 or not self.in_town(tx, ty, 4):
                continue
            style = "round" if g.BIOMES[v.biome]["tree"] == "round" else g.BIOMES[v.biome]["tree"]
            if all(self.deco_ok(tx + c, ty + dy) for c in range(TREE_SHAPES[style][0]) for dy in (-2, -1, 0, 1)):
                if self.tree_at(tx, ty, style):
                    planted += 1

    def free_cell(self, x, y):
        return (x, y) not in self.taken and self.plain(x, y) and self.layers[""][y][x] != FLOWER_CELL

    def tree_at(self, x, y, style=None):
        """A tree (TREE_SHAPES) whose canopy bottom is on row y, from column x : its top rows over the characters, its
        canopy, its trunk. Packed trees : the top of a tree may cover the trunk of the tree above it, as in the forests
        of the old games."""
        style = style or self.tree_style(x, y)
        w, parts = TREE_SHAPES[style]
        for c in range(w):
            for dy, row, kind in parts:
                cx, cy = x + c, y + dy
                if not (0 <= cx < self.W and 0 <= cy < self.H):
                    return False
                if kind == "top":
                    if self.layers["T"][cy][cx] is not None or not self.plain(cx, cy):
                        return False
                elif not self.free_cell(cx, cy):
                    return False
        for c in range(w):
            for dy, row, kind in parts:
                cx, cy = x + c, y + dy
                if kind == "top":
                    self.set("T", cx, cy, self.chip.cell(("tree", style, row, c), True))
                    self.objects[(cx, cy)] = max(self.objects.get((cx, cy), 0), 1.2)
                    continue
                cell = self.chip.cell(("tree", style, row, c), False)
                self.set("", cx, cy, cell)
                if kind == "canopy":
                    self.set("T", cx, cy, cell)
                self.taken.add((cx, cy))
                self.objects[(cx, cy)] = 1.6
                self.walk[cy][cx] = False
        return True

    def place_nature(self):
        # Forests : packed, every 2 blocks, the rows shifted by one (a wall of trees), their edges a bit ragged ;
        # groves : small packed clumps ; a few lone trees
        for wide, step in ((True, 3), (False, 2)):  # the broadleaf trees (3 wide), the others (2 wide)
          for y in range(2, self.H - 2, step):
            for x0 in range(0, self.W - step, step):
                x = x0 + (y // step) % step
                if self.in_town(x, y, 4) or (self.tree_style(x, y) == "round") != wide:
                    continue
                forest = fbm(x / 13.0, y / 13.0, self.seed + 1)
                grove = fbm(x / 4.0, y / 4.0, self.seed + 3)
                r = h01(x, y, self.seed + 12)
                p = self.nature(x, y)
                dense = (forest > p["forest"] and r < 0.95) or (forest > p["forest"] - 0.05 and r < 0.4) \
                    or (grove > p["grove"] and r < 0.9) or r < p["lone"]
                if dense:
                    self.tree_at(x, y)
        for y in range(self.H):
            for x in range(self.W):
                if not self.free_cell(x, y) or self.in_town(x, y, 2):
                    continue
                p = self.nature(x, y)
                if fbm(x / 6.0, y / 6.0, self.seed + 2) > p["tall"] and fbm(x / 13.0, y / 13.0, self.seed + 1) < 0.5:
                    self.tall_grass(x, y)
                elif h01(x, y, self.seed + 21) < p["boulder"]:
                    self.set("M", x, y, self.chip.cell(("boulder",), False))
                    self.walk[y][x] = False
                    self.objects[(x, y)] = 0.4

    def tall_grass(self, x, y):
        """Tall grass : its leaves on the middle layer (the legs of a character in it are not drawn : its flag GRASS)."""
        self.set("M", x, y, TALL_CELL)

    def lamp(self, x, y):
        """A street lamp : its post on the block (blocks the way), its lantern on the block above (over the characters)."""
        self.set("T", x, y - 1, "lamphead")
        self.set("M", x, y, self.chip.cell(("lamp", 1), False))
        self.walk[y][x] = False
        self.taken.add((x, y))
        self.objects[(x, y)] = 0.6

    def put_object(self, layer, x, y, key, height=0.4):
        """A blocking object on a block (a campfire, a log...)."""
        self.set(layer, x, y, key if key in ANIMATED else self.chip.cell(key, False))
        self.walk[y][x] = False
        self.taken.add((x, y))
        self.objects[(x, y)] = height

    def item_event(self, x, y, item):
        cx, cy = x // CHUNK, y // CHUNK
        lx, ly = x - cx * CHUNK, y - cy * CHUNK
        self.add_event(cx * CHUNK, cy * CHUNK,
                       f"0 Levels/item.miniska, place, {item}, {lx}, {ly}, item_w_{cx}_{cy}_{lx}_{ly}")
        self.taken.add((x, y))
        self.items.append((x, y, item))

    def place_pois(self):
        """The places planned by plan_pois : their structures, their items, the rest houses (their rooms later)."""
        self.items = []
        for p in self.pois:
            ax, ay, w, h = p["x"], p["y"], p["w"], p["h"]
            self.taken |= {(ax + i, ay + j) for j in range(h) for i in range(w)} | set(p["spur"])
            if p["kind"] == "camp":
                canvas = ["roofo", "roofg", "roofb", "roofr"][int(h01(ax, ay, 3) * 4)]
                self.stamp(f"tent_{canvas}", ax + 1, ay)
                self.put_object("M", ax + 3, ay + 3, CAMPFIRE_CELL, 0.3)
                self.put_object("M", ax + 2, ay + 3, ("log seat",), 0.3)
                self.put_object("M", ax + 4, ay + 3, ("log seat",), 0.3)
                self.item_event(ax + 4, ay + 1, 0 if h01(ax, ay, 9) < 0.6 else 2)
            elif p["kind"] == "ruins":
                for i, (px, py) in enumerate(((0, 0), (3, 0), (6, 0), (0, 4), (6, 4), (0, 2), (6, 2))):
                    broken = h01(ax + px, ay + py, 11) < 0.45
                    self.stamp(f"pillar_{broken}", ax + px, ay + py)
                for i in range(2):
                    bx, by = ax + 1 + int(h01(ax, ay, 20 + i) * 5), ay + 6
                    if (bx, by) != (ax + 3, ay + 6):
                        self.put_object("M", bx, by, ("boulder",))
                self.item_event(ax + 3, ay + 3, 1 if h01(ax, ay, 13) < 0.6 else 2)
            else:
                biome = p["biome"]
                roof = VILLAGE_STYLE[biome]["roofs"][int(h01(ax, ay, 5) * len(VILLAGE_STYLE[biome]["roofs"]))]
                self.stamp(f"house_{roof}_{biome}", ax + 1, ay + h - 5, door=(2, 3))
                self.lone_buildings.append(("rest", roof, ax + 1, ay + h - 5, 4, 4, 2, 3))

    def add_event(self, x, y, line):
        """A map event of the chunk of block (x, y) (line : its text after the block position)."""
        cx, cy = x // CHUNK, y // CHUNK
        self.events.setdefault((cx, cy), []).append(f"{x - cx * CHUNK} {y - cy * CHUNK} {line}")

    def place_signs(self):
        """A sign with the name of the village beside each road leaving it, a few blocks out."""
        for i, v in enumerate(self.villages):
            ends = [path if a == i else path[::-1] for a, b, path in self.route_paths if i in (a, b)]
            for path in ends:
                out = [c for c in path if c not in v.cells and self.near_village.get(c, 99) >= 2]
                if not out:
                    continue
                k = 0
                x, y = out[0]
                nx, ny = out[min(1, len(out) - 1)]
                for sx, sy in ((x + (ny - y), y + (nx - x)), (x - (ny - y), y - (nx - x)),
                               (x + (ny - y) * 2, y + (nx - x) * 2), (x - (ny - y) * 2, y - (nx - x) * 2)):
                    if 0 <= sx < self.W and 0 <= sy < self.H and (sx, sy) not in self.taken \
                            and self.walk[sy][sx] and self.layers["M"][sy][sx] is None and self.flat(sx, sy):
                        self.set("M", sx, sy, self.chip.cell(("sign",), False))
                        self.walk[sy][sx] = False
                        self.taken.add((sx, sy))
                        self.objects[(sx, sy)] = 0.4
                        self.add_event(sx, sy, f"1 Levels/sign.miniska, {v.name}¤{VILLAGE_STYLE[v.biome]['line']}")
                        break

    def reachable(self):
        """The blocks the player can walk to from the start (on land : the edges of the chunks are walked across)."""
        start = (self.town[0], self.town[1] + 1)
        seen = {start}
        front = [start]
        while front:
            x, y = front.pop()
            for dx, dy in D8[:4]:
                c = (x + dx, y + dy)
                if c not in seen and 0 <= c[0] < self.W and 0 <= c[1] < self.H and self.walk[c[1]][c[0]]:
                    seen.add(c)
                    front.append(c)
        return seen

    def place_items(self):
        """Items to find (Levels/item.miniska), ITEMS_PER_CHUNK at most : on open ground the player can walk to, far
        from the roads, rather on the mountains and in the forest clearings."""
        reach = self.reachable()
        for cy in range(self.ch):
            for cx in range(self.cw):
                cands = []
                for y in range(cy * CHUNK + 3, cy * CHUNK + CHUNK - 3):
                    for x in range(cx * CHUNK + 3, cx * CHUNK + CHUNK - 3):
                        if (x, y) not in reach or (x, y) in self.taken or self.in_town(x, y, 4) \
                                or self.near_route.get((x, y), 9) <= 3 or self.layers["M"][y][x] is not None:
                            continue
                        if not all(self.walk[y + dy][x + dx] for dx in (-1, 0, 1) for dy in (-1, 0, 1)):
                            continue
                        trees = sum(1 for dx in range(-3, 4) for dy in range(-3, 4)
                                    if self.objects.get((x + dx, y + dy), 0) >= 1.6)
                        cands.append((1.5 * self.level(x, y) + 0.15 * trees + h01(x, y, self.seed + 77), x, y))
                chosen = []
                for score, x, y in sorted(cands, reverse=True):
                    if len(chosen) >= ITEMS_PER_CHUNK:
                        break
                    if score < 1.2 or any(math.hypot(x - a, y - b) < 12 for a, b in chosen):
                        continue
                    chosen.append((x, y))
                for x, y in chosen:
                    r = h01(x, y, self.seed + 78)
                    item = ITEMS[-1][0]
                    for iid, chance in ITEMS:
                        if r < chance:
                            item = iid
                            break
                        r -= chance
                    lx, ly = x - cx * CHUNK, y - cy * CHUNK
                    flag = f"item_w_{cx}_{cy}_{lx}_{ly}"
                    self.add_event(cx * CHUNK, cy * CHUNK, f"0 Levels/item.miniska, place, {item}, {lx}, {ly}, {flag}")
                    self.taken.add((x, y))
                    self.items.append((x, y, item))

    def place_rooms(self):
        """A room for each building, from the templates of its kind : its door leads in, the exit mats lead out."""
        self.rooms = []
        self.door_events = {}
        buildings = [b for v in self.villages for b in v.buildings] + self.lone_buildings
        for i, (kind, roof, x0, y0, w, h, dc, dr) in enumerate(buildings):
            choices = rooms.templates(kind)
            tname, rows = choices[int(h01(i, self.seed, 31) * len(choices))]
            layers, walk, exits, counter, beds = rooms.build_room(self.chip, kind, rows, self.seed + i)
            # Centred in a map at least as large as the screen (the camera keeps a smaller map in a corner) ; the
            # empty blocks around are void : they block the player
            rw, rh = len(rows[0]), len(rows)
            mw, mh = max(rw, ROOM_MAP[0]), max(rh, ROOM_MAP[1])
            ox, oy = (mw - rw) // 2, (mh - rh) // 2
            pad = lambda grid: [[grid[y - oy][x - ox] if 0 <= x - ox < rw and 0 <= y - oy < rh else None
                                 for x in range(mw)] for y in range(mh)]
            layers = {l: pad(grid) for l, grid in layers.items()}
            walk = [[bool(walk[y - oy][x - ox]) if 0 <= x - ox < rw and 0 <= y - oy < rh else False
                     for x in range(mw)] for y in range(mh)]
            rows = ["#" * mw if not (oy <= y < oy + rh) else "#" * ox + rows[y - oy] + "#" * (mw - rw - ox)
                    for y in range(mh)]
            exits = [(x + ox, y + oy) for x, y in exits]
            counter = [(x + ox, y + oy) for x, y in counter]
            beds = [(x + ox, y + oy) for x, y in beds] if kind == "rest" else []
            name = f"w_room_{i}"
            door = (x0 + dc, y0 + dr)
            cx, cy = door[0] // CHUNK, door[1] // CHUNK
            lx, ly = door[0] - cx * CHUNK, door[1] - cy * CHUNK
            ex, ey = exits[0]
            self.door_events.setdefault((cx, cy), []).append(
                f"{lx} {ly} 2 Levels/teleport.miniska, {name}, {ex}, {ey - 1}")
            events = [f"{x} {y} 2 Levels/teleport.miniska, w_{cx}_{cy}, {lx}, {ly + 1}" for x, y in exits]
            events += [f"{x} {y} 1 Levels/pokescript.miniska" for x, y in counter]
            events += [f"{x} {y} 1 Levels/rest.miniska" for x, y in beds]
            self.rooms.append({"name": name, "template": tname, "rows": rows, "layers": layers, "walk": walk,
                               "events": events})

    # Light -----------------------------------------------------------------------------------------------
    def top(self, x, y):
        """How high a block stands : its ground (a plateau : 1) and what is on it."""
        if not (0 <= x < self.W and 0 <= y < self.H):
            return 0.0
        return self.level(x, y) + self.objects.get((x, y), 0.0)

    def corner_light(self, i, j):
        """The light at the corner (i, j) of the blocks (between the blocks i - 1, i and j - 1, j) : an RGB colour,
        multiplied with the map by the game (white : unchanged). Light from the top left, like the tiles."""
        ground = sum(self.level(i + dx, j + dy) for dx in (-1, 0) for dy in (-1, 0)) / 4
        # Occlusion : what stands higher around the corner
        ao = 0.0
        for dy in range(-3, 3):
            for dx in range(-3, 3):
                d = math.hypot(dx + 0.5, dy + 0.5)
                if d < 3.2:
                    ao += max(0.0, self.top(i + dx, j + dy) - ground) / (1 + d * d)
        # Cast shadow : something high up-left of the corner
        shadow = 0.0
        for k in range(1, 4):
            if self.top(i - k, j - k) - ground > 0.45 * k:
                shadow = max(shadow, 1.0 - (k - 1) * 0.3)
        cloud = fbm(i / 11.0, j / 11.0, self.seed + 80, 3) - 0.5
        # Subtle : the deepest shade keeps 82 % of the colour
        l = 0.99 - 0.06 * min(ao, 1.6) - 0.05 * shadow + 0.01 * ground + 0.05 * cloud
        l = max(0.82, min(1.0, l))
        t = (l - 0.82) / 0.18                      # 0 in the deepest shade, 1 in the full light
        return (int(255 * l * (0.96 + 0.04 * t)), int(255 * l * (0.98 + 0.02 * t)), int(255 * l * (1.0 - 0.02 * t)))

    def light_image(self, cx, cy):
        img = Image.new("RGB", (CHUNK + 1, CHUNK + 1))
        for j in range(CHUNK + 1):
            for i in range(CHUNK + 1):
                img.putpixel((i, j), self.corner_light(cx * CHUNK + i, cy * CHUNK + j))
        return img

    # Output ----------------------------------------------------------------------------------------------
    def walkable(self, x, y):
        return 0 <= x < self.W and 0 <= y < self.H and self.walk[y][x]

    def chunk_events(self, cx, cy):
        lines = []
        x0, y0 = cx * CHUNK, cy * CHUNK
        for i in range(CHUNK):
            # (edge block, the block of the neighbour where the player arrives, map, x, y in the neighbour)
            for (ex, ey), (dx, dy), (ncx, ncy), (ax, ay) in (
                    ((0, i), (-2, 0), (cx - 1, cy), (CHUNK - 2, i)),
                    ((CHUNK - 1, i), (2, 0), (cx + 1, cy), (1, i)),
                    ((i, 0), (0, -2), (cx, cy - 1), (i, CHUNK - 2)),
                    ((i, CHUNK - 1), (0, 2), (cx, cy + 1), (i, 1))):
                if not (0 <= ncx < self.cw and 0 <= ncy < self.ch):
                    continue
                wx, wy = x0 + ex, y0 + ey
                if self.walkable(wx, wy) and self.walkable(wx + dx, wy + dy):
                    lines.append(f"{ex} {ey} 2 Levels/teleport.miniska, w_{ncx}_{ncy}, {ax}, {ay}")
        return lines + self.door_events.get((cx, cy), []) + self.events.get((cx, cy), [])

    def write(self, root, world):
        """The world in Data/Worlds/<world> : a folder per map (chunks and rooms), Chipsets (a chipset per map). The
        game finds it through the save ([World] directory of its trainer.ini : write_save)."""
        corr = correspondance(f"{root}/Resources/Chipsets/corr.png")
        colour = {(x, y): corr.getpixel((x, y)) for x in range(corr.width) for y in range(corr.height)}
        levels = f"{root}/Data/Worlds/{world}"
        if os.path.exists(levels):                  # an earlier run
            shutil.rmtree(levels)
        chips = f"{levels}/Chipsets"
        os.makedirs(chips)
        scripts = f"{root}/Resources/Chipsets/tile_scripts"
        chip_path = f"./Data/Worlds/{world}/Chipsets"
        self.stats = {}
        g.use_biome("meadow")
        g.item_ball_charset().convert("RGB").save(f"{root}/Sprites/Charsets/pnj{BALL_SPRITE}.png")
        header = "x y declenchement(1:presser espace||2:sous le hero||0:automatique au chargement de la map) " \
                 "parametresSupplementaires"
        for cy in range(self.ch):
            for cx in range(self.cw):
                name = f"w_{cx}_{cy}"
                folder = f"{levels}/{name}"
                os.makedirs(folder)
                blocks = [(cx * CHUNK + x, cy * CHUNK + y) for y in range(CHUNK) for x in range(CHUNK)]
                used = {(self.layers[l][y][x], self.pal[y][x]) for l in ("", "M", "T") for x, y in blocks
                        if self.layers[l][y][x] is not None}
                chip = Chipset(self.chip, used)
                chip.save(chips, f"chipset_{name}", scripts)
                self.stats[name] = (len(used), chip.used)
                for l in ("", "M", "T"):
                    im = Image.new("RGB", (CHUNK, CHUNK))
                    for y in range(CHUNK):
                        for x in range(CHUNK):
                            wx, wy = cx * CHUNK + x, cy * CHUNK + y
                            c = self.layers[l][wy][wx]
                            im.putpixel((x, y), colour[chip.cells[(c, self.pal[wy][wx])]] if c is not None else EMPTY)
                    im.save(f"{folder}/{name}{l}.bmp")
                self.light_image(cx, cy).save(f"{folder}/{name}L.png")
                biome = self.biome[(cx, cy)]
                with open(f"{folder}/{name}.ini", "w") as f:
                    f.write(f"[Chipset]\nfile={chip_path}/chipset_{name}.png\n\n"
                            "[Bgm]\nfile=./Music/Greencity.mp3\nvolume=1\n")
                with open(f"{folder}/{name}E.txt", "w", encoding="utf-8") as f:
                    f.write("\n".join([header] + self.chunk_events(cx, cy)) + "\n")
                write_spawns(f"{folder}/Monsters", biome)
                open(f"{folder}/scripts.data", "w").close()
        # The rooms : one chipset for all (chipset_rooms)
        used = {(c, MEADOW) for room in self.rooms for grid in room["layers"].values() for line in grid for c in line
                if c is not None}
        room_chip = Chipset(self.chip, used)
        room_chip.save(chips, "chipset_rooms", scripts)
        self.stats["rooms"] = (len(used), room_chip.used)
        for room in self.rooms:
            name, rows = room["name"], room["rows"]
            w, h = len(rows[0]), len(rows)
            folder = f"{levels}/{name}"
            os.makedirs(folder)
            for l in ("", "M", "T"):
                im = Image.new("RGB", (w, h), EMPTY)
                grid = room["layers"].get(l)
                if grid:
                    for y in range(h):
                        for x in range(w):
                            if grid[y][x] is not None:
                                im.putpixel((x, y), colour[room_chip.cells[(grid[y][x], MEADOW)]])
                im.save(f"{folder}/{name}{l}.bmp")
            light = Image.new("RGB", (w + 1, h + 1))
            for j, line in enumerate(rooms.light(rows, room["walk"])):
                for i, c in enumerate(line):
                    light.putpixel((i, j), c)
            light.save(f"{folder}/{name}L.png")
            with open(f"{folder}/{name}.ini", "w") as f:
                f.write(f"[Chipset]\nfile={chip_path}/chipset_rooms.png\n\n"
                        "[Bgm]\nfile=./Music/Greencity.mp3\nvolume=1\n")
            with open(f"{folder}/{name}E.txt", "w", encoding="utf-8") as f:
                f.write("\n".join([header] + room["events"]) + "\n")
            open(f"{folder}/scripts.data", "w").close()

    def write_save(self, root, name, world):
        """A save that starts in the village of the world : Data/Saves/<name> (made from new_game when it does not
        exist ; new_game itself : the new game), its trainer.ini pointing to the world ([World] directory)."""
        cx, cy = self.town[0] // CHUNK, self.town[1] // CHUNK
        dest = f"{root}/Data/Saves/{name}"
        if name != "new_game":
            if os.path.exists(dest):
                shutil.rmtree(dest)
            shutil.copytree(f"{root}/Data/Saves/new_game", dest)
        path = f"{dest}/trainer.ini"
        lines = [l for l in open(path, encoding="utf-8").read().splitlines()]
        out, in_world = [], False
        for line in lines:
            if line.startswith("["):
                in_world = line == "[World]"
                if in_world:
                    continue
            if in_world:
                continue                             # the old [World] section : written again below
            if line.startswith("start_map_name="):
                line = f"start_map_name=w_{cx}_{cy}"
            elif line.startswith("start_posx="):
                line = f"start_posx={self.town[0] - cx * CHUNK}"
            elif line.startswith("start_posy="):
                line = f"start_posy={self.town[1] - cy * CHUNK + 1}"
            out.append(line)
        if name != "new_game":                       # the game generates the world of the save from its seed
            out += ["[World]", "generate=true", f"seed={self.seed}"]
        open(path, "w", encoding="utf-8").write("\n".join(out) + "\n")

    def preview(self, path, scale=1, light=True):
        """The whole world in one image, each block P = T * scale pixels (the tiles shrunk one by one)."""
        P = max(1, int(round(T * scale)))
        img = Image.new("RGBA", (self.W * P, self.H * P), (0, 0, 0, 255))
        cache = {}
        for l in ("", "M", "T"):
            for y in range(self.H):
                for x in range(self.W):
                    c = self.layers[l][y][x]
                    if c is not None:
                        key = (c, self.pal[y][x])
                        if key not in cache:
                            tile = self.chip.frames(*key)[0].convert("RGBA")
                            px = tile.load()
                            for j in range(T):
                                for i in range(T):
                                    if px[i, j][:3] == KEY:
                                        px[i, j] = (0, 0, 0, 0)
                            cache[key] = tile.resize((P, P), Image.LANCZOS) if P != T else tile
                        img.alpha_composite(cache[key], (x * P, y * P))
        if light:
            from PIL import ImageChops
            full = Image.new("RGB", (self.W + 1, self.H + 1))
            for j in range(self.H + 1):
                for i in range(self.W + 1):
                    full.putpixel((i, j), self.corner_light(i, j))
            big = full.resize(((self.W + 1) * P, (self.H + 1) * P), Image.BILINEAR)
            big = big.crop((P // 2, P // 2, P // 2 + self.W * P, P // 2 + self.H * P))
            img = ImageChops.multiply(img.convert("RGB"), big).convert("RGBA")
        img.convert("RGB").save(path)


def correspondance(path):
    """Resources/Chipsets/corr.png : a colour per cell of the chipsets (the colour of a block in the map images names
    its cell). Made CHIP_COLS x CHIP_ROWS here when it is smaller : the colours it has are kept, the new ones are all
    different (and never the white of an empty block)."""
    corr = Image.open(path).convert("RGB")
    if corr.size == (CHIP_COLS, CHIP_ROWS):
        return corr
    out = Image.new("RGB", (CHIP_COLS, CHIP_ROWS))
    seen = {EMPTY, (0, 0, 0)}
    for y in range(min(corr.height, CHIP_ROWS)):
        for x in range(min(corr.width, CHIP_COLS)):
            seen.add(corr.getpixel((x, y)))
            out.putpixel((x, y), corr.getpixel((x, y)))
    n = 0
    for y in range(CHIP_ROWS):
        for x in range(CHIP_COLS):
            if x < corr.width and y < corr.height:
                continue
            while True:
                n += 1
                c = (int(h01(n, 1, 99) * 256), int(h01(n, 2, 99) * 256), int(h01(n, 3, 99) * 256))
                if c not in seen:
                    break
            seen.add(c)
            out.putpixel((x, y), c)
    out.save(path)
    return out


def write_spawns(folder, biome):
    """The wild Pokemon of a map (<map folder>/Monsters/<i>.ini, read by the game : MobSpawningSystem)."""
    os.makedirs(folder)
    for i, (pid, lo, hi, path, chance, excluded) in enumerate(SPAWNS[biome]):
        with open(f"{folder}/{i}.ini", "w") as f:
            f.write(f"[Data]\nid={pid}\nlevel_min={lo}\nlevel_max={hi}\npath_type={path}\n\n"
                    f"[Spawn]\nprobability_percent={chance}\n")
            for k, block in enumerate(excluded):
                f.write(f"on_blockid_{k}={block}\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--chunks", default="8x8")
    ap.add_argument("--root", default=".")
    ap.add_argument("--world", help="the folder of the world in Data/Worlds (default : seed<N>)")
    ap.add_argument("--save", action="append", default=[],
                    help="a save starting in this world (Data/Saves/<NAME> ; new_game : the new game) ; repeatable")
    ap.add_argument("--preview")
    a = ap.parse_args()
    cw, ch = (int(v) for v in a.chunks.split("x"))
    world = World(a.seed, cw, ch)
    world.build()
    name = a.world or f"seed{a.seed}"
    world.write(a.root, name)
    for save in a.save:
        world.write_save(a.root, save, name)
    if a.preview:
        world.preview(a.preview, 0.25)
    print("villages : " + ", ".join(f"{v.name} ({v.biome}, w_{v.center[0] // CHUNK}_{v.center[1] // CHUNK})"
                                    for v in world.villages)
          + f" ; {len(world.route_paths)} roads, {len(world.items)} items, places : "
          + ", ".join(f"{p['kind']} (w_{p['x'] // CHUNK}_{p['y'] // CHUNK} {p['x'] % CHUNK},{p['y'] % CHUNK})"
                      for p in world.pois))
    biomes = " ".join(f"w_{cx}_{cy}={b}" for (cx, cy), b in sorted(world.biome.items(), key=lambda i: i[0][::-1]))
    print(f"world {a.seed} : {cw} x {ch} chunks of {CHUNK} x {CHUNK} ({biomes}), {len(world.rooms)} rooms "
          f"({', '.join(r['template'] for r in world.rooms)}), {len(world.chip.ids)} tiles ; cells of the chipsets : "
          + ", ".join(f"{name} {cells}" for name, (_, cells) in world.stats.items()) + f" (of {CHIP_COLS * CHIP_ROWS})")


if __name__ == "__main__":
    main()
