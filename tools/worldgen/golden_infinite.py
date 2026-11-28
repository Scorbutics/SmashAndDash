#!/usr/bin/env python3
"""golden_infinite : reference values of tools/worldgen/infinite.py for its C++ port (World/Generation/Infinite),
checked by tests/worldgen (sad_worldgen_test), as golden.py for the finite world : FNV-1a 64 sums of the exact values.

  TMPDIR=build/golden-tmp tools/worldgen/golden_infinite.py > tests/worldgen/infinite.txt     (about 8 minutes)
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import infinite as inf  # noqa: E402
import worldgen as w  # noqa: E402
from golden import cells, doubles, fnv  # noqa: E402

MATS = ["water", "sand", "path", "grass", "pave", "reef"]
SEAS = [None, "shallows", "deep", "reef", "bar"]


def samples(seed):
    """Blocks around the centre of the world and far from it, every 37 blocks."""
    out = []
    for oy in (inf.CENTER - 900, inf.CENTER + 3000):
        for ox in (inf.CENTER - 900, inf.CENTER - 4100):
            out += [(ox + i * 37, oy + j * 37) for j in range(40) for i in range(40)]
    return out


def climate(world, seed):
    pts = samples(seed)
    return [
        f"rank {doubles(inf.rank(inf.RANK3, v / 64) for v in range(-3, 68))} {doubles(inf.rank(inf.RANK4, v / 64) for v in range(-3, 68))}",
        f"cellseed {seed} {fnv(b''.join(struct.pack('<q', inf.cell_seed(seed, i, j)) for i in range(60, 80) for j in range(60, 80)))}",
        f"continent {seed} {doubles(world.continent(x, y) for x, y in pts)}",
        f"relief {seed} {doubles(world.relief(x, y) for x, y in pts)}",
        f"climate {seed} {fnv(bytes(w.BIOMES.index(world.climate_at(x, y)) for x, y in pts))}",
        f"base {seed} {fnv(bytes(w.BIOMES.index(world.base_biome(x, y)) for x, y in pts))}",
        f"landheight {seed} {doubles(world.land_height(x, y) for x, y in pts)}",
        f"height {seed} {doubles(world.height(x, y) for x, y in pts)}",
        f"biome {seed} {fnv(bytes(w.BIOMES.index(world.biome(x, y)) for x, y in pts))}",
        f"ocean {seed} {fnv(bytes(SEAS.index(world.ocean(x, y)) for x, y in pts))}",
    ]


def villages(world, seed):
    si, sj = world.start()
    out = [f"start {seed} {si} {sj}"]
    for j in range(sj - 3, sj + 4):
        for i in range(si - 3, si + 4):
            got = world.site(i, j)
            if got is None:
                out.append(f"site {seed} {i} {j} none")
                continue
            (x, y), biome = got
            v = world.village(i, j)
            name = v.name.encode("utf-8").hex()
            near = sorted(v.near_biome.items())
            out.append(f"village {seed} {i} {j} {x} {y} {w.BIOMES.index(biome)} {int(v.first)} {name} "
                       f"{len(v.buildings)} {fnv(repr(v.buildings).encode())} {cells(v.road)} {cells(v.footprint)} "
                       f"{cells(v.flowers)} {cells(v.yard)} {cells(v.pond)} {cells(v.pond_ring)} "
                       f"{fnv(b''.join(struct.pack('<iii', x, y, d) for (x, y), d in near))} {len(v.near)}")
    for j in range(sj - 2, sj + 3):
        for i in range(si - 2, si + 3):
            links = world.links(i, j)
            out.append(f"links {seed} {i} {j} " + " ".join(f"{a},{b}" for a, b in links))
    for ca, cb in world.roads(si - 1, sj - 1, si + 1, sj + 1):
        path = world.road_path(ca, cb)
        blocks = world.road(ca, cb)
        if path is None:
            out.append(f"road {seed} {ca[0]} {ca[1]} {cb[0]} {cb[1]} none")
            continue
        out.append(f"road {seed} {ca[0]} {ca[1]} {cb[0]} {cb[1]} {len(path)} "
                   f"{fnv(b''.join(struct.pack('<ii', x, y) for x, y in path))} {cells(blocks)}")
    return out


def terrain(world, seed, rx, ry):
    x0, y0 = rx * inf.REGION, ry * inf.REGION
    cls = [world.classify(x0 + i, y0 + j) for j in range(0, inf.REGION, 3) for i in range(0, inf.REGION, 3)]
    out = [f"classify {seed} {rx} {ry} {fnv(bytes(v for m, lv in cls for v in (MATS.index(m), lv)))}"]
    area = world.area(x0, y0, x0 + inf.REGION, y0 + inf.REGION)
    out.append(f"terrain {seed} {rx} {ry} "
               f"{fnv(bytes(v for line in area['terrain'] for m, lv in line for v in (MATS.index(m), lv)))}")
    out.append(f"stairs {seed} {rx} {ry} {len(area['stairs'])} {cells((x, y) for y, x in area['stairs'])} "
               f"{area['unreached']}")
    return out


def region(seed, rx, ry):
    """The tiles of a region (RegionWorld : the stages of worldgen.World.build on a window around it) and its files."""
    import shutil
    import tempfile
    from golden import file_hash, grid
    world = inf.RegionWorld(inf.Infinite(seed), rx, ry)
    pre = f"{seed} {rx} {ry}"
    X0, Y0, W, H = world.X0, world.Y0, world.W, world.H
    out = [f"rwindow {pre} {X0} {Y0} {W} {H} {world.P0} {world.PY0} {world.PW}"]
    out.append(f"rterrain {pre} {fnv(bytes(v for y in range(world.PY0, world.PY0 + world.PW) for x in range(world.P0, world.P0 + world.PW) for v in (MATS.index(world.terrain[y][x][0]), world.terrain[y][x][1])))}")
    out.append(f"rvillages {pre} {len(world.villages)} " + " ".join(f"{v.cell[0]},{v.cell[1]}" for v in world.villages))
    out.append(f"rroutes {pre} {cells(world.routes)} {len(world.route_paths)}")
    for a, b, path in world.route_paths:
        out.append(f"rroute {pre} {a} {b} {len(path)} {fnv(b''.join(struct.pack('<ii', x, y) for x, y in path))}")
    near = sorted(world.near_route.items())
    out.append(f"rnearroute {pre} {fnv(b''.join(struct.pack('<iii', x, y, d) for (x, y), d in near))}")
    near = sorted(world.near_village.items())
    out.append(f"rnearvillage {pre} {fnv(b''.join(struct.pack('<iii', x, y, d) for (x, y), d in near))}")
    for p in world.pois:
        spur = b''.join(struct.pack('<ii', x, y) for x, y in p["spur"])
        out.append(f"rpoi {pre} {p['kind']} {p['x']} {p['y']} {p['w']} {p['h']} {fnv(spur)} {w.BIOMES.index(p['biome'])}")
    out.append(f"rweights {pre} {doubles(world.weights[y][x].get(b, 0.0) for y in range(Y0, Y0 + H) for x in range(X0, X0 + W) for b in w.BIOMES)}")
    out.append(f"rmixes {pre} {fnv(bytes(n for y in range(Y0, Y0 + H) for x in range(X0, X0 + W) for corner in world.pal[y][x] for n in corner))}")
    out.append(f"rchunks {pre} " + "".join(str(w.BIOMES.index(world.biome[c])) for c in world.chunks()))
    out.append(f"rsea {pre} {len(world.reefs)} " + "".join("1" if world.spawn_kind(cx, cy) == "ocean" else "0" for cx, cy in world.chunks()))
    world.build()
    keys = sorted(world.chip.ids.items(), key=lambda kv: list(world.chip.ids).index(kv[0]))
    out.append(f"rtiles {pre} {len(keys)} {fnv(chr(10).join(repr(k) for k, _ in keys).encode())}")
    for name, l in (("base", ""), ("middle", "M"), ("top", "T")):
        g = [[world.layers[l][y][x] for x in range(X0, X0 + W)] for y in range(Y0, Y0 + H)]
        out.append(f"rlayer {pre} {name} {grid(g)}")
    out.append(f"rwalk {pre} {fnv(bytes(int(world.walk[y][x]) for y in range(Y0, Y0 + H) for x in range(X0, X0 + W)))}")
    out.append(f"rtaken {pre} {cells(world.taken)}")
    out.append(f"robjects {pre} {fnv(b''.join(struct.pack('<iid', x, y, v) for (x, y), v in sorted(world.objects.items())))}")
    out.append(f"ritems {pre} {fnv(b''.join(struct.pack('<iii', x, y, i) for x, y, i in world.items))}")
    events = [chr(10).join(world.chunk_events(cx, cy)) for cx, cy in world.chunks()]
    out.append(f"revents {pre} {fnv(chr(12).join(events).encode('utf-8'))}")
    light = bytes(v for j in range(H + 1) for i in range(W + 1) for v in world.corner_light(X0 + i, Y0 + j))
    out.append(f"rlight {pre} {fnv(light)}")
    for room in world.rooms:
        out.append(f"rroom {pre} {room['name']} {room['template']} {fnv(chr(10).join(room['rows']).encode())} "
                   f"{grid(room['layers'][''])} {grid(room['layers']['M'])} "
                   f"{fnv(bytes(int(v) for line in room['walk'] for v in line))} "
                   f"{fnv(chr(10).join(room['events']).encode('utf-8'))}")
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.abspath(os.path.join(here, "..", ".."))
    tmp = tempfile.mkdtemp(prefix="golden_region_")
    try:
        world.write_region(root, tmp, "./World/Chipsets")
        files = []
        for folder, _, names in sorted(os.walk(tmp)):
            for name in sorted(names):
                path = os.path.join(folder, name)
                files.append(f"rfile {pre} {os.path.relpath(path, tmp)} {file_hash(path)}")
        out += sorted(files)
    finally:
        shutil.rmtree(tmp)
    return out


def main():
    out = []
    for seed in (7, 42):
        world = inf.Infinite(seed)
        out += climate(world, seed)
        out += villages(world, seed)
        if seed == 7:
            rx, ry = inf.start_region(world)
            out += terrain(world, seed, rx, ry)
            if "--no-region" not in sys.argv:
                out += region(seed, rx, ry)
                out += region(seed, rx, ry + 1)       # the coast and the sea (reefs, deep water, ocean chunks)
    print("\n".join(out))


if __name__ == "__main__":
    main()
