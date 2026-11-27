#!/usr/bin/env python3
"""tileroom : a test map with every tile of the generator (its objects, trees, buildings, grounds, cliffs, water, the
furniture of the rooms), placed by the code of the generator (worldgen.World), to walk around them in the game : what
blocks, what is drawn over the characters.

  tools/worldgen/tileroom.py <World folder of a save> [--name w_tiles]

It writes the map <name> and its chipset there (ex : the save of a run : /tmp/ska-runs-<uid>/<id>/Saves/save1/World),
then prints each exhibit and the block in front of it (south) :

  tools/sad-run up tiles
  tools/worldgen/tileroom.py /tmp/ska-runs-$(id -u)/tiles/Saves/save1/World
  external/ska/tools/ska-console --run tiles run tools/agent/warp.miniska w_tiles 3 6

The walls of the game against the blocks the generator made walkable (tools/agent/walls.miniska : its rows in a file) :

  external/ska/tools/ska-console --run tiles run tools/agent/warp.miniska w_tiles 1 1
  external/ska/tools/ska-console --run tiles run tools/agent/walls.miniska 52 54 > walls.json
  tools/worldgen/tileroom.py --walls walls.json --at 1 1       (--at : the block of the player, a wall for the game)

It checks that it shows every kind of tile the generator draws (draw_key, the animated tiles, the trees, the
structures, the furniture) : a new kind of tile not placed here fails it (place it in lay_out). And the depth of the
tiles (worldgen.tile_base, the property "base" of the chipsets : the game draws a standing tile sorted with the
characters at the base line of its object) : each tile of the top layer over a walkable block stands over the blocking
base of its object (a lamp post, a trunk, the walls of a building) ; no tile of the top layer is flat ; an object that
blocks the way (on the middle layer, out of the water) is not flat (drawn under the characters behind it).
"""
import argparse
import inspect
import json
import os
import re
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import worldgen as w  # noqa: E402
import rooms  # noqa: E402

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
W, H = 52, 54

# The interior : every piece of furniture of the room templates (Resources/Worldgen/rooms/README.md)
ROOM = ["############",
        "=W==F==W====",
        "S..K...P...B",
        "...........B",
        "..cTTc......",
        ".RRRR..CCH..",
        "............",
        ".....EE....."]


