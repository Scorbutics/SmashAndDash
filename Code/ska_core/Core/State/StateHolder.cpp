#include "StateHolder.h"
#include "../../Exceptions/StateDiedException.h"
#include "../../Utils/Observer.h"

ska::StateHolder::StateHolder(GameEventDispatcher& ged) :
	m_eventDispatcher(ged),
	m_sceneLoaded(false) {

}

void ska::StateHolder::graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) {
	m_currentState->graphicUpdate(ellapsedTime, drawables);
}

void ska::StateHolder::eventUpdate(unsigned int ellapsedTime) {
	m_currentState->eventUpdate(ellapsedTime);
}

void ska::StateHolder::update() {
	if (m_nextState != nullptr) {
		bool firstState;
		bool triggerChangeScene;
		if (m_currentState != nullptr) {
			triggerChangeScene = !m_currentState->unload();
			firstState = false;
		} else {
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
				StateEvent se(STATE_CHANGED);
				m_eventDispatcher.ska::Observable<ska::StateEvent>::notifyObservers(se);
				throw ska::StateDiedException("");
			}

			StateEvent se(FIRST_STATE_LOADED);
			m_eventDispatcher.ska::Observable<ska::StateEvent>::notifyObservers(se);
		}
	}

	/* If exists, an helper that executes current running task once */
	refresh();
}
