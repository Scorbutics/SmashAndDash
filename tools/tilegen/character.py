#!/usr/bin/env python3
"""character : the sprite sheet of the hero, a 3D voxel model rendered in the style of the tiles (tilegen).

The model is made of simple volumes (boxes, ellipsoids) in its own space (x right, y forward, z up, the feet at
z = 0). For each direction it is turned, cut into voxels, then each pixel of the frame casts a ray into the voxels,
looking down from the front like the camera of the map : the top faces are lit, the front faces less, the sides by the
light from the left ; the silhouette gets the ink outline of the tiles.

The sheet has the layout of the character sheets (Sprites/Charsets) : 6 x 8 frames of 72 px, 3 animated (a step, the
standing pose, the other step). Rows : 0 back, 1 right, 2 front, 3 left, 4 back-left, 5 back-right, 6 front-left,
7 front-right.

  tools/tilegen/character.py [sheet.png]     (default Sprites/Charsets/0.png ; then tools/assets/surf_sprite.py
                                              makes the surfing sheet from it)
"""
import math
import sys

from PIL import Image

import tilegen as g

FRAME = 72
V = 3                                   # pixels per voxel, as the tiles
ELEVATION = math.radians(28)            # the camera looks down : the top faces show
FEET = 66                               # the row of the frame the feet stand on (the surfing sheet cuts at 52)
ROWS_YAW = [180, 90, 0, 270, 225, 135, 315, 45]
SCALE = 1.15                            # the model, larger in the voxels (about 1.25 block high)
INK = (28, 34, 44, 255)

# The colours of the hero : ramps of 4 tones, darkest to lightest (as the tiles)
PAL = {
    "skin": [(168, 104, 74), (214, 150, 108), (240, 190, 148), (252, 222, 186)],
    "hair": [(54, 32, 22), (88, 54, 34), (126, 82, 50), (164, 116, 72)],
    "cap": [(140, 36, 34), (196, 58, 48), (230, 96, 70), (248, 150, 112)],
    "capwhite": [(170, 176, 186), (220, 224, 230), (244, 246, 248), (255, 255, 255)],
    "jacket": [(22, 76, 98), (34, 116, 138), (60, 160, 172), (128, 210, 210)],
    "shirt": [(170, 176, 186), (220, 224, 230), (244, 246, 248), (255, 255, 255)],
    "pants": [(26, 30, 52), (40, 48, 82), (62, 74, 116), (98, 112, 156)],
    "shoes": [(120, 30, 30), (178, 50, 42), (220, 84, 62), (244, 140, 110)],
    "sole": [(150, 150, 150), (200, 200, 200), (232, 232, 232), (250, 250, 250)],
    "bag": [(150, 98, 20), (206, 146, 34), (238, 188, 62), (252, 226, 120)],
    "eye": [(20, 22, 34)] * 4,
}


def box(q, x0, x1, y0, y1, z0, z1):
    return x0 <= q[0] <= x1 and y0 <= q[1] <= y1 and z0 <= q[2] <= z1


def ellipsoid(q, cx, cy, cz, rx, ry, rz):
    return ((q[0] - cx) / rx) ** 2 + ((q[1] - cy) / ry) ** 2 + ((q[2] - cz) / rz) ** 2 <= 1.0


def material(q, step):
    """The material at a point of the model (x right, y forward, z up ; the face looks to -y), or None.
    step : -1, 0, 1, the walk (the left foot forward, standing, the right foot forward)."""
    x, y, z = q
    swing = 1.3 * step
    # The head : big (chibi), the face to the front ; the eyes ; the cap and its brim ; the hair under it
    head = (0.0, -0.3, 13.6)
    if ellipsoid(q, head[0], head[1], head[2], 4.6, 4.3, 4.2):
        if z > 15.0:
            return "capwhite" if y < -2.2 and z < 17.0 and abs(x) < 2.2 else "cap"
        if abs(abs(x) - 1.6) < 0.6 and 11.0 <= z <= 12.9 and y < -2.6:
            return "eye"
        if y > -1.2 or abs(x) > 3.7 or (z > 14.0 and y > -3.6):
            return "hair"
        return "skin"
    if box(q, -2.8, 2.8, -5.6, -3.0, 14.6, 15.4) and ((x / 3.0) ** 2 + ((y + 3.2) / 2.5) ** 2) <= 1.0:
        return "cap"                                     # the brim
    if box(q, -0.9, 0.9, -0.6, 0.6, 9.4, 10.2):
        return "skin"                                    # the neck
    # The backpack, behind the body
    if box(q, -2.2, 2.2, 1.6, 3.4, 5.6, 9.8):
        return "bag"
    # The body : the jacket open on the shirt
    if ellipsoid(q, 0.0, 0.0, 7.6, 3.0, 2.0, 2.9) and z >= 5.0:
        if abs(x) < 0.8 and y < -1.0 and z > 6.0:
            return "shirt"
        return "jacket"
    # The arms, swinging against the legs, the hands
    for side in (-1, 1):
        ay = -swing * side * 0.8
        if box(q, side * 3.0 - 0.75, side * 3.0 + 0.75, ay - 0.8, ay + 0.8, 6.2, 9.4):
            return "jacket"
        if box(q, side * 3.0 - 0.7, side * 3.0 + 0.7, ay - 0.7, ay + 0.7, 5.0, 6.2):
            return "skin"
    # The legs and the shoes, a step forward and back
    for side in (-1, 1):
        ly = swing * side
        if box(q, side * 1.3 - 0.9, side * 1.3 + 0.9, ly - 0.9, ly + 0.9, 1.6, 5.2):
            return "pants"
        if box(q, side * 1.3 - 1.0, side * 1.3 + 1.0, ly - 1.7, ly + 1.0, 0.0, 1.6):
            return "sole" if z < 0.5 else "shoes"
    return None


