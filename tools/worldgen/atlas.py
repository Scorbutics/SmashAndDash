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
The painted tiles (tilegen.painted : rendered by tools/render3d, not voxels) keep their pixels : painted.png, their frames
packed in cells of 48 px, PAINTED_COLUMNS a row ; painted.txt a line per tile and biome, "<biome> <cell> <key>" : the
cell of its first frame (the others follow it). The grounds and the banks of the water painted are not there : the C++
composes them as worldgen.painted_ground and worldgen.water_overlay do, from the painted grounds of each biome (in
painted.txt as "texture <name>" : ground_sand, ground_grass_0...) and the masks of their edges (masks.png, a cell of
48 px a mask, white inside ; masks.txt a line per cell, "<cell> <straight 0/1> <8 bits N E S W NE SE SW NW>" :
worldgen.smooth_intrusion).

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
    for kind in ("bench", "bin", "mailbox", "barrel", "bush", "well", "lily", "boulder", "log seat", "sign", "reef"):
        out.append(((kind,), False))
    for v in range(3):
        out.append((("deep", v), True))
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


PAINTED_COLUMNS = 16
# The painted grounds the C++ composes the grounds and the banks with (tilegen.ground_texture)
TEXTURES = [f"ground_grass_{v}" for v in range(g.GRASS_VARIANTS)] + ["ground_sand", "ground_path", "ground_pave"]


def texture_of(mat, variant=0):
    return f"ground_grass_{variant % g.GRASS_VARIANTS}" if mat == "grass" else f"ground_{mat}"


def composed(key):
    """A ground or a bank the C++ composes in the current biome (worldgen.painted_ground, water_overlay) : its painted
    grounds are there."""
    if isinstance(key, str) or key[0] not in ("ground", "bank"):
        return False
    if key[0] == "bank":
        mats, variant = [key[1]], 0
    elif len(key) == 6:
        mats, variant = [key[1]], key[5]
    else:
        mats, variant = [key[1], key[2]], 0
    return all(g.painted(texture_of(m, variant)) is not None for m in mats)


def is_painted(img):
    """Not a voxel drawing : its pixels are not the same in each voxel (a painted tile, tilegen.painted)."""
    img = img.convert("RGB")
    return img.tobytes() != voxels(img).resize((T, T), Image.NEAREST).tobytes()


def main():
    folder = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "Resources", "Worldgen")
    entries = [(name, 4, (w.tile_base(name), w.tile_flags(name), w.tile_mask(name, False)), lambda f, n=name: w.ANIMATED[n](f), None)
               for name in w.ANIMATED]
    seen = set()
    for key, walkable in keys():
        if repr(key) in seen:
            raise SystemExit(f"atlas : the key {key!r} twice")
        seen.add(repr(key))
        entries.append((repr(key), 1, (w.tile_base(key), w.tile_flags(key), w.tile_mask(key, False)),
                        lambda f, k=key, wk=walkable: w.draw_key(k, wk), key))
    atlas = Image.new("RGB", (len(w.BIOMES) * 4 * N, len(entries) * N), g.KEY)
    painted = []
    for b, biome in enumerate(w.BIOMES):
        g.use_biome(biome)
        for row, (name, frames, _, draw, key) in enumerate(entries):
            images = [draw(f) for f in range(frames)]
            for f, img in enumerate(images):
                atlas.paste(voxels(img), ((b * 4 + f) * N, row * N))
            if any(is_painted(img) for img in images) and not (key is not None and composed(key)):
                painted.append((b, name, [img.convert("RGB") for img in images]))
        for name in TEXTURES:
            image = g.painted(name)
            if image is not None:
                painted.append((b, f"texture {name}", [image.convert("RGB")]))
    g.use_biome("meadow")
    atlas.save(os.path.join(folder, "tiles.png"), optimize=True)
    cells = sum(len(images) for _, _, images in painted)
    sheet = Image.new("RGB", (PAINTED_COLUMNS * T, max(1, (cells + PAINTED_COLUMNS - 1) // PAINTED_COLUMNS) * T), g.KEY)
    lines, cell = [], 0
    for b, name, images in painted:
        lines.append(f"{b} {cell} {name}\n")
        for img in images:
            sheet.paste(img, ((cell % PAINTED_COLUMNS) * T, (cell // PAINTED_COLUMNS) * T))
            cell += 1
    sheet.save(os.path.join(folder, "painted.png"), optimize=True)
    with open(os.path.join(folder, "painted.txt"), "w") as out:
        out.write("".join(lines))
    with open(os.path.join(folder, "tiles.txt"), "w") as out:
        out.write("".join(f"{frames} {base} {flags} {mask} {name}\n" for name, frames, (base, flags, mask), _, _ in entries))
    # The masks of the edges of the grounds (worldgen.smooth_intrusion), rounded and straight (a paving)
    shapes = [(straight, m) for straight in (False, True) for m in masks()]
    sheet = Image.new("RGB", (PAINTED_COLUMNS * T, (len(shapes) + PAINTED_COLUMNS - 1) // PAINTED_COLUMNS * T), (0, 0, 0))
    with open(os.path.join(folder, "masks.txt"), "w") as out:
        for cell, (straight, m) in enumerate(shapes):
            sheet.paste(w.smooth_intrusion(m, straight).convert("RGB"), ((cell % PAINTED_COLUMNS) * T, (cell // PAINTED_COLUMNS) * T))
            out.write(f"{cell} {int(straight)} {''.join('1' if b else '0' for b in m)}\n")
    sheet.save(os.path.join(folder, "masks.png"), optimize=True)
    print(f"{folder}/tiles.png : {len(entries)} tiles, {len(painted)} painted (painted.png)")


if __name__ == "__main__":
    main()
