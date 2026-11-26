#!/usr/bin/env python3
"""pokeball : the pokeball of the battles in the style of the tiles (tilegen) : chunky pixels, an ink outline, soft light
from the top left.

Writes, in Sprites/Fight :
  pokeball.png        4 frames of 16 x 16 : the ball spinning in the air (PokeballSystem)
  pokeball-open.png   16 x 16 : the ball open on the ground, light coming out of it
  pokeball-burst.png  64 x 64 : the light of the ball opening (rays and a halo), scaled and faded by the game

  tools/tilegen/pokeball.py [root]       (default : the current directory)
"""
import math
import os
import sys

from PIL import Image

INK = (34, 40, 54, 255)
CLEAR = (0, 0, 0, 0)
RED = [(150, 32, 40), (206, 58, 56), (238, 100, 84), (252, 160, 136)]
WHITE = [(190, 194, 204), (226, 230, 236), (246, 248, 250), (255, 255, 255)]
LIGHT = (255, 250, 214)


def ramp(colors, t):
    t = max(0.0, min(len(colors) - 1.0, t))
    i = min(len(colors) - 2, int(t))
    f = t - i
    a, b = colors[i], colors[i + 1]
    return tuple(int(round(a[k] + (b[k] - a[k]) * f)) for k in range(3)) + (255,)


def shade(x, y, cx, cy, r):
    """Light from the top left : 0 (shadow) .. 3 (highlight)."""
    dx, dy = (x - cx) / r, (y - cy) / r
    return 1.6 - 1.5 * (dx * 0.6 + dy * 0.8) - 0.6 * (dx * dx + dy * dy)


def outline(image):
    """An ink outline around the opaque pixels."""
    w, h = image.size
    pixels = image.load()
    out = image.copy()
    o = out.load()
    for y in range(h):
        for x in range(w):
            if pixels[x, y][3] != 0:
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                nx, ny = x + dx, y + dy
                if 0 <= nx < w and 0 <= ny < h and pixels[nx, ny][3] != 0 and pixels[nx, ny] != INK:
                    o[x, y] = INK
                    break
    return out


def ball(angle, size=16, radius=6.5):
    """The ball turned by "angle" (radians, around the axis facing the viewer) : red half, white half, a band, the button."""
    image = Image.new("RGBA", (size, size), CLEAR)
    p = image.load()
    c = (size - 1) / 2.0
    ca, sa = math.cos(angle), math.sin(angle)
    for y in range(size):
        for x in range(size):
            dx, dy = x - c, y - c
            if dx * dx + dy * dy > radius * radius:
                continue
            # In the frame of the ball
            u = dx * ca + dy * sa
            v = -dx * sa + dy * ca
            light = shade(x, y, c, c, radius)
            if u * u + v * v <= 2.2 * 2.2:
                p[x, y] = INK if u * u + v * v > 1.3 * 1.3 else ramp(WHITE, light + 0.5)
            elif abs(v) <= 0.9:
                p[x, y] = INK
            elif v < 0:
                p[x, y] = ramp(RED, light)
            else:
                p[x, y] = ramp(WHITE, light)
    return outline(image)


def open_ball(size=16):
    """The ball open : the white half on the ground, the red half lifted and tilted, light between them."""
    image = Image.new("RGBA", (size, size), CLEAR)
    p = image.load()
    c = (size - 1) / 2.0
    r = 6.0
    for y in range(size):
        for x in range(size):
            dx = x - c
            # The bottom half, on the ground
            dy = y - (c + 3)
            if dx * dx + dy * dy <= r * r and dy >= 0:
                p[x, y] = INK if dy < 1 else ramp(WHITE, shade(x, y, c, c + 3, r))
            # The top half, lifted
            dy = y - (c - 1)
            if dx * dx + dy * dy <= r * r and dy <= -0.5:
                p[x, y] = INK if dy > -1.5 else ramp(RED, shade(x, y, c, c - 1, r))
    # The light inside
    for y in range(int(c - 1), int(c + 4)):
        for x in range(size):
            dx = abs(x - c)
            if dx < r - 1 and p[x, y][3] == 0:
                p[x, y] = LIGHT + (255,)
    return outline(image)


def burst(size=64):
    """Rays and a halo of light, white in the middle : drawn over the ball when it opens."""
    image = Image.new("RGBA", (size, size), CLEAR)
    p = image.load()
    c = (size - 1) / 2.0
    rays = 10
    for y in range(size):
        for x in range(size):
            dx, dy = x - c, y - c
            d = math.hypot(dx, dy) / c
            if d > 1.0:
                continue
            angle = math.atan2(dy, dx)
            ray = abs(math.cos(angle * rays / 2.0)) ** 8
            halo = max(0.0, 1.0 - d / 0.5) ** 0.7
            strength = max(halo, ray * max(0.0, 1.0 - d))
            # Chunky : 4 steps of light
            level = math.floor(strength * 4.0) / 4.0
            if level <= 0:
                continue
            white = min(1.0, level * 1.4)
            color = tuple(int(LIGHT[k] + (255 - LIGHT[k]) * white) for k in range(3))
            p[x, y] = color + (int(255 * min(1.0, level * 1.2 + 0.3)),)
    return image


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    folder = os.path.join(root, "Sprites", "Fight")
    os.makedirs(folder, exist_ok=True)
    spin = Image.new("RGBA", (64, 16), CLEAR)
    for frame in range(4):
        spin.alpha_composite(ball(frame * math.pi / 2.0), (frame * 16, 0))
    spin.save(os.path.join(folder, "pokeball.png"))
    open_ball().save(os.path.join(folder, "pokeball-open.png"))
    burst().save(os.path.join(folder, "pokeball-burst.png"))


if __name__ == "__main__":
    main()
