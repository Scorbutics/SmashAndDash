#pragma once
#include <algorithm>
#include <optional>
#include <string>
#include <vector>

// The chunks of the world (plain C++, tests/chunks) : the outdoors is a grid of chunks "w_<x>_<y>" played as one map, in
// blocks of the world (the chunk (x, y) from the block (x * width, y * height)). A room ("w_room_0", any other map) is a
// world of one chunk.
namespace chunks {
	struct Coordinates {
		int x = 0;
		int y = 0;
		bool operator==(const Coordinates& other) const { return x == other.x && y == other.y; }
		bool operator<(const Coordinates& other) const { return x != other.x ? x < other.x : y < other.y; }
	};

	struct Grid {
		// In chunks
		int width = 1;
		int height = 1;
		// A chunk, in blocks
		int chunkWidth = 1;
		int chunkHeight = 1;
		// Not empty : the world of this single map (a room), else the outdoors
		std::string single;

		std::string name(Coordinates c) const {
			return single.empty() ? "w_" + std::to_string(c.x) + "_" + std::to_string(c.y) : single;
		}
		bool contains(Coordinates c) const { return c.x >= 0 && c.y >= 0 && c.x < width && c.y < height; }
		// The chunk of a block of the world (the nearest one for a block out of the world)
		Coordinates at(int blockX, int blockY) const {
			return { std::clamp(blockX, 0, width * chunkWidth - 1) / chunkWidth, std::clamp(blockY, 0, height * chunkHeight - 1) / chunkHeight };
		}
		int originX(Coordinates c) const { return c.x * chunkWidth; }
		int originY(Coordinates c) const { return c.y * chunkHeight; }
		// Blocks between a block and the chunk (0 : in it), the largest of the two axes
		int distance(Coordinates c, int blockX, int blockY) const {
			const auto dx = std::max({ originX(c) - blockX, blockX - (originX(c) + chunkWidth - 1), 0 });
			const auto dy = std::max({ originY(c) - blockY, blockY - (originY(c) + chunkHeight - 1), 0 });
			return std::max(dx, dy);
		}
		// The chunks within "radius" blocks of a block, the nearest first
		std::vector<Coordinates> around(int blockX, int blockY, int radius) const {
			auto result = std::vector<Coordinates>{};
			const auto first = at(blockX - radius, blockY - radius);
			const auto last = at(blockX + radius, blockY + radius);
			for (auto y = first.y; y <= last.y; y++) {
				for (auto x = first.x; x <= last.x; x++) {
					if (distance({ x, y }, blockX, blockY) <= radius) {
						result.push_back({ x, y });
					}
				}
			}
			std::stable_sort(result.begin(), result.end(), [&](const Coordinates& a, const Coordinates& b) {
				return distance(a, blockX, blockY) < distance(b, blockX, blockY);
			});
			return result;
		}
	};

	// A chunk is loaded within LoadRadius blocks of the player and leaves past UnloadRadius (no coming and going on a
	// border). The screen shows about 9 blocks around the player : under VisibleRadius, the game waits for the chunk
	constexpr int LoadRadius = 20;
	constexpr int UnloadRadius = 28;
	constexpr int VisibleRadius = 12;

	// The coordinates of an outdoor chunk from its name ("w_5_4"), none for another map
	inline std::optional<Coordinates> Parse(const std::string& name) {
		if (name.rfind("w_", 0) != 0) {
			return std::nullopt;
		}
		const auto number = [&name](std::size_t& i) -> std::optional<int> {
			auto value = 0;
			const auto start = i;
			while (i < name.size() && name[i] >= '0' && name[i] <= '9' && i - start < 6) {
				value = value * 10 + (name[i++] - '0');
			}
			return i == start ? std::nullopt : std::optional<int>{ value };
		};
		auto i = std::size_t{ 2 };
		const auto x = number(i);
		if (!x || i >= name.size() || name[i++] != '_') {
			return std::nullopt;
		}
		const auto y = number(i);
		if (!y || i != name.size()) {
			return std::nullopt;
		}
		return Coordinates{ *x, *y };
	}
}
