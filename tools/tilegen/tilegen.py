"""tilegen : voxel-style tiles for Smash & Dash, generated from code.

Every tile is a small voxel scene (16x16 voxels of 3x3 pixels, 48 px) : each voxel has a material and a height or a
tone ; the renderer lights it from the top-left and picks one of the flat tones of the material (no gradient inside a
voxel). The style (palette, light, outline) lives here, not in the description of the tiles.

Structures (bridge, houses, trees) are drawn whole, several tiles wide ; tools/worldgen cuts them into the cells of
its chipset. A library : tools/worldgen imports it.
"""
import math
import os
from PIL import Image

T = 48          # tile size (px)
V = 3           # voxel size (px)
N = T // V      # voxels per tile side
KEY = (190, 151, 255)

# Material ramps : darkest .. lightest. The light picks the tone.
# Ramps after Cube World : yellow-lime lights, plain greens, deep same-hue shadows ; earthy rock and wood, calm water.
# Saturated, but the hue moves only a little along a ramp (warmer in the light)
PAL = {
    "ink": [(28, 40, 26)] * 4,
    # (the grass : its middle tone the painted grass of the meadow, tools/render3d tiles.js : the voxel drawings not
    # painted yet stand on the same ground)
    "grass": [(78, 138, 40), (110, 168, 48), (140, 198, 58), (188, 226, 98)],
    "grass2": [(34, 122, 26), (60, 152, 34), (104, 194, 54), (168, 232, 80)],
    "water": [(22, 74, 150), (32, 112, 196), (58, 152, 224), (156, 212, 244)],
    "rock": [(92, 70, 52), (150, 116, 76), (196, 162, 106), (232, 210, 150)],
    "stone": [(84, 90, 96), (130, 138, 140), (178, 184, 180), (226, 230, 220)],
    "leaf": [(16, 74, 40), (32, 116, 52), (66, 160, 66), (130, 206, 98)],
    "pave": [(98, 102, 112), (150, 154, 162), (190, 194, 200), (226, 228, 232)],
    "lamp": [(40, 44, 58), (66, 72, 92), (100, 108, 130), (150, 160, 184)],
    "soil": [(86, 56, 34), (124, 84, 52), (160, 116, 74), (196, 156, 108)],
    "wood": [(92, 58, 34), (146, 96, 52), (190, 140, 78), (228, 190, 120)],
    "roof": [(30, 84, 110), (42, 122, 146), (66, 160, 172), (138, 206, 204)],
    "roofr": [(118, 40, 36), (176, 66, 48), (214, 104, 66), (240, 156, 102)],
    "roofb": [(40, 58, 124), (56, 92, 172), (84, 132, 208), (148, 186, 236)],
    "brick": [(100, 44, 38), (152, 70, 52), (192, 102, 70), (226, 150, 104)],
    "wall": [(150, 130, 108), (218, 200, 166), (242, 230, 198), (255, 248, 228)],
    "glass": [(36, 66, 130), (62, 132, 204), (132, 196, 240), (224, 244, 255)],
    "flower": [(196, 92, 30), (240, 160, 30), (252, 212, 52), (255, 242, 150)],
    "white": [(150, 156, 160), (212, 216, 214), (240, 242, 238), (255, 255, 252)],
    "orange": [(164, 60, 34), (222, 108, 42), (248, 150, 58), (255, 200, 110)],
    "tallgrass": [(18, 96, 30), (40, 140, 34), (84, 182, 44), (160, 224, 70)],
    "roofg": [(36, 84, 44), (56, 124, 58), (84, 162, 74), (140, 202, 108)],
    "roofo": [(120, 66, 30), (176, 104, 44), (214, 146, 64), (240, 192, 110)],
    "sand": [(170, 132, 82), (208, 174, 110), (236, 210, 146), (252, 238, 190)],
    "path": [(118, 86, 56), (160, 122, 80), (196, 162, 108), (224, 198, 146)],
}

# Biomes : the same tiles in other colours (the ramps they change), and their tree. use_biome(name) sets them for the
# drawings that follow.
BIOMES = {
    "meadow": {"tree": "round", "palette": {}},
    "autumn": {"tree": "round", "palette": {
        "grass": [(112, 126, 48), (150, 160, 60), (190, 190, 84), (226, 218, 128)],
        "grass2": [(108, 118, 34), (144, 148, 42), (188, 180, 66), (228, 214, 110)],
        "leaf": [(120, 30, 30), (180, 56, 40), (222, 100, 52), (246, 160, 84)],
        "tallgrass": [(94, 68, 24), (138, 98, 34), (184, 138, 52), (226, 190, 98)],
    }},
    "snow": {"tree": "snowpine", "palette": {
        "grass": [(140, 160, 188), (186, 202, 222), (222, 232, 244), (248, 251, 255)],
        "grass2": [(140, 160, 188), (180, 196, 218), (214, 226, 240), (242, 247, 252)],
        "leaf": [(16, 56, 52), (26, 82, 68), (44, 112, 84), (84, 146, 104)],
        "tallgrass": [(52, 84, 96), (82, 120, 128), (134, 170, 172), (200, 224, 226)],
        "sand": [(132, 134, 140), (172, 174, 178), (204, 206, 208), (232, 234, 234)],
        "rock": [(70, 72, 86), (112, 114, 128), (156, 160, 172), (206, 210, 220)],
        "path": [(108, 100, 98), (148, 140, 134), (184, 178, 172), (214, 208, 202)],
        "flower": [(70, 110, 190), (110, 156, 226), (160, 200, 244), (222, 240, 255)],
    }},
    "desert": {"tree": "cactus", "palette": {
        "grass": [(178, 122, 62), (210, 156, 90), (232, 190, 124), (248, 222, 166)],
        "grass2": [(178, 122, 62), (204, 150, 86), (226, 184, 118), (244, 216, 158)],
        "sand": [(196, 166, 118), (224, 198, 148), (240, 222, 178), (252, 242, 214)],
        "leaf": [(30, 92, 56), (50, 128, 70), (86, 164, 84), (146, 200, 112)],
        "tallgrass": [(116, 82, 34), (154, 116, 48), (192, 156, 72), (228, 200, 118)],
        "rock": [(118, 60, 40), (168, 94, 58), (204, 134, 82), (236, 184, 124)],
        "path": [(146, 98, 60), (182, 132, 84), (210, 168, 112), (232, 202, 150)],
    }},
}
BASE_PAL = {k: list(v) for k, v in PAL.items()}
BIOME = "meadow"


def biome_palette(name):
    pal = dict(BASE_PAL)
    pal.update(BIOMES[name]["palette"])
    return pal


def use_mix(a, b, t):
    """The drawings that follow in a mix of two biomes : each tone of each ramp from a to b (t : 0 a, 1 b). The tree
    is the one of the nearest biome (tree_image takes another one)."""
    global BIOME
    pa, pb = biome_palette(a), biome_palette(b)
    PAL.clear()
    for mat in BASE_PAL:
        PAL[mat] = [tuple(int(round(ca[i] + (cb[i] - ca[i]) * t)) for i in range(3)) for ca, cb in zip(pa[mat], pb[mat])]
    BIOME = a if t < 0.5 else b


# The painted tiles (tools/render3d : 3D models rendered in the look of the game, Resources/Worldgen/painted/<biome>/<name>.png)
# replace the voxel drawings of their biome, tile by tile as they are made (tools/worldgen/paint.sh)
PAINTED = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "Resources", "Worldgen", "painted")
_PAINTED = {}


