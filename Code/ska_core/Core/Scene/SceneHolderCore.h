#pragma once
#include "SceneHolder.h"

namespace ska {
    class SceneHolderCore : 
		public SceneHolder,
		public HasGraphic,
		public HasLogic {
    public:
        SceneHolderCore() = default;
        ~SceneHolderCore() = default;

		void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override;
		void eventUpdate(unsigned int ellapsedTime) override;

        void update() override;
	
	private:
		ScenePtr m_currentScene;
	    bool m_sceneLoaded;
		std::unique_ptr<Scene> m_lastScene;
    };
}
