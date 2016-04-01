#include "SceneHolderCore.h"
#include "../../ska/Exceptions/SceneDiedException.h"

void SceneHolderCore::nextScene(std::unique_ptr<ska::Scene>& scene) {
	bool firstScene;
	if (m_currentScene != NULL) {
		m_currentScene->unload();
		firstScene = false;
	}
	else {
		firstScene = true;
	}

	m_currentScene = std::move(scene);
	m_currentScene->load();

	/* We have to invalidate the current iterating (old) scene. */
	if (!firstScene) {
		throw ska::SceneDiedException("");
	}
}

ska::ScenePtr& SceneHolderCore::getScene() {
	return m_currentScene;
}
