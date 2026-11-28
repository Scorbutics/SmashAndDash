#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include "ChunkImages.h"

namespace chunks {
	struct Prepared {
		// The request it answers (a request given up is thrown away by the caller)
		std::uint64_t id = 0;
		Images images;
	};

	// Prepares the images of the chunks away from the game, in the order of the requests : the results are taken by poll
	class PrepareQueue {
	public:
		virtual ~PrepareQueue() = default;
		virtual void request(std::uint64_t id, const Paths& paths) = 0;
		// A chunk prepared, none when no new one is ready
		virtual std::optional<Prepared> poll() = 0;
	};

	// A thread (native), a web worker (web : chunk_worker.js), or at once in the request ("synchronous" : the fixed step
	// runs, the same chunks at the same steps every time)
	std::unique_ptr<PrepareQueue> MakePrepareQueue(bool synchronous);
}
