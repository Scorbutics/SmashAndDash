// The C++ port of the world generator (src/src/Game/Gameplay/World/Generation) against the values of the Python
// generator (tests/worldgen/golden.txt, made by tools/worldgen/golden.py) : the same numbers, bit for bit.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "doctest.h"
#include "Game/Gameplay/World/Generation/Noise.h"
#include "Game/Gameplay/World/Generation/Climate.h"

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
