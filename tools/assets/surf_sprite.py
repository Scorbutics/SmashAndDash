#!/usr/bin/env python3
# Generates the sprite sheet of the hero surfing (Sprites/Charsets/<id>.png) : the upper body of the hero (its sheet)
# sitting on a water Pokemon, on the 4 directions, 3 frames bobbing on the water. Same layout as the character
# sheets : 6 x 8 frames of 72 px, 3 animated, rows 0 up, 1 right, 2 down, 3 left, 4 to 7 the diagonals.
# Usage : tools/assets/surf_sprite.py Sprites/Charsets/0.png Sprites/Charsets/1000.png (needs Pillow)
import sys
from PIL import Image, ImageDraw

FRAME = 72
PIXEL = 2            # the mount is drawn with 2 px pixels (the size of the pixels of the hero)
LOW = FRAME // PIXEL
KEY = (190, 151, 255, 255)
OUTLINE = (34, 52, 104, 255)
BODY = (76, 136, 214, 255)
LIGHT = (150, 200, 245, 255)
SHELL = (206, 206, 220, 255)
SHELL_DARK = (140, 140, 162, 255)
FOAM = (236, 246, 255, 255)
EYE = (20, 20, 40, 255)
# The waist of the hero in its frame : the part above is shown, moved down by SEAT to sit on the mount
WAIST = 52
SEAT = -1
# The center of the mount (in 2 px pixels)
CX, CY = 18, 25


def ellipse(draw, cx, cy, rx, ry, fill, outline=None):
    draw.ellipse((cx - rx, cy - ry, cx + rx, cy + ry), fill=fill, outline=outline)


def mount(direction, bob, layer):
    # layer 0 : under the hero, layer 1 : over the hero (the head in front of it)
    image = Image.new('RGBA', (LOW, LOW), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    y = CY + bob
    horizontal = direction in (1, 3)
    rx, ry = (13, 6) if horizontal else (11, 7)
    if layer == 0:
        # Foam around the mount, moving with the bob
        ellipse(draw, CX, y + 4, rx + 3 - bob, 3, FOAM)
        ellipse(draw, CX, y + 4, rx + 1 - bob, 2, (0, 0, 0, 0))
        ellipse(draw, CX, y, rx, ry, BODY, OUTLINE)
        ellipse(draw, CX, y + 2, rx - 3, ry - 3, LIGHT)
        # The shell on its back, around the hero
        ellipse(draw, CX, y - 1, rx - 4, ry - 2, SHELL, SHELL_DARK)
        for kx in (-4, 0, 4):
            draw.point((CX + kx, y - 2), fill=SHELL_DARK)
        if direction == 0:
            # Its head is far, behind the hero : the neck rises above
            draw.rectangle((CX - 1, y - 15, CX + 1, y - 6), fill=BODY, outline=OUTLINE)
            ellipse(draw, CX, y - 16, 3, 2, BODY, OUTLINE)
        if horizontal:
            # Its tail
            side = 1 if direction == 1 else -1
            tx = CX - side * (rx + 1)
            draw.polygon([(tx, y), (tx - side * 3, y - 3), (tx - side * 3, y + 2)], fill=BODY, outline=OUTLINE)
    else:
        if direction == 2:
            # Its head, in front of the hero
            ellipse(draw, CX, y + 5, 4, 3, BODY, OUTLINE)
            draw.point((CX - 2, y + 4), fill=EYE)
            draw.point((CX + 2, y + 4), fill=EYE)
        if horizontal:
            side = 1 if direction == 1 else -1
            hx = CX + side * (rx + 1)
            draw.line((CX + side * (rx - 4), y - 1, hx, y - 8), fill=OUTLINE, width=3)
            draw.line((CX + side * (rx - 4), y - 1, hx, y - 8), fill=BODY, width=1)
            ellipse(draw, hx + side, y - 10, 3, 2, BODY, OUTLINE)
            draw.point((hx + side * 2, y - 11), fill=EYE)
    return image.resize((FRAME, FRAME), Image.NEAREST)


def hero_upper(hero, direction, bob):
    # The standing frame of the hero (column 1), cut at the waist, sitting on the mount
    frame = hero.crop((FRAME, direction * FRAME, 2 * FRAME, (direction + 1) * FRAME)).convert('RGBA')
    pixels = frame.load()
    for y in range(FRAME):
        for x in range(FRAME):
            if pixels[x, y][:3] == KEY[:3] or y >= WAIST:
                pixels[x, y] = (0, 0, 0, 0)
    upper = Image.new('RGBA', (FRAME, FRAME), (0, 0, 0, 0))
    upper.paste(frame, (0, SEAT + bob * PIXEL), frame)
    return upper


def main(hero_path, output):
    hero = Image.open(hero_path).convert('RGBA')
    sheet = Image.new('RGBA', (FRAME * 6, FRAME * 8), KEY)
    # Rows of the sheet : the direction drawn on each (the diagonals : the nearest direction)
    rows = [0, 1, 2, 3, 0, 1, 2, 3]
    for row, direction in enumerate(rows):
        for column, bob in enumerate((0, 1, 0)):
            frame = Image.new('RGBA', (FRAME, FRAME), (0, 0, 0, 0))
            for part in (mount(direction, bob, 0), hero_upper(hero, direction, bob), mount(direction, bob, 1)):
                frame.alpha_composite(part)
            cell = Image.new('RGBA', (FRAME, FRAME), KEY)
            cell.alpha_composite(frame)
            sheet.paste(cell, (column * FRAME, row * FRAME))
    sheet.convert('RGB').save(output)
    print(f'{output} : the hero surfing')


main(sys.argv[1], sys.argv[2])
