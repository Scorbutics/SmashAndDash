#pragma once
#include "StateHolder.h"
#include "../../Data/Events/GameEventDispatcher.h"

namespace ska {
    class StateHolderCore : 
		public StateHolder,
		public HasGraphic,
		public HasLogic {
    public:
	    explicit StateHolderCore(GameEventDispatcher& ged);
        ~StateHolderCore() = default;

		void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override;
		void eventUpdate(unsigned int ellapsedTime) override;

        void update() override;
	
	private:
		GameEventDispatcher& m_eventDispatcher;
		StatePtr m_currentState;
	    bool m_sceneLoaded;
		std::unique_ptr<State> m_lastState;
    };
}
