#!/usr/bin/env python3
"""Copies the upper foliage of the trees of a map on its top layer (<map>T.bmp).

A tree of the chipset is 3 rows : its tip, on the top layer, then its foliage and its trunk, on the ground layer (solid).
The ground layer is drawn under the characters : a Pokemon standing behind a tree, that moves by pixels in a battle,
had its feet drawn over the foliage. A copy of the foliage on the top layer (drawn over the characters) hides them ;
the ground layer keeps its tile, so the tree stays solid (the collisions only read the ground and middle layers).

Usage : tree_tops.py <level directory>...   (ex : Levels/foret)
"""
import os
import sys
from PIL import Image

CORRESPONDANCE = 'Resources/Chipsets/corr.png'
# The upper foliage of the tree (chipset blocks)
FOLIAGE = [(13, 8), (14, 8)]
EMPTY = (255, 255, 255)


def copy_foliage(level):
    name = os.path.basename(os.path.normpath(level))
    correspondance = Image.open(CORRESPONDANCE).convert('RGB')
    foliage = {correspondance.getpixel(block) for block in FOLIAGE}
    top_path = os.path.join(level, name + 'T.bmp')
    top = Image.open(top_path).convert('RGB')
    copied = 0
    for suffix in ('', 'M'):
        layer = Image.open(os.path.join(level, name + suffix + '.bmp')).convert('RGB')
        for y in range(layer.height):
            for x in range(layer.width):
                color = layer.getpixel((x, y))
                if color in foliage and top.getpixel((x, y)) == EMPTY:
                    top.putpixel((x, y), color)
                    copied += 1
    top.save(top_path)
    return copied


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    for level in sys.argv[1:]:
        print(level, copy_foliage(level), 'blocks of foliage copied on the top layer')
    return 0


if __name__ == '__main__':
    sys.exit(main())
