#include "StateHolderCore.h"
#include "../../Exceptions/StateDiedException.h"

void ska::StateHolderCore::graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) {
	m_currentState->graphicUpdate(ellapsedTime, drawables);
}

void ska::StateHolderCore::eventUpdate(unsigned ellapsedTime) {
	m_currentState->eventUpdate(ellapsedTime);
}

void ska::StateHolderCore::update() {
	if (m_nextState != nullptr) {
		bool firstState;
		bool triggerChangeScene;
		if (m_currentState != nullptr) {
			triggerChangeScene = !m_currentState->unload();
			firstState = false;
		}
		else {
			triggerChangeScene = true;
			firstState = true;
		}

		if (triggerChangeScene) {
			auto lastScene = move(firstState ? std::unique_ptr<ska::State>(nullptr) : move(m_currentState));
			m_currentState = move(m_nextState);
			m_nextState = nullptr;
			m_currentState->load(firstState ? nullptr : &lastScene);

			/* We have to invalidate the current iterating (old) scene. */
			if (!firstState) {
				throw ska::StateDiedException("");
			}
		}
	}

	/* If exists, an helper that executes current running task once */
	refresh();
}
