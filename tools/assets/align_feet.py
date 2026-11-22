#!/usr/bin/env python3
"""Puts the feet of a character at the same height in all the frames of its sheet (Sprites/Charsets : 6 x 8 frames).

The lowest pixel of each frame (the feet) is moved to BASELINE : the character does not jump up or down when it turns
or walks, its feet are on the bottom of its block (a 72 px frame is drawn 17 px above its block : row 64 is the bottom
row of the block), so they are not drawn on the block under it (ex : the trunk of a tree it stands behind), and its
shadow (ShadowSystem) is under its feet. A frame drawn up to its top is moved up as far as it can.

Usage : align_feet.py <sheet.png> [output.png]
"""
import sys
from PIL import Image

COLUMNS, ROWS = 6, 8
BASELINE = 64


def align(sheet):
    sheet = sheet.convert('RGBA')
    key = sheet.getpixel((0, 0))
    width, height = sheet.width // COLUMNS, sheet.height // ROWS
    result = Image.new('RGBA', sheet.size, key)
    for row in range(ROWS):
        for column in range(COLUMNS):
            box = (column * width, row * height, (column + 1) * width, (row + 1) * height)
            frame = sheet.crop(box)
            pixels = frame.load()
            drawn = [y for y in range(height) for x in range(width) if pixels[x, y][:3] != key[:3] and pixels[x, y][3] > 0]
            if not drawn:
                result.paste(frame, box[:2])
                continue
            shift = BASELINE - max(drawn)
            if min(drawn) + shift < 0:
                shift = -min(drawn)
            moved = Image.new('RGBA', (width, height), key)
            moved.paste(frame, (0, shift))
            result.paste(moved, box[:2])
    return result


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    output = sys.argv[2] if len(sys.argv) > 2 else sys.argv[1]
    align(Image.open(sys.argv[1])).save(output)
    return 0


if __name__ == '__main__':
    sys.exit(main())
