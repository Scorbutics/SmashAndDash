#pragma once
#include "StateHolder.h"

namespace ska {
    class StateHolderCore : 
		public StateHolder,
		public HasGraphic,
		public HasLogic {
    public:
        StateHolderCore() = default;
        ~StateHolderCore() = default;

		void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override;
		void eventUpdate(unsigned int ellapsedTime) override;

        void update() override;
	
	private:
		ScenePtr m_currentScene;
	    bool m_sceneLoaded;
		std::unique_ptr<State> m_lastState;
    };
}