def painted(name):
    """The painted image of a drawing (RGBA on the colour key) in the current biome, None : not painted (or the
    palette is a mix of biomes : the voxel drawing)."""
    if PAL != biome_palette(BIOME):
        return None
    path = os.path.join(PAINTED, BIOME, name + ".png")
    if path not in _PAINTED:
        _PAINTED[path] = Image.open(path).convert("RGBA") if os.path.exists(path) else None
    image = _PAINTED[path]
    return image.copy() if image is not None else None


def use_biome(name):
    """The biome of the drawings that follow : its palette, its tree."""
    use_mix(name, name, 0.0)


LIGHT = (-0.55, -0.75, 1.0)
_l = math.sqrt(sum(c * c for c in LIGHT))
LIGHT = tuple(c / _l for c in LIGHT)


def lerp_ramp(ramp, t):
    """The colour at t (0 .. 3) on a ramp of 4 tones : a whole t is a tone, between two tones a mix."""
    i = min(int(t), 2)
    f = t - i
    a, b = ramp[i], ramp[i + 1]
    return tuple(int(round(a[k] + (b[k] - a[k]) * f)) for k in range(3))


def h01(x, y, s=0):
    """A hash of a voxel position in [0, 1) : the same everywhere, so the tiles stay seamless."""
    n = (x * 374761393 + y * 668265263 + s * 2147483647) & 0xFFFFFFFF
    n = (n ^ (n >> 13)) * 1274126177 & 0xFFFFFFFF
    return ((n ^ (n >> 16)) & 0xFFFF) / 65536.0


class Scene:
    """A voxel scene w x h voxels : material, height, primitive id (outlines between primitives), forced tone."""

    def __init__(self, w, h):
        self.w, self.h = w, h
        self.mat = [[None] * w for _ in range(h)]
        self.hgt = [[0.0] * w for _ in range(h)]
        self.pid = [[0] * w for _ in range(h)]
        self.tone = [[None] * w for _ in range(h)]
        self.next = 1

    def put(self, x, y, mat, hgt=0.0, pid=0, tone=None):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.mat[y][x], self.hgt[y][x], self.pid[y][x], self.tone[y][x] = mat, hgt, pid, tone

    def new_id(self):
        self.next += 1
        return self.next

    def ellipsoid(self, cx, cy, rx, ry, mat, height=4.0, base=0.0, tone_shift=0):
        pid = self.new_id()
        for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
            for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
                d = ((x + 0.5 - cx) / rx) ** 2 + ((y + 0.5 - cy) / ry) ** 2
                if d <= 1:
                    self.put(x, y, mat, base + height * math.sqrt(1 - d), pid)
        return pid

    def box(self, x0, y0, x1, y1, mat, hgt=0.0, tone=None):
        pid = self.new_id()
        for y in range(y0, y1):
            for x in range(x0, x1):
                self.put(x, y, mat, hgt, pid, tone)
        return pid

    def render(self, wrap=False):
        img = Image.new("RGBA", (self.w * V, self.h * V), KEY + (255,))
        px = img.load()

        def H(x, y, pid):
            if wrap:
                x %= self.w
                y %= self.h
            if 0 <= x < self.w and 0 <= y < self.h and self.mat[y][x] is not None and self.pid[y][x] == pid:
                return self.hgt[y][x]
            return None

        for y in range(self.h):
            for x in range(self.w):
                m = self.mat[y][x]
                if m is None:
                    continue
                ramp = PAL[m]
                if self.tone[y][x] is not None:
                    t = self.tone[y][x]
                else:
                    pid, h = self.pid[y][x], self.hgt[y][x]
                    hl = H(x - 1, y, pid); hr = H(x + 1, y, pid)
                    hu = H(x, y - 1, pid); hd = H(x, y + 1, pid)
                    # Height falls off at the silhouette : the edges turn away from the light like a rounded volume
                    edge = 1.5 if pid > 1 else 0.0
                    hl = h - edge if hl is None else hl
                    hr = h - edge if hr is None else hr
                    hu = h - edge if hu is None else hu
                    hd = h - edge if hd is None else hd
                    nx, ny, nz = (hl - hr) / 2, (hu - hd) / 2, 1.0
                    nl = math.sqrt(nx * nx + ny * ny + nz * nz)
                    lum = (nx * LIGHT[0] + ny * LIGHT[1] + nz * LIGHT[2]) / nl
                    # A gradient across the voxels : the light picks a point on the ramp, between two tones
                    t = max(0.0, min(3.0, (lum - 0.25) / 0.72 * 3.0))
                    # Outline : a voxel at the border of its primitive, toward the viewer (down) or out of the shape
                    if pid > 1:
                        below = (x, y + 1)
                        bx, by = below
                        out_below = not (0 <= by < self.h) or self.mat[by][bx] is None
                        if out_below or (self.pid[by][bx] != pid and self.hgt[by][bx] < h - 0.5):
                            t = 0
                        else:
                            for ax, ay in ((x - 1, y), (x + 1, y), (x, y - 1)):
                                if not (0 <= ax < self.w and 0 <= ay < self.h) or self.mat[ay][ax] is None:
                                    t = min(t, 1) if ay == y - 1 else 0
                    # Cast shadow : a higher voxel up-left of this one
                    for k in (1, 2):
                        sx, sy = x - k, y - k
                        if wrap:
                            sx %= self.w
                            sy %= self.h
                        if 0 <= sx < self.w and 0 <= sy < self.h and self.mat[sy][sx] is not None:
                            if self.hgt[sy][sx] > self.hgt[y][x] + 1.2 * k:
                                t = max(0, t - 1.2)
                                break
                c = lerp_ramp(ramp, t) + (255,)
                for j in range(V):
                    for i in range(V):
                        px[x * V + i, y * V + j] = c
        return img


# ---------------------------------------------------------------- terrain (seamless : hashes of the voxel position)

# ---------------------------------------------------------------- drawing helpers

def ink_outline(s):
    """The silhouette in ink : a voxel next to the outside (4 neighbours) turns dark, like a hand drawn outline."""
    edge = []
    for y in range(s.h):
        for x in range(s.w):
            if s.mat[y][x] is None:
                continue
            for ax, ay in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                if not (0 <= ax < s.w and 0 <= ay < s.h) or s.mat[ay][ax] is None:
                    edge.append((x, y))
                    break
    for x, y in edge:
        s.put(x, y, "ink", 0, 0, 0)


def rect(s, x0, y0, x1, y1, mat, tone):
    """Voxels x0..x1-1, y0..y1-1 in one flat tone."""
    for y in range(y0, y1):
        for x in range(x0, x1):
            s.put(x, y, mat, 0, 0, tone)


def frame(s, x0, y0, x1, y1, mat, tone):
    for x in range(x0, x1):
        s.put(x, y0, mat, 0, 0, tone)
        s.put(x, y1 - 1, mat, 0, 0, tone)
    for y in range(y0, y1):
        s.put(x0, y, mat, 0, 0, tone)
        s.put(x1 - 1, y, mat, 0, 0, tone)


def shingles(s, x0, x1, y0, y1, mat, shade=None):
    """Roof shingles in staggered rows of 4 voxels : a lit top edge, a dark seam. shade(x, y) -> tone offset."""
    for y in range(y0, y1):
        row, r = divmod(y - y0, 4)
        for x in range(x0, x1):
            seam = (x + 3 * (row % 2)) % 6 == 0
            t = 1 if r == 3 or seam else 3 if r == 0 and (x + 3 * (row % 2)) % 6 == 1 else 2
            if shade:
                t = max(0, min(3, t + shade(x, y)))
            s.put(x, y, mat, 0, 0, t)


