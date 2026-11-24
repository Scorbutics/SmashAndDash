#!/usr/bin/env python3
"""character : the sprite sheet of the hero, a 3D voxel model rendered in the style of the tiles (tilegen).

A chibi in the manner of Cube World : a big rounded head (about half of the height), big eyes low on the face, a
red cap over voluminous hair (tufts out of it), a small round body, round hands floating at its sides, stubby feet. The model is made of
rounded volumes (superellipsoids : rounded cubes) in its own space (x right, y forward, z up, the feet at z = 0, the
face looking to -y). For each direction it is turned and cut into voxels ; each pixel of the frame casts a ray into the
voxels, looking down from the front like the camera of the map. A voxel is shaded from the normal of the surface around
it (its empty neighbours), lit from the top left like the tiles : the round volumes get round gradients. The silhouette
gets the ink outline of the tiles.

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
SCALE = 1.0
INK = (28, 30, 40, 255)
# The light, in the space of the view (x right, y away from the camera, z up) : from the top left, in front
LIGHT = (-0.45, -0.6, 0.9)
_l = math.sqrt(sum(c * c for c in LIGHT))
LIGHT = tuple(c / _l for c in LIGHT)

# The colours of the hero : ramps of 4 tones, darkest to lightest (as the tiles) ; vivid, as in Cube World
PAL = {
    "skin": [(186, 110, 80), (228, 156, 116), (248, 196, 156), (255, 228, 196)],
    "hair": [(48, 26, 22), (84, 46, 32), (126, 74, 46), (172, 112, 70)],
    "cap": [(140, 30, 34), (200, 54, 50), (236, 92, 72), (252, 148, 116)],
    "capwhite": [(176, 180, 192), (222, 226, 234), (244, 246, 250), (255, 255, 255)],
    "tunic": [(28, 60, 140), (46, 98, 196), (82, 144, 232), (150, 200, 250)],
    "scarf": [(136, 26, 36), (198, 46, 52), (238, 84, 76), (252, 146, 124)],
    "belt": [(70, 40, 22), (110, 66, 36), (150, 100, 58), (190, 144, 96)],
    "pants": [(48, 40, 44), (76, 64, 66), (108, 94, 92), (146, 132, 126)],
    "boots": [(70, 38, 22), (112, 64, 36), (154, 98, 58), (196, 142, 96)],
    "eye": [(18, 20, 34)] * 4,
    "eyehi": [(255, 255, 255)] * 4,
    "blush": [(236, 128, 124)] * 4,
    "mouth": [(132, 48, 46)] * 4,
}


def rounded(q, c, r, p=2.6):
    """In a superellipsoid (a rounded cube for p > 2) of centre c and radii r."""
    return sum(abs((q[i] - c[i]) / r[i]) ** p for i in range(3)) <= 1.0


def material(q, step):
    """The material at a point of the model (x right, y forward, z up ; the face looks to -y), or None.
    step : -1, 0, 1, the walk (the left foot forward, standing, the right foot forward)."""
    x, y, z = q
    swing = 1.1 * step
    # The face : eyes (a highlight on top), blush, a small mouth, on the front of the head
    in_head = rounded(q, (0.0, 0.0, 13.4), (5.3, 4.8, 4.6))
    if in_head and y < -2.4:
        if 1.0 <= abs(x) <= 3.0 and 11.0 <= z <= 14.0:
            ex = x - (2.0 if x > 0 else -2.0)        # in the eye : -1 its left .. 1 its right
            return "eyehi" if z >= 13.0 and ex < 0 else "eye"
        if 3.0 < abs(x) <= 4.3 and 10.0 <= z <= 11.2:
            return "blush"
        if abs(x) <= 0.5 and 9.9 <= z <= 10.8:
            return "mouth"
    # The cap : a rounded dome on the hair, a white panel in front, a button on top ; a short brim over the eyes
    if rounded(q, (0.0, 0.7, 16.0), (5.6, 5.2, 3.0)) and z >= 15.6:
        if z >= 18.5 and abs(x) < 0.8 and abs(y - 0.7) < 0.8:
            return "capwhite"
        return "capwhite" if y < -2.6 and abs(x) < 1.8 and z < 17.8 else "cap"
    if 15.1 <= z <= 15.9 and (x / 4.2) ** 2 + ((y + 4.3) / 2.6) ** 2 <= 1.0 and y < -2.5:
        return "cap"
    # The hair : a shell over the head, open on the face, the fringe in points ; side locks ; tufts out of the cap
    fringe = 15.2 - (0.8 if math.floor(x + 10) % 3 == 0 else 0.0)
    if rounded(q, (0.0, 0.9, 14.1), (5.8, 5.2, 5.0)):
        face = in_head and y < -1.0 and z < fringe and abs(x) < 4.7
        if not face:
            return "hair"
    for c, r in (((0.4, 5.4, 15.0), (1.5, 1.8, 1.4)), ((-2.6, 5.0, 14.4), (1.3, 1.6, 1.3)),
                 ((5.4, 2.0, 15.0), (1.3, 1.4, 1.4)), ((-5.4, 1.4, 14.8), (1.3, 1.4, 1.3))):
        if rounded(q, c, r, 2.0):
            return "hair"
    if in_head:
        return "skin"
    # A scarf around the neck, its end behind
    if 8.3 <= z <= 9.5 and (x / 2.7) ** 2 + (y / 2.3) ** 2 <= 1.0:
        return "scarf"
    if 0.4 <= x <= 1.6 and 1.6 <= y <= 2.6 and 6.0 <= z <= 8.6:
        return "scarf"
    # The body : a round tunic, a belt
    if rounded(q, (0.0, 0.0, 6.2), (2.9, 2.3, 2.8), 2.4):
        return "belt" if 3.9 <= z <= 4.6 else "tunic"
    # The hands, round, floating at the sides, swinging against the feet
    for side in (-1, 1):
        if rounded(q, (side * 3.3, -0.5 - swing * side * 0.9, 5.0), (1.2, 1.2, 1.2), 2.0):
            return "skin"
    # Stubby legs and round boots, a step forward and back
    for side in (-1, 1):
        ly = swing * side
        if 1.6 <= z <= 3.8 and ((x - side * 1.2) / 0.95) ** 2 + ((y - ly) / 0.95) ** 2 <= 1.0:
            return "pants"
        if rounded(q, (side * 1.3, ly - 0.4, 1.05), (1.25, 1.9, 1.15), 2.2):
            return "boots"
    return None


def voxels(yaw, step):
    """The model turned (yaw in degrees : 0 the face to the camera, 90 to the right), cut into voxels :
    {(x, y, z): material}, x right, y away from the camera, z up."""
    a = math.radians(yaw)
    c, s = math.cos(a), math.sin(a)
    grid = {}
    for z in range(0, 24):
        for y in range(-10, 10):
            for x in range(-10, 10):
                # The centre of the voxel, back into the model space (turned by -yaw)
                px, py, pz = (x + 0.5) / SCALE, (y + 0.5) / SCALE, (z + 0.5) / SCALE
                qx, qy = c * px + s * py, -s * px + c * py
                m = material((qx, qy, pz), step)
                if m:
                    grid[(x, y, z)] = m
    return grid


def normal(grid, cell):
    """The normal of the surface at a voxel : away from its filled neighbours (3 x 3 x 3)."""
    nx = ny = nz = 0.0
    for dz in (-1, 0, 1):
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if (dx or dy or dz) and (cell[0] + dx, cell[1] + dy, cell[2] + dz) in grid:
                    nx -= dx
                    ny -= dy
                    nz -= dz
    n = math.sqrt(nx * nx + ny * ny + nz * nz)
    return (0.0, -0.5, 0.86) if n == 0 else (nx / n, ny / n, nz / n)


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
            y, z = -12.0, (h + 12.0 * se) / ce
            for _ in range(180):
                if z < -1 or y > 12:
                    break
                cell = (math.floor(x), math.floor(y), math.floor(z))
                m = grid.get(cell)
                if m:
                    hit[(u, v)] = (m, cell)
                    break
                y += 0.15 * ce
                z -= 0.15 * se
    shades = {}
    for (u, v), (m, cell) in hit.items():
        if cell not in shades:
            n = normal(grid, cell)
            lum = n[0] * LIGHT[0] + n[1] * LIGHT[1] + n[2] * LIGHT[2]
            shades[cell] = max(0.0, min(3.0, 0.5 + 2.6 * max(0.0, lum)))
        px[u, v] = g.lerp_ramp(PAL[m], shades[cell]) + (255,)
    for (u, v) in hit:                               # the ink outline of the silhouette
        for du, dv in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            n = (u + du, v + dv)
            if 0 <= n[0] < FRAME and 0 <= n[1] < FRAME and n not in hit:
                px[n[0], n[1]] = INK
    return img


def sheet():
    out = Image.new("RGBA", (6 * FRAME, 8 * FRAME), g.KEY + (255,))
    for row, yaw in enumerate(ROWS_YAW):
        for col, step in enumerate((-1, 0, 1)):
            out.paste(render(voxels(yaw, step)), (col * FRAME, row * FRAME))
    return out


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "Sprites/Charsets/0.png"
    sheet().save(path)
    print(path)
