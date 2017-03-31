#pragma once
#include "SceneHolder.h"

namespace ska {
    class SceneHolderCore : public SceneHolder {
    public:
        SceneHolderCore() = default;
        ~SceneHolderCore() = default;

        void update() override;
        void nextScene(std::unique_ptr<ska::Scene>& scene) override;
        ska::ScenePtr& getScene() override;

	private:
		ScenePtr m_currentScene;
		ScenePtr m_nextScene;
    };
}
