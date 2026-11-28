#!/usr/bin/env bash
# The painted tiles of the generator : the 3D scenes tools/render3d/scenes/tiles*.js rendered for each biome into
# Resources/Worldgen/painted/<biome> (the same geometry, the colours of the biome : the blocks between two biomes blend
# them ; the Okami brush, brush=1), then the atlas of the C++ generator again (tools/worldgen/atlas.py : tiles.png and painted.png).
# Needs npm install --prefix tools/render3d. After it :
#   TMPDIR=build/golden-tmp tools/worldgen/golden.py > tests/worldgen/golden.txt
set -euo pipefail
cd "$(dirname "$0")/../.."
for biome in meadow autumn snow desert; do
	out="Resources/Worldgen/painted/$biome"
	rm -rf "$out"
	for scene in tools/render3d/scenes/tiles*.js; do
		node tools/render3d/render.mjs "$(basename "$scene" .js)" --option "biome=$biome" --option brush=1 --out "$out"
	done
done
python3 tools/worldgen/atlas.py
