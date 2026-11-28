#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "ChunkGrid.h"

namespace worldgen {
	class InfiniteWriter;
}

namespace chunks {
	// The chunks of an infinite world written as the player comes near them (worldgen::InfiniteWriter, plain C++,
	// tests/chunks) : on a thread (native), a web worker (the web : world_worker.js, its files sent back ; a chunk wanted
	// at once is written in the page), else in the steps of the game (the fixed step runs : the same chunks at the same
	// steps every time)
	class Generator {
	public:
		Generator(std::unique_ptr<worldgen::InfiniteWriter> writer, bool synchronous);
		~Generator();
		Generator(const Generator&) = delete;
		Generator& operator=(const Generator&) = delete;

		const std::string& folder() const { return m_folder; }
		bool synchronous() const;
		// The files of a chunk are there
		bool written(Coordinates c);
		// The chunks to write, the most urgent first (replaces the wishes before)
		void wish(std::vector<Coordinates> chunks);
		// A chunk written now (waits for the thread)
		void now(Coordinates c);
		// A step of the game : synchronous, the first chunk wished written
		void step();

	private:
		struct State;
#ifdef __EMSCRIPTEN__
		// The web : the worker that writes the chunks (world_worker.js), its replies
		void startWorker();
		static void Ready(char* data, int size, void* argument);
		static void Written(char* data, int size, void* argument);
#endif
		std::string m_folder;
		std::shared_ptr<State> m_state;
	};

	// The writer of a world given by the loading screen to the game (its region planned) : taken by the game for the
	// world of this folder
	void HandOver(std::unique_ptr<worldgen::InfiniteWriter> writer);
	std::unique_ptr<worldgen::InfiniteWriter> TakeOver(const std::string& folder);
}
