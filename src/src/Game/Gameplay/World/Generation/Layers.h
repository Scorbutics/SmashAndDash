#pragma once
#include <array>
#include <cstdint>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "Terrain.h"

// The tiles of a generated world on its blocks (the C++ port of tools/worldgen : World.build and what it calls, rooms.py
// build_room, World.chunk_events, World.corner_light) : the three layers of each block (the ground, the middle, the top
// over the characters), the blocks walked on, the events of the maps, the light of the corners, the rooms. A tile is a
// key (what it shows, the text of the key of the Python generator) : the images are drawn apart (the chipsets).
namespace worldgen {
	// A tile on a block : NoTile, a tile of the world (>= 0 : its id in Tiles), an animated tile (AnimatedTile)
	using TileId = int;
	constexpr TileId NoTile = -1;

	// The animated tiles (4 frames), in the order of the Python generator (ANIMATED)
	enum class Animated : int {
		Water, Flower, Tall, Campfire, LampHead, Butterfly, Fountain00, Fountain01, Fountain10, Fountain11,
		FlowerbedRoofr, FlowerbedFlower, FlowerbedRoofb, FlowerbedOrange
	};
	constexpr int AnimatedCount = 14;
	constexpr TileId AnimatedTile(Animated animated) { return -2 - static_cast<int>(animated); }
	constexpr bool IsAnimated(TileId tile) { return tile <= -2; }
	constexpr Animated AnimatedOf(TileId tile) { return static_cast<Animated>(-2 - tile); }
	// "water", "fountain_0_1", "flowerbed_roofr"...
	const char* AnimatedName(Animated animated);
	bool AnimatedWalkable(Animated animated);
	// A tile as the reference values name it : its id, the name of an animated tile, None
	std::string TileName(TileId tile);

	// The tiles of a world : an id per key and walkability, in the order they are first used
	class Tiles {
	public:
		// Stable : the tiles of a region of the infinite world, their order in a chipset by their keys (infinite.py
		// StableTiles), not by their ids (the order they were first used in : another window, other ids)
		explicit Tiles(bool stable = false) : m_stable(stable) {}
		TileId cell(const std::string& key, bool walkable);
		// The text a chipset sorts the tile by (the str of its Python id)
		std::string sortName(TileId tile) const;
		std::size_t size() const { return m_keys.size(); }
		const std::string& key(TileId tile) const { return m_keys[static_cast<std::size_t>(tile)].first; }
		bool walkable(TileId tile) const { return m_keys[static_cast<std::size_t>(tile)].second; }
	private:
		std::map<std::pair<std::string, bool>, TileId> m_ids;
		std::vector<std::pair<std::string, bool>> m_keys;
		bool m_stable = false;
	};

	// Which tiles of a drawn structure are not empty
	struct Shape {
		int w = 0;
		int h = 0;
		std::vector<bool> filled;
		bool at(int c, int r) const { return filled[static_cast<std::size_t>(r * w + c)]; }
	};

	// The shapes of the structures (Resources/Worldgen/shapes.txt, made by tools/worldgen/shapes.py)
	struct Shapes {
		std::map<std::string, Shape> stamps;
		std::map<std::string, Shape> furniture; // "<char> <columns> <rows> <extra or ->"
		static Shapes Parse(std::istream& in);
	};

	// A room template (Resources/Worldgen/rooms/<name>.txt : its README)
	struct RoomTemplate {
		std::string name;
		std::vector<std::string> rows;
		static RoomTemplate Parse(const std::string& name, std::istream& in);
	};
	// The templates of a folder, sorted by name
	std::vector<RoomTemplate> LoadRoomTemplates(const std::string& directory);

	struct Room {
		std::string name;     // w_room_<n>
		std::string templateName;
		std::vector<std::string> rows;
		int width = 0;
		int height = 0;
		std::vector<TileId> base, middle;
		std::vector<bool> walk;
		std::vector<std::string> events; // UTF-8
	};

	struct Item {
		int x, y, item;
	};

	enum class Layer : std::size_t { Base = 0, Middle = 1, Top = 2 };

	// A region of the infinite world built as a world (infinite.py RegionWorld) : the blocks built (a window around the
	// region, inside the blocks of its terrain), its chunks (written, their items placed), the region (the rooms of the
	// buildings whose door is in it, named by their door)
	struct RegionOptions {
		int x0 = 0, y0 = 0, width = 0, height = 0;
		std::vector<std::pair<int, int>> chunks;
		int regionX0 = 0, regionY0 = 0, regionSize = 0;
		// The sea of the blocks built, by rows : SeaNone, SeaReef (a rock out of the water), SeaDeep (darker waves)
		std::vector<std::uint8_t> sea;
		static constexpr std::uint8_t SeaNone = 0, SeaReef = 1, SeaDeep = 2;
	};

