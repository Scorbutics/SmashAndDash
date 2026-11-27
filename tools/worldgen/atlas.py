#!/usr/bin/env python3
"""atlas : every tile the world generator can make, drawn in the colours of each biome, for the C++ port of the
generator (src/src/Game/Gameplay/World/Generation : it places the tiles and blends the biomes, Python draws them) :
Resources/Worldgen/tiles.png and tiles.txt.

The tiles are drawn on a grid of voxels (tilegen : V x V pixels each) : the atlas keeps one pixel per voxel (16 x 16 a
tile). A row of the atlas per tile : 4 biomes (BIOMES order) of 4 frames each (one for a tile that is not animated).
tiles.txt : a line per row, "<frames> <base> <flags> <mask> <key>" : the base of the tile (worldgen.tile_base : -1
flat, n a part of an object standing n blocks lower), its properties for the game (worldgen.tile_flags : HIDEOUT ; both
written in the .prop of the chipsets), its collision mask when it blocks (worldgen.tile_mask : 16 bits, the .png.mask of
the chipsets), the key of the tile (the text of its Python key,
World.build and rooms.build_room), or the name of an animated tile.

  tools/worldgen/atlas.py [folder]      (after a change of the drawings, of the keys or of a room template)
"""
import itertools
import os
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import worldgen as w  # noqa: E402
import rooms  # noqa: E402
import tilegen as g  # noqa: E402
from tilegen import N, T  # noqa: E402

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
GROUNDS = ["sand", "path", "grass", "pave"]


def masks():
    """The masks of mask8 (a diagonal only when its two sides are not set), but the empty one."""
    out = set()
    for bits in itertools.product((False, True), repeat=8):
        out.add(w.mask8(lambda a, b: bits[w.D8.index((a, b))], 0, 0))
    return sorted(m for m in out if any(m))


def keys():
    """The keys of every tile the generator can make (not animated) : (key, walkable)."""
    out = []
    for low in masks():
        for stairs in (False, True):
            for foot in ((False, True) if low[2] else (False,)):
                out.append((("cliff", low, stairs, foot), stairs or not any(low[:4])))
    for over in ("sand", "grass", "pave"):
        for mk in masks():
            out.append((("bank", over, mk), True))
    for m in GROUNDS:
        for west, east, stairs in itertools.product((False, True), repeat=3):
            out.append((("foot", m, west, east, stairs), stairs))
    for m in GROUNDS:
        for over in GROUNDS:
            if w.PRIORITY[over] > w.PRIORITY[m]:
                for mk in masks():
                    for shade in (False, True):
                        out.append((("ground", m, over, mk, shade), True))
    for v in range(4):
        out.append((("grass", v), True))
    for m in GROUNDS:
        for shade in (False, True):
            out.append((("ground", m, None, None, shade, 0), True))
    for name, make in w.STAMP_MAKERS.items():
        img = make()
        for r in range(img.height // T):
            for c in range(img.width // T):
                if not g.blank(w.crop(img, c, r)):
                    out.append((("stamp", name, c, r), r == 0))
    for style, (width, parts) in w.TREE_SHAPES.items():
        for c in range(width):
            for dy, row, kind in parts:
                out.append((("tree", style, row, c), kind == "top"))
    for c in range(2):
        for r in range(2):
            out.append((("emblem", c, r), True))
    out.append((("lamp", 1), False))
    for kind in ("tl", "tr", None, "v"):
        out.append((("fence", kind), False))
    for colour in w.FLOWERBED_COLOURS:
        out.append((("flowerpot", colour), False))
    for kind in ("bench", "bin", "mailbox", "barrel", "bush", "well", "lily", "boulder", "log seat", "sign"):
        out.append(((kind,), False))
    out.append((("crops",), True))
    for kind in ("house", "center", "rest"):
        out.append((("room floor", kind), True))
    out.append((("room wall top",), False))
    for item in (None, "W", "F"):
        for paper in ("white", "wall"):
            out.append((("room wall", item, paper), False))
    for mk in itertools.product((False, True), repeat=4):
        out.append((("room rug", mk), True))
    out.append((("room mat",), True))
    for line in open(os.path.join(ROOT, "Resources", "Worldgen", "shapes.txt")):
        words = line.split()
        if words[0] != "furniture":
            continue
        char, rw, th, extra = words[1], int(words[2]), int(words[3]), None if words[4] == "-" else words[4]
        for r, row in enumerate(words[5].split("/")):
            for c, filled in enumerate(row):
                if filled == "#":
                    out.append((("room furniture", char, rw, th, extra, c, r), False))
    out.append((("room furniture", "H"), False))
    return out


def voxels(img):
    """A tile at one pixel per voxel."""
    return img.convert("RGB").resize((N, N), Image.NEAREST)


def main():
    folder = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "Resources", "Worldgen")
    entries = [(name, 4, (w.tile_base(name), w.tile_flags(name), w.tile_mask(name, False)), lambda f, n=name: w.ANIMATED[n](f))
               for name in w.ANIMATED]
    seen = set()
    for key, walkable in keys():
        if repr(key) in seen:
            raise SystemExit(f"atlas : the key {key!r} twice")
        seen.add(repr(key))
        entries.append((repr(key), 1, (w.tile_base(key), w.tile_flags(key), w.tile_mask(key, False)),
                        lambda f, k=key, wk=walkable: w.draw_key(k, wk)))
    atlas = Image.new("RGB", (len(w.BIOMES) * 4 * N, len(entries) * N), g.KEY)
    for b, biome in enumerate(w.BIOMES):
        g.use_biome(biome)
        for row, (_, frames, _, draw) in enumerate(entries):
            for f in range(frames):
                atlas.paste(voxels(draw(f)), ((b * 4 + f) * N, row * N))
    g.use_biome("meadow")
    atlas.save(os.path.join(folder, "tiles.png"), optimize=True)
    with open(os.path.join(folder, "tiles.txt"), "w") as out:
        out.write("".join(f"{frames} {base} {flags} {mask} {name}\n" for name, frames, (base, flags, mask), _ in entries))
    print(f"{folder}/tiles.png : {len(entries)} tiles")


if __name__ == "__main__":
    main()
