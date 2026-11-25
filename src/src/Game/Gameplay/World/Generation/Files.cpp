#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "Files.h"

namespace {
	using namespace worldgen;
	namespace fs = std::filesystem;

	constexpr std::array<std::uint8_t, 3> Empty{ 255, 255, 255 };
	const char* EventsHeader = "x y declenchement(1:presser espace||2:sous le hero||0:automatique au chargement de la map) parametresSupplementaires";

	// The wild Pokemon of a biome (SPAWNS) : id, level min, level max, path type (1 wanders, 2 stays), chance %
	struct Spawn { int id, levelMin, levelMax, path, chance; };
	const std::vector<Spawn>& SpawnsOf(Biome biome) {
		static const std::array<std::vector<Spawn>, BiomeCount> spawns{ {
			{ { 16, 2, 5, 2, 100 }, { 19, 2, 4, 1, 75 }, { 25, 2, 4, 1, 35 }, { 1, 2, 4, 1, 15 } },
			{ { 10, 3, 6, 1, 80 }, { 16, 3, 6, 2, 100 }, { 1, 3, 5, 1, 30 }, { 19, 3, 5, 1, 40 } },
			{ { 7, 4, 7, 1, 60 }, { 19, 4, 6, 1, 50 }, { 25, 4, 7, 1, 25 } },
			{ { 4, 4, 7, 1, 60 }, { 19, 4, 6, 1, 60 }, { 16, 4, 6, 2, 50 } },
		} };
		return spawns[static_cast<std::size_t>(biome)];
	}

	void WriteText(const fs::path& path, const std::string& text) {
		auto file = std::ofstream{ path, std::ios::binary | std::ios::trunc };
		if (!file) {
			throw std::runtime_error{ "worldgen : cannot write " + path.string() };
		}
		file << text;
	}

	std::string MapIniText(const std::string& chipset) {
		return "[Chipset]\nfile=" + chipset + "\n\n[Bgm]\nfile=./Music/Greencity.mp3\nvolume=1\n";
	}

	std::string EventsText(const std::vector<std::string>& lines) {
		auto text = std::string{ EventsHeader };
		for (const auto& line : lines) {
			text += "\n" + line;
		}
		return text + "\n";
	}

	// The chipset files : its image, its collisions, its properties (not read by the game yet), its touch scripts (the
	// water surfs)
	void SaveChipset(const Chipset& chip, const fs::path& chips, const std::string& name, const std::string& tileScripts) {
		const auto base = chips / name;
		WriteFile((chips / (name + ".png")).string(), EncodePng(chip.image));
		WriteFile((chips / (name + ".png.col")).string(), EncodePng(chip.col));
		WriteFile((chips / (name + ".png.prop")).string(), EncodePng(RgbImage{ chip.col.width, chip.col.height }));
		fs::create_directories(base / "Scripts");
		auto list = std::string{};
		for (const auto& [x, y] : chip.water) {
			list += std::to_string(x) + "_" + std::to_string(y) + "\n";
		}
		WriteText(base / "scripts.txt", list);
		for (const auto& [x, y] : chip.water) {
			fs::copy_file(fs::path{ tileScripts } / "water.miniska", base / "Scripts" / (std::to_string(x) + "_" + std::to_string(y) + "_4.miniska"),
				fs::copy_options::overwrite_existing);
		}
	}

	void PutColour(RgbImage& image, int x, int y, const std::uint8_t* colour) {
		std::copy(colour, colour + 3, image.at(x, y));
	}
}

worldgen::RgbImage worldgen::RoomLight(const Room& room) {
	const auto w = room.width;
	const auto h = room.height;
	auto out = RgbImage{ w + 1, h + 1 };
	for (auto j = 0; j <= h; j++) {
		for (auto i = 0; i <= w; i++) {
			auto blocked = 0;
			for (const auto dx : { -1, 0 }) {
				for (const auto dy : { -1, 0 }) {
					const auto x = i + dx;
					const auto y = j + dy;
					blocked += !(0 <= x && x < w && 0 <= y && y < h) || !room.walk[static_cast<std::size_t>(y * w + x)] ? 1 : 0;
				}
			}
			const auto nearWall = std::max(0, 4 - j) / 4.0; // the back wall shades the floor in front of it
			auto l = 0.99 - 0.035 * blocked - 0.05 * nearWall;
			l = std::max(0.85, std::min(1.0, l));
			auto* p = out.at(i, j);
			p[0] = static_cast<std::uint8_t>(static_cast<int>(255 * l * 0.98));
			p[1] = static_cast<std::uint8_t>(static_cast<int>(255 * l * 0.99));
			p[2] = static_cast<std::uint8_t>(static_cast<int>(255 * l));
		}
	}
	return out;
}

