#!/usr/bin/env python3
# Makes the chipsets crisp again. Most of their tiles are 16 px pixel art enlarged x3 with a smoothing filter (blurry
# edges). For each 48 px tile : one pixel per 3x3 cell is kept (the transparent color, 190 151 255, stays exact), and
# when a smooth enlargement of that 16 px tile gives the original tile back (it was made that way), the tile is
# replaced by its x3 enlargement without smoothing. The other tiles (drawn at 48 px) are not changed.
# Usage : tools/assets/crisp_chipsets.py <chipset.png>... (needs Pillow ; the files are replaced)
import sys
from PIL import Image

KEY = (190, 151, 255)
TILE = 48
# Mean difference (sum of the 3 channels) under which a tile is an enlarged 16 px tile
THRESHOLD = 8


def dist(a, b):
    return sum(abs(a[i] - b[i]) for i in range(3))


def small_pixel(pixels, x, y, width, height):
    cell = [pixels[min(width - 1, 3 * x + i), min(height - 1, 3 * y + j)] for j in range(3) for i in range(3)]
    center = cell[4]
    keys = sum(1 for p in cell if dist(p, KEY) < 30)
    if keys >= 5 or dist(center, KEY) < 30:
        return KEY
    if keys > 0 and dist(center, KEY) < 200:
        # A blend with the transparent color : the most opaque pixel of the cell
        return max(cell, key=lambda p: dist(p, KEY))
    return center


def crisp(path):
    image = Image.open(path).convert('RGB')
    pixels = image.load()
    width, height = image.size
    small = Image.new('RGB', (width // 3, height // 3))
    small_pixels = small.load()
    for y in range(height // 3):
        for x in range(width // 3):
            small_pixels[x, y] = small_pixel(pixels, x, y, width, height)
    sharp = small.resize((width, height), Image.NEAREST)
    smooth = small.resize((width, height), Image.BICUBIC)
    smooth_pixels = smooth.load()
    result = image.copy()
    replaced = 0
    for ty in range(height // TILE):
        for tx in range(width // TILE):
            box = (tx * TILE, ty * TILE, tx * TILE + TILE, ty * TILE + TILE)
            difference = sum(dist(pixels[x, y], smooth_pixels[x, y]) for y in range(box[1], box[3], 2) for x in range(box[0], box[2], 2))
            if difference / (TILE * TILE / 4) < THRESHOLD:
                result.paste(sharp.crop(box), box)
                replaced += 1
    result.save(path)
    print(f'{path} : {replaced} / {(width // TILE) * (height // TILE)} tiles made crisp')


for argument in sys.argv[1:]:
    crisp(argument)