	class World {
	public:
		World(const Terrain& terrain, const Shapes& shapes, const std::vector<RoomTemplate>& rooms, const RegionOptions* region = nullptr);

		const Terrain& terrain() const { return m_terrain; }
		// The blocks built : the world, or the window of a region
		int x0() const { return m_x0; }
		int y0() const { return m_y0; }
		int width() const { return m_width; }
		int height() const { return m_height; }
		// The chunks written
		const std::vector<std::pair<int, int>>& chunks() const { return m_chunks; }
		bool region() const { return m_region; }
		TileId tile(Layer layer, int x, int y) const { return inMap(x, y) ? m_layers[static_cast<std::size_t>(layer)][index(x, y)] : NoTile; }
		bool walk(int x, int y) const { return inMap(x, y) && m_walk[index(x, y)] != 0; }
		const Tiles& tiles() const { return m_tiles; }
		const Blocks& taken() const { return m_taken; }
		// The height of what stands on a block (the light)
		const std::map<Block, double>& objects() const { return m_objects; }
		const std::vector<Item>& items() const { return m_items; }
		const std::vector<Room>& rooms() const { return m_rooms; }
		// The events of a chunk (the lines of its <map>E.txt, UTF-8) : the edges to the neighbours, the doors, the rest
		std::vector<std::string> chunkEvents(int cx, int cy) const;
		// The light of a corner of the blocks (i, j : 0 .. width, 0 .. height) : multiplied with the map
		std::array<std::uint8_t, 3> cornerLight(int i, int j) const;

	private:
		std::size_t index(int x, int y) const { return static_cast<std::size_t>(y - m_y0) * static_cast<std::size_t>(m_width) + static_cast<std::size_t>(x - m_x0); }
		bool inMap(int x, int y) const { return m_x0 <= x && x < m_x0 + m_width && m_y0 <= y && y < m_y0 + m_height; }
		// Off the blocks built : lost (a village or a tree cut by the edge of the window)
		void setWalk(int x, int y, bool walk) {
			if (inMap(x, y)) {
				m_walk[index(x, y)] = walk ? 1 : 0;
			}
		}
		Ground mat(int x, int y) const { return m_terrain.ground(x, y); }
		int level(int x, int y) const { return m_terrain.level(x, y); }
		void set(Layer layer, int x, int y, TileId tile);
		TileId& at(Layer layer, int x, int y) { return m_layers[static_cast<std::size_t>(layer)][index(x, y)]; }
		bool taken(int x, int y) const { return m_taken.count({ x, y }) > 0; }
		void take(int x, int y) { m_taken.insert({ x, y }); }
		bool plain(int x, int y) const;
		double weight(int x, int y, Biome biome) const { return m_terrain.plan().weight(x, y, biome); }

		void build();
		void placeSea();
		void stamp(const std::string& name, int x0, int y0, std::optional<std::pair<int, int>> door);
		void placeTown();
		bool decoOk(int x, int y) const;
		void decorate(const Village& v);
		void putObject(Layer layer, int x, int y, TileId tile, double height);
		std::string treeStyle(int x, int y) const;
		bool freeCell(int x, int y) const;
		bool treeAt(int x, int y, const std::string& style);
		void placeNature();
		void itemEvent(int x, int y, int item);
		void addEvent(int x, int y, const std::string& line);
		void placePois();
		void placeSigns();
		std::vector<char> reachable() const;
		void placeItems();
		void placeRooms(const std::vector<RoomTemplate>& templates);
		double top(int x, int y) const;

		const Terrain& m_terrain;
		const Shapes& m_shapes;
		std::int64_t m_seed;
		int m_x0 = 0, m_y0 = 0, m_width = 0, m_height = 0;
		std::vector<std::pair<int, int>> m_chunks;
		bool m_region = false;
		int m_regionX0 = 0, m_regionY0 = 0, m_regionSize = 0;
		std::vector<std::uint8_t> m_sea;
		std::array<std::vector<TileId>, 3> m_layers;
		std::vector<char> m_walk;
		Blocks m_taken;
		std::map<Block, double> m_objects;
		Tiles m_tiles;
		std::vector<Building> m_loneBuildings;
		std::map<Block, std::vector<std::string>> m_events;
		std::map<Block, std::vector<std::string>> m_doorEvents;
		std::vector<Item> m_items;
		std::vector<Room> m_rooms;
	};
}
