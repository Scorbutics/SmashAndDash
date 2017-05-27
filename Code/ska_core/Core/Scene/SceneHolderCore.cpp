#include "SceneHolderCore.h"
#include "../../Exceptions/SceneDiedException.h"

void ska::SceneHolderCore::graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) {
	m_currentScene->graphicUpdate(ellapsedTime, drawables);
}

void ska::SceneHolderCore::eventUpdate(unsigned ellapsedTime) {
	m_currentScene->eventUpdate(ellapsedTime);
}

void ska::SceneHolderCore::update() {
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
			auto lastScene = move(firstScene ? std::unique_ptr<ska::Scene>(nullptr) : move(m_currentScene));
			m_currentScene = move(m_nextScene);
			m_nextScene = nullptr;
			m_currentScene->load(firstScene ? nullptr : &lastScene);

			/* We have to invalidate the current iterating (old) scene. */
			if (!firstScene) {
				throw ska::SceneDiedException("");
			}
		}
	}

	/* If exists, an helper that executes current running task once */
	refresh();
}