class TileRoom(w.World):
    """A flat meadow (no climate, no villages) : the generator places the exhibits on it."""

    def __init__(self):
        self.seed, self.cw, self.ch = 7, 1, 1
        self.W, self.H = W, H
        self.near_village, self.routes, self.stairs = {}, set(), set()
        self.layers = {l: [[None] * W for _ in range(H)] for l in ("", "M", "T")}
        self.walk = [[True] * W for _ in range(H)]
        self.taken, self.objects = set(), {}
        self.chip = w.Tiles()
        self.terrain = [[("grass", 0)] * W for _ in range(H)]
        self.exhibits = []

    def in_town(self, x, y, r=3):
        return True                                  # no flowers here and there : only the exhibits

    def show(self, name, x, y):
        """An exhibit : where to stand to see it (x, y : a free block in front of it)."""
        self.exhibits.append((name, x, y))

    def lay_out(self):
        # The grounds : a terrace with stairs, a pond, sand, a path, paving
        for y in range(36, 42):
            for x in range(3, 15):
                self.terrain[y][x] = ("grass", 1)
        for y in range(37, 42):
            for x in range(18, 25):
                self.terrain[y][x] = ("water", 0)
        for y in range(37, 41):
            for x in range(28, 33):
                self.terrain[y][x] = ("sand", 0)
        for x in range(36, 46):
            self.terrain[37][x] = self.terrain[38][x] = ("path", 0)
            self.terrain[40][x] = self.terrain[41][x] = ("pave", 0)
        self.stairs = {next((x, 41) for x in range(5, 14) if self.stair_ok(x, 41))}
        self.ground()
        for name, x, y in (("terrace (cliffs, stairs)", 8, 44), ("pond (banks)", 21, 43), ("sand", 30, 42),
                           ("path, paving", 40, 43)):
            self.show(name, x, y)
        for x, y in ((19, 38), (23, 40)):
            self.set("M", x, y, self.chip.cell(("lily",), False))

        # The objects of the villages and of the places, one block apart
        x = 3
        self.lamp(x, 4)
        self.show("lamp", x, 5)
        objects = [("bench",), ("bin",), ("mailbox",)] + [("flowerpot", c) for c in w.FLOWERBED_COLOURS] \
            + [("barrel",), ("bush",), ("well",), ("boulder",), ("log seat",), ("sign",), w.CAMPFIRE_CELL]
        for key in objects:
            x += 3
            self.put_object("M", x, 4, key)
            self.show(key if isinstance(key, str) else " ".join(map(str, key)), x, 5)
        for c in range(2):
            for r in range(2):
                self.put_object("M", 3 + c, 8 + r, f"fountain_{c}_{r}")
                self.set("M", 8 + c, 8 + r, self.chip.cell(("emblem", c, r), True))
        self.show("fountain", 3, 10)
        self.show("emblem", 8, 10)
        for i, colour in enumerate(w.FLOWERBED_COLOURS):
            self.set("M", 12 + 2 * i, 9, f"flowerbed_{colour}")
        self.show("flowerbeds", 14, 10)
        for i in range(3):
            for j in range(2):
                self.tall_grass(22 + i, 8 + j)
        self.show("tall grass", 23, 10)
        self.set("", 27, 9, w.FLOWER_CELL)
        self.set("", 29, 9, "butterfly")
        self.show("flowers, butterfly", 28, 10)
        # A vegetable garden (World.decorate) : fenced on three sides
        for i in range(6):
            self.set("M", 32 + i, 8, self.chip.cell(("fence", "tl" if i == 0 else "tr" if i == 5 else None), False))
            self.walk[8][32 + i] = False
        for i in (0, 5):
            self.set("M", 32 + i, 9, self.chip.cell(("fence", "v"), False))
            self.walk[9][32 + i] = False
        for i in range(1, 5):
            self.set("M", 32 + i, 9, self.chip.cell(("crops",), True))
        self.show("garden (fences, crops)", 34, 10)

        # The trees, a column of free blocks between them
        x = 3
        for style, (width, _) in w.TREE_SHAPES.items():
            if not self.tree_at(x, 16, style):
                raise SystemExit(f"tileroom : no room for the tree {style}")
            self.show(f"tree {style}", x, 18)
            x += width + 3

        # The structures : the buildings of each roof (the meadow), the healing centre, the tents, the pillars
        x = 3
        for name, door in [(f"house_{roof}_meadow", (2, 3)) for roof in w.ROOFS] + [("center_meadow", (2, 4))] \
                + [(f"tent_{c}", None) for c in ("roofo", "roofg", "roofb", "roofr")] \
                + [("pillar_False", None), ("pillar_True", None)]:
            img = w.STAMP_MAKERS[name]()
            self.stamp(name, x, 23, door)
            self.show(name, x + (door[0] if door else 0), 23 + img.height // w.T)
            x += img.width // w.T + 1
            if x > W - 2:
                raise SystemExit("tileroom : the map is too narrow for the structures")

        # The furniture of the rooms, on their floor and walls
        layers, walk, _, _, _ = rooms.build_room(self.chip, "house", ROOM, 1)
        ox, oy = 20, 45
        for l, grid in layers.items():
            for y, line in enumerate(grid):
                for x, cell in enumerate(line):
                    self.set(l, ox + x, oy + y, cell)
                    if l == "":
                        self.set("M", ox + x, oy + y, None)
        for y, line in enumerate(walk):
            for x, ok in enumerate(line):
                self.walk[oy + y][ox + x] = ok
        self.show("room furniture", ox + 6, oy + 6)

    # Checks ------------------------------------------------------------------------------------------------------
    def kinds(self):
        """The kinds of tiles shown : the first word of the keys, the animated tiles, the families of structures."""
        used = {cell for l in self.layers.values() for line in l for cell in line if cell is not None}
        keys = {key for (key, _), tid in self.chip.ids.items() if tid in used}
        out = {key[0] for key in keys} | {c for c in used if isinstance(c, str)}
        out |= {f"stamp {key[1].split('_')[0]}" for key in keys if key[0] == "stamp"}
        out |= {f"tree {key[1]}" for key in keys if key[0] == "tree"}
        out |= {f"furniture {key[1]}" for key in keys if key[0] == "room furniture"}
        return out

    @staticmethod
    def expected():
        """Every kind of tile the generator draws (read from its code : a new kind is expected here at once)."""
        source = inspect.getsource(w.draw_key) + inspect.getsource(rooms.draw_key)
        out = set(re.findall(r'kind == "([^"]+)"', source))
        out |= set(re.findall(r'"([a-z ]+)": g\.', inspect.getsource(w.draw_key)))
        out |= set(w.ANIMATED)
        out |= {f"stamp {name.split('_')[0]}" for name in w.STAMP_MAKERS}
        out |= {f"tree {style}" for style in w.TREE_SHAPES}
        out |= {f"furniture {c}" for c in re.findall(r'kind == "(\w)"', inspect.getsource(rooms.furniture))}
        return out

    def floating_tops(self):
        """The tiles of the top layer over a walkable block with no blocking base under them (3 blocks down). The front
        leaves of the tall grass are not a top : they cover the legs of the character in the grass."""
        return [(x, y) for y in range(H) for x in range(W)
                if self.layers["T"][y][x] not in (None, w.TALLFRONT_CELL) and self.walk[y][x]
                and not any(0 <= y + k < H and not self.walk[y + k][x] for k in (1, 2, 3))]

    def wall_mismatches(self, rows, player=None):
        """The blocks where the walls of the game (rows : "." free, "#" a wall or a character) are not what the
        generator wants (the water aside : free for the physics, its surf script stops the walk)."""
        out = []
        for y, row in enumerate(rows[:H]):
            for x, c in enumerate(row[:W]):
                if self.terrain[y][x][0] == "water" or self.walk[y][x] == (c == ".") or (x, y) == player:
                    continue
                near = min(self.exhibits, key=lambda e: abs(e[1] - x) + abs(e[2] - y))[0]
                out.append(f"({x}, {y}) {'free' if c == '.' else 'a wall'} in the game, "
                           f"{'walkable' if self.walk[y][x] else 'blocked'} for the generator (near : {near})")
        return out

    def flat_objects(self):
        """The tiles that should stand but are flat (worldgen.tile_base) : on the top layer, or blocking on the middle
        layer out of the water."""
        out = []
        keys = {tid: key for (key, _), tid in self.chip.ids.items()}
        for y in range(H):
            for x in range(W):
                for layer in ("M", "T"):
                    cell = self.layers[layer][y][x]
                    if cell is None:
                        continue
                    base = w.tile_base(cell if isinstance(cell, str) else keys[cell])
                    blocking = layer == "M" and not self.walk[y][x] and self.terrain[y][x][0] != "water"
                    if base == w.FLAT and (layer == "T" or blocking):
                        out.append((x, y, cell if isinstance(cell, str) else keys[cell]))
        return out

    # Output -----------------------------------------------------------------------------------------------------
    def write_map(self, world, name):
        corr = w.correspondance(f"{ROOT}/Resources/Chipsets/corr.png")
        colour = {(x, y): corr.getpixel((x, y)) for x in range(corr.width) for y in range(corr.height)}
        chips = os.path.join(world, "Chipsets")
        os.makedirs(chips, exist_ok=True)
        chipset = f"chipset_{name}"
        for old in (f"{chips}/{chipset}/Scripts", f"{chips}/{chipset}"):
            if os.path.isdir(old):
                for f in os.listdir(old):
                    if os.path.isfile(os.path.join(old, f)):
                        os.remove(os.path.join(old, f))
        if os.path.isdir(f"{chips}/{chipset}/Scripts"):
            os.rmdir(f"{chips}/{chipset}/Scripts")
        if os.path.isdir(f"{chips}/{chipset}"):
            os.rmdir(f"{chips}/{chipset}")
        used = {(c, w.MEADOW) for l in self.layers.values() for line in l for c in line if c is not None}
        chip = w.Chipset(self.chip, used)
        chip.save(chips, chipset, f"{ROOT}/Resources/Chipsets/tile_scripts")
        folder = os.path.join(world, name)
        os.makedirs(folder, exist_ok=True)
        for l in ("", "M", "T"):
            im = Image.new("RGB", (W, H), w.EMPTY)
            for y in range(H):
                for x in range(W):
                    c = self.layers[l][y][x]
                    if c is not None:
                        im.putpixel((x, y), colour[chip.cells[(c, w.MEADOW)]])
            im.save(f"{folder}/{name}{l}.bmp")
        light = Image.new("RGB", (W + 1, H + 1))
        for j in range(H + 1):
            for i in range(W + 1):
                light.putpixel((i, j), self.corner_light(i, j))
        light.save(f"{folder}/{name}L.png")
        with open(f"{folder}/{name}.ini", "w") as f:
            f.write(f"[Chipset]\nfile={os.path.abspath(chips)}/{chipset}.png\n\n"
                    "[Bgm]\nfile=./Music/Greencity.mp3\nvolume=1\n")
        with open(f"{folder}/{name}E.txt", "w", encoding="utf-8") as f:
            f.write("x y declenchement parametresSupplementaires\n")
        open(f"{folder}/scripts.data", "w").close()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("world", nargs="?", help="the World folder of a save (none : the checks only)")
    ap.add_argument("--name", default="w_tiles")
    ap.add_argument("--walls", help="the reply of tools/agent/walls.miniska on the map (or its rows) : compared with "
                                    "the blocks the generator made walkable")
    ap.add_argument("--at", nargs=2, type=int, metavar=("X", "Y"), help="the block of the player (--walls)")
    a = ap.parse_args()
    room = TileRoom()
    room.lay_out()
    failed = False
    missing = sorted(TileRoom.expected() - room.kinds())
    if missing:
        print("tileroom : kinds of tiles not shown : " + ", ".join(missing))
        failed = True
    floating = room.floating_tops()
    if floating:
        print("tileroom : tiles over the characters with no blocking base under them : "
              + ", ".join(f"({x}, {y})" for x, y in floating))
        failed = True
    flat = room.flat_objects()
    if flat:
        print("tileroom : objects with no base (worldgen.tile_base : add their kind to STANDING) : "
              + ", ".join(f"({x}, {y}) {key}" for x, y, key in flat))
        failed = True
    if a.walls:
        with open(a.walls) as f:
            text = f.read()
        if text.lstrip().startswith("{"):           # the reply of ska-console
            text = json.loads(text)["value"]
        rows = [line.strip() for line in text.splitlines() if line.strip() and set(line.strip()) <= set(".#")]
        mismatches = room.wall_mismatches(rows, tuple(a.at) if a.at else None)
        for line in mismatches:
            print("tileroom : " + line)
        if len(rows) < H:
            print(f"tileroom : {len(rows)} rows of walls, the map has {H}")
            failed = True
        elif mismatches:
            failed = True
        else:
            print(f"walls : the game blocks what the generator blocks ({W} x {H} blocks)")
    if a.world:
        room.write_map(a.world, a.name)
        print(f"{a.name} : {W} x {H} blocks in {os.path.abspath(a.world)}")
    for name, x, y in room.exhibits:
        print(f"{name} : {x} {y}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
