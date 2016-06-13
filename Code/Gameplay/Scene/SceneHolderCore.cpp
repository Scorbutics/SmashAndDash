#include "SceneHolderCore.h"
#include "../../ska/Exceptions/SceneDiedException.h"

void SceneHolderCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	bool firstScene;
	bool triggerChangeScene;
	if (m_currentScene != NULL) {
		triggerChangeScene = !m_currentScene->unload();
		firstScene = false;
	} else {
		triggerChangeScene = true;
		firstScene = true;
	}
	
	if (triggerChangeScene) {
		std::unique_ptr<ska::Scene>& lastScene = std::move(firstScene ? nullptr : std::move(m_currentScene));
		m_currentScene = std::move(scene);
		m_currentScene->load(firstScene ? nullptr : &lastScene);

		/* We have to invalidate the current iterating (old) scene. */
		if (!firstScene) {
			throw ska::SceneDiedException("");
		}
	}
}

ska::ScenePtr& SceneHolderCore::getScene() {
	return m_currentScene;
}