def window(s, x0, y0, x1, y1, frame_mat="white", bar=True):
    rect(s, x0, y0, x1, y1, "glass", 2)
    frame(s, x0, y0, x1, y1, frame_mat, 3)
    for k in range(3):                               # the reflection : a diagonal
        for d in range(2):
            x, y = x0 + 2 + k + d, y0 + 1 + k
            if x < x1 - 1 and y < y1 - 1:
                s.put(x, y, "glass", 0, 0, 3)
    if bar:
        mx, my = (x0 + x1) // 2, (y0 + y1) // 2
        for y in range(y0, y1):
            s.put(mx, y, frame_mat, 0, 0, 3)
        for x in range(x0, x1):
            s.put(x, my, frame_mat, 0, 0, 3)
    for x in range(x0 + 1, x1 - 1):                  # the inner top edge in the shade
        s.put(x, y0 + 1, "glass", 0, 0, 1)


# ---------------------------------------------------------------- terrain (seamless : hashes of the voxel position)

def grass_field(s, seed=0):
    """Calm grass : an even green, a slow shade across the voxels, a few faint speckles."""
    for y in range(N):
        for x in range(N):
            s.put(x, y, "grass", 0, 1, 2.0 + 0.06 * math.sin((x + 2 * y + seed) * 0.9))
    for i in range(3):
        x, y = int(h01(i, 1, 70 + seed) * N), int(h01(i, 2, 70 + seed) * N)
        s.put(x, y, "grass", 0, 1, 2.35 if i % 2 else 1.7)


def tuft_at(s, x, y, flip=False):
    """A small tuft : two short blades, a little darker than the grass (no bright tips)."""
    for dx, hgt in ((0, 1), (2, 2)):
        bx = x + (2 - dx if flip else dx)
        for k in range(hgt):
            s.put(bx % N, (y - k) % N, "grass", 0, 1, 1.55 if k == 0 else 1.75)


GRASS_VARIANTS = 4


def ground_texture(mat, variant=0):
    """The painted ground of a material in the current biome (tools/render3d tiles.js : ground_<mat>, the grass in its
    variants), None : not painted."""
    return painted(f"ground_grass_{variant % GRASS_VARIANTS}" if mat == "grass" else f"ground_{mat}")


def grass(variant=0):
    image = ground_texture("grass", variant)
    if image is not None:
        return image
    s = Scene(N, N)
    grass_field(s, variant)
    if variant == 1:
        tuft_at(s, 2, 5)
        tuft_at(s, 9, 12, flip=True)
    elif variant == 2:
        tuft_at(s, 10, 4)
    elif variant == 3:
        tuft_at(s, 6, 9)
    return s.render()


def water(frame):
    s = Scene(N, N)
    for y in range(N):
        for x in range(N):
            s.put(x, y, "water", 0, 1, 2)
    # Wave crests : a light line over a dark trough, drifting right, wrapping in the tile
    for i, (wx, wy, n) in enumerate(((1, 2, 5), (10, 6, 4), (4, 10, 5), (12, 13, 3))):
        for k in range(n):
            s.put((wx + k + frame) % N, wy, "water", 0, 1, 3 if 0 < k < n - 1 else 2)
            s.put((wx + k + frame - 1) % N, wy + 1, "water", 0, 1, 1)
    return s.render()


def flowers(frame):
    s = Scene(N, N)
    grass_field(s, 5)
    sway = (0, 1, 0, -1)[frame]
    for i, (fx, fy, m) in enumerate(((4, 4, "flower"), (11, 7, "white"), (5, 12, "white"), (12, 13, "flower"))):
        ox = sway if i % 2 == 0 else -sway
        fx += ox
        s.put(fx - ox, fy + 2, "grass", 0, 1, 0)
        s.put(fx - ox, fy + 1, "grass", 0, 1, 1)
        pid = s.new_id()
        for dx, dy in ((0, -1), (-1, 0), (1, 0), (0, 1)):
            s.put(fx + dx, fy + dy, m, 2, pid, 3 if dy < 0 or dx < 0 else 1)
        s.put(fx, fy, "orange", 3, pid, 2)
    return s.render()


