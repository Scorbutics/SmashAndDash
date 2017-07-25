#pragma once
#include "State.h"
#include "../../Task/TaskQueue.h"
#include "../../Data/Events/GameEventDispatcher.h"

namespace ska {
    class StateHolder :
		public TaskQueue {
    public:
	    explicit StateHolder(GameEventDispatcher& ged);
		StateHolder(const StateHolder&) = delete;
		StateHolder& operator=(const StateHolder&) = delete;
        ~StateHolder() = default;

		void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables);
		void eventUpdate(unsigned int ellapsedTime);

        template<class S>
		void nextState(std::unique_ptr<S>&& state) {
			m_nextState = std::move(state);
		}

        void update();

	private:
		StatePtr m_nextState;
		GameEventDispatcher& m_eventDispatcher;
		StatePtr m_currentState;
	    bool m_sceneLoaded;
		StatePtr m_lastState;
    };
}
