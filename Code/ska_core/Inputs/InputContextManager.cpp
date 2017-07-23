#include "InputContextManager.h"
#include "../Data/Events/GameEventDispatcher.h"

/* Default InputContextManager settings.
   Contexts should be added depending of the current State in order to
   enable or disable inputs in specific State */
ska::InputContextManager::InputContextManager(RawInputListener& ril, GameEventDispatcher& ged) : 
	m_eventDispatcher(ged),
	m_ril(ril) {
	for (auto& c : m_contexts) {
		c.second->buildCodeMap();
	}
	m_ranges.resize(INPUT_MAX_RANGE);
}

void ska::InputContextManager::addContext(EnumContextManager ecm, InputContextPtr&& icp) {
	m_contexts[ecm] = std::move(icp);
	m_contexts[ecm]->buildCodeMap();

}

void ska::InputContextManager::disableContext(EnumContextManager ecm, bool disable) {
	if (disable && m_contexts.find(ecm) != m_contexts.end()) {
		m_disabledContexts[ecm] = std::move(m_contexts[ecm]);
		m_contexts.erase(ecm);
	} else if (!disable && m_disabledContexts.find(ecm) != m_disabledContexts.end()) {
		m_contexts[ecm] = std::move(m_disabledContexts[ecm]);
		m_disabledContexts.erase(ecm);
	}
}

void ska::InputContextManager::refresh() {
	m_ril.update();
	m_actions.reset();
	for(auto& r : m_ranges) {
	    r.x = 0;
        r.y = 0;
    }
	m_toggles.reset();
	for (auto& c : m_contexts) {
		c.second->queryActions(m_actions);
		c.second->queryRanges(m_ranges);
		c.second->queryToggles(m_toggles);
		m_textInput = c.second->queryText();
	}

	InputKeyEvent ike(*this);
	InputMouseEvent ime(*this);
	m_eventDispatcher.ska::Observable<ska::InputKeyEvent>::notifyObservers(ike);
	m_eventDispatcher.ska::Observable<ska::InputMouseEvent>::notifyObservers(ime);
}

const ska::InputRangeContainer& ska::InputContextManager::getRanges() const {
	return m_ranges;
}

const ska::InputActionContainer& ska::InputContextManager::getActions() const {
	return m_actions;
}

const ska::InputToggleContainer& ska::InputContextManager::getToggles() const {
	return m_toggles;
}

const std::wstring& ska::InputContextManager::getTextInput() const {
	return m_textInput;
}

ska::InputContextManager::~InputContextManager()
{
}
