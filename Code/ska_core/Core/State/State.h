#pragma once
#include <memory>
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class StateHolder;
	class ISystem;
	class IGraphicSystem;
	class DrawableContainer;

	class State : public HasGraphic, public HasLogic {
        template <class EM, class ED>
        friend class StateBase;

	public:
		explicit State(StateHolder& holder);
        State(State& oldScene);

		void operator=(const State&) = delete;

		virtual void graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables) override = 0;
		virtual void eventUpdate(unsigned int) override = 0;
		virtual void load(std::unique_ptr<State>* lastScene) = 0;
		virtual bool unload() = 0;

		virtual ~State() = default;

    private:
        StateHolder& m_holder;
	};

	using ScenePtr = std::unique_ptr<State>;
}

