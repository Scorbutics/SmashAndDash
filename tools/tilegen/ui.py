#!/usr/bin/env python3
"""ui : the interface of the game, in the Okami look of the painted tiles (tools/render3d) : panels of warm parchment
(its fibres, soft stains, aged towards its edges) framed by a sumi brush stroke that swells and tapers ; the coloured
pills are washes of their colour (teal, cinnabar, sumi) on the same paper.

Writes the theme of the GUI (Menu/default_theme : the windows and dialogs are a 9-slice of menu.png, 32 px parts ;
buttons, fields, boxes, sliders, slots, hit point bar, cursors), at the sizes the game expects, and the icons of the
action buttons (Sprites/Icones/100..104 : bag, trainer card, team, pokedex, settings) as small voxel models.

  tools/tilegen/ui.py [root]       (default : the current directory)
"""
import math
import os
import sys

from PIL import Image

import tilegen as g

KEY = g.KEY + (255,)
CLEAR = (0, 0, 0, 0)
INK = (27, 23, 18, 255)                              # sumi
RAMPS = {
    "frame": [(26, 92, 108), (42, 136, 148), (84, 186, 188), (156, 228, 218)],
    "body": [(206, 196, 176), (232, 226, 208), (248, 244, 232), (255, 254, 248)],
    "dark": [(34, 40, 56), (52, 60, 80), (76, 86, 110), (112, 124, 150)],
    "red": [(150, 32, 40), (206, 58, 56), (238, 100, 84), (252, 160, 136)],
    "gold": [(170, 112, 20), (222, 164, 40), (246, 206, 80), (255, 238, 150)],
    "white": [(190, 194, 204), (226, 230, 236), (246, 248, 250), (255, 255, 255)],
}


def tone(ramp, t):
    return g.lerp_ramp(RAMPS[ramp], max(0.0, min(3.0, t))) + (255,)


def rounded_mask(w, h, r):
    """The cells of a w x h rectangle with rounded corners (radius r cells)."""
    inside = set()
    for y in range(h):
        for x in range(w):
            dx = max(r - x - 0.5, 0, x + 0.5 - (w - r))
            dy = max(r - y - 0.5, 0, y + 0.5 - (h - r))
            if dx * dx + dy * dy <= r * r:
                inside.add((x, y))
    return inside


# The washes of the panels (their body) and the inks of their border, after the names of the ramps
WASH = {"body": (243, 230, 200), "frame": (112, 190, 184), "red": (198, 58, 46), "dark": (54, 46, 38),
        "gold": (236, 196, 96), "white": (250, 246, 236)}
BORDER_INK = {"frame": (27, 23, 18), "dark": (20, 16, 12), "red": (27, 23, 18), "gold": (160, 104, 20),
              "body": (27, 23, 18), "white": (27, 23, 18)}
SS = 4                                               # supersampling of the brush


def _lattice(seed, n):
    """A periodic value noise of n x n cells (a function of 0 .. 1 x 0 .. 1)."""
    rng = __import__("random").Random(seed)
    grid = [[rng.random() for _ in range(n)] for _ in range(n)]

    def at(u, v):
        x, y = u * n, v * n
        i, j = int(math.floor(x)) % n, int(math.floor(y)) % n
        fx, fy = x - math.floor(x), y - math.floor(y)
        fx, fy = fx * fx * (3 - 2 * fx), fy * fy * (3 - 2 * fy)
        a, b = grid[j][i], grid[j][(i + 1) % n]
        c, d = grid[(j + 1) % n][i], grid[(j + 1) % n][(i + 1) % n]
        return (a + (b - a) * fx) * (1 - fy) + (c + (d - c) * fx) * fy
    return at


