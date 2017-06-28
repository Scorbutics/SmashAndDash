#include "DialogTransition.h"
#include "../../../CustomEntityManager.h"
#include "ECS/Basics/Graphic/DialogComponent.h"
#include "Task/Task.h"

DialogTransition::DialogTransition(unsigned int delay, CustomEntityManager& em, PokemonGameEventDispatcher& ged, const ska::EntityId& trainerId, const std::string& message) :
	ska::Task([&, delay](ska::Task& t) {
		if (m_loadState == 0) {
			m_loadState++;

			ska::GUIEvent ge(ska::GUIEventType::ADD_BALLOON);
			ge.text = m_message;
			ge.delay = delay;
			ge.windowName = "trainerBalloon";
			m_eventDispatcher.ska::Observable<ska::GUIEvent>::notifyObservers(ge);

			ska::DialogComponent dc;
			dc.handle = ge.balloonHandle;
			dc.name = ge.windowName;
			m_entityManager.addComponent<ska::DialogComponent>(m_trainerId, dc);

			return true;
		}
		// Continue until dialog is still visible
		return m_entityManager.hasComponent<ska::DialogComponent>(m_trainerId);
	}),
	m_loadState(0),
	m_trainerId(trainerId),
    m_message(message),
    m_entityManager(em),
    m_eventDispatcher(ged){

}
