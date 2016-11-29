#include "InputContextManager.h"
#include "KeyboardInputMapContext.h"
#include "KeyboardInputGUIContext.h"

/* Default InputContextManager settings.
   Contexts should be added depending of the current Scene in order to
   enable or disable inputs in specific Scene */
ska::InputContextManager::InputContextManager(ska::RawInputListener& ril) : m_ril(ril) {	
	for (auto& c : m_contexts) {
		c.second->buildCodeMap();
	}
	m_ranges.resize(INPUT_MAX_RANGE);
}

void ska::InputContextManager::addContext(EnumContextManager ecm, InputContextPtr& icp) {
	icp->buildCodeMap();
	m_contexts[ecm] = std::move(icp);
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
	InputRange empty;
	std::fill(m_ranges.begin(), m_ranges.end(), empty);
	m_toggles.reset();
	for (auto& c : m_contexts) {
		c.second->queryActions(m_ril, m_actions);
		c.second->queryRanges(m_ril, m_ranges);
		c.second->queryToggles(m_ril, m_toggles);
		m_textInput = c.second->queryText(m_ril);
	}
}

ska::InputContextManager ska::InputContextManager::instantiateEmpty(ska::InputContextManager& icm) {
	return ska::InputContextManager(icm.m_ril);
}

ska::InputContextManager::InputContextManager(const ska::InputContextManager& icm) : m_ril(icm.m_ril) {
	*this = icm;
}

void ska::InputContextManager::operator=(const InputContextManager& icm) {
	m_ril = icm.m_ril;
	m_ranges = icm.m_ranges;
	m_toggles = icm.m_toggles;
	m_textInput = icm.m_textInput;
	/* Cannot copy contexts : they are uniques */
	//m_contexts = icm.m_contexts;
	m_actions = icm.m_actions;
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
