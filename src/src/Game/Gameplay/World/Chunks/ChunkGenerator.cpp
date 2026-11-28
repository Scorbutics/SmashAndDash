#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include "ChunkGenerator.h"
#include "../Generation/InfiniteFiles.h"
#include "FileBundle.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <filesystem>
#else
#include <condition_variable>
#include <mutex>
#include <thread>
#endif

struct chunks::Generator::State {
	std::unique_ptr<worldgen::InfiniteWriter> writer;
	bool synchronous = true;
	std::set<Coordinates> written;
	std::deque<Coordinates> wishes;
	std::deque<Coordinates> urgent;
	std::string error;
#ifdef __EMSCRIPTEN__
	// The web : the worker that writes the chunks (world_worker.js), set up, the chunk it writes
	worker_handle worker = 0;
	bool ready = false;
	bool busy = false;
	Coordinates writing;
	// A reply comes after the generator is gone : it is dropped
	std::shared_ptr<bool> alive = std::make_shared<bool>(true);
#else
	std::mutex mutex;
	std::condition_variable changed;
	std::thread thread;
	bool stop = false;
#endif

	// A chunk written (an error : told once, the chunk counted as written, its map is then missing)
	void write(Coordinates c) {
		try {
			if (!writer->written(c.x, c.y)) {
				writer->write(c.x, c.y);
			}
		} catch (std::exception& e) {
			std::cerr << "Chunk generator : the chunk " << c.x << " " << c.y << " cannot be written : " << e.what() << std::endl;
		}
	}
};

chunks::Generator::Generator(std::unique_ptr<worldgen::InfiniteWriter> writer, bool synchronous) :
	m_folder(writer->folder()),
	m_state(std::make_shared<State>()) {
	m_state->writer = std::move(writer);
#ifdef __EMSCRIPTEN__
	m_state->synchronous = synchronous;
	if (!synchronous) {
		startWorker();
	}
#else
	m_state->synchronous = synchronous;
	if (!synchronous) {
		m_state->thread = std::thread{ [state = m_state] {
			auto lock = std::unique_lock<std::mutex>{ state->mutex };
			while (true) {
				state->changed.wait(lock, [&state] { return state->stop || !state->urgent.empty() || !state->wishes.empty(); });
				if (state->stop) {
					return;
				}
				auto& from = state->urgent.empty() ? state->wishes : state->urgent;
				const auto c = from.front();
				from.pop_front();
				if (state->written.count(c) != 0) {
					continue;
				}
				lock.unlock();
				state->write(c);
				lock.lock();
				state->written.insert(c);
				state->changed.notify_all();
			}
		} };
	}
#endif
}

chunks::Generator::~Generator() {
#ifdef __EMSCRIPTEN__
	*m_state->alive = false;
	if (m_state->worker != 0) {
		emscripten_destroy_worker(m_state->worker);
	}
#else
	if (m_state->thread.joinable()) {
		{
			auto lock = std::lock_guard<std::mutex>{ m_state->mutex };
			m_state->stop = true;
		}
		m_state->changed.notify_all();
		m_state->thread.join();
	}
#endif
}

bool chunks::Generator::synchronous() const {
	return m_state->synchronous;
}

bool chunks::Generator::written(Coordinates c) {
#ifndef __EMSCRIPTEN__
	auto lock = std::unique_lock<std::mutex>{ m_state->mutex, std::defer_lock };
	if (!m_state->synchronous) {
		lock.lock();
	}
#endif
	if (m_state->written.count(c) != 0) {
		return true;
	}
	// Written before (by an earlier game) : its last file is there
	if (m_state->writer->written(c.x, c.y)) {
		m_state->written.insert(c);
		return true;
	}
	return false;
}

void chunks::Generator::wish(std::vector<Coordinates> chunks) {
#ifndef __EMSCRIPTEN__
	if (!m_state->synchronous) {
		{
			auto lock = std::lock_guard<std::mutex>{ m_state->mutex };
			m_state->wishes.assign(chunks.begin(), chunks.end());
		}
		m_state->changed.notify_all();
		return;
	}
#endif
	m_state->wishes.assign(chunks.begin(), chunks.end());
}

void chunks::Generator::now(Coordinates c) {
	if (written(c)) {
		return;
	}
#ifndef __EMSCRIPTEN__
	if (!m_state->synchronous) {
		auto lock = std::unique_lock<std::mutex>{ m_state->mutex };
		m_state->urgent.push_front(c);
		m_state->changed.notify_all();
		m_state->changed.wait(lock, [this, c] { return m_state->written.count(c) != 0; });
		return;
	}
#endif
	m_state->write(c);
	m_state->written.insert(c);
}

