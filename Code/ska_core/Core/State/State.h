#pragma once
#include <memory>

namespace ska {
	class StateHolder;
	class ISystem;
	class IGraphicSystem;
	class DrawableContainer;

	class State {
		template <class EM, class ED>
		friend class StateBase;
	public:
		explicit State(StateHolder& holder);
        State(State& lastState);

		void operator=(const State&) = delete;

		virtual void graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables) = 0;
		virtual void eventUpdate(unsigned int) = 0;
		virtual void load(std::unique_ptr<State>* lastState) = 0;
		virtual bool unload() = 0;

		virtual ~State() = default;
	
	private:
		StateHolder& m_holder;
	};

	using StatePtr = std::unique_ptr<State>;
}