def panel(w, h, px=3, frame="frame", body="body", radius=3, border=2, light=0.0, background=CLEAR):
    """A panel of w x h pixels : a wash of parchment (or of the colour of "body") with its fibres and stains, darker
    towards its edges, framed by a brush stroke of sumi (its width : "border" cells of "px" pixels, swelling and tapering
    along it, heavier at the corners). light : lighter (hover) or darker (pressed). (The cells of the voxel panels are
    gone : px, radius and border only give the sizes.)"""
    W, H = w * SS, h * SS
    r = max(2.0, radius * px) * SS
    stroke = max(1.6, border * px * 0.9) * SS
    inset = stroke * 0.55
    seed = hash((w, h, frame, body, radius, border)) & 0xFFFF
    paper = _lattice(seed, 5)
    stains = _lattice(seed + 7, 3)
    fibre = _lattice(seed + 13, 23)
    wash = WASH.get(body, WASH["body"])
    k = 1.0 + 0.07 * light
    img = Image.new("RGBA", (W, H), background)
    pix = img.load()

    def inside(x, y, shrink):
        # the distance out of the rounded rectangle inset by "shrink" (< 0 : inside)
        cx = min(max(x, r + shrink), W - r - shrink)
        cy = min(max(y, r + shrink), H - r - shrink)
        return math.hypot(x - cx, y - cy) - (r)

    for y in range(H):
        for x in range(W):
            d = inside(x + 0.5, y + 0.5, inset)
            if d > stroke * 0.6:
                continue
            u, v = x / W, y / H
            tone_ = 0.94 + 0.08 * paper(u, v) - 0.06 * stains(u * 1.3, v * 1.3) + 0.03 * (fibre(u * 3, v) - 0.5)
            edge = max(0.0, min(1.0, (d + 6 * SS) / (6 * SS)))   # aged towards the edge
            tone_ *= 1.0 - 0.1 * edge
            c = tuple(max(0, min(255, int(ch * tone_ * k))) for ch in wash)
            pix[x, y] = c + (255,)
    # The brush : around the shape, its width swelling and tapering (a periodic noise along it), heavier at the corners
    ink = BORDER_INK.get(frame, BORDER_INK["frame"])
    along = _lattice(seed + 29, 7)
    left, top, right, bottom = inset, inset, W - inset, H - inset
    rr = max(1.0, min(r - inset, (right - left) / 2, (bottom - top) / 2))
    step = SS * 0.5
    points = []                                      # (x, y, at a corner)

    def line(x0, y0, x1, y1):
        n = max(1, int(math.hypot(x1 - x0, y1 - y0) / step))
        points.extend((x0 + (x1 - x0) * i / n, y0 + (y1 - y0) * i / n, False) for i in range(n))

    def arc(cx, cy, a0):
        n = max(2, int(math.pi * rr / 2 / step))
        points.extend((cx + rr * math.cos(a0 + math.pi / 2 * i / n), cy + rr * math.sin(a0 + math.pi / 2 * i / n), True)
                      for i in range(n))

    line(left + rr, top, right - rr, top)
    arc(right - rr, top + rr, -math.pi / 2)
    line(right, top + rr, right, bottom - rr)
    arc(right - rr, bottom - rr, 0.0)
    line(right - rr, bottom, left + rr, bottom)
    arc(left + rr, bottom - rr, math.pi / 2)
    line(left, bottom - rr, left, top + rr)
    arc(left + rr, top + rr, math.pi)
    path = [(x, y, stroke * (0.5 + 0.95 * along(i / len(points), 0.5)) * (1.25 if corner else 1.0))
            for i, (x, y, corner) in enumerate(points)]
    from PIL import ImageDraw
    draw = ImageDraw.Draw(img)
    for x, y, width in path:
        rad = width / 2
        draw.ellipse((x - rad, y - rad, x + rad, y + rad), fill=ink + (255,))
    out = img.resize((w, h), Image.LANCZOS)
    return out


def balloon():
    """The speech balloon (96 x 96, 9-slice) : a panel with a tail at the bottom left."""
    img = panel(96, 90, background=CLEAR)
    out = Image.new("RGBA", (96, 96), CLEAR)
    out.paste(img, (0, 0))
    pix = out.load()
    for j in range(8):                               # the tail, under the bottom left part
        for i in range(9 - j):
            x, y = 15 + i, 84 + j
            pix[x, y] = INK if i in (0, 8 - j) or j == 7 else tone("body", 1.6)
    return out


