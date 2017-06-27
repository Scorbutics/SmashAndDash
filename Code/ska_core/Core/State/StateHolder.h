#pragma once
#include "State.h"
#include "../../Task/TaskQueue.h"

namespace ska {
	class StateHolder : public TaskQueue {
	public:
		StateHolder() = default;
		StateHolder(const StateHolder&) = delete;
		StateHolder& operator=(const StateHolder&) = delete;
		virtual ~StateHolder() = default;

		virtual void update() = 0;

		template<class S>
		void nextState(std::unique_ptr<S>&& state) {
			m_nextState = std::move(state);
		}

	protected:
		ScenePtr m_nextState;
	};
}
