#include "SceneHolderCore.h"
#include "../../ska/Exceptions/SceneDiedException.h"

void SceneHolderCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	m_nextScene = std::move(scene);
}

void SceneHolderCore::update() {
	if (m_nextScene != nullptr) {
		bool firstScene;
		bool triggerChangeScene;
		if (m_currentScene != nullptr) {
			triggerChangeScene = !m_currentScene->unload();
			firstScene = false;
		}
		else {
			triggerChangeScene = true;
			firstScene = true;
		}

		if (triggerChangeScene) {
			std::unique_ptr<ska::Scene>& lastScene = std::move(firstScene ? std::unique_ptr<ska::Scene>(nullptr) : std::move(m_currentScene));
			m_currentScene = std::move(m_nextScene);
			m_nextScene = nullptr;
			m_currentScene->load(firstScene ? nullptr : &lastScene);

			/* We have to invalidate the current iterating (old) scene. */
			if (!firstScene) {
				throw ska::SceneDiedException("");
			}
		}
	}

	/* If exists, an helper that executes current running task once */
	ska::TaskQueue::refresh();
}

ska::ScenePtr& SceneHolderCore::getScene() {
	return m_currentScene;
}