def field(w, h, border="frame", light=0.0):
    """A text field, a list : a cream body sunk in a frame (the shade at its top)."""
    return panel(w, h, px=1, radius=3, border=2, light=light, frame=border)


def listbox(w, h, light=0.0):
    img = field(w, h, light=light)
    pix = img.load()
    a = w - h                                        # the arrow button at the right
    for y in range(1, h - 1):
        for x in range(a, w - 1):
            if pix[x, y] != CLEAR and pix[x, y] != INK:
                pix[x, y] = tone("frame", 1.9 + light + (0.5 if y < h / 2 else 0))
        pix[a, y] = INK
    cx, cy = a + h // 2, h // 2
    for k in range(4):                               # the arrow, down
        for x in range(cx - 3 + k, cx + 4 - k):
            pix[x, cy - 1 + k] = tone("white", 3)
    return img


def button(light=0.0):
    """A pill button (96 x 16)."""
    return panel(96, 16, px=1, frame="frame", body="frame", radius=6, border=1, light=light)


def check(w, round_=False, light=0.0):
    img = panel(w, w, px=1, radius=w // 2 if round_ else 3, border=2, light=light)
    return img


def mark(w, round_=False):
    """The mark of a check box (a check) or a radio button (a dot), w x w."""
    img = Image.new("RGBA", (w, w), CLEAR)
    pix = img.load()
    if round_:
        for y in range(w):
            for x in range(w):
                d = math.hypot(x + 0.5 - w / 2, y + 0.5 - w / 2)
                if d <= w / 2 - 1.5:
                    pix[x, y] = tone("frame", 2.6 if y < w / 2 - 1 else 1.7)
                elif d <= w / 2 - 0.5:
                    pix[x, y] = INK
        return img
    for k, (x, y) in enumerate([(1, 5), (2, 6), (3, 7), (4, 8), (5, 7), (6, 6), (7, 5), (8, 4), (9, 3)]):
        for dy in (-1, 0, 1):
            pix[x, y + dy] = INK if dy != 0 else tone("frame", 2.6)
    return img


def close_button(light=0.0):
    img = Image.new("RGBA", (16, 16), CLEAR)
    pix = img.load()
    for y in range(16):
        for x in range(16):
            d = math.hypot(x + 0.5 - 8, y + 0.5 - 8)
            if d <= 7.5:
                pix[x, y] = INK if d > 6.5 else tone("red", 2.2 + light + (0.6 if y < 7 else 0))
    for k in range(-3, 4):                           # the cross
        for x, y in ((8 + k, 8 + k), (8 + k, 7 - k)):
            pix[x, y] = tone("white", 3)
    return img


def slot(highlight=False):
    """An inventory slot (32 x 32)."""
    return panel(32, 32, px=2, radius=3, border=1, frame="gold" if highlight else "frame",
                 light=0.4 if highlight else 0.0)


def hpbar():
    """The frame of the hit point bar (69 x 15) : a dark sunk groove."""
    return panel(69, 15, px=1, frame="dark", body="dark", radius=5, border=1, light=-0.6)


def hpbar_content():
    """The content of the hit point bar (74 x 15) : red at the left (few hit points) to green, a light at its top."""
    img = Image.new("RGBA", (74, 15), CLEAR)
    pix = img.load()
    stops = [(0.0, (214, 52, 52)), (0.35, (240, 150, 40)), (0.6, (236, 214, 60)), (1.0, (64, 196, 86))]
    for x in range(74):
        t = x / 73
        for (t0, c0), (t1, c1) in zip(stops, stops[1:]):
            if t0 <= t <= t1:
                f = (t - t0) / (t1 - t0)
                base = tuple(int(c0[i] + (c1[i] - c0[i]) * f) for i in range(3))
        for y in range(15):
            k = 1.25 if y < 4 else 1.0 if y < 11 else 0.78
            pix[x, y] = tuple(min(255, int(v * k)) for v in base) + (255,)
    return img


def cursor(grab=False):
    w = 15 if grab else 16
    img = Image.new("RGBA", (w, w), CLEAR)
    pix = img.load()
    if grab:                                         # a closed hand
        for y in range(3, 13):
            for x in range(2, 13):
                if (x - 7.5) ** 2 / 30 + (y - 8) ** 2 / 22 <= 1:
                    pix[x, y] = tone("white", 3 if y < 7 else 2)
        for y in range(w):
            for x in range(w):
                if pix[x, y] == CLEAR and any(0 <= x + dx < w and 0 <= y + dy < w and pix[x + dx, y + dy] != CLEAR
                                              and pix[x + dx, y + dy] != INK for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                    pix[x, y] = INK
        return img
    for y in range(13):                              # an arrow
        for x in range(y + 1):
            if x <= 9 - max(0, y - 9) * 3:
                pix[x, y] = tone("white", 3 if x < y / 2 else 2)
    for y in range(9, 15):
        for x in range(4, 7):
            pix[x + (y - 9) // 2, y] = tone("white", 2.5)
    for y in range(w):
        for x in range(w):
            if pix[x, y] == CLEAR and any(0 <= x + dx < w and 0 <= y + dy < w and pix[x + dx, y + dy] not in (CLEAR, INK)
                                          for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                pix[x, y] = INK
    return img


# ---------------------------------------------------------------- the icons (small voxel models)

def icon_scene(w, h):
    return g.Scene(w, h)


def icon_team():
    """A ball : red top, white bottom, a thin black band, a white button ringed in black."""
    s = icon_scene(11, 11)
    pid = s.ellipsoid(5.5, 5.5, 5.2, 5.2, "roofr", 5.0)
    for y in range(11):
        for x in range(11):
            if s.pid[y][x] == pid:
                if y >= 6:
                    s.mat[y][x] = "white"
                if y == 5:
                    s.mat[y][x], s.tone[y][x] = "ink", 0
    for x, y in ((5, 4), (4, 5), (6, 5), (5, 6)):
        s.put(x, y, "ink", 6, pid, 0)
    s.put(5, 5, "white", 6, pid, 3)
    g.ink_outline(s)
    return s.render()


def icon_bag():
    """A backpack : its body, its flap, two pockets, the strap."""
    s = icon_scene(14, 12)
    s.box(1, 3, 13, 12, "roofb", 2.0)
    s.box(2, 1, 12, 5, "roofb", 3.0)
    for x in range(2, 12):
        s.put(x, 5, "roofb", 3.0, s.pid[5][x], 1)
    for x0 in (3, 8):
        s.box(x0, 7, x0 + 3, 11, "roofb", 3.0, 3)
        s.put(x0 + 1, 7, "flower", 3.5, s.pid[7][x0], 3)
    s.box(6, 0, 8, 2, "wood", 3.0)
    g.ink_outline(s)
    return s.render()


def icon_card():
    """A trainer card : a white card, a red band, a photo, lines."""
    s = icon_scene(11, 9)
    s.box(0, 1, 11, 9, "white", 2.0)
    for x in range(11):
        s.put(x, 1, "roofr", 2.0, s.pid[1][0], 2)
        s.put(x, 2, "roofr", 2.0, s.pid[1][0], 2)
    s.box(1, 4, 4, 8, "roofb", 2.5, 2)
    for y in (4, 6):
        for x in range(5, 10):
            s.put(x, y, "ink", 2.0, s.pid[4][1], 0)
    g.ink_outline(s)
    return s.render()


def icon_dex():
    """A pokedex : a red folding device, its screen, a button."""
    s = icon_scene(14, 12)
    s.box(0, 1, 14, 12, "roofr", 2.0)
    s.box(2, 3, 10, 9, "glass", 3.0, 2)
    s.ellipsoid(11.5, 9.5, 1.4, 1.4, "roofb", 3.5)
    s.ellipsoid(2, 0.8, 1.6, 1.2, "glass", 3.0)
    g.ink_outline(s)
    return s.render()


def icon_settings():
    """The settings : a light metal gear, its hole."""
    s = icon_scene(13, 13)
    pid = s.new_id()
    for y in range(13):
        for x in range(13):
            dx, dy = x + 0.5 - 6.5, y + 0.5 - 6.5
            d = math.hypot(dx, dy)
            r = 4.6 + (1.6 if math.cos(math.atan2(dy, dx) * 6) > 0.2 else 0.0)
            if 1.8 < d <= r:
                s.put(x, y, "white", 5.0 - d * 0.5, pid)
    g.ink_outline(s)
    return s.render()


def on_key(img, size):
    """An icon centred on a transparent (colour key) image of the size the game expects."""
    out = Image.new("RGBA", size, KEY)
    out.paste(img, ((size[0] - img.width) // 2, (size[1] - img.height) // 2), img.convert("RGBA").split()[3]
              if img.mode == "RGBA" else None)
    pix = out.load()
    for y in range(out.height):                      # the key colour of the tiles stays the key
        for x in range(out.width):
            if pix[x, y][:3] == g.KEY:
                pix[x, y] = KEY
    return out


def write(root):
    g.use_biome("meadow")
    theme = os.path.join(root, "Menu", "default_theme")
    files = {
        "menu.png": panel(96, 96),
        "balloon.png": balloon(),
        "button.png": button(),
        "button_hover.png": button(0.5),
        "button_pressed.png": button(-0.6),
        "listbox.png": listbox(96, 16),
        "listbox_hover.png": listbox(96, 16, 0.4),
        "listbox_pressed.png": listbox(96, 16, -0.5),
        "listbox-clean.png": field(96, 12),
        "listbox-clean_hover.png": field(96, 12, light=0.4),
        "listbox-clean_pressed.png": field(96, 12, light=-0.5),
        "textfield.png": field(128, 20),
        "textfield_hover.png": field(128, 20, light=0.4),
        "textfield_pressed.png": field(128, 20, border="gold"),
        "checkbox.png": check(15),
        "checkbox_hover.png": check(15, light=0.4),
        "checkbox_pressed.png": check(15, light=-0.5),
        "checkbox_check.png": mark(11),
        "radiobutton.png": check(15, True),
        "radiobutton_hover.png": check(15, True, 0.4),
        "radiobutton_pressed.png": check(15, True, -0.5),
        "radiobutton_check.png": mark(11, True),
        "slider.png": check(15, True, 0.2),
        "slider_hover.png": check(15, True, 0.6),
        "slider_pressed.png": check(15, True, -0.4),
        "slider_bar.png": field(96, 16, border="dark"),
        "close_button.png": close_button(),
        "close_button_hover.png": close_button(0.5),
        "close_button_pressed.png": close_button(-0.6),
        "inventory_square.png": slot(),
        "inventory_square_highlight.png": slot(True),
        "hpbar.png": hpbar(),
        "hpbarcontent.png": hpbar_content(),
        # Small panels (48 x 48, 9-slice of 16 px parts) for the screens that draw themselves (the battle : Theme)
        "panel_small.png": panel(48, 48, px=2, radius=3, border=1),
        "panel_small_teal.png": panel(48, 48, px=2, radius=3, border=1, body="frame", light=0.3),
        "panel_small_red.png": panel(48, 48, px=2, radius=3, border=1, frame="red", body="red"),
        "panel_small_dark.png": panel(48, 48, px=2, radius=3, border=1, frame="dark", body="dark", light=0.2),
        "mouse_cursor.png": cursor(),
        "mouse_cursor_grab.png": cursor(True),
    }
    for name, img in files.items():
        img.save(os.path.join(theme, name))
    icons = os.path.join(root, "Sprites", "Icones")
    for name, (img, size) in {"100": (icon_bag(), (45, 38)), "101": (icon_card(), (32, 32)),
                              "102": (icon_team(), (32, 32)), "103": (icon_dex(), (45, 37)),
                              "104": (icon_settings(), (40, 40))}.items():
        on_key(img, size).convert("RGB").save(os.path.join(icons, name + ".png"))
    return len(files) + 5


if __name__ == "__main__":
    print(f"{write(sys.argv[1] if len(sys.argv) > 1 else '.')} images")
