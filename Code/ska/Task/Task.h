#pragma once
#include <functional>

namespace ska {
	class Task {
	public:
		Task(const std::function<bool(void)>& f) : m_f(f) {
		};

		bool operator()() {
			return m_f();
		}

		virtual ~Task() = default;

	private:
		std::function<bool(void)>  m_f;
	};
}