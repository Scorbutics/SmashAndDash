#pragma once
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include "Files.h"
#include "Region.h"

// The files of an infinite world (Infinite.h, Region.h) : its chunks w_<cx>_<cy> written as the player comes near them,
// each from its region (planned once, kept while its chunks are written), the rooms of a region with its first chunk
// (w_room_<x>_<y>, a chipset for them : chipset_rooms_<rx>_<ry>). The folder of the world has a file infinite.ini : its
// seed (the game reads the world as WorldChunks x WorldChunks chunks : SCRIPTING.md "The world").
namespace worldgen {
	constexpr const char* InfiniteFile = "infinite.ini";

	// The seed of the infinite world of a folder (its infinite.ini), none for another world
	std::optional<std::int64_t> InfiniteSeed(const std::string& folder);

	class InfiniteWriter {
	public:
		InfiniteWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources = {});
		~InfiniteWriter();
		InfiniteWriter(const InfiniteWriter&) = delete;
		InfiniteWriter& operator=(const InfiniteWriter&) = delete;

		std::int64_t seed() const { return m_seed; }
		const std::string& folder() const { return m_folder; }
		// The data of the generator (the shapes, the atlas, the room templates), infinite.ini written : once, first
		void prepare();
		// Where the player starts : the block below the centre of the first village
		Start start();
		// The files of a chunk are there
		bool written(int cx, int cy) const;
		// The region of a chunk planned and built (kept : its chunks are then written quickly)
		void plan(int rx, int ry);
		// The files of a chunk written (and the rooms of its region, once)
		void write(int cx, int cy);

	private:
		struct Built;
		Built& region(int rx, int ry);

		std::int64_t m_seed;
		std::string m_folder, m_worldPath;
		Resources m_resources;
		std::unique_ptr<Infinite> m_world;
		struct Data;
		std::unique_ptr<Data> m_data;
		// The regions kept, the last used first
		std::list<std::unique_ptr<Built>> m_regions;
		std::optional<Start> m_start;
	};

	// A new infinite world, step by step (the loading screen : a step, a few hundred milliseconds at most) : the data,
	// the start, its region, the chunks around the start ; the chunks further are written by the game (release : the
	// writer, its region kept)
	class InfiniteWorldWriter {
	public:
		InfiniteWorldWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources = {});
		~InfiniteWorldWriter();
		// Does the next step : false once done
		bool step();
		bool done() const { return m_step >= steps(); }
		double progress() const { return static_cast<double>(m_step) / steps(); }
		// What the next step does (French, UTF-8 : the loading screen)
		std::string stage() const;
		const Start& start() const { return m_start; }
		std::unique_ptr<InfiniteWriter> release() { return std::move(m_writer); }
		// The chunks written around the start : within StartRadius chunks
		static constexpr int StartRadius = 1;
	private:
		int steps() const { return 3 + (2 * StartRadius + 1) * (2 * StartRadius + 1); }
		std::unique_ptr<InfiniteWriter> m_writer;
		int m_step = 0;
		Start m_start;
		int m_cx = 0, m_cy = 0;
	};

	// A new infinite world written at once (InfiniteWorldWriter)
	Start GenerateInfiniteWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources = {});
}
