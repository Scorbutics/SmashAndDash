#!/usr/bin/env python3
"""shapes : which tiles of the drawn structures are empty, for the C++ port of the generator (it places the tiles, the
images are drawn by Python) : Resources/Worldgen/shapes.txt.

  stamp <name> <columns> <rows> <rows of '#' (a tile) and '.' (empty), joined by '/'>
  furniture <char> <columns> <rows> <extra or -> <rows>

The stamps : the buildings of the villages and of the places (World.stamp) ; the furniture : the pieces of the room
templates (rooms.build_room), each size they come in.

  tools/worldgen/shapes.py [out]           (after a change of the drawings of a structure or of a room template)
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import worldgen as w  # noqa: E402
import rooms  # noqa: E402
import tilegen as g  # noqa: E402
from tilegen import T  # noqa: E402

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")


def mask(img):
    tw, th = img.width // T, img.height // T
    return "/".join("".join("." if g.blank(img.crop((c * T, r * T, c * T + T, r * T + T))) else "#" for c in range(tw))
                    for r in range(th)), tw, th


def furniture():
    sizes = set()
    for kind in ("house", "center", "rest"):
        for _, rows in rooms.templates(kind):
            for char in "BTcPSKC":
                for _, _, rw, rh in rooms.rects(rows, char):
                    th = rh + (1 if char in "SK" else 0)
                    for extra in (["roofb", "roofr", "roofg"] if char == "B" else [None]):
                        sizes.add((char, rw, th, extra))
    return sorted(sizes, key=lambda s: (s[0], s[1], s[2], s[3] or ""))


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "Resources", "Worldgen", "shapes.txt")
    lines = []
    for b in w.BIOMES:                               # the palette of the biome, as the generator draws them
        g.use_biome(b)
    g.use_biome("meadow")
    for name, make in sorted(w.STAMP_MAKERS.items()):
        rows, tw, th = mask(make())
        lines.append(f"stamp {name} {tw} {th} {rows}")
    for char, rw, th, extra in furniture():
        rows, tw, th2 = mask(rooms.furniture(char, rw, th, extra))
        lines.append(f"furniture {char} {rw} {th} {extra or '-'} {rows}")
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")
    print(path)


if __name__ == "__main__":
    main()
