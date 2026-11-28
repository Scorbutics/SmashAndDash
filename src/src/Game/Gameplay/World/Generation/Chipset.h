#pragma once
#include <array>
#include <cstdint>
#include <istream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "Layers.h"
#include "Png.h"

// The chipsets of a generated world (the C++ port of tools/worldgen : Tiles.frames, blend_mask, Chipset) : the tiles a map
// uses, each in the mix of the biomes of its corners, blended across it from the tiles drawn in each biome (the atlas,
// Resources/Worldgen/tiles.png, made by tools/worldgen/atlas.py), in the cells of its chipset.
namespace worldgen {
	constexpr int TileSize = 48;  // pixels
	constexpr int Voxels = 16;    // voxels a tile side (3 x 3 pixels each)
	constexpr int ChipColumns = 30;
	constexpr int ChipRows = 64;  // Resources/Chipsets/corr.png names 30 x 64 cells
	constexpr int MaskCells = 4;  // the collision mask of a tile : 4 x 4 cells (tools/worldgen tile_mask)

	// The biomes of the 4 corners of a block (NW NE SW SE) : MixSteps shares of each biome
	using Corner = std::array<std::uint8_t, BiomeCount>;
	using Mix = std::array<Corner, 4>;
	Mix PureMix(Biome biome);

	// A tile : at one pixel per voxel (Voxels a side : the drawings of tools/tilegen), or at one pixel per pixel (TileSize
	// a side : a painted tile, tools/render3d)
	struct TileImage {
		int side = Voxels;
		std::vector<std::uint8_t> rgb;
		bool operator<(const TileImage& other) const { return side != other.side ? side < other.side : rgb < other.rgb; }
		bool operator==(const TileImage& other) const { return side == other.side && rgb == other.rgb; }
		// At one pixel per pixel (a voxel : 3 x 3 pixels)
		TileImage full() const;
	};

	// The tiles drawn in each biome : a row per tile, its frames in each biome
	class Atlas {
	public:
		Atlas(RgbImage image, std::istream& index);
		// With its painted tiles (tools/worldgen/atlas.py : painted.png, cells of 48 px ; painted.txt "<biome> <cell> <key>")
		Atlas(RgbImage image, std::istream& index, RgbImage painted, std::istream& paintedIndex);
		// The frames (1, or 4 for an animated tile) of a tile (its key, the name of an animated tile) in a biome
		std::vector<TileImage> frames(const std::string& key, Biome biome) const;
		bool has(const std::string& key) const { return m_rows.count(key) > 0; }
		// The depth of a tile (tools/worldgen tile_base : -1 flat, n a part of an object standing n blocks lower)
		int base(const std::string& key) const;
		// Its properties for the game (tools/worldgen tile_flags : 2 a hideout of the wild Pokemon)
		int flags(const std::string& key) const;
		// Its collision mask when it blocks (tools/worldgen tile_mask : 4 x 4 cells, a bit by solid cell)
		int mask(const std::string& key) const;
	private:
		RgbImage m_image;
		std::map<std::string, std::pair<int, int>> m_rows; // key -> row, frames
		std::map<std::string, int> m_bases;
		std::map<std::string, int> m_flags;
		std::map<std::string, int> m_masks;
		RgbImage m_painted;
		std::map<std::pair<int, std::string>, int> m_paintedRows; // (biome, key) -> the cell of its first frame
	};

	// The frames of the tiles of a world in the mixes of the biomes (cached)
	class TileImages {
	public:
		TileImages(const Atlas& atlas, const Tiles& tiles) : m_atlas(atlas), m_tiles(tiles) {}
		const std::vector<TileImage>& frames(TileId tile, const Mix& mix);
		bool walkable(TileId tile) const { return IsAnimated(tile) ? AnimatedWalkable(AnimatedOf(tile)) : m_tiles.walkable(tile); }
		std::string key(TileId tile) const { return IsAnimated(tile) ? AnimatedName(AnimatedOf(tile)) : m_tiles.key(tile); }
		int base(TileId tile) const { return m_atlas.base(key(tile)); }
		int flags(TileId tile) const { return m_atlas.flags(key(tile)); }
		// Walked on : no solid part
		int mask(TileId tile) const { return walkable(tile) ? 0 : m_atlas.mask(key(tile)); }
	private:
		const std::vector<TileImage>& pure(TileId tile, Biome biome);

		const Atlas& m_atlas;
		const Tiles& m_tiles;
		std::map<std::pair<TileId, Biome>, std::vector<TileImage>> m_pure;
		std::map<std::pair<TileId, Mix>, std::vector<TileImage>> m_cache;
	};

	using TileUse = std::pair<TileId, Mix>;

	// The chipset of a map : the tiles it uses in their mixes (the same images share a cell), its collisions (.col : white
	// walked on, 0xAA walked on and animated, 0x46 blocked and animated, black blocked), its properties (.prop : red, the
	// properties of the tile for the game ; green, the base of the tile + 1, 0 flat : ska TileProperties), its water cells
	struct Chipset {
		Chipset(TileImages& images, const std::set<TileUse>& used);

		RgbImage image;
		RgbImage col;
		// The collision masks of its cells (tools/worldgen tile_mask) : 4 x 4 pixels a cell, white a solid part
		RgbImage mask;
		RgbImage prop;
		std::map<TileUse, Block> cells;
		std::vector<Block> water;
		int rows = 0;
		int used = 0;
	};

	// The tiles of a chunk of a world, in the mixes of their blocks ; the tiles of all its rooms (in the meadow)
	std::set<TileUse> ChunkTiles(const World& world, int cx, int cy);
	std::set<TileUse> RoomTiles(const World& world);
	Mix BlockMix(const WorldPlan& plan, int x, int y);
}