void chunks::Generator::step() {
#ifdef __EMSCRIPTEN__
	if (!m_state->synchronous) {
		// The worker : a chunk at a time, the most urgent first
		while (m_state->ready && !m_state->busy && !m_state->wishes.empty()) {
			const auto c = m_state->wishes.front();
			m_state->wishes.pop_front();
			if (written(c)) {
				continue;
			}
			m_state->busy = true;
			m_state->writing = c;
			auto request = std::to_string(c.x) + " " + std::to_string(c.y);
			auto* call = new std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>{ m_state, m_state->alive };
			emscripten_call_worker(m_state->worker, "writeChunk", request.data(), static_cast<int>(request.size()), &Generator::Written, call);
		}
		return;
	}
#endif
	if (!m_state->synchronous) {
		return;
	}
	while (!m_state->wishes.empty()) {
		const auto c = m_state->wishes.front();
		m_state->wishes.pop_front();
		if (!written(c)) {
			m_state->write(c);
			m_state->written.insert(c);
			return;
		}
	}
}

#ifdef __EMSCRIPTEN__
namespace {
	// The data of the generator, from the files of the game (InfiniteWriter::prepare reads them)
	void AddFiles(chunks::FileBundle& files, const std::filesystem::path& path) {
		if (std::filesystem::is_directory(path)) {
			for (const auto& entry : std::filesystem::directory_iterator{ path }) {
				AddFiles(files, entry.path());
			}
			return;
		}
		auto in = std::ifstream{ path, std::ios::binary };
		files.emplace_back(path.lexically_normal().generic_string(), std::vector<std::uint8_t>(std::istreambuf_iterator<char>{ in }, std::istreambuf_iterator<char>{}));
	}
}

void chunks::Generator::startWorker() {
	auto& state = *m_state;
	state.worker = emscripten_create_worker("world_worker.js");
	auto files = FileBundle{};
	const auto world = std::to_string(state.writer->seed()) + "\n" + state.writer->folder();
	files.emplace_back("world", std::vector<std::uint8_t>(world.begin(), world.end()));
	for (const auto* path : { "Resources/Worldgen/shapes.txt", "Resources/Worldgen/tiles.txt", "Resources/Worldgen/tiles.png",
		"Resources/Worldgen/painted.png", "Resources/Worldgen/painted.txt", "Resources/Worldgen/rooms", "Resources/Chipsets/corr.png",
		"Resources/Chipsets/tile_scripts" }) {
		if (std::filesystem::exists(path)) {
			AddFiles(files, path);
		}
	}
	auto message = PackBundle(files);
	auto* call = new std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>{ m_state, m_state->alive };
	emscripten_call_worker(state.worker, "setupWorld", reinterpret_cast<char*>(message.data()), static_cast<int>(message.size()), &Generator::Ready, call);
}

void chunks::Generator::Ready(char* data, int size, void* argument) {
	const auto call = std::unique_ptr<std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>>{ static_cast<std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>*>(argument) };
	if (!*call->second) {
		return;
	}
	auto& state = *call->first;
	const auto files = UnpackBundle(reinterpret_cast<const std::uint8_t*>(data), static_cast<std::size_t>(size));
	if (!files.empty()) {
		std::cerr << "Chunk generator : the world worker cannot start : " << std::string(files[0].second.begin(), files[0].second.end()) << std::endl;
		return;
	}
	std::cout << "Chunk generator : the world worker is ready" << std::endl;
	state.ready = true;
}

void chunks::Generator::Written(char* data, int size, void* argument) {
	const auto call = std::unique_ptr<std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>>{ static_cast<std::pair<std::shared_ptr<State>, std::shared_ptr<bool>>*>(argument) };
	if (!*call->second) {
		return;
	}
	auto& state = *call->first;
	state.busy = false;
	try {
		const auto files = UnpackBundle(reinterpret_cast<const std::uint8_t*>(data), static_cast<std::size_t>(size));
		if (files.size() == 1 && files[0].first == "error") {
			std::cerr << "Chunk generator : the chunk " << state.writing.x << " " << state.writing.y << " cannot be written : "
				<< std::string(files[0].second.begin(), files[0].second.end()) << std::endl;
		} else {
			WriteBundle(files, state.writer->folder());
		}
	} catch (const std::exception& e) {
		std::cerr << "Chunk generator : the files of the chunk " << state.writing.x << " " << state.writing.y << " : " << e.what() << std::endl;
	}
	state.written.insert(state.writing);
}
#endif

namespace {
	std::unique_ptr<worldgen::InfiniteWriter>& Handed() {
		static auto writer = std::unique_ptr<worldgen::InfiniteWriter>{};
		return writer;
	}
}

void chunks::HandOver(std::unique_ptr<worldgen::InfiniteWriter> writer) {
	Handed() = std::move(writer);
}

std::unique_ptr<worldgen::InfiniteWriter> chunks::TakeOver(const std::string& folder) {
	auto& handed = Handed();
	if (handed != nullptr && handed->folder() == folder) {
		return std::move(handed);
	}
	handed = nullptr;
	return nullptr;
}
