#pragma once

namespace ska {
	class Runnable {
	public:
		Runnable() = default;
		virtual ~Runnable() {
		}
		virtual bool operator()() = 0;
	};
}
