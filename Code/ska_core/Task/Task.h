#pragma once
#include <functional>
#include "Runnable.h"
#include "../Utils/Identity.h"

namespace ska {
	
	class Task : 
		public Runnable {
	public:

        using F = std::function<bool(Task&)>;
        using LAMBDA = meta::Identity<F>::type;

		explicit Task(LAMBDA const& f) :
			m_f(f) {
		}

		virtual bool operator()() override {
			return run();
		}

		virtual ~Task() {
		}

	private:
		bool run() {
			return m_f(*this);
		}

		F m_f;
	};

}
