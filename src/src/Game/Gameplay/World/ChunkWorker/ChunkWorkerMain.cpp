// The web worker of the chunks (chunk_worker.js, src/CMakeLists.txt) : it decodes the files of a chunk the page sends
// (ChunkPrepareQueue.cpp) and sends their images back. A module of its own, with no engine.
#include <emscripten/emscripten.h>
#include "../Chunks/ChunkImages.h"

extern "C" EMSCRIPTEN_KEEPALIVE void prepareChunk(char* data, int size) {
	auto images = chunks::Images{};
	try {
		images = chunks::Decode(chunks::UnpackFiles(reinterpret_cast<const std::uint8_t*>(data), static_cast<std::size_t>(size)));
	} catch (const std::exception& e) {
		images.error = e.what();
	}
	auto reply = chunks::Pack(images);
	emscripten_worker_respond(reinterpret_cast<char*>(reply.data()), static_cast<int>(reply.size()));
}
