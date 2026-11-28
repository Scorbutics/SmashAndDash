#!/usr/bin/env python3
"""infinite : the reference of the infinite world (a continuous world, generated region by region as the player walks).

Every value is a pure function of the block (x, y, any integers) and the seed : no stage looks at the whole world, so a
region generated alone is the same as the same region generated with its neighbours, in any order. The stages of
worldgen.py that looked at the whole world are replaced, one by one :

  climate     the noises are ranked by fixed tables (RANK3, RANK4 : their distribution measured once, it does not
              depend on the seed) instead of their ranks over the world ; the small patches of biome are smoothed by a
              local majority filter (BIOME_FILTER) instead of a flood fill over the world
  ocean       a slow continentalness noise : the land ends at coasts, the sea has shallows, sand bars, reefs and
              chains of islands (instead of the sea all around one island)
  terrain     the height, the shores, the terraces of the mountains and the plateaus (as worldgen.classify)
  villages    one site at most in each cell of a lattice (VILLAGE_CELL blocks), jittered, kept on flat land in one
              biome (instead of the best sites over the world) : a village and everything it changes (its levelled
              ground, its biome all around) stays inside its cell ; the first village : the nearest meadow one to
              the origin (a search over the cells around it)
  roads       each village to its nearest villages (up to ROAD_CELLS cells away), each road an A* inside a box around
              its two villages, around the villages there (instead of a spanning tree and an A* over the world) : a
              road depends only on the cells of its box, the same whichever region asks for it
  terraces    worldgen.clean and worldgen.place_stairs (flood fills over the world, until nothing changes) with a
              bounded reach : CLEAN_PASSES passes, each a block changed from its neighbours as they were (not as the
              scan left them) ; a terrace is closed when it fits in TERRACE_OPEN blocks (it gets its stairs, or goes
              down a level), open beyond (its stairs along its long faces, as worldgen) ; a region is computed
              inside a window of APRON blocks around it, wider than what reaches its blocks
  tiles       a region built by the stages of worldgen (worldgen.World.build : the grounds, the cliffs, the villages
              decorated, the trees, the places beside the roads, the signs, the items, the rooms) in a window around it
              (RegionWorld : its arrays from the blocks of the world, X0, Y0), its chunks and rooms written ; the
              stages that looked at the whole world made local : the places (kept by priority among their
              neighbours), the items (on walkable patches big enough instead of reached from the start), the rooms
              (named by their door : w_room_<x>_<y>), the tiles (their ids : their keys, StableTiles)

The blocks of the world are the blocks of the game : the chunks 0 .. WORLD_CHUNKS - 1, the start at its centre.

  tools/worldgen/infinite.py preview [--seed N] [--regions R] [--at RX RY] [out.png]
      the regions around (RX, RY) generated one by one and stitched (a pixel a block) : the shape of the world
  tools/worldgen/infinite.py map [--seed N] [--regions R] [--step S] [--at RX RY] [out.png]
      a coarse map (a pixel every S blocks), the villages and the roads drawn over it
  tools/worldgen/infinite.py build OUT [--seed N] [--at RX RY]   a region built (its tiles, villages, trees, places,
      items, rooms) and written in OUT : a folder per map (w_<cx>_<cy>, w_room_<x>_<y>), Chipsets
  tools/worldgen/infinite.py tiles OUT [--seed N]   the region of the first village built in two windows : the same
  tools/worldgen/infinite.py check [--seed N]     a region alone equals the same region inside a bigger window
"""
import argparse
import filecmp
import heapq
import math
import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import worldgen as w  # noqa: E402
from worldgen import fbm, smooth  # noqa: E402

# A region : the unit of generation (8 x 8 chunks of 40 blocks)
REGION_CHUNKS = 8
REGION = REGION_CHUNKS * w.CHUNK

# The distribution of fbm (3 and 4 octaves) : its value at each 1/32 of its cumulative distribution, over many seeds and
# positions (it does not depend on the seed) : rank(v) is the share of the world under v, as the ranks over a world were
RANK3 = [0.033, 0.2443, 0.2831, 0.3106, 0.3322, 0.3505, 0.3669, 0.3824, 0.3973, 0.4116, 0.4246, 0.4372, 0.4498, 0.4623,
         0.4748, 0.4867, 0.4989, 0.5109, 0.5226, 0.5347, 0.5472, 0.56, 0.5732, 0.5865, 0.6006, 0.6148, 0.6302, 0.6466,
         0.6656, 0.6874, 0.7144, 0.7532, 0.9573]
RANK4 = [0.0539, 0.2602, 0.2967, 0.3222, 0.3425, 0.3609, 0.3762, 0.3904, 0.4044, 0.4173, 0.4295, 0.4414, 0.4534, 0.4654,
         0.4769, 0.4885, 0.4997, 0.5111, 0.5227, 0.5341, 0.5458, 0.5576, 0.5697, 0.5823, 0.5953, 0.6091, 0.6236, 0.6398,
         0.6573, 0.6785, 0.7032, 0.7399, 0.9452]


def rank(table, v):
    """The share of the world under the value v (0 .. 1), from a table of the distribution."""
    if v <= table[0]:
        return 0.0
    if v >= table[-1]:
        return 1.0
    lo, hi = 0, len(table) - 1
    while hi - lo > 1:
        mid = (lo + hi) // 2
        if table[mid] <= v:
            lo = mid
        else:
            hi = mid
    return (lo + (v - table[lo]) / (table[hi] - table[lo])) / (len(table) - 1)


# The continents : a slow noise (CONTINENT_SCALE blocks), ranked ; under OCEAN the open sea, the land from OCEAN + COAST
CONTINENT_SCALE = 520.0
OCEAN, COAST = 0.34, 0.1
# The biomes are smoothed : the majority of the climate around a block, sampled every BIOME_STEP blocks within
# BIOME_FILTER blocks (local : a region alone and inside a bigger window get the same)
BIOME_STEP, BIOME_FILTER = 4, 8


# The biome of a climate (worldgen.climate_biome with narrower extremes : over an endless world the temperate biomes
# lead, the snow and the desert are the coldest and the hottest 22 %)
COLD, HOT, WET = 0.22, 0.78, 0.55

# The villages : a cell of the lattice is VILLAGE_CELL blocks, its site at least VILLAGE_MARGIN from its edges (a village
# reaches 26 blocks from its centre, its ground levelled 8 more, its biome kept 12 more : all inside its cell) ; a site
# is kept on land (VILLAGE_LAND), flat enough, in one biome (VILLAGE_SHARE of the blocks around), VILLAGE_CHANCE of them
VILLAGE_CELL, VILLAGE_MARGIN = 144, 40
VILLAGE_LAND, VILLAGE_CHANCE, VILLAGE_SHARE = 0.7, 0.85, 0.6
VILLAGE_TRIES = 3                                  # the sites tried in a cell : 3 x 3, jittered, the best kept
VILLAGE_LEVEL, VILLAGE_BIOME = 8, 12
START_SEARCH = 12                                  # cells around the centre searched for the first village
# The blocks of the world are the blocks of the game : the chunks 0 .. WORLD_CHUNKS - 1 (no negative index), the world
# starts at its centre CENTER (far from its edges, near enough to 0 for the precision of the floats)
WORLD_CHUNKS = 512
CENTER = WORLD_CHUNKS // 2 * w.CHUNK
# The roads : each village to its ROAD_NEAREST nearest villages within ROAD_CELLS cells and ROAD_REACH blocks (a road
# when either end chooses the other) ; an A* inside the box of its two villages and ROAD_MARGIN blocks around (dearer
# within ROAD_EDGE blocks of its edge : no road along it ; the box and 3 blocks stay inside the cells of its villages)
ROAD_NEAREST, ROAD_CELLS, ROAD_REACH, ROAD_MARGIN, ROAD_EDGE = 2, 2, 300, 36, 12


