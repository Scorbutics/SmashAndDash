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

		void graphicUpdate(DrawableContainer& drawables) override;
		void eventUpdate(unsigned int ellapsedTime) override;

        void update() override;
	
	private:
		ScenePtr m_currentScene;
    };
}
