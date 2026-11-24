"""rooms : the inside of the buildings, from the templates of tools/worldgen/rooms (see its README).

A room is drawn with the tiles of the world (tilegen style) : the floor and the walls on the ground layer, the
furniture on the middle layer (the floor shows around it). build_room gives its layers, the blocks the player can walk
on, its exit mats and its healing counter.
"""
import math
import os

import tilegen as g
from tilegen import Scene, N, T, h01

ROOMS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "rooms")
WALKABLE = set(".ERe")


def templates(kind):
    """The templates of a kind of building ("house", "center"), sorted : [(name, rows)]."""
    out = []
    for name in sorted(os.listdir(ROOMS)):
        if name.startswith(kind + "_") and name.endswith(".txt"):
            with open(os.path.join(ROOMS, name)) as f:
                rows = [line.rstrip("\n") for line in f if line.strip()]
            width = len(rows[0])
            if any(len(r) != width for r in rows):
                raise SystemExit(f"worldgen : room {name} : its rows do not have the same length")
            out.append((name[:-4], rows))
    return out


# ---------------------------------------------------------------- tiles

def floor(kind):
    s = Scene(N, N)
    if kind == "center":                            # tiles : a checker of light stone
        for y in range(N):
            for x in range(N):
                t = 3 if ((x // 8) + (y // 8)) % 2 == 0 else 2
                if x % 8 == 7 or y % 8 == 7:
                    t = 1
                s.put(x, y, "white", 0, 1, t)
    else:                                            # planks, their joints staggered
        for y in range(N):
            row = y // 4
            for x in range(N):
                joint = (x + row * 6) % 16 == 0
                t = 1 if y % 4 == 3 or joint else 3 if y % 4 == 0 and h01(x // 4, row, 5) < 0.2 else 2
                s.put(x, y, "wood", 0, 1, t)
    return s


def floor_tile(kind):
    return floor(kind).render()


def wall_top():
    s = Scene(N, N)
    for y in range(N):
        for x in range(N):
            s.put(x, y, "ink", 0, 1, 0)
    for x in range(N):
        s.put(x, N - 2, "wood", 0, 1, 0)
        s.put(x, N - 1, "wood", 0, 1, 2)
    return s.render()


def wall_face(item, paper):
    """The back wall : striped wallpaper, a baseboard ; a window or a picture on it."""
    s = Scene(N, N)
    for y in range(N):
        for x in range(N):
            s.put(x, y, paper, 0, 1, 3 if x % 4 == 0 else 2)
    for x in range(N):
        s.put(x, 0, paper, 0, 1, 1)                  # the shade under the top of the wall
        s.put(x, N - 3, "wood", 0, 1, 2)
        s.put(x, N - 2, "wood", 0, 1, 1)
        s.put(x, N - 1, "wood", 0, 1, 0)
    if item == "W":
        g.window(s, 3, 3, 13, 11)
    elif item == "F":
        g.rect(s, 4, 3, 12, 10, "wood", 1)
        g.rect(s, 5, 4, 11, 9, "glass", 3)          # a landscape : sky, a hill
        for x in range(5, 11):
            for y in range(7 - (1 if 6 < x < 10 else 0), 9):
                s.put(x, y, "grass", 0, 1, 2)
    return s.render()


def rug(mask):
    """A piece of rug over the floor : its border where the rug ends (mask : rug at N E S W)."""
    s = floor("house")
    for y in range(N):
        for x in range(N):
            edge = (not mask[0] and y < 2) or (not mask[1] and x > N - 3) or (not mask[2] and y > N - 3) \
                or (not mask[3] and x < 2)
            inner = (not mask[0] and y < 4) or (not mask[1] and x > N - 5) or (not mask[2] and y > N - 5) \
                or (not mask[3] and x < 4)
            t = 1 if edge else 3 if inner else 2 if (x + y) % 6 else 3
            s.put(x, y, "roofr", 0, 1, t)
    return s.render()


def mat():
    s = floor("house")
    g.rect(s, 1, 3, N - 1, N - 3, "orange", 2)
    g.frame(s, 1, 3, N - 1, N - 3, "orange", 1)
    for x in range(4, N - 4, 2):
        s.put(x, 8, "orange", 0, 1, 3)
    return s.render()


def furniture(kind, w, h, extra=None):
    """A piece of furniture, w x h blocks (the wall pieces : one block more on top, over the wall face)."""
    W, H = w * N, h * N
    s = Scene(W, H)
    if kind == "B":                                  # bed : the head against the wall, a pillow, a blanket
        g.rect(s, 1, 1, W - 1, H - 2, "wood", 1)
        g.rect(s, 2, 3, W - 2, 10, "white", 3)
        g.rect(s, 2, 10, W - 2, H - 3, extra or "roofb", 2)
        for y in range(10, H - 3, 3):
            for x in range(2, W - 2):
                s.put(x, y, extra or "roofb", 0, 0, 3 if y == 10 else 1)
        g.rect(s, 1, H - 3, W - 1, H - 1, "wood", 0)
    elif kind == "T":                                # table : the top, its front edge, the legs
        g.rect(s, 1, 2, W - 1, H - 5, "wood", 3)
        for x in range(2, W - 2, 6):
            s.put(x, 4, "wood", 0, 0, 2)
        g.rect(s, 1, H - 5, W - 1, H - 3, "wood", 1)
        for x in (2, W - 4):
            g.rect(s, x, H - 3, x + 2, H, "wood", 0)
    elif kind == "c":                                # stool
        s.ellipsoid(8, 9, 5, 3.5, "wood", 2.0)
        g.rect(s, 5, 11, 7, 15, "wood", 0)
        g.rect(s, 9, 11, 11, 15, "wood", 0)
    elif kind == "P":                                # plant : leaves over a pot
        g.rect(s, 4, 9, 12, 15, "orange", 2)
        g.rect(s, 4, 9, 12, 10, "orange", 3)
        s.ellipsoid(8, 6, 6, 5, "leaf", 5.0)
        s.ellipsoid(6, 4, 3, 3, "leaf", 6.0)
    elif kind == "S":                                # bookshelf : the top over the wall, rows of books
        g.rect(s, 1, 2, W - 1, H, "wood", 1)
        for shelf in range(4, H - 2, 6):
            for x in range(2, W - 2):
                book = h01(x, shelf, 3)
                mat_ = "roofr" if book < 0.3 else "roofb" if book < 0.6 else "roofg" if book < 0.8 else "orange"
                for y in range(shelf, shelf + 4):
                    s.put(x, y, mat_, 0, 0, 3 if y == shelf else 2)
            g.rect(s, 2, shelf + 4, W - 2, shelf + 5, "wood", 0)
    elif kind == "K":                                # kitchen : cupboards on the wall, the counter, the oven
        g.rect(s, 1, 2, W - 1, 12, "wood", 2)
        for x in range(1, W - 1, 8):
            g.frame(s, x, 2, x + 8, 12, "wood", 1)
        g.rect(s, 0, N + 2, W, N + 4, "stone", 3)
        g.rect(s, 1, N + 4, W - 1, H, "wood", 2)
        for x in range(1, W - 1, 8):
            g.frame(s, x, N + 5, x + 8, H, "wood", 1)
        g.rect(s, W - 7, N + 6, W - 2, N + 12, "glass", 1)
    elif kind == "C":                                # healing counter : white, an orange band
        g.rect(s, 0, 3, W, H - 1, "white", 2)
        g.rect(s, 0, 3, W, 6, "white", 3)
        g.rect(s, 0, H - 7, W, H - 5, "orange", 2)
        g.rect(s, 0, H - 1, W, H, "white", 0)
    elif kind == "H":                                # healing machine : a glass dome on the counter
        g.rect(s, 2, 8, W - 2, H - 2, "white", 3)
        s.ellipsoid(W / 2, 6, 5, 4, "glass", 4.0)
        s.put(W // 2, 11, "orange", 0, 0, 3)
    g.ink_outline(s)
    return s.render()


# ---------------------------------------------------------------- a room

def rects(rows, char):
    """The rectangles of a letter (each a connected group, read as its bounding box)."""
    seen, out = set(), []
    for y, row in enumerate(rows):
        for x, c in enumerate(row):
            if c != char or (x, y) in seen:
                continue
            group, front = [(x, y)], [(x, y)]
            seen.add((x, y))
            while front:
                a, b = front.pop()
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = a + dx, b + dy
                    if 0 <= ny < len(rows) and 0 <= nx < len(rows[ny]) and rows[ny][nx] == char \
                            and (nx, ny) not in seen:
                        seen.add((nx, ny))
                        group.append((nx, ny))
                        front.append((nx, ny))
            xs, ys = [p[0] for p in group], [p[1] for p in group]
            out.append((min(xs), min(ys), max(xs) - min(xs) + 1, max(ys) - min(ys) + 1))
    return out


def build_room(chip, kind, rows, seed):
    """The layers of a room : {"": grid, "M": grid}, walk grid, exits [(x, y)], counter blocks [(x, y)], beds [(x, y)]."""
    w, h = len(rows[0]), len(rows)
    base = [[None] * w for _ in range(h)]
    mid = [[None] * w for _ in range(h)]
    walk = [[c in WALKABLE for c in row] for row in rows]
    paper = "white" if kind == "center" else "wall"
    floor_cell = chip.cell(("room floor", kind), lambda: floor_tile(kind), True)          # planks but in a centre
    for y, row in enumerate(rows):
        for x, c in enumerate(row):
            if c == "#":
                base[y][x] = chip.cell(("room wall top",), wall_top, False)
                walk[y][x] = False
            elif c in "=WF":
                item = c if c in "WF" else None
                base[y][x] = chip.cell(("room wall", item, paper), lambda: wall_face(item, paper), False)
                walk[y][x] = False
            elif c == "R":
                mk = tuple(0 <= y + dy < h and 0 <= x + dx < w and rows[y + dy][x + dx] == "R"
                           for dx, dy in ((0, -1), (1, 0), (0, 1), (-1, 0)))
                base[y][x] = chip.cell(("room rug", mk), lambda: rug(mk), True)
            elif c == "E":
                base[y][x] = chip.cell(("room mat",), mat, True)
            else:
                base[y][x] = floor_cell
    blanket = ["roofb", "roofr", "roofg"][int(h01(seed, 1, 7) * 3)]
    counter = []
    for char in "BTcPSKC":
        for x0, y0, rw, rh in rects(rows, char):
            on_wall = char in "SK"
            top = y0 - 1 if on_wall else y0
            th = rh + (1 if on_wall else 0)
            extra = blanket if char == "B" else None
            img = furniture(char, rw, th, extra)
            for r in range(th):
                for c in range(rw):
                    tile = img.crop((c * T, r * T, c * T + T, r * T + T))
                    if g.blank(tile):
                        continue
                    mid[top + r][x0 + c] = chip.cell(("room furniture", char, rw, th, extra, c, r), lambda: tile, False)
                    walk[top + r][x0 + c] = False
            if char == "C":
                counter += [(x, y0 + rh - 1) for x in range(x0, x0 + rw)]
    for x0, y0, rw, rh in rects(rows, "H"):
        img = furniture("H", 1, 1)
        mid[y0][x0] = chip.cell(("room furniture", "H"), lambda: img, False)
        walk[y0][x0] = False
    exits = [(x, y) for y, row in enumerate(rows) for x, c in enumerate(row) if c == "E"]
    beds = [(x, y) for y, row in enumerate(rows) for x, c in enumerate(row) if c == "B"]
    return {"": base, "M": mid}, walk, exits, counter, beds


def light(rows, walk):
    """The light of a room, one value per block corner : the shade of the back wall and of the furniture."""
    w, h = len(rows[0]), len(rows)
    out = []
    for j in range(h + 1):
        line = []
        for i in range(w + 1):
            blocked = sum(1 for dx in (-1, 0) for dy in (-1, 0)
                          if not (0 <= i + dx < w and 0 <= j + dy < h) or not walk[j + dy][i + dx])
            near_wall = max(0, 4 - j) / 4               # the back wall shades the floor in front of it
            l = 0.99 - 0.035 * blocked - 0.05 * near_wall
            l = max(0.85, min(1.0, l))
            line.append((int(255 * l * 0.98), int(255 * l * 0.99), int(255 * l)))
        out.append(line)
    return out