def cell_seed(seed, i, j):
    """The seed of a cell of the lattice (a 31 bits hash : the C++ port computes the same)."""
    n = (i * 73856093 ^ j * 19349663 ^ seed * 83492791) & 0xFFFFFFFF
    n = (n ^ (n >> 15)) * 2246822519 & 0xFFFFFFFF
    return (n ^ (n >> 13)) & 0x7FFFFFFF


def climate_biome(t, h):
    if t < COLD:
        return "snow"
    if t > HOT:
        return "desert"
    return "autumn" if h > WET else "meadow"


class Infinite:
    """The infinite world of a seed : every function takes the blocks of the world (any integers)."""

    def __init__(self, seed):
        self.seed = seed
        # Off the lattice of the noise (as worldgen)
        self.ox, self.oy = w.h01(seed, 1, 301) * 500, w.h01(seed, 2, 301) * 500
        self._climate = {}
        self._base = {}
        self._villages = {}        # cell -> Village or None
        self._roads = {}           # (cell, cell) -> set of the blocks of the road, or None
        self._paths = {}           # (cell, cell) -> the blocks of its middle, from the first cell, or None
        self._cell_roads = {}      # cell -> the blocks of the roads through it
        self._nearest = {}
        self._start = None

    # ---------------------------------------------------------------- climate

    def continent(self, x, y):
        """0 the open sea .. 1 the land (the coast between)."""
        c = rank(RANK4, fbm((x + self.ox) / CONTINENT_SCALE, (y + self.oy) / CONTINENT_SCALE, self.seed + 500, 4))
        return smooth(max(0.0, min(1.0, (c - OCEAN) / COAST)))

    def relief(self, x, y):
        """0 flat .. 1 mountains (no warp : the mountains cross the biome borders)."""
        r = rank(RANK3, fbm((x + self.ox) / w.CLIMATE_SCALE, (y + self.oy) / w.CLIMATE_SCALE, self.seed + 320, 3))
        return smooth(max(0.0, min(1.0, (r - w.RELIEF_FROM) / (w.RELIEF_FULL - w.RELIEF_FROM))))

    def climate_at(self, x, y):
        """The biome of the climate at a block, before the smoothing : its borders warped (ragged at two scales)."""
        key = (x, y)
        if key not in self._climate:
            s = self.seed
            wx = x + 12 * (fbm(x / 30.0, y / 30.0, s + 404, 2) - 0.5) / 0.25 + 4 * (fbm(x / 9.0, y / 9.0, s + 400, 3) - 0.5) / 0.25
            wy = y + 12 * (fbm(x / 30.0, y / 30.0, s + 405, 2) - 0.5) / 0.25 + 4 * (fbm(x / 9.0, y / 9.0, s + 401, 3) - 0.5) / 0.25
            t = rank(RANK3, fbm((wx + self.ox) / w.CLIMATE_SCALE, (wy + self.oy) / w.CLIMATE_SCALE, s + 300, 3))
            h = rank(RANK3, fbm((wx + self.ox) / w.CLIMATE_SCALE, (wy + self.oy) / w.CLIMATE_SCALE, s + 310, 3))
            self._climate[key] = climate_biome(t, h)
        return self._climate[key]

    def biome(self, x, y):
        """The biome of a block : the biome of the village of its cell near it, else its base biome."""
        v = self.village_of(x, y)
        if v is not None and (x, y) in v.near_biome:
            return v.biome
        return self.base_biome(x, y)

    def base_biome(self, x, y):
        """The biome of a block before the villages : the majority of the climate sampled around it (the grid of BIOME_STEP blocks within
        BIOME_FILTER), a tie : the climate of the block. The ocean is not a biome of the colours : its water and its
        sand take the colours of the land biome there (the ocean is in ocean())."""
        gx, gy = x // BIOME_STEP * BIOME_STEP, y // BIOME_STEP * BIOME_STEP
        if (gx, gy) in self._base:
            return self._base[(gx, gy)]
        count = {}
        r = BIOME_FILTER // BIOME_STEP
        for j in range(-r, r + 1):
            for i in range(-r, r + 1):
                if i * i + j * j > r * r:
                    continue
                b = self.climate_at(gx + i * BIOME_STEP, gy + j * BIOME_STEP)
                count[b] = count.get(b, 0) + 1
        best = max(count.values())
        own = self.climate_at(gx, gy)
        b = own if count.get(own) == best else min(b for b, n in count.items() if n == best)
        self._base[(gx, gy)] = b
        return b

    # ---------------------------------------------------------------- the ocean, the height

    def height(self, x, y):
        """The height of a block : the land height, levelled around the village of its cell (worldgen.height : its
        edge follows the village)."""
        h = self.land_height(x, y)
        v = self.village_of(x, y)
        d = None if v is None else v.near.get((x, y))
        if d is None:
            return h
        k = smooth(max(0.0, min(1.0, (7 - d) / 5)))
        return h * (1 - k) + 0.52 * k

    def land_height(self, x, y):
        """The height of a block before the villages (worldgen.height, the island gone) : the land falls into the
        sea along the coasts ; in the sea, chains of islands where a noise rises."""
        s = self.seed
        h = 0.6 * fbm(x / 30.0, y / 30.0, s) + 0.4 * fbm(x / 11.0, y / 11.0, s + 50, 3)
        h += w.BIOME_LIFT[self.base_biome(x, y)]
        h += 0.06 * self.relief(x, y)
        land = self.continent(x, y)
        sea = 1.0 - land
        h -= 0.42 * sea
        # Islands : a ragged noise over the sea, chains of them
        isles = fbm(x / 46.0, y / 46.0, s + 520, 3)
        if isles > 0.6:
            h += 0.55 * sea * smooth(min(1.0, (isles - 0.6) / 0.18))
        return h

    def ocean(self, x, y):
        """The kind of sea of a block : None (land, or a lake : its water is still), "shallows", "deep", "reef"
        (rocks in the shallows), "bar" (a sand bar : walked on, in the sea)."""
        land = self.continent(x, y)
        if land > 0.6:
            return None
        h = self.height(x, y)
        if 0.40 <= h < 0.415 and land < 0.3:
            return "bar"
        if h >= 0.40:
            return None
        if h < 0.27:
            return "deep"
        if h > 0.34 and fbm(x / 5.0, y / 5.0, self.seed + 530, 2) > 0.66:
            return "reef"
        return "shallows"

    # ---------------------------------------------------------------- the villages

    def site(self, i, j):
        """The site of the cell (i, j) of the lattice and its biome, or None : a block jittered inside the cell, kept
        on land, flat, in one biome, VILLAGE_CHANCE of them (each test a pure function of the cell)."""
        cs = cell_seed(self.seed, i, j)
        if w.h01(cs, 0, 77) >= VILLAGE_CHANCE:
            return None
        span = (VILLAGE_CELL - 2 * VILLAGE_MARGIN) // VILLAGE_TRIES
        best, best_score = None, None
        for t in range(VILLAGE_TRIES * VILLAGE_TRIES):
            x = i * VILLAGE_CELL + VILLAGE_MARGIN + t % VILLAGE_TRIES * span + int(w.h01(cs, 2 * t + 1, 77) * span)
            y = j * VILLAGE_CELL + VILLAGE_MARGIN + t // VILLAGE_TRIES * span + int(w.h01(cs, 2 * t + 2, 77) * span)
            relief = self.relief(x, y)
            if self.continent(x, y) < VILLAGE_LAND or relief > 0.55 or self.land_height(x, y) < 0.45:
                continue
            biome = self.base_biome(x, y)
            box = [self.base_biome(x + dx, y + dy) == biome for dx in range(-20, 21, 5) for dy in range(-18, 19, 6)]
            share = sum(box) / len(box)
            if share < VILLAGE_SHARE:
                continue
            score = share - relief + 0.05 * w.h01(x, y, self.seed + 7)
            if best_score is None or score > best_score:
                best, best_score = ((x, y), biome), score
        return best

    def start(self):
        """The cell of the first village (the game starts there) : of the meadow sites, the nearest to the centre
        of the world (the cells searched ring by ring, then the next ring : a site there can be nearer), else of any
        biome."""
        if self._start is None:
            found = None
            c = CENTER // VILLAGE_CELL
            for want in ("meadow", None):
                best = None
                for r in range(START_SEARCH + 1):
                    for j in range(c - r, c + r + 1):
                        for i in range(c - r, c + r + 1):
                            if max(abs(i - c), abs(j - c)) != r:
                                continue
                            got = self.site(i, j)
                            if got is None or (want and got[1] != want):
                                continue
                            (x, y), _ = got
                            key = ((x - CENTER) ** 2 + (y - CENTER) ** 2, j, i)
                            if best is None or key < best[0]:
                                best = (key, (i, j))
                    if best is not None and r > max(abs(best[1][0] - c), abs(best[1][1] - c)):
                        break
                if best is not None:
                    found = best[1]
                    break
            self._start = found or (c, c)
        return self._start

    def village(self, i, j):
        """The village of the cell (i, j), or None : grown as in worldgen (Village) with the rng of its cell, in the
        style of its biome ; the blocks near it (near : the distance up to VILLAGE_LEVEL, near_biome : up to
        VILLAGE_BIOME, 4 neighbours)."""
        key = (i, j)
        if key not in self._villages:
            got = self.site(i, j)
            v = None
            if got is not None:
                (x, y), biome = got
                first = (i, j) == self.start()
                rng = w.Rng(cell_seed(self.seed + 1000, i, j))
                style = w.VILLAGE_STYLE[biome]
                v = w.Village(x, y, rng, style["roofs"], (5, 8) if first else (3, 6))
                v.biome, v.first, v.cell = biome, first, (i, j)
                v.name = rng.choice(w.NAME_FORMS).format(rng.choice(style["words"]))
                dist = self.distance(v.cells, VILLAGE_BIOME)
                v.near = {c: d for c, d in dist.items() if d <= VILLAGE_LEVEL}
                v.near_biome = dist
            self._villages[key] = v
        return self._villages[key]

    def village_of(self, x, y):
        """The village of the cell of a block (the only one that can change it)."""
        return self.village(x // VILLAGE_CELL, y // VILLAGE_CELL)

    @staticmethod
    def distance(cells, limit):
        """The distance (blocks, 4 neighbours) to the nearest of the cells, up to limit (worldgen.distance_to ; the
        fronts sorted : no order of a set in the result)."""
        dist = {c: 0 for c in cells}
        front = sorted(cells)
        for d in range(1, limit + 1):
            nxt = []
            for x, y in front:
                for dx, dy in w.D8[:4]:
                    c = (x + dx, y + dy)
                    if c not in dist:
                        dist[c] = d
                        nxt.append(c)
            front = nxt
        return dist

    # ---------------------------------------------------------------- the roads

    def nearest(self, i, j):
        """The ROAD_NEAREST nearest villages of the village of the cell (i, j) (their cells), within ROAD_CELLS cells
        and ROAD_REACH blocks (the ties broken by the cell)."""
        if (i, j) in self._nearest:
            return self._nearest[(i, j)]
        v = self.village(i, j)
        if v is None:
            return []
        near = []
        for sj in range(j - ROAD_CELLS, j + ROAD_CELLS + 1):
            for si in range(i - ROAD_CELLS, i + ROAD_CELLS + 1):
                o = self.village(si, sj) if (si, sj) != (i, j) else None
                if o is None:
                    continue
                d = (o.center[0] - v.center[0]) ** 2 + (o.center[1] - v.center[1]) ** 2
                if d <= ROAD_REACH * ROAD_REACH:
                    near.append((d, sj, si))
        self._nearest[(i, j)] = [(si, sj) for _, sj, si in sorted(near)[:ROAD_NEAREST]]
        return self._nearest[(i, j)]

    def links(self, i, j):
        """The roads of the village of the cell (i, j) : the cells of the other ends (it chose them, or they chose it)."""
        out = set(self.nearest(i, j))
        for sj in range(j - ROAD_CELLS, j + ROAD_CELLS + 1):
            for si in range(i - ROAD_CELLS, i + ROAD_CELLS + 1):
                if (si, sj) != (i, j) and (i, j) in self.nearest(si, sj):
                    out.add((si, sj))
        return sorted(out)

    def road_box(self, a, b):
        (ax, ay), (bx, by) = a.center, b.center
        return (min(ax, bx) - ROAD_MARGIN, min(ay, by) - ROAD_MARGIN, max(ax, bx) + ROAD_MARGIN,
                max(ay, by) + ROAD_MARGIN)

    def road(self, ca, cb):
        """The blocks of the road between the villages of the cells ca and cb, or None (no way over the land) :
        worldgen.plan_routes between two villages, its A* inside their box and ROAD_MARGIN around, around the
        buildings and the ponds of the villages there (the same from either end : from the smaller cell)."""
        ca, cb = min(ca, cb), max(ca, cb)
        key = (ca, cb)
        if key in self._roads:
            return self._roads[key]
        a, b = self.village(*ca), self.village(*cb)
        box = self.road_box(a, b)
        blocked = set()
        for cj in range(box[1] // VILLAGE_CELL, box[3] // VILLAGE_CELL + 1):
            for ci in range(box[0] // VILLAGE_CELL, box[2] // VILLAGE_CELL + 1):
                o = self.village(ci, cj)
                if o is not None:
                    blocked |= o.footprint | o.pond
        memo = {}

        def cost(x, y):
            if (x, y) in memo:
                return memo[(x, y)]
            c = None
            if box[0] <= x <= box[2] and box[1] <= y <= box[3] and (x, y) not in blocked \
                    and self.height(x, y) >= 0.41:
                o = self.village_of(x, y)
                near = o is not None and (x, y) in o.near
                edge = min(x - box[0], box[2] - x, y - box[1], box[3] - y)
                c = 1.0 + 10.0 * self.relief(x, y) + (0.0 if near else 0.5) \
                    + 3.0 * fbm(x / 6.0, y / 6.0, self.seed + 500, 2) + 0.5 * max(0, ROAD_EDGE - edge)
            memo[(x, y)] = c
            return c

        path = astar(cost, a.center, b.center)
        blocks = None
        if path is not None:
            blocks = set()
            for n, (x, y) in enumerate(path):
                blocks.add((x, y))
                nx, ny = path[min(n + 1, len(path) - 1)]
                side = (x + 1, y) if nx == x else (x, y + 1)          # two blocks wide
                if cost(*side) is not None:
                    blocks.add(side)
            blocks -= blocked
        self._roads[key] = blocks
        self._paths[key] = path
        return blocks

    def road_path(self, ca, cb):
        """The middle of the road between the villages of the cells ca and cb, from the smaller cell, or None."""
        self.road(ca, cb)
        return self._paths[(min(ca, cb), max(ca, cb))]

    def roads(self, ci0, cj0, ci1, cj1):
        """The roads of the villages of the cells (ci0, cj0) .. (ci1, cj1) : (cell, cell) pairs, once each."""
        out = set()
        for cj in range(cj0, cj1 + 1):
            for ci in range(ci0, ci1 + 1):
                for o in self.links(ci, cj):
                    out.add((min((ci, cj), o), max((ci, cj), o)))
        return sorted(out)

    def roads_of_cell(self, i, j):
        """The blocks of every road through the cell (i, j) (its box and 3 blocks around meet the cell : its ends
        are within ROAD_CELLS cells)."""
        key = (i, j)
        if key not in self._cell_roads:
            x0, y0 = i * VILLAGE_CELL - 3, j * VILLAGE_CELL - 3
            x1, y1 = x0 + VILLAGE_CELL + 6, y0 + VILLAGE_CELL + 6
            out = set()
            for ca, cb in self.roads(i - ROAD_CELLS, j - ROAD_CELLS, i + ROAD_CELLS, j + ROAD_CELLS):
                if max(abs(ca[0] - i), abs(ca[1] - j)) > ROAD_CELLS or max(abs(cb[0] - i), abs(cb[1] - j)) > ROAD_CELLS:
                    continue
                bx0, by0, bx1, by1 = self.road_box(self.village(*ca), self.village(*cb))
                if bx1 < x0 or bx0 > x1 or by1 < y0 or by0 > y1:
                    continue
                out |= self.road(ca, cb) or set()
            self._cell_roads[key] = out
        return self._cell_roads[key]

    def on_road(self, x, y):
        return (x, y) in self.roads_of_cell(x // VILLAGE_CELL, y // VILLAGE_CELL)

    def near_road(self, x, y, limit=2):
        """Within limit blocks of a road (4 neighbours : the Manhattan distance, as worldgen.distance_to)."""
        roads = self.roads_of_cell(x // VILLAGE_CELL, y // VILLAGE_CELL)
        if not roads:
            return False
        return any((x + dx, y + dy) in roads for dy in range(-limit, limit + 1)
                   for dx in range(-limit + abs(dy), limit - abs(dy) + 1))

    # ---------------------------------------------------------------- the terrain

    def classify(self, x, y):
        """The ground of a block and its level (worldgen.classify)."""
        v = self.village_of(x, y)
        if v is not None:                            # the village : paved streets, sand yards, a pond
            if (x, y) in v.road or (x, y) in v.pond_ring:
                return ("pave", 0)
            if (x, y) in v.pond:
                return ("water", 0)
            if (x, y) in v.yard:
                return ("sand", 0)
        if self.on_road(x, y):
            return ("path", 0)
        h = self.height(x, y)
        sea = self.ocean(x, y)
        if sea == "bar":
            return ("sand", 0)
        if sea == "reef":
            return ("reef", 0)
        if h < 0.40:
            return ("water", 0)
        if h < 0.425:
            return ("sand", 0)
        if (v is not None and v.near.get((x, y), 99) <= 6) or self.near_road(x, y):
            return ("grass", 0)                      # the villages and the roads stay low
        a = self.relief(x, y) * self.continent(x, y)     # no mountains in the sea
        plateau = fbm(x / 15.0, y / 15.0, self.seed + 60, 3) > 0.6 and h > 0.47
        m = (h - 0.43) * 14 * a + 1.2 * a * (fbm(x / 7.0, y / 7.0, self.seed + 330, 2) - 0.5)
        return ("grass", max(1 if plateau else 0, min(w.MAX_LEVEL, int(max(0.0, m)))))

    def area(self, x0, y0, x1, y1):
        """The blocks x0 .. x1 - 1, y0 .. y1 - 1 : their terrain (cleaned), their stairs (blocks of the world), the
        terraces out of reach (their blocks), by rows ; computed in a window APRON blocks wider."""
        win = Window(self, x0 - APRON, y0 - APRON, x1 + APRON, y1 + APRON)
        return {
            "terrain": [win.t[j][APRON:APRON + x1 - x0] for j in range(APRON, APRON + y1 - y0)],
            "stairs": sorted((y0 + j - APRON, x0 + i - APRON) for i, j in win.stairs
                             if APRON <= i < APRON + x1 - x0 and APRON <= j < APRON + y1 - y0),
            "unreached": sum(1 for p in win.unreached
                             if any(APRON <= i < APRON + x1 - x0 and APRON <= j < APRON + y1 - y0 for i, j in p)),
        }

    def region(self, rx, ry):
        """A region (REGION x REGION blocks from (rx * REGION, ry * REGION)) : its terrain, its stairs (rows then
        columns), its biomes and seas, by rows."""
        x0, y0 = rx * REGION, ry * REGION
        area = self.area(x0, y0, x0 + REGION, y0 + REGION)
        return {
            "terrain": area["terrain"],
            "stairs": area["stairs"],
            "unreached": area["unreached"],
            "origin": (x0, y0),
            "biome": [[self.biome(x0 + i, y0 + j) for i in range(REGION)] for j in range(REGION)],
            "ocean": [[self.ocean(x0 + i, y0 + j) for i in range(REGION)] for j in range(REGION)],
        }


def astar(cost, start, goal):
    """worldgen.World.astar over a cost function (None : not crossed) : 4 neighbours, the Manhattan heuristic, the
    ties of the heap broken by the block."""
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
        for dx, dy in w.D8[:4]:
            nx, ny = x + dx, y + dy
            c = cost(nx, ny)
            if c is None:
                continue
            g = best[cur] + c
            if g < best.get((nx, ny), 1e18):
                best[(nx, ny)] = g
                came[(nx, ny)] = cur
                heapq.heappush(heap, (g + abs(nx - goal[0]) + abs(ny - goal[1]), (nx, ny)))
    return None


# ---------------------------------------------------------------- the terraces and their stairs

# The passes over the terrain : how far each reaches (the blocks that decide a block) : a clean pass 17 (a patch under
# 16 blocks and its sides, after the one block wide terraces : 1), the stairs of a terrace TERRACE_OPEN and 3 (the
# blocks a stair looks at : the flat ground above, the foot and the ground below)
CLEAN_PASSES, STAIR_ROUNDS, TERRACE_OPEN = 2, 1, 32
CLEAN_REACH, STAIR_REACH = 17, TERRACE_OPEN + 3
APRON = CLEAN_PASSES * CLEAN_REACH + STAIR_ROUNDS * (STAIR_REACH + CLEAN_REACH) + STAIR_REACH


class Window:
    """The terrain of the blocks x0 .. x1 - 1, y0 .. y1 - 1 cleaned and its stairs placed : right inside, APRON blocks
    from its edges (the blocks beyond are read as the sea, as worldgen reads the blocks off the map)."""

    def __init__(self, world, x0, y0, x1, y1):
        self.world, self.x0, self.y0, self.W, self.H = world, x0, y0, x1 - x0, y1 - y0
        self.t = [[world.classify(x0 + i, y0 + j) for i in range(self.W)] for j in range(self.H)]
        # The yards and the ponds of the villages stay (worldgen.village_ground)
        self.ground = set()
        for cj in range(y0 // VILLAGE_CELL, (y1 - 1) // VILLAGE_CELL + 1):
            for ci in range(x0 // VILLAGE_CELL, (x1 - 1) // VILLAGE_CELL + 1):
                v = world.village(ci, cj)
                if v is not None:
                    self.ground |= {(x - x0, y - y0) for x, y in v.yard | v.pond | v.pond_ring}
        for _ in range(CLEAN_PASSES):
            self.clean()
        for _ in range(STAIR_ROUNDS):
            self.place_stairs(lower=True)
            self.clean()
        self.unreached = self.place_stairs(lower=False)

    def mat(self, i, j):
        return self.t[j][i][0] if 0 <= i < self.W and 0 <= j < self.H else "water"

    def level(self, i, j):
        return self.t[j][i][1] if 0 <= i < self.W and 0 <= j < self.H else 0

    def low4(self, i, j):
        """The sides (N E S W) of a block lower than it (worldgen.low_mask, its first 4)."""
        lv = self.level(i, j)
        return tuple(lv > 0 and self.level(i + dx, j + dy) < lv for dx, dy in w.D8[:4])

    def is_foot(self, i, j):
        return 0 <= i < self.W and 0 <= j < self.H and self.mat(i, j) != "water" \
            and self.level(i, j - 1) > self.level(i, j)

    def flat(self, i, j):
        return 0 <= i < self.W and 0 <= j < self.H and self.mat(i, j) != "water" \
            and not any(self.low4(i, j)) and not self.is_foot(i, j)

    def stair_ok(self, i, j):
        """worldgen.stair_ok : stairs in the south face of a terrace, flat ground above, down to flat ground."""
        lv = self.level(i, j)
        n, e, s, wst = self.low4(i, j)
        return lv > 0 and s and not n and not e and not wst and self.level(i - 1, j) == lv \
            and self.level(i + 1, j) == lv and self.flat(i, j - 1) and self.is_foot(i, j + 1) and self.flat(i, j + 2)

    def patches(self, same):
        """The patches of blocks (4 neighbours) where same(a, b) links two blocks : lists of blocks, by their first
        block in the order of the rows."""
        seen = [[False] * self.W for _ in range(self.H)]
        out = []
        for j0 in range(self.H):
            for i0 in range(self.W):
                if seen[j0][i0]:
                    continue
                seen[j0][i0] = True
                patch, front = [(i0, j0)], [(i0, j0)]
                while front:
                    a, b = front.pop()
                    for dx, dy in w.D8[:4]:
                        i, j = a + dx, b + dy
                        if 0 <= i < self.W and 0 <= j < self.H and not seen[j][i] and same((a, b), (i, j)):
                            seen[j][i] = True
                            patch.append((i, j))
                            front.append((i, j))
                out.append(patch)
        return out

    def clean(self):
        """A pass of worldgen.clean : a terrace one block wide goes down (a cliff needs two blocks), and under a face
        (no room for the foot and a cliff) ; then a terrace under 16 blocks goes down a level, sand or water under 5
        take the land around (a sand bar : the sea). Each block from the terrain as it was before the pass."""
        L = self.level
        new = [row[:] for row in self.t]
        for j in range(self.H):
            for i in range(self.W):
                m, lv = self.t[j][i]
                if lv > 0 and ((L(i, j - 1) < lv and L(i, j + 1) < lv) or (L(i - 1, j) < lv and L(i + 1, j) < lv)
                               or (L(i, j - 1) > lv and any(self.low4(i, j)))):
                    new[j][i] = (m, lv - 1)
        self.t = new
        t = self.t
        for patch in self.patches(lambda a, b: t[a[1]][a[0]] == t[b[1]][b[0]]):
            kind = t[patch[0][1]][patch[0][0]]
            if any(c in self.ground for c in patch):
                continue
            if kind[0] == "grass" and kind[1] > 0 and len(patch) < 16:
                to = ("grass", kind[1] - 1)
            elif kind[0] == "sand" and len(patch) < 5:
                i, j = patch[0]
                to = ("water", 0) if self.world.ocean(self.x0 + i, self.y0 + j) == "bar" else ("grass", 0)
            elif kind[0] == "water" and len(patch) < 5:
                to = ("sand", 0)
            else:
                continue
            for i, j in patch:
                t[j][i] = to

    def place_stairs(self, lower):
        """worldgen.place_stairs : stairs along the long faces (every 9 blocks, half of them), then each closed
        terrace (it fits in TERRACE_OPEN blocks) with none gets one (the middle one of the blocks that can have it) ;
        with no room for one, it goes down a level (lower), or stays out of reach. Returns the terraces out of reach."""
        world = self.world
        self.stairs = {(i, j) for j in range(self.H) for i in range(self.W)
                       if (self.x0 + i) % 9 == 4 and w.h01((self.x0 + i) // 9, self.y0 + j, world.seed + 5) < 0.5
                       and self.stair_ok(i, j)}
        unreached = []
        t = self.t
        for patch in self.patches(lambda a, b: t[a[1]][a[0]][1] == t[b[1]][b[0]][1]):
            lv = self.level(*patch[0])
            if lv == 0:
                continue
            xs, ys = [i for i, _ in patch], [j for _, j in patch]
            if max(xs) - min(xs) >= TERRACE_OPEN or max(ys) - min(ys) >= TERRACE_OPEN or min(xs) == 0 \
                    or min(ys) == 0 or max(xs) == self.W - 1 or max(ys) == self.H - 1:
                continue                             # open : its stairs along its faces
            if any(c in self.stairs for c in patch):
                continue
            ok = sorted((j, i) for i, j in patch if self.stair_ok(i, j))
            if ok:
                j, i = ok[len(ok) // 2]
                self.stairs.add((i, j))
            elif lower:
                for i, j in patch:
                    t[j][i] = (t[j][i][0], lv - 1)
            else:
                unreached.append(patch)
        return unreached


# ---------------------------------------------------------------- the tiles of a region

# A region is built (worldgen.World.build : its tiles, its villages decorated, its trees, places, signs, items, rooms)
# in a window BUILD_MARGIN blocks around it : the villages that reach it whole, the trees before it (a tree takes its
# blocks : the trees placed before it, in the order of the rows, decide it) ; its terrain known PLAN_MARGIN blocks around
# (the places beside the roads : one is kept when no other within POI_SPACING has a higher priority, each tried on
# the terrain around it)
BUILD_MARGIN = 2 * w.CHUNK
PLAN_MARGIN = BUILD_MARGIN + 60
REACH_SIZE = 150                     # a walkable patch of the window that big (or cut by its edge) : reached


class Off:
    """A list indexed from off (the rows of a window of the world, the blocks of a row) ; beyond it, as off a map : a
    read gives the default, a write is lost (a village or a tree cut by the edge of the window)."""
    __slots__ = ("off", "rows", "default")

    def __init__(self, off, rows, default=None):
        self.off, self.rows, self.default = off, rows, default

    def __getitem__(self, i):
        k = i - self.off
        return self.rows[k] if 0 <= k < len(self.rows) else self.default

    def __setitem__(self, i, v):
        k = i - self.off
        if 0 <= k < len(self.rows):
            self.rows[k] = v


def window_grid(x0, y0, size, make, default):
    """A window of size x size blocks from (x0, y0) : make(x, y) each ; beyond, default."""
    return Off(y0, [Off(x0, [make(x, y) for x in range(x0, x0 + size)], default) for y in range(y0, y0 + size)],
               Off(0, [], default))


class StableTiles(w.Tiles):
    """worldgen.Tiles whose ids are their keys (not the order they were first used in : a chunk built in another
    window has the same chipset)."""

    def cell(self, key, walkable):
        full = (key, walkable)
        if full not in self.ids:
            tid = repr(full)
            self.ids[full], self.walkable[tid], self.base[tid], self.flags[tid] = tid, walkable, w.tile_base(key), \
                w.tile_flags(key)
            self.mask[tid] = w.tile_mask(key, walkable)
            self.draws[tid] = lambda: w.draw_key(key, walkable)
        return self.ids[full]


class RegionWorld(w.World):
    """A region of the infinite world built as worldgen builds a world : its arrays cover the window around it (the
    blocks of the world : X0, Y0), the stages of worldgen run on them ; its chunks and its rooms are written."""

    def __init__(self, inf, rx, ry, margin=BUILD_MARGIN, plan=PLAN_MARGIN - BUILD_MARGIN):
        self.inf, self.seed, self.rx, self.ry = inf, inf.seed, rx, ry
        self.cw = self.ch = REGION_CHUNKS
        gx, gy = rx * REGION, ry * REGION
        self.X0, self.Y0, self.W, self.H = gx - margin, gy - margin, REGION + 2 * margin, REGION + 2 * margin
        # The terrain (cleaned, its stairs) of the plan window
        pm = margin + plan
        self.P0, self.PY0, self.PW = gx - pm, gy - pm, REGION + 2 * pm
        area = inf.area(self.P0, self.PY0, self.P0 + self.PW, self.PY0 + self.PW)
        # (the reefs : water, a rock on it : sea())
        self.reefs = {(self.P0 + i, self.PY0 + j) for j, row in enumerate(area["terrain"]) for i, t in enumerate(row)
                      if t[0] == "reef"}
        self.terrain = Off(self.PY0, [Off(self.P0, [("water", 0) if t[0] == "reef" else t for t in row], ("water", 0))
                                      for row in area["terrain"]], Off(0, [], ("water", 0)))
        self.stairs = {(x, y) for y, x in area["stairs"]}
        # The villages of the cells of the plan window and around (the other ends of its roads)
        c0, c1 = self.P0 // VILLAGE_CELL - ROAD_CELLS, (self.P0 + self.PW) // VILLAGE_CELL + ROAD_CELLS
        d0, d1 = self.PY0 // VILLAGE_CELL - ROAD_CELLS, (self.PY0 + self.PW) // VILLAGE_CELL + ROAD_CELLS
        index, self.villages = {}, []
        for cj in range(d0, d1 + 1):
            for ci in range(c0, c1 + 1):
                v = inf.village(ci, cj)
                if v is not None:
                    index[(ci, cj)] = len(self.villages)
                    self.villages.append(v)
        self.near_village = {}
        for v in self.villages:
            for c, d in v.near.items():
                if d < self.near_village.get(c, 99):
                    self.near_village[c] = d
        # The roads that reach the plan window
        self.routes, self.route_paths = set(), []
        pc0, pc1 = self.P0 // VILLAGE_CELL, (self.P0 + self.PW) // VILLAGE_CELL
        pd0, pd1 = self.PY0 // VILLAGE_CELL, (self.PY0 + self.PW) // VILLAGE_CELL
        for ca, cb in inf.roads(pc0 - ROAD_CELLS, pd0 - ROAD_CELLS, pc1 + ROAD_CELLS, pd1 + ROAD_CELLS):
            bx0, by0, bx1, by1 = inf.road_box(inf.village(*ca), inf.village(*cb))
            if bx1 < self.P0 or bx0 >= self.P0 + self.PW or by1 < self.PY0 or by0 >= self.PY0 + self.PW:
                continue
            blocks = inf.road(ca, cb)
            if blocks is None:
                continue
            self.routes |= blocks
            self.route_paths.append((index.get(ca, -1), index.get(cb, -1), inf.road_path(ca, cb)))
        self.near_route = self.distance_to(self.routes, 3)
        self.biome_field()
        self.layers = {l: window_grid(self.X0, self.Y0, self.W, lambda x, y: None, None) for l in ("", "M", "T")}
        self.walk = window_grid(self.X0, self.Y0, self.W, lambda x, y: True, False)
        self.taken, self.objects, self.chip = set(), {}, StableTiles()
        self.plan_pois()

    # The terrain is known over the plan window (the layers over the window)
    def planned(self, x, y):
        return self.P0 <= x < self.P0 + self.PW and self.PY0 <= y < self.PY0 + self.PW

    def mat(self, x, y):
        return self.terrain[y][x][0] if self.planned(x, y) else "water"

    def level(self, x, y):
        return self.terrain[y][x][1] if self.planned(x, y) else 0

    def is_foot(self, x, y):
        return self.planned(x, y) and self.mat(x, y) != "water" and self.level(x, y - 1) > self.level(x, y)

    def flat(self, x, y):
        return self.planned(x, y) and self.mat(x, y) != "water" and not any(self.low_mask(x, y)[:4]) \
            and not self.is_foot(x, y)

    def poi_block_ok(self, x, y):
        return self.planned(x, y) and 1 < x % w.CHUNK < w.CHUNK - 2 and 1 < y % w.CHUNK < w.CHUNK - 2 \
            and self.terrain[y][x] == ("grass", 0) and self.flat(x, y) and (x, y) not in self.routes \
            and self.near_village.get((x, y), 99) > 6 and (x, y) not in self.stairs

    def ground(self):
        """worldgen.ground, then the sea : the reefs (rocks out of the water), the deep water (darker waves) where nothing
        is on the water (its banks)."""
        super().ground()
        for y in range(self.Y0, self.Y0 + self.H):
            for x in range(self.X0, self.X0 + self.W):
                if self.mat(x, y) != "water" or self.layers["M"][y][x] is not None:
                    continue
                if (x, y) in self.reefs:
                    self.set("M", x, y, self.chip.cell(("reef",), False))
                    self.objects[(x, y)] = 0.4
                elif self.inf.ocean(x, y) == "deep":
                    self.set("M", x, y, self.chip.cell(("deep", int(w.h01(x, y, self.seed + 541) * 3)), True))

    def spawn_kind(self, cx, cy):
        """The wild Pokemon of a chunk : of the ocean when most of it is sea (every other block), else of its biome."""
        sea = sum(1 for y in range(cy * w.CHUNK, cy * w.CHUNK + w.CHUNK, 2) for x in range(cx * w.CHUNK, cx * w.CHUNK + w.CHUNK, 2)
                  if self.inf.ocean(x, y) is not None)
        return "ocean" if 2 * sea > (w.CHUNK // 2) ** 2 else self.biome[(cx, cy)]

    def chunks(self):
        cx, cy = self.rx * REGION_CHUNKS, self.ry * REGION_CHUNKS
        return [(cx + i, cy + j) for j in range(REGION_CHUNKS) for i in range(REGION_CHUNKS)]

    def biome_field(self):
        """worldgen.biome_field : the biomes blurred (two box blurs of 5 blocks), in integers (the sums of the blocks
        around, the same in any window), the biomes of the corners ; the biome of each chunk : the most of its blocks."""
        inf, m = self.inf, 10
        x0, y0, n = self.X0 - m, self.Y0 - m, self.W + 2 * m
        label = [[inf.biome(x0 + i, y0 + j) for i in range(n)] for j in range(n)]

        def box_rows(g):                             # the sums of 11 blocks along the rows (5 shorter at each end)
            out = []
            for row in g:
                pre = [0]
                for v in row:
                    pre.append(pre[-1] + v)
                out.append([pre[i + 11] - pre[i] for i in range(len(row) - 10)])
            return out

        def box_columns(g):
            return [list(r) for r in zip(*box_rows([list(c) for c in zip(*g)]))]

        weights = [[{} for _ in range(self.W)] for _ in range(self.H)]
        for b in w.BIOMES:
            g = [[1 if v == b else 0 for v in row] for row in label]
            g = box_columns(box_rows(box_columns(box_rows(g))))
            for j in range(self.H):
                for i in range(self.W):
                    if g[j][i] > 0.001 * 11 ** 4:
                        weights[j][i][b] = g[j][i] / 11 ** 4
        self.weights = Off(self.Y0, [Off(self.X0, row) for row in weights])
        W, H = self.W, self.H
        corner = [[None] * (W + 1) for _ in range(H + 1)]
        for j in range(H + 1):
            for i in range(W + 1):
                acc = [0.0] * len(w.BIOMES)
                for dx in (-1, 0):
                    for dy in (-1, 0):
                        wts = weights[min(H - 1, max(0, j + dy))][min(W - 1, max(0, i + dx))]
                        for k, b in enumerate(w.BIOMES):
                            acc[k] += wts.get(b, 0.0)
                total = sum(acc)
                raw = [a / total * w.MIX_STEPS for a in acc]
                q = [int(v) for v in raw]
                for k in sorted(range(len(w.BIOMES)), key=lambda k: -(raw[k] - q[k]))[:w.MIX_STEPS - sum(q)]:
                    q[k] += 1
                corner[j][i] = tuple(q)
        self.pal = Off(self.Y0, [Off(self.X0, [(corner[y][x], corner[y][x + 1], corner[y + 1][x], corner[y + 1][x + 1])
                                               for x in range(W)]) for y in range(H)])
        self.biome = {}
        for cx, cy in self.chunks():
            count = {}
            for y in range(cy * w.CHUNK, cy * w.CHUNK + w.CHUNK, 2):
                for x in range(cx * w.CHUNK, cx * w.CHUNK + w.CHUNK, 2):
                    b = inf.biome(x, y)
                    count[b] = count.get(b, 0) + 1
            self.biome[(cx, cy)] = max(sorted(count), key=lambda b: count[b])

    def plan_pois(self):
        """worldgen.plan_pois, local : the places tried every 12 blocks of each road (the kind to try first from a
        hash) ; one is kept when no other found within POI_SPACING has a higher priority (a hash), and none is
        within POI_SPACING of a village ; its path from the road paved."""
        reach = (self.X0 - 12, self.Y0 - 12, self.X0 + self.W + 12, self.Y0 + self.H + 12)
        span = (reach[0] - w.POI_SPACING, reach[1] - w.POI_SPACING, reach[2] + w.POI_SPACING,
                reach[3] + w.POI_SPACING)
        found = []
        for _, _, path in self.route_paths:
            for i in range(10, len(path) - 10, 12):
                (x, y), (nx, ny) = path[i], path[i + 1]
                if not (span[0] <= x < span[2] and span[1] <= y < span[3]):
                    continue
                if any(math.dist((x, y), v.center) < w.POI_SPACING for v in self.villages):
                    continue
                first = int(w.h01(x, y, self.seed + 43) * len(w.POI_KINDS))
                for kind in w.POI_KINDS[first:] + w.POI_KINDS[:first]:
                    got = self.poi_area(kind, x, y, nx, ny)
                    if got:
                        found.append(((w.h01(x, y, self.seed + 44), y, x), kind, x, y, got))
                        break
        self.pois = []
        for prio, kind, x, y, (ax, ay, spur) in sorted(found):
            if not (reach[0] <= x < reach[2] and reach[1] <= y < reach[3]):
                continue
            if any(p2 > prio and math.dist((x, y), (x2, y2)) < w.POI_SPACING for p2, _, x2, y2, _ in found):
                continue
            wd, ht = w.POI_SIZE[kind]
            if kind == "rest" and (ax + 3, ay + ht - 1) not in spur:
                spur = spur + [(ax + 3, ay + ht - 1)]
            for cx, cy in spur:
                self.terrain[cy][cx] = ("path", 0)
            self.pois.append({"kind": kind, "x": ax, "y": ay, "w": wd, "h": ht, "spur": spur,
                              "biome": self.inf.biome(ax, ay)})

    def reachable(self):
        """The blocks the player can walk to : the walkable patches of the window of REACH_SIZE blocks or more, or
        cut by its edges (worldgen : from the start, over the whole world)."""
        seen, reach = set(), set()
        for y0 in range(self.Y0, self.Y0 + self.H):
            for x0 in range(self.X0, self.X0 + self.W):
                if (x0, y0) in seen or not self.walk[y0][x0]:
                    continue
                patch, front, edge = [(x0, y0)], [(x0, y0)], False
                seen.add((x0, y0))
                while front:
                    x, y = front.pop()
                    for dx, dy in w.D8[:4]:
                        c = (x + dx, y + dy)
                        if not self.inside(*c):
                            edge = True
                        elif c not in seen and self.walk[c[1]][c[0]]:
                            seen.add(c)
                            patch.append(c)
                            front.append(c)
                if edge or len(patch) >= REACH_SIZE:
                    reach.update(patch)
        return reach

    def in_region(self, x, y):
        gx, gy = self.rx * REGION, self.ry * REGION
        return gx <= x < gx + REGION and gy <= y < gy + REGION

    def place_rooms(self):
        """worldgen.place_rooms for the buildings whose door is in the region : each room named by its door
        (w_room_<x>_<y>, a block of the world), its template and its furniture from it."""
        self.rooms, self.door_events = [], {}
        buildings = [b for v in self.villages for b in v.buildings] + self.lone_buildings
        for kind, roof, x0, y0, wd, ht, dc, dr in buildings:
            door = (x0 + dc, y0 + dr)
            if not self.in_region(*door):
                continue
            choices = w.rooms.templates(kind)
            tname, rows = choices[int(w.h01(door[0], door[1], self.seed + 31) * len(choices))]
            layers, walk, exits, counter, beds = w.rooms.build_room(self.chip, kind, rows,
                                                                    int(w.h01(door[0], door[1], self.seed + 32) * 1e6))
            rw, rh = len(rows[0]), len(rows)
            mw, mh = max(rw, w.ROOM_MAP[0]), max(rh, w.ROOM_MAP[1])
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
            name = f"w_room_{door[0]}_{door[1]}"
            cx, cy = door[0] // w.CHUNK, door[1] // w.CHUNK
            lx, ly = door[0] - cx * w.CHUNK, door[1] - cy * w.CHUNK
            ex, ey = exits[0]
            self.door_events.setdefault((cx, cy), []).append(
                f"{lx} {ly} 2 Levels/teleport.miniska, {name}, {ex}, {ey - 1}")
            events = [f"{x} {y} 2 Levels/teleport.miniska, w_{cx}_{cy}, {lx}, {ly + 1}" for x, y in exits]
            events += [f"{x} {y} 1 Levels/pokescript.miniska" for x, y in counter]
            events += [f"{x} {y} 1 Levels/rest.miniska" for x, y in beds]
            self.rooms.append({"name": name, "template": tname, "rows": rows, "layers": layers, "walk": walk,
                               "events": events})

    def write_region(self, root, levels, chip_path):
        """The chunks of the region and its rooms in levels (a folder per map, Chipsets : a chipset per chunk, one
        for the rooms of the region) ; the maps name their chipsets in chip_path."""
        corr = w.correspondance(f"{root}/Resources/Chipsets/corr.png")
        colour = {(x, y): corr.getpixel((x, y)) for x in range(corr.width) for y in range(corr.height)}
        chips = f"{levels}/Chipsets"
        os.makedirs(chips, exist_ok=True)
        scripts = f"{root}/Resources/Chipsets/tile_scripts"
        self.stats = {}
        w.g.use_biome("meadow")
        header = "x y declenchement(1:presser espace||2:sous le hero||0:automatique au chargement de la map) " \
                 "parametresSupplementaires"
        for cx, cy in self.chunks():
            self.write_chunk(levels, chips, chip_path, scripts, colour, header, cx, cy)
        self.write_rooms(levels, chips, chip_path, scripts, colour, header,
                         chipset=f"chipset_rooms_{self.rx}_{self.ry}")


def build_region(seed, rx, ry, out, margin=BUILD_MARGIN, chip_path=None):
    """The region (rx, ry) of the world of a seed built and written in out (its maps, their chipsets ; the maps name
    them in chip_path, default : out from the root of the game)."""
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    world = RegionWorld(Infinite(seed), rx, ry, margin)
    world.build()
    world.write_region(root, out, chip_path or f"./{os.path.relpath(out, root)}/Chipsets")
    return world


# ---------------------------------------------------------------- previews and checks

COLOURS = {"meadow": (120, 190, 52), "autumn": (214, 126, 52), "snow": (236, 242, 250), "desert": (240, 214, 150)}
SEA = {"deep": (38, 92, 168), "shallows": (78, 150, 214), "reef": (196, 176, 150)}
ROADS = {"pave": (150, 132, 120), "path": (176, 118, 66)}


def region_image(region):
    img = Image.new("RGB", (REGION, REGION))
    px = img.load()
    for j in range(REGION):
        for i in range(REGION):
            mat, level = region["terrain"][j][i]
            sea = region["ocean"][j][i]
            if mat == "water":
                c = SEA.get(sea, (92, 160, 220))
            elif mat == "reef":
                c = SEA["reef"]
            elif mat == "sand":
                c = (238, 218, 160)
            elif mat in ROADS:
                c = ROADS[mat]
            else:
                base = COLOURS[region["biome"][j][i]]
                k = 1.0 - 0.09 * level
                c = tuple(int(v * k) for v in base)
            px[i, j] = c
    x0, y0 = region["origin"]
    for y, x in region["stairs"]:
        px[x - x0, y - y0] = (40, 30, 20)
    return img


def coarse(seed, regions, at, step, out):
    """A map of many regions, a block in "step" (the shape of the continents, the biomes, the seas)."""
    world = Infinite(seed)
    size = regions * REGION // step
    img = Image.new("RGB", (size, size))
    px = img.load()
    x0, y0 = at[0] * REGION, at[1] * REGION
    for j in range(size):
        for i in range(size):
            x, y = x0 + i * step, y0 + j * step
            mat, level = world.classify(x, y)
            sea = world.ocean(x, y)
            if mat == "water":
                c = SEA.get(sea, (92, 160, 220))
            elif mat == "reef":
                c = SEA["reef"]
            elif mat == "sand":
                c = (238, 218, 160)
            elif mat in ROADS:
                c = ROADS[mat]
            else:
                c = tuple(int(v * (1.0 - 0.09 * level)) for v in COLOURS[world.biome(x, y)])
            px[i, j] = c
        if j % 40 == 0:
            print(f"row {j} / {size}", file=sys.stderr)
    # The roads and the villages over it (thinner than a step : drawn from their blocks)
    ci0, cj0 = x0 // VILLAGE_CELL, y0 // VILLAGE_CELL
    ci1, cj1 = (x0 + regions * REGION) // VILLAGE_CELL, (y0 + regions * REGION) // VILLAGE_CELL
    start = world.start()
    for cj in range(cj0, cj1 + 1):
        for ci in range(ci0, ci1 + 1):
            for other in world.links(ci, cj):
                for bx, by in world.road((ci, cj), other) or ():
                    i, j = (bx - x0) // step, (by - y0) // step
                    if 0 <= i < size and 0 <= j < size:
                        px[i, j] = ROADS["path"]
    for cj in range(cj0, cj1 + 1):
        for ci in range(ci0, ci1 + 1):
            v = world.village(ci, cj)
            if v is None:
                continue
            for bx, by in v.road | v.footprint:
                i, j = (bx - x0) // step, (by - y0) // step
                if 0 <= i < size and 0 <= j < size:
                    px[i, j] = (200, 40, 40) if (bx, by) in v.footprint else ROADS["pave"]
            if (ci, cj) == start:
                cx, cy = (v.center[0] - x0) // step, (v.center[1] - y0) // step
                for d in range(-6, 7):
                    for a, b in ((cx + d, cy - 6), (cx + d, cy + 6), (cx - 6, cy + d), (cx + 6, cy + d)):
                        if 0 <= a < size and 0 <= b < size:
                            px[a, b] = (255, 255, 255)
            print(f"village {ci} {cj} {v.biome} {v.name} {v.center}", file=sys.stderr)
    # The regions : a faint grid
    for k in range(0, size, REGION // step):
        for t in range(size):
            for (a, b) in ((k, t), (t, k)):
                r, g, bl = px[a, b]
                px[a, b] = (r * 7 // 8, g * 7 // 8, bl * 7 // 8)
    img.save(out)
    print(out)


def preview(seed, regions, at, out):
    world = Infinite(seed)
    rx0, ry0 = at
    img = Image.new("RGB", (regions * REGION, regions * REGION))
    for j in range(regions):
        for i in range(regions):
            # Each region generated alone (a new world each time : no cache shared between the regions)
            alone = Infinite(seed)
            img.paste(region_image(alone.region(rx0 + i, ry0 + j)), (i * REGION, j * REGION))
            print(f"region {rx0 + i} {ry0 + j}", file=sys.stderr)
    del world
    img.save(out)
    print(out)


def check(seed):
    """The region of the first village alone, and the same blocks inside a bigger area generated by another world
    (another window around them, other caches) : the same (its villages, its roads, its terrain, its stairs)."""
    world = Infinite(seed)
    i, j = world.start()
    cx, cy = world.village(i, j).center
    x0, y0 = cx // REGION * REGION, cy // REGION * REGION
    alone = world.area(x0, y0, x0 + REGION, y0 + REGION)
    bx0, by0 = x0 - 97, y0 - 41                     # the region off the middle of the bigger area
    big = Infinite(seed).area(bx0, by0, x0 + REGION + 133, y0 + REGION + 58)
    inside = [row[x0 - bx0:x0 - bx0 + REGION] for row in big["terrain"][y0 - by0:y0 - by0 + REGION]]
    stairs = [(y, x) for y, x in big["stairs"] if x0 <= x < x0 + REGION and y0 <= y < y0 + REGION]
    same = alone["terrain"] == inside and alone["stairs"] == stairs
    print(f"stairs {len(alone['stairs'])}, terraces out of reach {alone['unreached']}")
    print("same" if same else "DIFFERENT")
    return same


def start_region(world):
    i, j = world.start()
    cx, cy = world.village(i, j).center
    return cx // REGION, cy // REGION


def same_folders(a, b):
    """The files of two folders (and below) : the same names, the same bytes."""
    cmp = filecmp.dircmp(a, b)
    if cmp.left_only or cmp.right_only or cmp.funny_files:
        print("only in one :", cmp.left_only[:5], cmp.right_only[:5])
        return False
    _, mismatch, errors = filecmp.cmpfiles(a, b, cmp.common_files, shallow=False)
    if mismatch or errors:
        print("different :", (mismatch + errors)[:10])
        return False
    return all(same_folders(os.path.join(a, d), os.path.join(b, d)) for d in cmp.common_dirs)


def check_tiles(seed, out):
    """The region of the first village built in two windows (BUILD_MARGIN, and wider) : the same files."""
    import shutil
    rx, ry = start_region(Infinite(seed))
    folders = []
    for margin in (BUILD_MARGIN, BUILD_MARGIN + w.CHUNK):
        folder = f"{out}/margin{margin}"
        if os.path.exists(folder):
            shutil.rmtree(folder)
        world = build_region(seed, rx, ry, folder, margin, "./Data/Worlds/infinite/Chipsets")
        print(f"region {rx} {ry} margin {margin} : {len(world.rooms)} rooms, {len(world.pois)} places, "
              f"{len(world.items)} items", file=sys.stderr)
        folders.append(folder)
    same = same_folders(*folders)
    print("same" if same else "DIFFERENT")
    return same


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("command", choices=["preview", "map", "check", "build", "tiles"])
    ap.add_argument("--step", type=int, default=4)
    ap.add_argument("out", nargs="?", default="infinite.png")
    ap.add_argument("--seed", type=int, default=7)
    ap.add_argument("--regions", type=int, default=3)
    ap.add_argument("--at", type=int, nargs=2, default=None, help="a region (default : the first village's)")
    args = ap.parse_args()
    if args.at is None:
        rx, ry = start_region(Infinite(args.seed))
        half = args.regions // 2 if args.command in ("preview", "map") else 0
        args.at = (rx - half, ry - half)
    if args.command == "preview":
        preview(args.seed, args.regions, args.at, args.out)
    elif args.command == "map":
        coarse(args.seed, args.regions, args.at, args.step, args.out)
    elif args.command == "build":
        world = build_region(args.seed, args.at[0], args.at[1], args.out)
        print(f"{args.out} : region {args.at[0]} {args.at[1]}, {len(world.rooms)} rooms, {len(world.pois)} places, "
              f"{len(world.items)} items")
    elif args.command == "tiles":
        sys.exit(0 if check_tiles(args.seed, args.out) else 1)
    else:
        sys.exit(0 if check(args.seed) else 1)


if __name__ == "__main__":
    main()
