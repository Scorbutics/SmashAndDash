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

  tools/worldgen/infinite.py preview [--seed N] [--regions R] [--at RX RY] [out.png]
      the regions around (RX, RY) generated one by one and stitched (a pixel a block) : the shape of the world
  tools/worldgen/infinite.py map [--seed N] [--regions R] [--step S] [--at RX RY] [out.png]
      a coarse map (a pixel every S blocks), the villages and the roads drawn over it
  tools/worldgen/infinite.py check [--seed N]     a region alone equals the same region inside a bigger window
"""
import argparse
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
START_SEARCH = 12                                  # cells around the origin searched for the first village
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
        self._roads = {}           # (cell, link) -> set of the blocks of the road, or None
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
        """The cell of the first village (the game starts there) : of the meadow sites, the nearest to the origin
        (the cells searched ring by ring, then the next ring : a site there can be nearer), else of any biome."""
        if self._start is None:
            found = None
            for want in ("meadow", None):
                best = None
                for r in range(START_SEARCH + 1):
                    for j in range(-r, r + 1):
                        for i in range(-r, r + 1):
                            if max(abs(i), abs(j)) != r:
                                continue
                            got = self.site(i, j)
                            if got is None or (want and got[1] != want):
                                continue
                            (x, y), _ = got
                            key = (x * x + y * y, j, i)
                            if best is None or key < best[0]:
                                best = (key, (i, j))
                    if best is not None and r > max(abs(best[1][0]), abs(best[1][1])):
                        break
                if best is not None:
                    found = best[1]
                    break
            self._start = found or (0, 0)
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
        return blocks

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

    def region(self, rx, ry):
        """A region (REGION x REGION blocks from (rx * REGION, ry * REGION)) : its terrain and its biomes, by rows."""
        x0, y0 = rx * REGION, ry * REGION
        return {
            "terrain": [[self.classify(x0 + i, y0 + j) for i in range(REGION)] for j in range(REGION)],
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
    """The region of the first village alone, and the same blocks generated after its neighbours (another order,
    the caches filled by them) : the same (its villages, its roads, its terrain)."""
    world = Infinite(seed)
    i, j = world.start()
    cx, cy = world.village(i, j).center
    rx, ry = cx // REGION, cy // REGION
    alone = Infinite(seed).region(rx, ry)
    other = Infinite(seed)
    for dx, dy in ((1, 0), (0, -1), (-1, 1)):
        other.region(rx + dx, ry + dy)
    inside = other.region(rx, ry)
    same = alone == inside
    print("same" if same else "DIFFERENT")
    return same


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("command", choices=["preview", "map", "check"])
    ap.add_argument("--step", type=int, default=4)
    ap.add_argument("out", nargs="?", default="infinite.png")
    ap.add_argument("--seed", type=int, default=7)
    ap.add_argument("--regions", type=int, default=3)
    ap.add_argument("--at", type=int, nargs=2, default=(0, 0))
    args = ap.parse_args()
    if args.command == "preview":
        preview(args.seed, args.regions, args.at, args.out)
    elif args.command == "map":
        coarse(args.seed, args.regions, args.at, args.step, args.out)
    else:
        sys.exit(0 if check(args.seed) else 1)


if __name__ == "__main__":
    main()
