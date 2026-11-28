// The C++ port of the infinite world (World/Generation/Infinite) against the values of tools/worldgen/infinite.py
// (tests/worldgen/infinite.txt, made by tools/worldgen/golden_infinite.py) : the same numbers, bit for bit.
#include <algorithm>
#include <cstdint>
#include <set>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "doctest.h"
#include "Game/Gameplay/World/Generation/Infinite.h"

namespace {
	std::uint64_t Fnv(const std::vector<unsigned char>& data) {
		auto h = 0xcbf29ce484222325ULL;
		for (const auto b : data) {
			h ^= b;
			h *= 0x100000001b3ULL;
		}
		return h;
	}

	template <class T>
	void Put(std::vector<unsigned char>& data, T value) {
		unsigned char bytes[sizeof(T)];
		std::memcpy(bytes, &value, sizeof(T));
		data.insert(data.end(), bytes, bytes + sizeof(T));
	}

	std::string Sum(const std::vector<unsigned char>& data) {
		return std::to_string(Fnv(data));
	}

	template <class Blocks>
	std::string Cells(const Blocks& blocks) {
		auto sorted = std::vector<worldgen::Block>(blocks.begin(), blocks.end());
		std::sort(sorted.begin(), sorted.end());
		auto data = std::vector<unsigned char>{};
		for (const auto& [x, y] : sorted) {
			Put<std::int32_t>(data, x);
			Put<std::int32_t>(data, y);
		}
		return Sum(data);
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

	std::vector<std::vector<std::string>> Golden() {
		auto file = std::ifstream{ "tests/worldgen/infinite.txt" };
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

	worldgen::Infinite& World(std::int64_t seed) {
		static auto worlds = std::map<std::int64_t, std::unique_ptr<worldgen::Infinite>>{};
		auto& world = worlds[seed];
		if (!world) {
			world = std::make_unique<worldgen::Infinite>(seed);
		}
		return *world;
	}

	// golden_infinite.py samples
	std::vector<worldgen::Block> Samples() {
		auto out = std::vector<worldgen::Block>{};
		for (const auto oy : { worldgen::Center - 900, worldgen::Center + 3000 }) {
			for (const auto ox : { worldgen::Center - 900, worldgen::Center - 4100 }) {
				for (auto j = 0; j < 40; j++) {
					for (auto i = 0; i < 40; i++) {
						out.push_back({ ox + i * 37, oy + j * 37 });
					}
				}
			}
		}
		return out;
	}

	std::string Pair(const worldgen::Cell2& c) {
		return std::to_string(c.first) + "," + std::to_string(c.second);
	}
}

TEST_CASE("the climate of the infinite world is the climate of infinite.py") {
	for (const auto& words : Golden()) {
		auto data = std::vector<unsigned char>{};
		if (words[0] == "rank") {
			auto other = std::vector<unsigned char>{};
			for (auto v = -3; v < 68; v++) {
				Put(data, worldgen::Rank3(v / 64.0));
				Put(other, worldgen::Rank4(v / 64.0));
			}
			CHECK(Sum(data) == words[1]);
			CHECK(Sum(other) == words[2]);
			continue;
		}
		if (words[0] == "cellseed") {
			for (auto i = 60; i < 80; i++) {
				for (auto j = 60; j < 80; j++) {
					Put<std::int64_t>(data, worldgen::CellSeed(std::stoll(words[1]), i, j));
				}
			}
			CHECK(Sum(data) == words[2]);
			continue;
		}
		static const auto fields = std::set<std::string>{ "continent", "relief", "climate", "base", "landheight", "height", "biome", "ocean" };
		if (fields.count(words[0]) == 0) {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		auto& world = World(seed);
		for (const auto& [x, y] : Samples()) {
			if (words[0] == "continent") {
				Put(data, world.continent(x, y));
			} else if (words[0] == "relief") {
				Put(data, world.relief(x, y));
			} else if (words[0] == "climate") {
				data.push_back(static_cast<unsigned char>(world.climateAt(x, y)));
			} else if (words[0] == "base") {
				data.push_back(static_cast<unsigned char>(world.baseBiome(x, y)));
			} else if (words[0] == "landheight") {
				Put(data, world.landHeight(x, y));
			} else if (words[0] == "height") {
				Put(data, world.height(x, y));
			} else if (words[0] == "biome") {
				data.push_back(static_cast<unsigned char>(world.biome(x, y)));
			} else {
				data.push_back(static_cast<unsigned char>(world.ocean(x, y)));
			}
		}
		CHECK_MESSAGE(Sum(data) == words[2], words[0] << " of seed " << seed);
	}
}

TEST_CASE("the villages and the roads of the infinite world are the ones of infinite.py") {
	for (const auto& words : Golden()) {
		if (words[0] != "start" && words[0] != "site" && words[0] != "village" && words[0] != "links" && words[0] != "road") {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		auto& world = World(seed);
		if (words[0] == "start") {
			const auto start = world.start();
			CHECK(std::to_string(start.first) == words[2]);
			CHECK(std::to_string(start.second) == words[3]);
		} else if (words[0] == "site") {
			CHECK_MESSAGE(!world.site(std::stoi(words[2]), std::stoi(words[3])), "site " << words[2] << " " << words[3]);
		} else if (words[0] == "village") {
			const auto* cv = world.village(std::stoi(words[2]), std::stoi(words[3]));
			REQUIRE_MESSAGE(cv != nullptr, "village " << words[2] << " " << words[3]);
			const auto& v = cv->v;
			CHECK(std::to_string(v.center.first) == words[4]);
			CHECK(std::to_string(v.center.second) == words[5]);
			CHECK(std::to_string(static_cast<int>(v.biome)) == words[6]);
			CHECK(std::to_string(cv->first ? 1 : 0) == words[7]);
			CHECK(Hex(v.name) == words[8]);
			CHECK(std::to_string(v.buildings.size()) == words[9]);
			auto repr = std::vector<unsigned char>{};
			for (const auto c : v.buildingsRepr()) {
				repr.push_back(static_cast<unsigned char>(c));
			}
			CHECK(Sum(repr) == words[10]);
			CHECK(Cells(v.road) == words[11]);
			CHECK(Cells(v.footprint) == words[12]);
			CHECK(Cells(v.flowers) == words[13]);
			CHECK(Cells(v.yard) == words[14]);
			CHECK(Cells(v.pond) == words[15]);
			CHECK(Cells(v.pondRing) == words[16]);
			auto near = std::vector<unsigned char>{};
			for (const auto& [c, d] : cv->nearBiome) {
				Put<std::int32_t>(near, c.first);
				Put<std::int32_t>(near, c.second);
				Put<std::int32_t>(near, d);
			}
			CHECK(Sum(near) == words[17]);
			CHECK(std::to_string(cv->near.size()) == words[18]);
		} else if (words[0] == "links") {
			auto text = std::string{};
			for (const auto& c : world.links(std::stoi(words[2]), std::stoi(words[3]))) {
				text += (text.empty() ? "" : " ") + Pair(c);
			}
			auto expected = std::string{};
			for (auto k = 4u; k < words.size(); k++) {
				expected += (expected.empty() ? "" : " ") + words[k];
			}
			CHECK_MESSAGE(text == expected, "links " << words[2] << " " << words[3]);
		} else {
			const auto ca = worldgen::Cell2{ std::stoi(words[2]), std::stoi(words[3]) };
			const auto cb = worldgen::Cell2{ std::stoi(words[4]), std::stoi(words[5]) };
			const auto* path = world.roadPath(ca, cb);
			if (words[6] == "none") {
				CHECK(path == nullptr);
				continue;
			}
			REQUIRE(path != nullptr);
			CHECK(std::to_string(path->size()) == words[6]);
			auto data = std::vector<unsigned char>{};
			for (const auto& [x, y] : *path) {
				Put<std::int32_t>(data, x);
				Put<std::int32_t>(data, y);
			}
			CHECK_MESSAGE(Sum(data) == words[7], "road " << Pair(ca) << " " << Pair(cb));
			CHECK(Cells(*world.road(ca, cb)) == words[8]);
		}
	}
}

TEST_CASE("the terrain of a region of the infinite world is the terrain of infinite.py") {
	for (const auto& words : Golden()) {
		if (words[0] != "classify" && words[0] != "terrain" && words[0] != "stairs") {
			continue;
		}
		const auto seed = std::stoll(words[1]);
		auto& world = World(seed);
		const auto x0 = std::stoi(words[2]) * worldgen::Region;
		const auto y0 = std::stoi(words[3]) * worldgen::Region;
		if (words[0] == "classify") {
			auto data = std::vector<unsigned char>{};
			for (auto j = 0; j < worldgen::Region; j += 3) {
				for (auto i = 0; i < worldgen::Region; i += 3) {
					const auto c = world.classify(x0 + i, y0 + j);
					data.push_back(static_cast<unsigned char>(c.ground));
					data.push_back(static_cast<unsigned char>(c.level));
				}
			}
			CHECK(Sum(data) == words[4]);
			continue;
		}
		static auto areas = std::map<std::pair<int, int>, worldgen::Area>{};
		auto found = areas.find({ x0, y0 });
		if (found == areas.end()) {
			found = areas.emplace(std::make_pair(x0, y0), world.area(x0, y0, x0 + worldgen::Region, y0 + worldgen::Region)).first;
		}
		const auto& area = found->second;
		if (words[0] == "terrain") {
			auto data = std::vector<unsigned char>{};
			for (const auto& c : area.terrain) {
				data.push_back(static_cast<unsigned char>(c.ground));
				data.push_back(static_cast<unsigned char>(c.level));
			}
			CHECK(Sum(data) == words[4]);
		} else {
			CHECK(std::to_string(area.stairs.size()) == words[4]);
			CHECK(Cells(area.stairs) == words[5]);
			CHECK(std::to_string(area.unreached) == words[6]);
		}
	}
}
