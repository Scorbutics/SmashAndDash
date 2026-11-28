#include <deque>
#include "ChunkPrepareQueue.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <condition_variable>
#include <mutex>
#include <thread>
#endif

namespace {
	class InlineQueue : public chunks::PrepareQueue {
	public:
		void request(std::uint64_t id, const chunks::Paths& paths) override {
			m_done.push_back({ id, chunks::Decode(chunks::Read(paths)) });
		}
		std::optional<chunks::Prepared> poll() override {
			if (m_done.empty()) {
				return std::nullopt;
			}
			auto result = std::move(m_done.front());
			m_done.pop_front();
			return result;
		}
	private:
		std::deque<chunks::Prepared> m_done;
	};

#ifdef __EMSCRIPTEN__
	// The files of the game are those of the page (the saves, the game files packed with it) : the page reads them and
	// sends their bytes, the worker decodes them (ChunkWorkerMain.cpp)
	class WorkerQueue : public chunks::PrepareQueue {
	public:
		WorkerQueue() : m_state(std::make_shared<State>()), m_worker(emscripten_create_worker("chunk_worker.js")) {}
		// (The replies still to come are dropped with the worker)
		~WorkerQueue() override { emscripten_destroy_worker(m_worker); }

		void request(std::uint64_t id, const chunks::Paths& paths) override {
			auto message = chunks::Pack(chunks::Read(paths));
			// Released by the reply
			auto* call = new Call{ m_state, id };
			emscripten_call_worker(m_worker, "prepareChunk", reinterpret_cast<char*>(message.data()), static_cast<int>(message.size()), &WorkerQueue::Reply, call);
		}

		std::optional<chunks::Prepared> poll() override {
			if (m_state->done.empty()) {
				return std::nullopt;
			}
			auto result = std::move(m_state->done.front());
			m_state->done.pop_front();
			return result;
		}

	private:
		struct State {
			std::deque<chunks::Prepared> done;
		};
		struct Call {
			std::shared_ptr<State> state;
			std::uint64_t id;
		};

		static void Reply(char* data, int size, void* argument) {
			const auto call = std::unique_ptr<Call>{ static_cast<Call*>(argument) };
			auto prepared = chunks::Prepared{ call->id, {} };
			try {
				prepared.images = chunks::UnpackImages(reinterpret_cast<const std::uint8_t*>(data), static_cast<std::size_t>(size));
			} catch (const std::exception& e) {
				prepared.images.error = e.what();
			}
			call->state->done.push_back(std::move(prepared));
		}

		std::shared_ptr<State> m_state;
		worker_handle m_worker;
	};
#else
	class ThreadQueue : public chunks::PrepareQueue {
	public:
		ThreadQueue() : m_thread([this]() { run(); }) {}
		~ThreadQueue() override {
			{
				const auto lock = std::lock_guard<std::mutex>{ m_mutex };
				m_stop = true;
			}
			m_wake.notify_one();
			m_thread.join();
		}

		void request(std::uint64_t id, const chunks::Paths& paths) override {
			{
				const auto lock = std::lock_guard<std::mutex>{ m_mutex };
				m_requests.push_back({ id, paths });
			}
			m_wake.notify_one();
		}

		std::optional<chunks::Prepared> poll() override {
			const auto lock = std::lock_guard<std::mutex>{ m_mutex };
			if (m_done.empty()) {
				return std::nullopt;
			}
			auto result = std::move(m_done.front());
			m_done.pop_front();
			return result;
		}

	private:
		void run() {
			while (true) {
				auto request = std::pair<std::uint64_t, chunks::Paths>{};
				{
					auto lock = std::unique_lock<std::mutex>{ m_mutex };
					m_wake.wait(lock, [this]() { return m_stop || !m_requests.empty(); });
					if (m_stop) {
						return;
					}
					request = std::move(m_requests.front());
					m_requests.pop_front();
				}
				auto prepared = chunks::Prepared{ request.first, {} };
				try {
					prepared.images = chunks::Decode(chunks::Read(request.second));
				} catch (const std::exception& e) {
					prepared.images.error = e.what();
				}
				const auto lock = std::lock_guard<std::mutex>{ m_mutex };
				m_done.push_back(std::move(prepared));
			}
		}

		std::mutex m_mutex;
		std::condition_variable m_wake;
		std::deque<std::pair<std::uint64_t, chunks::Paths>> m_requests;
		std::deque<chunks::Prepared> m_done;
		bool m_stop = false;
		// Started last : it uses the others
		std::thread m_thread;
	};
#endif
}

std::unique_ptr<chunks::PrepareQueue> chunks::MakePrepareQueue(bool synchronous) {
	if (synchronous) {
		return std::make_unique<InlineQueue>();
	}
#ifdef __EMSCRIPTEN__
	return std::make_unique<WorkerQueue>();
#else
	return std::make_unique<ThreadQueue>();
#endif
}
