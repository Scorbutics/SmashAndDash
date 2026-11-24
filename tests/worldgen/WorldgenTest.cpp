// The C++ port of the world generator (src/src/Game/Gameplay/World/Generation) against the values of the Python
// generator (tests/worldgen/golden.txt, made by tools/worldgen/golden.py) : the same numbers, bit for bit.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "doctest.h"
#include "Game/Gameplay/World/Generation/Noise.h"
#include "Game/Gameplay/World/Generation/Climate.h"
#include "Game/Gameplay/World/Generation/Villages.h"
#include "Game/Gameplay/World/Generation/Terrain.h"

namespace {
	std::uint64_t Fnv(const std::vector<unsigned char>& data) {
		auto h = 0xcbf29ce484222325ULL;
		for (const auto b : data) {
			h ^= b;
			h *= 0x100000001b3ULL;
		}
		return h;
	}

	void Append(std::vector<unsigned char>& data, double value) {
		unsigned char bytes[sizeof(double)];
		std::memcpy(bytes, &value, sizeof(double));
		data.insert(data.end(), bytes, bytes + sizeof(double));
	}

	// The lines of the reference file
	std::vector<std::vector<std::string>> Golden() {
		auto file = std::ifstream{ "tests/worldgen/golden.txt" };
		REQUIRE(file.good());
		auto lines = std::vector<std::vector<std::string>>{};
		auto line = std::string{};
		while (std::getline(file, line)) {
			auto words = std::vector<std::string>{};
			auto stream = std::istringstream{ line };
			auto word = std::string{};
			while (stream >> word) {
				words.push_back(word);
			}
			if (!words.empty()) {
				lines.push_back(std::move(words));
			}
		}
		return lines;
	}

	std::map<std::int64_t, worldgen::Climate>& Climates() {
		static auto climates = std::map<std::int64_t, worldgen::Climate>{};
		return climates;
	}

	const worldgen::Climate& ClimateOf(std::int64_t seed) {
		auto found = Climates().find(seed);
		if (found == Climates().end()) {
			found = Climates().emplace(seed, worldgen::Climate{ seed, 8, 8 }).first;
		}
		return found->second;
	}
}

TEST_CASE("the noise is the noise of the Python generator") {
	for (const auto& words : Golden()) {
		if (words[0] == "h01") {
			CHECK(worldgen::H01(std::stoll(words[1]), std::stoll(words[2]), std::stoll(words[3])) == std::stod(words[4]));
		} else if (words[0] == "fbm") {
			CHECK(worldgen::Fbm(std::stod(words[1]), std::stod(words[2]), std::stoll(words[3]), std::stoi(words[4])) == std::stod(words[5]));
		}
	}
}

TEST_CASE("the climate is the climate of the Python generator") {
	for (const auto& words : Golden()) {
		if (words[0] != "labels" && words[0] != "relief" && words[0] != "chunks" && words[0] != "height") {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		const auto& climate = ClimateOf(seed);
		auto data = std::vector<unsigned char>{};
		if (words[0] == "chunks") {
			auto chunks = std::string{};
			for (auto cy = 0; cy < 8; cy++) {
				for (auto cx = 0; cx < 8; cx++) {
					chunks += std::to_string(static_cast<int>(climate.chunkBiome(cx, cy)));
				}
			}
			CHECK_MESSAGE(chunks == words[2], "chunks of seed " << seed);
			continue;
		}
		for (auto y = 0; y < climate.height(); y++) {
			for (auto x = 0; x < climate.width(); x++) {
				if (words[0] == "labels") {
					data.push_back(static_cast<unsigned char>(climate.label(x, y)));
				} else if (words[0] == "relief") {
					Append(data, climate.relief(x, y));
				} else {
					Append(data, climate.rawHeight(x, y));
				}
			}
		}
		CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], words[0] << " of seed " << seed);
	}
}

namespace {
	std::map<std::int64_t, worldgen::WorldPlan>& Plans() {
		static auto plans = std::map<std::int64_t, worldgen::WorldPlan>{};
		return plans;
	}

	const worldgen::WorldPlan& PlanOf(std::int64_t seed) {
		auto found = Plans().find(seed);
		if (found == Plans().end()) {
			found = Plans().emplace(seed, worldgen::WorldPlan{ seed, 8, 8 }).first;
		}
		return found->second;
	}

	void AppendInt(std::vector<unsigned char>& data, std::int32_t value) {
		unsigned char bytes[sizeof(std::int32_t)];
		std::memcpy(bytes, &value, sizeof(std::int32_t));
		data.insert(data.end(), bytes, bytes + sizeof(std::int32_t));
	}

	std::string Cells(const worldgen::Blocks& blocks) {
		auto data = std::vector<unsigned char>{};
		for (const auto& [x, y] : blocks) {
			AppendInt(data, x);
			AppendInt(data, y);
		}
		return std::to_string(Fnv(data));
	}

	std::string Hex(const std::string& text) {
		static const char* digits = "0123456789abcdef";
		auto out = std::string{};
		for (const auto c : text) {
			const auto b = static_cast<unsigned char>(c);
			out += digits[b >> 4];
			out += digits[b & 15];
		}
		return out;
	}
}

