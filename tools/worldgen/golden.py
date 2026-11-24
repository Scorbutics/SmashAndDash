#!/usr/bin/env python3
"""golden : reference values of tools/worldgen for its C++ port (src/src/Game/Gameplay/World/Generation), checked by
tests/worldgen (sad_worldgen_test). Each stage of the port adds its values. The fields are summed up by an FNV-1a 64
hash of their exact values (the bits of the floats) : the port must give the same numbers, not close ones.

  tools/worldgen/golden.py > tests/worldgen/golden.txt
"""
import math
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import worldgen as w  # noqa: E402

FNV_OFFSET, FNV_PRIME = 0xcbf29ce484222325, 0x100000001b3


def fnv(data):
    h = FNV_OFFSET
    for b in data:
        h ^= b
        h = (h * FNV_PRIME) & 0xFFFFFFFFFFFFFFFF
    return h


def doubles(values):
    return fnv(b"".join(struct.pack("<d", v) for v in values))


def climate(seed, cw, ch):
    """Stage 1 : the climate (biomes, relief) and the height before the villages."""
    world = w.World.__new__(w.World)
    world.seed, world.cw, world.ch = seed, cw, ch
    world.W, world.H = cw * w.CHUNK, ch * w.CHUNK
    world.near_village = {}
    world.climate()
    return world


def cells(seq):
    """A set of blocks, sorted, as bytes (two 32 bits signed integers each)."""
    return fnv(b"".join(struct.pack("<ii", x, y) for x, y in sorted(seq)))


def villages(world):
    """Stage 2a : the villages, the distance to them, the biome weights and the mixes of the corners."""
    world.town = world.village_site()
    world.plan_villages()
    world.near_village = world.distance_to(set().union(*(v.cells for v in world.villages)), 8)
    world.biome_field()
    out = [f"town {world.seed} {world.town[0]} {world.town[1]}"]
    for i, v in enumerate(world.villages):
        name = v.name.encode("latin-1").hex()
        out.append(f"village {world.seed} {i} {v.center[0]} {v.center[1]} {w.BIOMES.index(v.biome)} {name} "
                   f"{len(v.buildings)} {fnv(repr(v.buildings).encode())} {cells(v.road)} {cells(v.footprint)} "
                   f"{cells(v.flowers)} {cells(v.yard)} {cells(v.pond)} {cells(v.pond_ring)}")
    near = sorted(world.near_village.items())
    out.append(f"near {world.seed} {fnv(b''.join(struct.pack('<iii', x, y, d) for (x, y), d in near))}")
    out.append(f"weights {world.seed} {doubles(wt.get(b, 0.0) for line in world.weights for wt in line for b in w.BIOMES)}")
    out.append(f"mixes {world.seed} {fnv(bytes(n for line in world.pal for mix in line for corner in mix for n in corner))}")
    return out


def main():
    out = []
    # Noise : a few values
    for args in ((0, 0, 0), (5, -3, 7), (-12, 40, 1234), (123456, 654321, 99)):
        out.append(f"h01 {args[0]} {args[1]} {args[2]} {w.h01(*args)!r}")
    for x, y, seed, octaves in ((0.5, 0.25, 7, 4), (-3.7, 12.9, 42, 3), (100.0 / 30.0, 7.0 / 11.0, 57, 2)):
        out.append(f"fbm {x!r} {y!r} {seed} {octaves} {w.fbm(x, y, seed, octaves)!r}")
    for seed in (7, 42):
        world = climate(seed, 8, 8)
        names = w.BIOMES
        out.append(f"labels {seed} {fnv(bytes(names.index(b) for line in world.label for b in line))}")
        out.append(f"relief {seed} {doubles(v for line in world.relief for v in line)}")
        out.append(f"chunks {seed} " + "".join(str(names.index(world.biome[(cx, cy)]))
                                                for cy in range(world.ch) for cx in range(world.cw)))
        out.append(f"height {seed} {doubles(world.height(x, y) for y in range(world.H) for x in range(world.W))}")
        out += villages(world)
    print("\n".join(out))


if __name__ == "__main__":
    main()
