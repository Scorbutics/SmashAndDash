#!/usr/bin/env python3
"""Extracts the character sheets of the Pokemon (Sprites/Charsets/<id>.png) from the sheet of the Kanto overworlds
(pkmnKanto.png : "Kanto Pokemon overworlds, ripped by -Dragoon for TSR", Sprites/Charsets/CREDITS.txt).

The source : cells of 64 x 128 px separated by lines of 1 px, 15 a row, in the order of the Pokedex with two more (the
female Florizarre after the male one, the female Pikachu after the male one : skipped). A cell : 2 columns of 32 px
frames, 4 rows : the left column is the back (2 frames of the walk) then the front (2) ; the right column faces the left
(2) then the right (2). Each cell has its background colour ; a few have a dot of colour (the gender) : removed.

The game (Graphic/CharsetSheet.h) : frames of 72 px, 6 columns (the walk : 0 to 2), 8 rows (the directions : back, right,
front, left, then the diagonals : back, back, front, front), pixels doubled, the background (190, 151, 255) its colour key,
the feet of each frame on the row 64 (tools/assets/align_feet.py).

Usage : kanto_charsets.py <pkmnKanto.png> [ids...] [--out Sprites/Charsets] [--force]
  ids : the Pokedex numbers to write (default : all 151) ; an existing sheet is kept unless --force
"""
import argparse
import os
import sys
from collections import Counter

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from align_feet import align  # noqa: E402

CELL_W, CELL_H, PER_ROW, FRAME = 65, 129, 15, 32
SCALE, OUT_FRAME, COLUMNS, ROWS = 2, 72, 6, 8
KEY = (190, 151, 255, 255)
# The cells of the source that are not a Pokemon of their own (the female forms)
SKIPPED = {3, 26}
# A part of a frame this small or smaller, apart from the Pokemon, is a mark of the sheet (the gender dots)
SPECK = 6


def cell_origin(index):
    """The top left of a cell : the lines between the cells fall on 64, 129, 194, 258... (the cells are 64 or 63 wide)."""
    column, row = index % PER_ROW, index // PER_ROW
    lines = [64, 129, 194, 258, 323, 388, 453, 518, 583, 648, 713, 778, 843, 908]
    x = 0 if column == 0 else lines[column - 1] + 1
    return x, row * CELL_H


def dex_cells():
    """The cell of each Pokedex number."""
    cells, dex = {}, 1
    for index in range(PER_ROW * 10 + 3):
        if index in SKIPPED:
            continue
        cells[dex] = index
        dex += 1
    return cells


def frame(sheet, x, y, background):
    """A frame of 32 px : its pixels, the background transparent, the specks removed."""
    image = sheet.crop((x, y, x + FRAME, y + FRAME)).convert('RGBA')
    pixels = image.load()
    # (The last row and the last column of cells are short : out of the sheet is the background, not the black of the crop)
    inside, across = min(FRAME, sheet.height - y), min(FRAME, sheet.width - x)
    drawn = {(i, j) for j in range(inside) for i in range(across) if pixels[i, j][:3] != background[:3] and pixels[i, j][:3] != (0, 64, 128)}
    # The parts of the drawing (4-connected) : the small ones are marks of the sheet
    seen, kept = set(), set()
    for start in drawn:
        if start in seen:
            continue
        part, stack = [], [start]
        seen.add(start)
        while stack:
            i, j = stack.pop()
            part.append((i, j))
            for n in ((i + 1, j), (i - 1, j), (i, j + 1), (i, j - 1)):
                if n in drawn and n not in seen:
                    seen.add(n)
                    stack.append(n)
        if len(part) > SPECK:
            kept.update(part)
    result = Image.new('RGBA', (FRAME, FRAME), KEY)
    out = result.load()
    for i, j in kept:
        out[i, j] = pixels[i, j][:3] + (255,)
    return result


def background_of(sheet, x, y):
    """The colour of a cell : the commonest one."""
    cell = sheet.crop((x, y, x + 63, y + 128)).convert('RGB')
    return Counter(cell.getpixel((i, j)) for j in range(cell.height) for i in range(cell.width)).most_common(1)[0][0]


def charset(sheet, index):
    x0, y0 = cell_origin(index)
    background = background_of(sheet, x0, y0)
    left, right = x0, x0 + FRAME
    back = [frame(sheet, left, y0, background), frame(sheet, left, y0 + FRAME, background)]
    front = [frame(sheet, left, y0 + 2 * FRAME, background), frame(sheet, left, y0 + 3 * FRAME, background)]
    facing_left = [frame(sheet, right, y0, background), frame(sheet, right, y0 + FRAME, background)]
    facing_right = [frame(sheet, right, y0 + 2 * FRAME, background), frame(sheet, right, y0 + 3 * FRAME, background)]
    rows = [back, facing_right, front, facing_left, back, back, front, front]
    result = Image.new('RGBA', (OUT_FRAME * COLUMNS, OUT_FRAME * ROWS), KEY)
    for row, frames in enumerate(rows):
        # The walk : a step, the other, the first again
        for column, source in enumerate((frames[0], frames[1], frames[0])):
            big = source.resize((FRAME * SCALE, FRAME * SCALE), Image.NEAREST)
            offset = (OUT_FRAME - FRAME * SCALE) // 2
            result.paste(big, (column * OUT_FRAME + offset, row * OUT_FRAME + offset), big)
    return align(result)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('source')
    parser.add_argument('ids', nargs='*', type=int)
    parser.add_argument('--out', default='Sprites/Charsets')
    parser.add_argument('--force', action='store_true')
    args = parser.parse_args()
    sheet = Image.open(args.source).convert('RGB')
    cells = dex_cells()
    written = []
    for dex in args.ids or sorted(cells):
        path = os.path.join(args.out, f'{dex}.png')
        if os.path.exists(path) and not args.force:
            continue
        charset(sheet, cells[dex]).save(path)
        written.append(dex)
    print(f'{len(written)} sheets written in {args.out}' + (f' : {written}' if len(written) < 30 else ''))


if __name__ == '__main__':
    main()