def tall_grass(frame):
    """Wild grass : two rows of pointed leaves, the back row taller, packed so that the tiles make one field.
    The leaves wrap at the tile edges ; the tips sway with the frames."""
    image = painted(f"tall_{frame}")
    if image is not None:
        return image
    s = Scene(N, N)
    sway = (0, 1, 0, -1)[frame]

    def leaf(cx, tip, base, lean):
        for y in range(tip, base):
            k = y - tip
            hw = min(2, k // 2)                      # 1, 3 then 5 voxels wide
            shift = lean if k < 2 else 0
            for dx in range(-hw, hw + 1):
                x = (cx + dx + shift) % N
                t = 3 if dx < 0 or (k == 0) else 2
                if dx == hw and hw > 0:
                    t = 1                           # the shaded side of the leaf
                s.put(x, y, "tallgrass", 0, 0, t)

    for cx in (2, 7, 12):                            # back row
        leaf(cx, 1 + (cx % 3 == 0), 12, sway)
    for cx in (0, 5, 10):                            # front row, between the back leaves
        leaf(cx, 5, 14, -sway)
    for y in range(12, N):                           # the roots : dense and in the shade
        for x in range(N):
            if s.mat[y][x] is None or y >= 14:
                s.put(x, y, "tallgrass", 0, 0, 1)
    return s.render()


def tuft():
    s = Scene(N, N)
    for i, (bx, top) in enumerate(((6, 9), (8, 7), (10, 9))):
        for y in range(top, 13):
            s.put(bx, y, "grass", 1, 9, 3 if y == top else 2)
    for x in range(5, 12):
        s.put(x, 13, "grass", 0, 9, 1)
    return s.render()


def boulder():
    s = Scene(N, N)
    s.ellipsoid(8.6, 12.3, 6.5, 2.2, "grass", 0.0)
    for y in range(N):
        for x in range(N):
            if s.mat[y][x]:
                s.tone[y][x] = 1
    s.ellipsoid(8, 8.5, 6.5, 5.8, "stone", 6.0, 0.5)
    return s.render()


def sign():
    s = Scene(N, N)
    s.box(7, 9, 9, 15, "wood", 0, None)
    s.ellipsoid(8, 15, 3, 1, "grass", 0)
    for y in range(15, 16):
        for x in range(5, 12):
            if s.mat[y][x]:
                s.tone[y][x] = 0
    pid = s.new_id()
    for y in range(3, 10):
        for x in range(2, 14):
            t = 0 if y == 9 or x in (2, 13) else 3 if y == 3 else 2
            s.put(x, y, "wood", 1, pid, t)
    for x in range(4, 12, 2):
        s.put(x, 6, "wood", 1, pid, 1)
    return s.render()


# ---------------------------------------------------------------- structures (several tiles, sliced)

def tree_scene(snow=False):
    """A 2 x 4 tiles pine : row 0 the tip, rows 1 and 2 the tiers (the same), row 3 the trunk on the grass.
    snow : snow on the top of each tier."""
    s = Scene(2 * N, 4 * N)
    w = 2 * N
    for y in range(3 * N, 4 * N):
        for x in range(w):
            s.put(x, y, "grass", 0, 1, 2)
    s.ellipsoid(w / 2 + 1.5, 3 * N + 9.5, 10, 3.4, "grass", 0)
    for y in range(3 * N, 4 * N):
        for x in range(w):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0
    s.box(14, 3 * N + 2, 18, 3 * N + 10, "wood", 1.0)
    tiers = [(0 * N + 7, 4.6, 5.5), (0 * N + 13, 7.5, 6.0), (1 * N + 2, 10.0, 6.5), (1 * N + 10, 12.5, 7.0),
             (2 * N + 2, 10.0, 6.5), (2 * N + 10, 12.5, 7.0), (3 * N - 3, 14.0, 6.5)]
    for cy, rx, ry in tiers:
        pid = s.ellipsoid(w / 2, cy, rx, ry, "leaf", 7.0)
        if snow:                                     # the upper part of the tier, its edge wavy
            for y in range(int(cy - ry) - 1, int(cy) + 1):
                for x in range(s.w):
                    if 0 <= y < s.h and s.pid[y][x] == pid and y < cy - ry * (0.25 + 0.2 * h01(x // 2, 3, 9)):
                        s.mat[y][x] = "white"
    return s


def cactus_scene():
    """A 2 x 3 tiles cactus (tip, body, foot on the ground) : a ribbed column, two arms, a few spines."""
    s = Scene(2 * N, 3 * N)
    w = 2 * N
    for y in range(2 * N, 3 * N):
        for x in range(w):
            s.put(x, y, "grass", 0, 1, 2)
    s.ellipsoid(w / 2 + 1.5, 2 * N + 10, 8, 2.8, "grass", 0)
    for y in range(2 * N, 3 * N):
        for x in range(w):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0
    s.ellipsoid(w / 2 - 6, 18, 2.6, 7.5, "leaf", 4.0)          # the arm on the left, up
    s.box(10, 22, 14, 26, "leaf", 2.5)
    s.ellipsoid(w / 2 + 6, 13, 2.6, 7.0, "leaf", 4.0)          # the arm on the right, higher
    s.box(19, 17, 23, 21, "leaf", 2.5)
    body = s.ellipsoid(w / 2, 24, 4.2, 22, "leaf", 6.0)
    for y in range(s.h):
        for x in range(s.w):
            if s.mat[y][x] == "leaf" and x % 3 == 0 and s.pid[y][x] == body:
                s.tone[y][x] = 1                            # the ribs
    for i in range(9):
        x, y = 12 + int(h01(i, 1, 33) * 9), 6 + int(h01(i, 2, 33) * 34)
        if s.mat[y][x] == "leaf":
            s.put(x, y, "white", 7.0, s.pid[y][x], 3)
    return s


def tree_image(style=None):
    """A tree, 2 x 4 tiles (rows 0, 1, 3 used : the tip, the canopy, the trunk). style : "pine", "snowpine", "cactus"
    (by default the tree of the biome)."""
    style = style or BIOMES[BIOME]["tree"]
    if style == "round":
        image = painted("tree_round")
        return image if image is not None else round_tree().render()
    small = painted(f"tree_{style}")
    if small is not None:                            # painted : its 3 rows (tip, canopy, trunk) in the rows 0, 1, 3
        img = Image.new("RGBA", (2 * T, 4 * T), KEY + (255,))
        for row, src in enumerate((0, 1, 1, 2)):
            img.paste(small.crop((0, src * T, 2 * T, src * T + T)), (0, row * T))
        return img
    if style != "cactus":
        return tree_scene(snow=style == "snowpine").render()
    small = cactus_scene().render()
    img = Image.new("RGBA", (2 * T, 4 * T), KEY + (255,))
    for row, src in enumerate((0, 1, 1, 2)):
        img.paste(small.crop((0, src * T, 2 * T, src * T + T)), (0, row * T))
    return img


def tree():
    """The pine of the meadow (a Scene)."""
    return tree_scene()


def house(roof="roofr"):
    """4 x 4 tiles : row 0 the top of the roof (drawn over the characters), the door in column 2 of row 3.
    roof : the material of the roof, the shutters and the awning."""
    W, H = 4 * N, 4 * N
    s = Scene(W, H)
    # Chimney, behind the ridge
    rect(s, 46, 1, 53, 12, "brick", 2)
    for y in range(3, 12, 2):
        for x in range(46, 53):
            if (x + (y // 2) * 2) % 4 == 0:
                s.put(x, y, "brick", 0, 0, 1)
    rect(s, 45, 0, 54, 2, "stone", 3)
    rect(s, 52, 2, 53, 12, "brick", 0)
    # Roof : overhanging the walls, rounded at the ridge, lit from the west
    shingles(s, 2, W - 2, 11, 38, roof,
             lambda x, y: 1 if x < 5 else -1 if x > W - 6 else 0)
    for x in range(4, W - 4):                        # the ridge cap
        s.put(x, 8, roof, 0, 0, 3)
        s.put(x, 9, roof, 0, 0, 3 if x % 4 else 2)
        s.put(x, 10, roof, 0, 0, 1)
    for x in (3, W - 4):
        for y in (9, 10):
            s.put(x, y, roof, 0, 0, 2)
    rect(s, 1, 38, W - 1, 40, roof, 0)            # the eaves
    # Walls : siding, posts at the corners, the shadow of the eaves
    for y in range(40, 61):
        for x in range(4, W - 4):
            s.put(x, y, "wall", 0, 0, 1 if (y - 40) % 4 == 3 else 2)
    rect(s, 4, 40, W - 4, 42, "wall", 0)
    rect(s, 4, 42, 6, 61, "wood", 3)
    rect(s, W - 6, 42, W - 4, 61, "wood", 1)
    rect(s, 3, 61, W - 3, 63, "stone", 1)
    rect(s, 3, 61, W - 3, 62, "stone", 2)
    # Window with shutters and a flower box (column 0-1)
    window(s, 11, 45, 25, 55)
    for x0 in (8, 25):
        rect(s, x0, 45, x0 + 3, 55, roof, 2)
        for y in range(46, 55, 2):
            s.put(x0 + 1, y, roof, 0, 0, 1)
    rect(s, 10, 56, 26, 58, "wood", 1)
    rect(s, 10, 56, 26, 57, "wood", 2)
    for i, x in enumerate(range(11, 25, 2)):
        s.put(x, 55, "flower" if i % 2 else "white", 0, 0, 3)
        s.put(x + 1, 55, "leaf", 0, 0, 2)
    # Door under an awning (column 2)
    rect(s, 36, 46, 44, 61, "wood", 2)
    frame(s, 35, 46, 45, 61, "wood", 0)
    for y in range(49, 60):
        s.put(38, y, "wood", 0, 0, 1)
        s.put(41, y, "wood", 0, 0, 1)
    rect(s, 37, 48, 43, 50, "glass", 2)
    s.put(42, 54, "flower", 0, 0, 3)
    rect(s, 33, 43, 47, 46, roof, 2)
    rect(s, 33, 43, 47, 44, roof, 3)
    rect(s, 34, 46, 46, 47, "wall", 0)
    rect(s, 35, 61, 45, 63, "stone", 3)
    # A small round window (column 3)
    window(s, 51, 46, 56, 52, bar=False)
    ink_outline(s)
    return s


def styled(s, style):
    """A building in the style of its biome (a Scene, changed in place) : "snow" snow on the roofs, "desert" adobe
    walls, "autumn" log walls ; else unchanged."""
    roofs = {"roof", "roofr", "roofb", "roofg", "roofo", "brick"}
    if style == "snow":
        for x in range(s.w):
            depth = 0
            for y in range(s.h):
                m = s.mat[y][x]
                if m is None:
                    continue
                cap = 5 + int(h01(x // 3, 0, 21) * 4)      # a thick cap, its lower edge uneven
                if m in roofs and depth < cap:
                    s.mat[y][x], s.tone[y][x] = "white", 3 if depth < 2 else 2 if depth < cap - 1 else 1
                    depth += 1
                elif m not in roofs and depth:
                    break
    elif style in ("desert", "autumn"):
        wall = "path" if style == "desert" else "wood"
        for y in range(s.h):
            for x in range(s.w):
                if s.mat[y][x] == "wall":
                    s.mat[y][x] = wall
    return s


def tent(canvas="roofo"):
    """A 2 x 2 tiles tent : a canvas A-frame, its opening at the bottom, pegs, its shadow on the ground."""
    W, H = 2 * N, 2 * N
    s = Scene(W, H)
    s.ellipsoid(W / 2 + 1.5, H - 3.5, 15, 3, "grass", 0)
    for y in range(H):
        for x in range(W):
            if s.pid[y][x] > 1:
                s.mat[y][x], s.tone[y][x] = "grass", 0
    pid = s.new_id()
    for y in range(3, H - 3):
        hw = (y - 3) * 0.55 + 1
        for x in range(W):
            d = x + 0.5 - W / 2
            if abs(d) <= hw:
                t = 3 if d < -hw * 0.45 else 2 if d < 0 else 1
                if abs(d) < 0.8:
                    t = 3                              # the ridge
                s.put(x, y, canvas, 0, pid, t)
    for y in range(H - 12, H - 3):                    # the opening
        hw = (y - (H - 12)) * 0.45
        for x in range(W):
            if abs(x + 0.5 - W / 2) <= hw:
                s.put(x, y, "ink", 0, pid, 0)
    for x in (2, W - 3):                              # pegs and ropes
        s.put(x, H - 3, "wood", 0, pid, 1)
    ink_outline(s)
    return s.render()


def campfire(frame):
    """A campfire (animated, 4 frames) : a ring of stones, crossed logs, flames that flicker."""
    s = Scene(N, N)
    grass_field(s, 3)
    for i in range(8):
        a = 2 * math.pi * i / 8
        s.ellipsoid(8 + 5.2 * math.cos(a), 10 + 3.2 * math.sin(a), 1.6, 1.3, "stone", 1.5)
    s.box(4, 10, 12, 12, "wood", 1.0, 1)
    s.box(6, 9, 10, 13, "wood", 1.0, 2)
    sway = (0, 1, 0, -1)[frame]
    tall = (7, 8, 6, 8)[frame]
    for y in range(10 - tall, 11):
        k = (10 - y) / tall
        hw = max(0.5, 3.2 * (1 - k) + 0.3)
        cx = 8 + sway * k
        for x in range(N):
            d = abs(x + 0.5 - cx)
            if d <= hw:
                tone = 3 if d < hw * 0.4 and k < 0.7 else 2 if k < 0.6 else 1
                s.put(x, y, "flower" if tone >= 2 else "orange", 3, 0, tone)
    return s.render()


def log_seat():
    """A log to sit on, lying across the block."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 12, 7, 1.6, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.mat[y][x], s.tone[y][x] = "grass", 0
    pid = s.new_id()
    for y in range(7, 12):
        for x in range(2, 14):
            t = 3 if y == 7 else 2 if y < 10 else 1
            if (x + y) % 5 == 0:
                t = max(0, t - 1)
            s.put(x, y, "wood", 0, pid, t)
    for y in range(7, 12):
        s.put(13, y, "wood", 0, pid, 3 if y in (8, 9) else 2)
    ink_outline(s)
    return s.render()


def pillar(broken=False):
    """A 1 x 2 tiles stone pillar of a ruin (its capital, its column, its base) ; broken : snapped, rubble at its foot."""
    W, H = N, 2 * N
    s = Scene(W, H)
    s.ellipsoid(9, H - 3, 7, 2.2, "grass", 0)
    for y in range(H):
        for x in range(W):
            if s.pid[y][x] > 1:
                s.mat[y][x], s.tone[y][x] = "grass", 0
    pid = s.new_id()
    top = 13 if broken else 3
    for y in range(top, H - 3):
        for x in range(4, 12):
            t = 3 if x < 6 else 1 if x > 9 else 2
            if y % 6 == 0:
                t = max(0, t - 1)                      # the joints of the stones
            if broken and y < top + 3 and h01(x, y, 5) < 0.5:
                continue                               # the snapped top
            s.put(x, y, "stone", 0, pid, t)
    if not broken:
        for y in range(1, 4):
            for x in range(2, 14):
                s.put(x, y, "stone", 0, pid, 3 if y == 1 else 2)
    for y in range(H - 5, H - 2):
        for x in range(2, 14):
            s.put(x, y, "stone", 0, pid, 2 if y == H - 5 else 1)
    if broken:
        for cx, cy in ((2, H - 3), (13, H - 4)):
            s.ellipsoid(cx, cy, 1.8, 1.4, "stone", 1.5)
    ink_outline(s)
    return s.render()


def item_ball_charset():
    """A sheet of character frames (6 x 8 frames of 72 px, as Sprites/Charsets) : an item ball in each frame, on the
    ground of the frame (the feet of a character)."""
    v = Scene(24, 24)
    for y in range(20, 23):
        for x in range(7, 17):
            if ((x + 0.5 - 12) / 5) ** 2 + ((y + 0.5 - 21.5) / 1.3) ** 2 <= 1:
                v.put(x, y, "ink", 0, 1, 0)
    pid = v.ellipsoid(12, 16.5, 5.2, 5.0, "roofr", 5.0)
    for y in range(v.h):
        for x in range(v.w):
            if v.pid[y][x] == pid and y >= 17:
                v.mat[y][x] = "white"
            if v.pid[y][x] == pid and y in (16, 17):
                v.mat[y][x], v.tone[y][x] = "ink", 0
    for x, y in ((11, 16), (12, 16), (11, 17), (12, 17)):
        v.mat[y][x], v.tone[y][x] = "white", 3
    ink_outline(v)
    frame = v.render()
    sheet = Image.new("RGBA", (6 * 72, 8 * 72), KEY + (255,))
    for row in range(8):
        for col in range(6):
            sheet.paste(frame, (col * 72, row * 72))
    return sheet


def healing_center():
    """5 x 5 tiles : row 0 the top of the roof (over the characters), the sliding door in column 2 of row 4."""
    W, H = 5 * N, 5 * N
    s = Scene(W, H)
    top, eave = 8, 44
    # Hipped roof : the front slope in shingles, the west hip lit, the east hip in the shade
    for y in range(top, eave):
        k = (y - top) / (eave - top)
        inset = max(0, int(12 * (1 - k * 2.2)))       # the roof widens to its full width in its upper part
        for x in range(2 + inset, W - 2 - inset):
            hip = 14 - (y - top) * 12 / 22
            if x < 2 + max(hip, 0) + 1:
                t = 3
            elif x > W - 3 - max(hip, 0) - 1:
                t = 1
            else:
                row, r = divmod(y - top, 4)
                seam = (x + 3 * (row % 2)) % 6 == 0
                t = 1 if r == 3 or seam else 2
            s.put(x, y, "roof", 0, 0, t)
    for x in range(14, W - 14):                      # the ridge
        s.put(x, top, "roof", 0, 0, 3)
        s.put(x, top + 1, "roof", 0, 0, 3 if x % 4 else 2)
    # Gable over the entrance, with the emblem
    cx = W // 2
    for y in range(22, eave):
        hw = (y - 22) * 15 // 22 + 1
        for x in range(cx - hw, cx + hw):
            edge = x - (cx - hw) < 2 or (cx + hw - 1) - x < 2
            s.put(x, y, "roof" if edge else "white", 0, 0, (3 if x < cx else 1) if edge else 2)
    for y in range(30, 41):
        for x in range(cx - 6, cx + 6):
            d = ((x + 0.5 - cx) / 5.5) ** 2 + ((y + 0.5 - 35.5) / 5.5) ** 2
            if d <= 1:
                s.put(x, y, "orange", 0, 0, 3 if d < 0.3 and x < cx and y < 35 else 2 if d < 0.8 else 1)
    for dx, dy in ((0, -3), (1, -3), (-1, -2), (0, -2), (1, -2), (-1, -1), (0, -1), (-2, 0), (-1, 0), (-3, 1),
                   (-2, 1), (1, 0), (2, -1)):
        s.put(cx + dx, 36 + dy, "white", 0, 0, 3)
    rect(s, 1, eave, W - 1, eave + 2, "roof", 0)     # the eaves
    # Walls
    for y in range(eave + 2, 78):
        for x in range(4, W - 4):
            s.put(x, y, "white", 0, 0, 2)
    rect(s, 4, eave + 2, W - 4, eave + 4, "white", 1)
    rect(s, 4, 66, W - 4, 68, "roof", 1)
    rect(s, 4, 66, W - 4, 67, "roof", 2)
    rect(s, 4, eave + 4, 7, 78, "white", 3)
    rect(s, W - 7, eave + 4, W - 4, 78, "white", 1)
    rect(s, 3, 78, W - 3, 80, "stone", 1)
    rect(s, 3, 78, W - 3, 79, "stone", 2)
    # Large windows on both sides
    for x0 in (10, W - 28):
        window(s, x0, 51, x0 + 18, 64, frame_mat="roof")
    # Sliding glass door under a striped awning (column 2)
    rect(s, 33, 58, 47, 78, "glass", 2)
    frame(s, 32, 58, 48, 78, "roof", 0)
    rect(s, 39, 58, 41, 78, "roof", 1)
    for k in range(4):
        s.put(34 + k, 60 + k, "glass", 0, 0, 3)
        s.put(42 + k, 60 + k, "glass", 0, 0, 3)
    for x in range(29, 51):
        for y in range(52, 57):
            s.put(x, y, "orange" if (x // 2) % 2 else "white", 0, 0, 2 if y < 56 else 0)
        s.put(x, 52, "orange" if (x // 2) % 2 else "white", 0, 0, 3)
    for x in range(30, 50, 2):
        s.put(x, 57, "orange", 0, 0, 1)
    rect(s, 31, 78, 49, 80, "stone", 3)
    ink_outline(s)
    return s


def round_tree():
    """A 3 x 4 tiles broadleaf tree : a big round canopy (rows 0 to 2), one volume whose height is the union of
    leaf clusters (a lumpy silhouette, round light), leaves in its texture ; a short trunk on its shadow (row 3)."""
    W, H = 3 * N, 4 * N
    s = Scene(W, H)
    s.box(20, 3 * N - 4, 28, 3 * N + 8, "wood", 1.0)
    lumps = [(24, 26, 21, 17, 9.0), (11, 27, 11, 11, 7.5), (37, 27, 11, 11, 7.5), (15, 15, 11, 10, 8.5),
             (33, 14, 11, 10, 8.5), (24, 9, 12, 8, 9.0), (24, 36, 17, 8, 6.0)]
    pid = s.new_id()
    for y in range(H):
        for x in range(W):
            hgt = None
            for cx, cy, rx, ry, top in lumps:
                d = ((x + 0.5 - cx) / rx) ** 2 + ((y + 0.5 - cy) / ry) ** 2
                if d <= 1:
                    h = 4.0 + top * math.sqrt(1 - d)
                    hgt = h if hgt is None else max(hgt, h)
            if hgt is not None:
                s.put(x, y, "leaf", hgt, pid)
    for y in range(H):                               # leaves : darker and lighter voxels in the canopy
        for x in range(W):
            if s.mat[y][x] == "leaf":
                r = h01(x, y, 41)
                if r < 0.08:
                    s.hgt[y][x] -= 1.4
                elif r > 0.95:
                    s.hgt[y][x] += 0.8
    ink_outline(s)
    for y in range(3 * N, H):                        # the ground, after the outline : no frame around it
        for x in range(W):
            if s.mat[y][x] is None:
                d = ((x + 0.5 - W / 2 - 2) / 17) ** 2 + ((y + 0.5 - 3 * N - 7) / 4.5) ** 2
                s.put(x, y, "grass", 0, 1, 0.9 if d <= 1 else 2)
    return s


def lamp():
    """A street lamp, 1 x 2 tiles : its lantern (row 0, over the characters ; animated : lamp_head), its post on a
    stone foot (row 1)."""
    return lamp_scene(0).render()


def bush():
    """A round bush (blocks the way)."""
    s = Scene(N, N)
    s.ellipsoid(9, 13, 7, 2.2, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    s.ellipsoid(8, 8.5, 7, 6, "leaf", 6.0)
    s.ellipsoid(5.5, 6, 3.5, 3, "leaf", 7.0)
    s.ellipsoid(11, 6.5, 3.5, 3, "leaf", 7.0)
    ink_outline(s)
    return s.render()


def fence():
    """A wooden fence across the tile : two rails between posts."""
    s = Scene(N, N)
    grass_field(s, 6)
    pid = s.new_id()
    for x in (2, 13):
        for y in range(4, 14):
            s.put(x, y, "wood", 2, pid, 3 if y == 4 else 2)
            s.put(x + 1, y, "wood", 2, pid, 1)
    for y, t in ((6, 3), (7, 2), (10, 3), (11, 2)):
        for x in range(N):
            if s.mat[y][x] != "wood" or x not in (2, 3, 13, 14):
                s.put(x, y, "wood", 1.5, pid, t)
    for x in range(N):
        s.put(x, 14, "grass", 0, 1, 1.2)             # its shadow
    return s.render()


def crops():
    """A row of a vegetable garden : furrows of soil, leafy plants on them."""
    s = Scene(N, N)
    for y in range(N):
        for x in range(N):
            s.put(x, y, "soil", 0, 1, 1.4 if y % 8 in (6, 7) else 2.0)
    for row in (1, 9):
        for cx in (3, 8, 13):
            pid = s.ellipsoid(cx, row + 2.5, 2.6, 2.4, "leaf", 3.0)
            s.put(cx, row + 1, "leaf", 4.0, pid, 3)
    return s.render()


def flowerbed(colour="roofr"):
    """A bed of flowers on the grass : many small blossoms of a colour, their leaves."""
    s = Scene(N, N)
    grass_field(s, 7)
    for y in range(N):
        for x in range(N):
            if h01(x, y, 17) < 0.35:
                s.put(x, y, "leaf", 0, 1, 1.6)
    for i in range(9):
        cx, cy = 2 + int(h01(i, 1, 29) * 12), 2 + int(h01(i, 2, 29) * 12)
        pid = s.new_id()
        for dx, dy in ((0, 0), (1, 0), (0, 1), (1, 1)):
            s.put(cx + dx, cy + dy, colour, 1.0, pid, 3 if dy == 0 else 2)
        s.put(cx, cy, "white", 1.2, pid, 3)
    return s.render()


def emblem(c, r):
    """A part (c, r : 0..1) of a 2 x 2 tiles emblem painted on the pavement : a round sign, as the plazas have."""
    s = Scene(N, N)
    R = N - 2
    for y in range(N):
        for x in range(N):
            gx, gy = x + c * N + 0.5 - N, y + r * N + 0.5 - N      # from the centre of the 2 x 2
            d = math.hypot(gx, gy)
            if R - 1.6 <= d <= R or (abs(gy) < 0.8 and d < R) or 3.0 <= d <= 4.4:
                s.put(x, y, "white", 0, 1, 2.6)
    return s.render()


def mailbox():
    """A mailbox on a post, by a door."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 14, 4, 1.4, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    s.box(7, 8, 9, 14, "wood", 1.0)
    pid = s.ellipsoid(8, 5.5, 5, 3.6, "roofr", 4.0)
    for y in range(2, 9):
        for x in range(3, 13):
            if s.pid[y][x] == pid and x == 3:
                s.tone[y][x] = 0
    s.put(12, 3, "flower", 5.0, pid, 3)
    ink_outline(s)
    return s.render()


def lamp_head(frame):
    """The lantern of a street lamp (animated, 4 frames : its flame flickers, a halo of light around it)."""
    full = lamp_scene(frame)
    return full.render().crop((0, 0, T, T))


def lamp_scene(frame=0):
    s = Scene(N, 2 * N)
    s.ellipsoid(9, 2 * N - 3, 5, 1.8, "grass", 0)
    for x in range(N):
        for y in range(N, 2 * N):
            if s.pid[y][x] > 1:
                s.mat[y][x], s.tone[y][x] = "grass", 0.6
    s.box(7, 8, 10, 2 * N - 3, "lamp", 2.0)
    s.box(5, 2 * N - 5, 12, 2 * N - 2, "lamp", 1.0)
    s.ellipsoid(8.5, 6, 4.2, 4.4, "lamp", 3.0)
    glow = (3.0, 2.5, 2.9, 2.2)[frame]
    for y in range(3, 9):
        for x in range(6, 11):
            s.put(x, y, "flower", 3.5, 99, glow if y < 6 else glow - 0.6)
    s.box(5, 1, 12, 3, "lamp", 3.0)
    ink_outline(s)
    if frame in (0, 2):                              # sparks of light around the lantern
        for i, (x, y) in enumerate(((2, 4), (14, 6), (3, 9), (13, 2))):
            if (i + frame // 2) % 2 == 0:
                s.put(x, y, "flower", 4.0, 98, 3)
    return s


def fountain(c, r, frame):
    """A part (c, r : 0..1) of a 2 x 2 tiles fountain (animated) : a round stone basin of water, a pillar, jets of
    water arcing out of it, their drops falling in turn."""
    W = 2 * N
    s = Scene(W, W)
    for y in range(W):
        for x in range(W):
            d = math.hypot((x + 0.5 - W / 2) / 14.5, (y + 0.5 - W / 2 - 2) / 11.5)
            if d <= 1.0:
                s.put(x, y, "stone", 2.0, 2, 3 if y < W / 2 - 6 else 2 if d > 0.82 else None)
                if d <= 0.82:
                    ripple = (x + y + frame * 2) % 6 == 0 or math.hypot(x - 16, y - 18) % 5 < 0.6 + frame * 0.1
                    s.put(x, y, "water", 0, 3, 3 if ripple else 1.6)
    s.box(14, 6, 18, 18, "stone", 6.0)
    s.ellipsoid(16, 6, 3.4, 2.2, "stone", 7.0)
    for side in (-1, 1):                             # the jets, their drops moving down the arc
        for k in range(10):
            t = (k + frame * 2.5) % 10 / 10.0
            x = 16 + side * (1.5 + t * 9)
            y = 4 - 6 * math.sin(t * math.pi) + t * 12
            for dx, dy in ((0, 0), (side, 0), (0, 1)):
                s.put(int(x) + dx, int(y) + dy, "white", 8.0, 50, 3 if dy == 0 else 2)
    ink_outline(s)
    img = s.render()
    return img.crop((c * T, r * T, c * T + T, r * T + T))


def flowerbed_anim(colour, frame):
    """A bed of flowers (animated) : the blossoms sway in the breeze, in turn."""
    s = Scene(N, N)
    grass_field(s, 7)
    for y in range(N):
        for x in range(N):
            if h01(x, y, 17) < 0.35:
                s.put(x, y, "leaf", 0, 1, 1.6)
    for i in range(9):
        sway = (0, 1, 0, -1)[(frame + i) % 4] if i % 2 else 0
        cx, cy = 2 + int(h01(i, 1, 29) * 12) + sway, 2 + int(h01(i, 2, 29) * 12)
        pid = s.new_id()
        for dx, dy in ((0, 0), (1, 0), (0, 1), (1, 1)):
            s.put(cx + dx, cy + dy, colour, 1.0, pid, 3 if dy == 0 else 2)
        s.put(cx, cy, "white", 1.2, pid, 3)
    return s.render()


def butterfly(frame):
    """A butterfly over the grass (animated) : it flutters round, its wings open and close."""
    s = Scene(N, N)
    grass_field(s, 8)
    a = frame * math.pi / 2
    cx, cy = int(8 + 3 * math.cos(a)), int(7 + 2 * math.sin(a))
    open_ = frame % 2 == 0
    pid = s.new_id()
    s.put(cx, cy, "ink", 4, pid, 0)
    s.put(cx, cy + 1, "ink", 4, pid, 0)
    for side in (-1, 1):
        wings = [(side, 0), (side * 2, 0), (side, -1), (side * 2, -1), (side, 1)] if open_ else [(side, 0), (side, -1)]
        for dx, dy in wings:
            s.put(cx + dx, cy + dy, "roofo" if dy < 1 else "flower", 4, pid, 3)
    s.put(cx + 1, cy + 5, "grass", 0, 1, 1.4)       # its shadow on the grass
    s.put(cx, cy + 5, "grass", 0, 1, 1.4)
    return s.render()


def bench():
    """A wooden bench across the tile : its seat, its back, iron feet."""
    s = Scene(N, N)
    pid = s.new_id()
    for y, t in ((3, 3), (4, 2), (5, 2)):           # the back
        for x in range(1, N - 1):
            s.put(x, y, "wood", 3, pid, t if x % 5 else t - 1)
    for y, t in ((8, 3), (9, 2), (10, 1)):          # the seat
        for x in range(1, N - 1):
            s.put(x, y, "wood", 2, pid, t)
    for x in (2, 13):
        for y in range(4, 13):
            if s.mat[y][x] != "wood" or y > 10:
                s.put(x, y, "lamp", 2, pid, 1)
    ink_outline(s)
    return s.render()


def bin_():
    """A litter bin : a green bin, its lid."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 13.5, 5, 1.6, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    s.box(4, 5, 12, 14, "roofg", 3.0)
    for x in range(4, 12):
        s.put(x, 4, "lamp", 4.0, s.pid[6][6], 3)
        s.put(x, 5, "lamp", 4.0, s.pid[6][6], 2)
    for y in range(6, 14, 3):
        for x in range(4, 12):
            s.put(x, y, "roofg", 3.0, s.pid[7][6], 1)
    ink_outline(s)
    return s.render()


def flowerpot(colour="roofr"):
    """A clay pot of flowers, by a door."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 14, 4.5, 1.4, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    s.box(5, 9, 12, 14, "orange", 2.0)
    s.ellipsoid(8.5, 6.5, 4.5, 3.5, "leaf", 4.0)
    for i, (x, y) in enumerate(((6, 5), (9, 4), (11, 7), (7, 8))):
        s.put(x, y, colour, 5.0, 90 + i, 3)
    ink_outline(s)
    return s.render()


def well():
    """A stone well : a round wall of stones, dark water inside, a little wooden roof."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 13.5, 7, 2, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    pid = s.new_id()
    for y in range(6, 15):
        for x in range(2, 15):
            d = ((x + 0.5 - 8.5) / 6.5) ** 2 + ((y + 0.5 - 10) / 4.5) ** 2
            if d <= 1:
                inner = ((x + 0.5 - 8.5) / 4.5) ** 2 + ((y + 0.5 - 9) / 2.2) ** 2 <= 1
                s.put(x, y, "water" if inner else "stone", 3, pid, 0.4 if inner else 3 if (x + y) % 3 else 2)
    for x in (3, 13):
        s.box(x, 1, x + 1, 9, "wood", 4.0)
    s.box(1, 0, 16, 3, "roofr", 5.0)
    ink_outline(s)
    return s.render()


def barrel():
    """A wooden barrel, its hoops."""
    s = Scene(N, N)
    s.ellipsoid(8.5, 13.5, 5, 1.6, "grass", 0)
    for y in range(N):
        for x in range(N):
            if s.pid[y][x] > 1:
                s.tone[y][x] = 0.7
    pid = s.ellipsoid(8, 8.5, 5, 6, "wood", 5.0)
    for y in (5, 12):
        for x in range(2, 14):
            if s.pid[y][x] == pid:
                s.mat[y][x], s.tone[y][x] = "lamp", 1
    s.ellipsoid(8, 3.5, 4, 1.4, "wood", 6.0)
    ink_outline(s)
    return s.render()


def lily():
    """Water lilies on a pond : round pads, a pink flower (over the water)."""
    s = Scene(N, N)
    for i, (cx, cy, r) in enumerate(((5, 5, 3.2), (11, 10, 2.6), (4, 12, 2.0))):
        pid = s.ellipsoid(cx, cy, r, r * 0.8, "leaf", 1.0)
        for y in range(N):
            for x in range(N):
                if s.pid[y][x] == pid and x > cx and abs(y - cy) < 0.6:
                    s.mat[y][x] = None                # the notch of the pad
    for x, y in ((5, 4), (6, 4), (5, 5)):
        s.put(x, y, "roofr", 2.0, 77, 3)
    s.put(6, 5, "white", 2.0, 77, 3)
    ink_outline(s)
    return s.render()


def fence_post(kind):
    """The fences of a garden : "v" a side (posts down the tile, rails along), "tl"/"tr" the corners."""
    s = Scene(N, N)
    grass_field(s, 6)
    pid = s.new_id()
    if kind == "v":
        for y in range(N):
            for x in (7, 8):
                s.put(x, y, "wood", 2, pid, 3 if x == 7 else 1)
        for y in (2, 10):
            for x in range(6, 10):
                s.put(x, y, "wood", 2.5, pid, 3)
                s.put(x, y + 1, "wood", 2.5, pid, 1)
        return s.render()
    for y in range(4, N):
        for x in (7, 8):
            s.put(x, y, "wood", 2, pid, 3 if x == 7 else 1)
    rng = range(8, N) if kind == "tl" else range(0, 9)
    for y, t in ((6, 3), (7, 2), (10, 3), (11, 2)):
        for x in rng:
            s.put(x, y, "wood", 1.5, pid, t)
    return s.render()


def bridge():
    """6 x 3 tiles : the deck in planks across the river, a rail at the back and at the front, the side of the deck
    at the bottom. Columns 1 to 4 are the same (the planks repeat every 16 voxels)."""
    W, H = 6 * N, 3 * N
    s = Scene(W, H)
    end = lambda x: x < N or x >= W - N
    for y in range(H):
        for x in range(W):
            if y < 2 or y > 45:                      # around the bridge : the ground at the ends, the water below
                s.put(x, y, "grass" if end(x) else "water", 0, 0, 1)
    for x in range(W):                               # the planks : boards laid across, their ends staggered
        p, r = divmod(x, 4)
        cut = 8 + int(h01(p % 4, 0, 9) * 26)        # where two boards meet (period 16 voxels : the tiles repeat)
        for y in range(4, 41):
            t = 1 if r == 3 else 3 if r == 0 else 2
            if y == cut and r != 3:
                t = 1
            if r == 1 and y in (cut - 2, cut + 2):
                t = 0                                # the nails at the ends of the boards
            s.put(x, y, "wood", 0, 0, t)
    for x in range(W):
        s.put(x, 4, "wood", 0, 0, 1 if x % 4 != 3 else 0)  # the shadow of the back rail
    for y0 in (1, 37):                               # rails : a thin beam on low posts every 16 voxels
        for x in range(W):
            s.put(x, y0, "wood", 0, 0, 3)
            s.put(x, y0 + 1, "wood", 0, 0, 1)
        for px in range(6, W, 16):
            for x in (px, px + 1):
                for y in range(y0 - 1, y0 + 4):
                    s.put(x, y, "wood", 0, 0, 3 if x == px else 1)
            s.put(px, y0 + 4, "wood", 0, 0, 0)
            s.put(px + 1, y0 + 4, "wood", 0, 0, 0)
    for y in range(41, 46):                          # the side of the deck
        for x in range(W):
            s.put(x, y, "wood", 0, 0, 1 if y < 44 else 0)
        for x in range(1, W, 4):
            s.put(x, 42, "wood", 0, 0, 2)
    for x in range(W):
        s.put(x, 46, "water" if not end(x) else "grass", 0, 0, 0)
    for x in (0, 1, W - 2, W - 1):                   # the beams at the ends of the deck
        for y in range(2, 46):
            s.put(x, y, "wood", 0, 0, 0 if x in (0, W - 1) else 1)
    return s


# ---------------------------------------------------------------- restyle : a tile kept for its shape (cliffs, mountains)


# ---------------------------------------------------------------- tiles over tiles

def over(tile, ground):
    """The tile over the ground : its key colour shows the ground."""
    out = ground.copy()
    mask = Image.new("L", (T, T), 0)
    mp, tp = mask.load(), tile.load()
    for y in range(T):
        for x in range(T):
            mp[x, y] = 0 if tp[x, y][:3] == KEY else 255
    out.paste(tile, (0, 0), mask)
    return out


def blank(tile):
    p = tile.load()
    return all(p[x, y][:3] == KEY for y in range(T) for x in range(T))


# ---------------------------------------------------------------- the painted drawings

def _painted_or(name):
    """A drawing replaced by its painted image when it is there (painted : name(*args), the arguments of the drawing)."""
    def wrap(draw):
        def drawing(*args):
            image = painted(name(*args))
            return image if image is not None else draw(*args)
        drawing.__doc__ = draw.__doc__
        return drawing
    return wrap


# The objects, the posts, the animated tiles (tools/render3d scenes/tiles_objects.js)
for _name in ("bench", "mailbox", "barrel", "bush", "well", "lily", "boulder", "log_seat", "sign", "crops"):
    globals()[_name] = _painted_or(lambda n=_name: f"obj_{n}")(globals()[_name])
bin_ = _painted_or(lambda: "obj_bin")(bin_)
lamp = _painted_or(lambda: "lamp")(lamp)
lamp_head = _painted_or(lambda f: f"lamphead_{f}")(lamp_head)
fence = _painted_or(lambda: "fence")(fence)
fence_post = _painted_or(lambda kind: f"fence_{kind}")(fence_post)
flowerpot = _painted_or(lambda colour="roofr": f"flowerpot_{colour}")(flowerpot)
emblem = _painted_or(lambda c, r: f"emblem_{c}_{r}")(emblem)
water = _painted_or(lambda f: f"water_{f}")(water)
flowers = _painted_or(lambda f: f"flower_{f}")(flowers)
campfire = _painted_or(lambda f: f"campfire_{f}")(campfire)
butterfly = _painted_or(lambda f: f"butterfly_{f}")(butterfly)
fountain = _painted_or(lambda c, r, f: f"fountain_{c}_{r}_{f}")(fountain)
flowerbed_anim = _painted_or(lambda colour, f: f"flowerbed_{colour}_{f}")(flowerbed_anim)
