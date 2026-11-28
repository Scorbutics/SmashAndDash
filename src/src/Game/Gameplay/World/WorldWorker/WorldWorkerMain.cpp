// The web worker of the infinite world (world_worker.js, src/CMakeLists.txt) : it writes the chunks of the world away from
// the game (the web has no thread : a new region takes seconds to plan). A module of its own, with no engine : the page
// sends it the data of the generator once (setupWorld), then a chunk to write at a time (writeChunk) ; it writes it in
// its own memory and sends its files back (ChunkGenerator.cpp), the files of the rooms of its region with the first one.
#include <emscripten/emscripten.h>
#include <filesystem>
#include <memory>
#include <sstream>
#include "../Chunks/FileBundle.h"
#include "../Generation/InfiniteFiles.h"

namespace {
	constexpr const char* Folder = "/world/";
	std::unique_ptr<worldgen::InfiniteWriter> Writer;

	void Respond(const chunks::FileBundle& files) {
		auto reply = chunks::PackBundle(files);
		emscripten_worker_respond(reinterpret_cast<char*>(reply.data()), static_cast<int>(reply.size()));
	}

	chunks::FileBundle Error(const std::string& text) {
		return { { "error", std::vector<std::uint8_t>(text.begin(), text.end()) } };
	}
}

// The data of the generator : "world" (its seed, a line, then the folder of the world as the maps name it) and the files
// of Resources (their paths from the game : "Resources/Worldgen/tiles.png"...)
extern "C" EMSCRIPTEN_KEEPALIVE void setupWorld(char* data, int size) {
	try {
		auto files = chunks::UnpackBundle(reinterpret_cast<const std::uint8_t*>(data), static_cast<std::size_t>(size));
		auto seed = std::int64_t{ 0 };
		auto worldPath = std::string{};
		auto resources = chunks::FileBundle{};
		for (auto& [path, bytes] : files) {
			if (path == "world") {
				auto in = std::istringstream{ std::string(bytes.begin(), bytes.end()) };
				in >> seed;
				in.ignore();
				std::getline(in, worldPath);
			} else {
				resources.emplace_back(path, std::move(bytes));
			}
		}
		chunks::WriteBundle(resources, "/");
		Writer = std::make_unique<worldgen::InfiniteWriter>(seed, Folder, worldPath, worldgen::Resources{ "/Resources" });
		Writer->prepare();
		std::filesystem::remove_all(Folder);
		Respond({});
	} catch (const std::exception& e) {
		Respond(Error(e.what()));
	}
}

// A chunk ("<cx> <cy>") : its files (from the folder of the world), the rooms of its region with the first chunk of it
extern "C" EMSCRIPTEN_KEEPALIVE void writeChunk(char* data, int size) {
	try {
		if (Writer == nullptr) {
			throw std::runtime_error{ "the world worker is not set up" };
		}
		auto in = std::istringstream{ std::string(data, static_cast<std::size_t>(size)) };
		auto cx = 0;
		auto cy = 0;
		in >> cx >> cy;
		Writer->write(cx, cy);
		const auto files = chunks::ReadBundle(Folder);
		// Sent : the memory of the worker keeps nothing of them
		for (const auto& entry : std::filesystem::directory_iterator{ Folder }) {
			std::filesystem::remove_all(entry.path());
		}
		Respond(files);
	} catch (const std::exception& e) {
		Respond(Error(e.what()));
	}
}