void worldgen::WriteWorld(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
	const std::string& folder, const std::string& worldPath) {
	const auto root = fs::path{ folder };
	if (fs::exists(root)) { // an earlier run
		fs::remove_all(root);
	}
	const auto chips = root / "Chipsets";
	fs::create_directories(chips);
	const auto chipPath = worldPath + "Chipsets";
	const auto& plan = world.terrain().plan();
	const auto colour = [&corr](const Block& cell) { return corr.at(cell.first, cell.second); };
	for (auto cy = 0; cy < world.height() / Chunk; cy++) {
		for (auto cx = 0; cx < world.width() / Chunk; cx++) {
			const auto name = "w_" + std::to_string(cx) + "_" + std::to_string(cy);
			const auto map = root / name;
			fs::create_directories(map / "Monsters");
			const auto chip = Chipset{ images, ChunkTiles(world, cx, cy) };
			SaveChipset(chip, chips, "chipset_" + name, tileScripts);
			const std::pair<Layer, const char*> layers[] = { { Layer::Base, "" }, { Layer::Middle, "M" }, { Layer::Top, "T" } };
			for (const auto& [layer, suffix] : layers) {
				auto image = RgbImage{ Chunk, Chunk, Empty[0], Empty[1], Empty[2] };
				for (auto y = 0; y < Chunk; y++) {
					for (auto x = 0; x < Chunk; x++) {
						const auto wx = cx * Chunk + x;
						const auto wy = cy * Chunk + y;
						const auto tile = world.tile(layer, wx, wy);
						if (tile != NoTile) {
							PutColour(image, x, y, colour(chip.cells.at({ tile, BlockMix(plan, wx, wy) })));
						}
					}
				}
				WriteFile((map / (name + suffix + ".bmp")).string(), EncodeBmp(image));
			}
			auto light = RgbImage{ Chunk + 1, Chunk + 1 };
			for (auto j = 0; j <= Chunk; j++) {
				for (auto i = 0; i <= Chunk; i++) {
					const auto c = world.cornerLight(cx * Chunk + i, cy * Chunk + j);
					PutColour(light, i, j, c.data());
				}
			}
			WriteFile((map / (name + "L.png")).string(), EncodePng(light));
			WriteText(map / (name + ".ini"), MapIniText(chipPath + "/chipset_" + name + ".png"));
			WriteText(map / (name + "E.txt"), EventsText(world.chunkEvents(cx, cy)));
			const auto& spawns = SpawnsOf(plan.climate().chunkBiome(cx, cy));
			for (auto i = 0u; i < spawns.size(); i++) {
				const auto& s = spawns[i];
				WriteText(map / "Monsters" / (std::to_string(i) + ".ini"), "[Data]\nid=" + std::to_string(s.id) + "\nlevel_min=" + std::to_string(s.levelMin)
					+ "\nlevel_max=" + std::to_string(s.levelMax) + "\npath_type=" + std::to_string(s.path) + "\n\n[Spawn]\nprobability_percent="
					+ std::to_string(s.chance) + "\n");
			}
			WriteText(map / "scripts.data", "");
		}
	}
	// The rooms : one chipset for all (chipset_rooms)
	const auto roomChip = Chipset{ images, RoomTiles(world) };
	SaveChipset(roomChip, chips, "chipset_rooms", tileScripts);
	const auto meadow = PureMix(Biome::Meadow);
	for (const auto& room : world.rooms()) {
		const auto map = root / room.name;
		fs::create_directories(map);
		const std::pair<const std::vector<TileId>*, const char*> layers[] = { { &room.base, "" }, { &room.middle, "M" }, { nullptr, "T" } };
		for (const auto& [grid, suffix] : layers) {
			auto image = RgbImage{ room.width, room.height, Empty[0], Empty[1], Empty[2] };
			if (grid != nullptr) {
				for (auto y = 0; y < room.height; y++) {
					for (auto x = 0; x < room.width; x++) {
						const auto tile = (*grid)[static_cast<std::size_t>(y * room.width + x)];
						if (tile != NoTile) {
							PutColour(image, x, y, colour(roomChip.cells.at({ tile, meadow })));
						}
					}
				}
			}
			WriteFile((map / (room.name + suffix + ".bmp")).string(), EncodeBmp(image));
		}
		WriteFile((map / (room.name + "L.png")).string(), EncodePng(RoomLight(room)));
		WriteText(map / (room.name + ".ini"), MapIniText(chipPath + "/chipset_rooms.png"));
		WriteText(map / (room.name + "E.txt"), EventsText(room.events));
		WriteText(map / "scripts.data", "");
	}
}

worldgen::Start worldgen::StartOf(const WorldPlan& plan) {
	const auto [tx, ty] = plan.town();
	const auto cx = tx / Chunk;
	const auto cy = ty / Chunk;
	return { "w_" + std::to_string(cx) + "_" + std::to_string(cy), tx - cx * Chunk, ty - cy * Chunk + 1 };
}

worldgen::Start worldgen::GenerateWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources) {
	const auto data = resources.root + "/Worldgen";
	auto shapesFile = std::ifstream{ data + "/shapes.txt" };
	auto index = std::ifstream{ data + "/tiles.txt" };
	if (!shapesFile || !index) {
		throw std::runtime_error{ "worldgen : no " + data + "/shapes.txt or tiles.txt" };
	}
	const auto shapes = Shapes::Parse(shapesFile);
	const auto templates = LoadRoomTemplates(data + "/rooms");
	const auto atlas = Atlas{ ReadPng(data + "/tiles.png"), index };
	const auto plan = WorldPlan{ seed, 8, 8 };
	const auto terrain = Terrain{ plan };
	const auto world = World{ terrain, shapes, templates };
	auto images = TileImages{ atlas, world.tiles() };
	WriteWorld(world, images, ReadPng(resources.root + "/Chipsets/corr.png"), resources.root + "/Chipsets/tile_scripts", folder, worldPath);
	return StartOf(plan);
}
