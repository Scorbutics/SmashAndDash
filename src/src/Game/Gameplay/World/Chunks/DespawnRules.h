#pragma once

// When the wild Pokemon go away (plain C++, tests/chunks) : far from the player and out of the screen, by chance, the
// farther ones at once, as Minecraft. Never one the player sees, nor one coming at them. The characters never.
namespace chunks {
	// Blocks from the player : under NearRadius they stay, over it they may leave each second, over FarRadius they leave
	constexpr float NearRadius = 24.F;
	constexpr float FarRadius = 56.F;
	// Chance each second (percent) between the two
	constexpr int LeaveChance = 2;
	// A wild Pokemon of a chunk gone for longer than this (ms of play) is not brought back
	constexpr long long ForgetAfter = 120000;
	// Wild Pokemon around the player (SpawnRadius blocks) : no new one past SpawnCap ; a chunk keeps ChunkCap of them ;
	// past LiveCap on the map, the farthest leave
	constexpr float SpawnRadius = 32.F;
	constexpr int SpawnCap = 8;
	constexpr int ChunkCap = 6;
	constexpr int LiveCap = 64;

	// distance : blocks from the player ; seen : on the screen (or just out of it) ; engaged : coming at the player ;
	// roll : 0 .. 99
	inline bool Despawns(float distance, bool seen, bool engaged, int roll) {
		if (seen || engaged || distance <= NearRadius) {
			return false;
		}
		return distance > FarRadius || roll < LeaveChance;
	}

	inline bool Forgotten(long long now, long long left) {
		return now - left > ForgetAfter;
	}
}
