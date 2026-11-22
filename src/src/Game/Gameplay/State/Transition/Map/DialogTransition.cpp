#include "Game/__internalConfig/LoggerConfig.h"
#include "DialogTransition.h"
#include <limits>
#include "../../../CustomEntityManager.h"
#include "../../../Data/DialogEvent.h"
#include "Base/Task/Task.h"

DialogTransition::DialogTransition(unsigned int, CustomEntityManager& em, PokemonGameEventDispatcher& ged, const ska::EntityId& trainerId, const std::string& message) :
	ska::Task([&](ska::Task&) {
		if (m_loadState == 0) {
			m_loadState++;
			// The dialog window of the game (as the messages of the scripts), closed by the player (action key)
			static auto dialogNumber = 0u;
			auto dialog = DialogEvent{ m_message, "battleDialog" + std::to_string(dialogNumber++), ska::Rectangle{ 0, std::numeric_limits<int>::max(), -1, 2 * 48 }, true };
			auto closed = m_closed;
			dialog.onClose = [closed](bool) { *closed = true; };
			m_eventDispatcher.ska::Observable<DialogEvent>::notifyObservers(dialog);
			return true;
		}
		// Until the player closes the dialog
		return !*m_closed;
	}),
	m_loadState(0),
	m_trainerId(trainerId),
	m_message(message),
	m_entityManager(em),
	m_eventDispatcher(ged) {
}