TEST_CASE("the villages and the biome weights are those of the Python generator") {
	for (const auto& words : Golden()) {
		const auto& kind = words[0];
		if (kind != "town" && kind != "village" && kind != "near" && kind != "weights" && kind != "mixes") {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		const auto& plan = PlanOf(seed);
		if (kind == "town") {
			CHECK(plan.town().first == std::stoi(words[2]));
			CHECK(plan.town().second == std::stoi(words[3]));
		} else if (kind == "village") {
			const auto i = std::stoul(words[2]);
			REQUIRE(i < plan.villages().size());
			const auto& v = plan.villages()[i];
			INFO("village " << i << " of seed " << seed);
			CHECK(v.center.first == std::stoi(words[3]));
			CHECK(v.center.second == std::stoi(words[4]));
			CHECK(static_cast<int>(v.biome) == std::stoi(words[5]));
			CHECK(Hex(v.name) == words[6]);
			CHECK(v.buildings.size() == std::stoul(words[7]));
			const auto repr = v.buildingsRepr();
			CHECK(std::to_string(Fnv(std::vector<unsigned char>(repr.begin(), repr.end()))) == words[8]);
			CHECK(Cells(v.road) == words[9]);
			CHECK(Cells(v.footprint) == words[10]);
			CHECK(Cells(v.flowers) == words[11]);
			CHECK(Cells(v.yard) == words[12]);
			CHECK(Cells(v.pond) == words[13]);
			CHECK(Cells(v.pondRing) == words[14]);
		} else if (kind == "near") {
			auto data = std::vector<unsigned char>{};
			for (const auto& [c, d] : plan.nearVillage()) {
				AppendInt(data, c.first);
				AppendInt(data, c.second);
				AppendInt(data, d);
			}
			CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], "near of seed " << seed);
		} else if (kind == "weights") {
			auto data = std::vector<unsigned char>{};
			for (auto y = 0; y < plan.climate().height(); y++) {
				for (auto x = 0; x < plan.climate().width(); x++) {
					for (auto b = 0; b < worldgen::BiomeCount; b++) {
						Append(data, plan.weight(x, y, static_cast<worldgen::Biome>(b)));
					}
				}
			}
			CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], "weights of seed " << seed);
		} else {
			auto data = std::vector<unsigned char>{};
			for (auto y = 0; y < plan.climate().height(); y++) {
				for (auto x = 0; x < plan.climate().width(); x++) {
					for (const auto& [i, j] : { std::pair<int, int>{ x, y }, { x + 1, y }, { x, y + 1 }, { x + 1, y + 1 } }) {
						for (const auto n : plan.corner(i, j)) {
							data.push_back(n);
						}
					}
				}
			}
			CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], "mixes of seed " << seed);
		}
	}
}

namespace {
	const worldgen::Terrain& TerrainOf(std::int64_t seed) {
		static auto terrains = std::map<std::int64_t, std::unique_ptr<worldgen::Terrain>>{};
		auto found = terrains.find(seed);
		if (found == terrains.end()) {
			found = terrains.emplace(seed, std::make_unique<worldgen::Terrain>(PlanOf(seed))).first;
		}
		return *found->second;
	}

	std::string Path(const std::vector<worldgen::Block>& path) {
		auto data = std::vector<unsigned char>{};
		for (const auto& [x, y] : path) {
			AppendInt(data, x);
			AppendInt(data, y);
		}
		return std::to_string(Fnv(data));
	}
}

TEST_CASE("the roads, the terrain, the stairs and the places are those of the Python generator") {
	auto routeIndex = std::map<std::int64_t, std::size_t>{};
	auto poiIndex = std::map<std::int64_t, std::size_t>{};
	for (const auto& words : Golden()) {
		const auto& kind = words[0];
		if (kind != "routes" && kind != "route" && kind != "nearroute" && kind != "terrain" && kind != "stairs" && kind != "poi") {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		const auto& terrain = TerrainOf(seed);
		if (kind == "routes") {
			CHECK_MESSAGE(Cells(terrain.routes()) == words[2], "routes of seed " << seed);
			CHECK(terrain.routePaths().size() == std::stoul(words[3]));
		} else if (kind == "route") {
			const auto i = routeIndex[seed]++;
			REQUIRE(i < terrain.routePaths().size());
			const auto& route = terrain.routePaths()[i];
			INFO("route " << i << " of seed " << seed);
			CHECK(route.from == std::stoi(words[2]));
			CHECK(route.to == std::stoi(words[3]));
			CHECK(route.path.size() == std::stoul(words[4]));
			CHECK(Path(route.path) == words[5]);
		} else if (kind == "nearroute") {
			auto data = std::vector<unsigned char>{};
			for (const auto& [c, d] : terrain.nearRoute()) {
				AppendInt(data, c.first);
				AppendInt(data, c.second);
				AppendInt(data, d);
			}
			CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], "near route of seed " << seed);
		} else if (kind == "terrain") {
			auto data = std::vector<unsigned char>{};
			for (auto y = 0; y < terrain.height(); y++) {
				for (auto x = 0; x < terrain.width(); x++) {
					data.push_back(static_cast<unsigned char>(terrain.cell(x, y).ground));
					data.push_back(static_cast<unsigned char>(terrain.cell(x, y).level));
				}
			}
			CHECK_MESSAGE(std::to_string(Fnv(data)) == words[2], "terrain of seed " << seed);
		} else if (kind == "stairs") {
			CHECK_MESSAGE(Cells(terrain.stairs()) == words[2], "stairs of seed " << seed);
		} else {
			const auto i = poiIndex[seed]++;
			REQUIRE(i < terrain.pois().size());
			const auto& poi = terrain.pois()[i];
			INFO("place " << i << " of seed " << seed);
			CHECK(poi.kind == words[2]);
			CHECK(poi.x == std::stoi(words[3]));
			CHECK(poi.y == std::stoi(words[4]));
			CHECK(poi.w == std::stoi(words[5]));
			CHECK(poi.h == std::stoi(words[6]));
			CHECK(Path(poi.spur) == words[7]);
			CHECK(static_cast<int>(poi.biome) == std::stoi(words[8]));
		}
	}
	for (const auto& [seed, count] : poiIndex) {
		CHECK_MESSAGE(TerrainOf(seed).pois().size() == count, "the number of places of seed " << seed);
	}
}