def voxels(yaw, step):
    """The model turned (yaw in degrees : 0 the face to the camera, 90 to the right), cut into voxels :
    {(x, y, z): material}, x right, y away from the camera, z up."""
    a = math.radians(yaw)
    c, s = math.cos(a), math.sin(a)
    grid = {}
    for z in range(0, 23):
        for y in range(-10, 10):
            for x in range(-10, 10):
                # The centre of the voxel, back into the model space (turned by -yaw)
                px, py, pz = (x + 0.5) / SCALE, (y + 0.5) / SCALE, (z + 0.5) / SCALE
                qx, qy = c * px + s * py, -s * px + c * py
                m = material((qx, qy, pz), STEP[0])
                if m:
                    grid[(x, y, z)] = m
    return grid


STEP = [0]


def render(grid):
    """The frame of a voxel grid : each pixel casts a ray from the camera (in front, above) into the grid."""
    img = Image.new("RGBA", (FRAME, FRAME), g.KEY + (255,))
    px = img.load()
    ce, se = math.cos(ELEVATION), math.sin(ELEVATION)
    # Screen : u = x * V ; v = FEET - (z * ce + y * se) * V. The ray goes away and down : (0, ce, -se)
    hit = {}
    for v in range(FRAME):
        for u in range(FRAME):
            x = (u + 0.5 - FRAME / 2) / V
            h = (FEET - (v + 0.5)) / V               # = z * ce + y * se along the ray
            # Start in front and above : y = -12, z from h
            y, z = -12.0, (h + 12.0 * se) / ce
            prev = None
            for _ in range(160):
                cell = (math.floor(x), math.floor(y), math.floor(z))
                if z < -1 or y > 12:
                    break
                m = grid.get(cell)
                if m:
                    # The face it entered by : from above (the top), from the front, or a side
                    face = "front"
                    if prev is not None:
                        if prev[2] != cell[2]:
                            face = "top"
                        elif prev[0] != cell[0]:
                            face = "side"
                    lit = grid.get((cell[0], cell[1], cell[2] + 1)) is None
                    hit[(u, v)] = (m, face, cell, lit)
                    break
                prev = cell
                y += 0.15 * ce
                z -= 0.15 * se
    for (u, v), (m, face, cell, lit) in hit.items():
        t = {"top": 2.7, "front": 1.8, "side": 1.3}[face]
        if face == "front" and not lit:
            t -= 0.3                                 # under something (the brim, the head) : in its shadow
        if face == "side" and grid.get((cell[0] - 1, cell[1], cell[2])) is None:
            t += 0.7                                 # the side to the left, in the light
        px[u, v] = g.lerp_ramp(PAL[m], max(0.0, min(3.0, t))) + (255,)
    # The ink outline of the silhouette
    for (u, v) in hit:
        for du, dv in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            n = (u + du, v + dv)
            if 0 <= n[0] < FRAME and 0 <= n[1] < FRAME and n not in hit:
                px[n[0], n[1]] = INK
    return img


def sheet():
    out = Image.new("RGBA", (6 * FRAME, 8 * FRAME), g.KEY + (255,))
    for row, yaw in enumerate(ROWS_YAW):
        for col, step in enumerate((-1, 0, 1)):
            STEP[0] = step
            out.paste(render(voxels(yaw, step)), (col * FRAME, row * FRAME))
    return out


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "Sprites/Charsets/0.png"
    sheet().save(path)
    print(path)
