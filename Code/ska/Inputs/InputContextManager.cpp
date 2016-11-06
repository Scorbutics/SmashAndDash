#include "InputContextManager.h"
#include "KeyboardInputMapContext.h"
#include "KeyboardInputGUIContext.h"

/* Default InputContextManager settings.
   Contexts should be added depending of the current Scene in order to
   enable or disable inputs in specific Scene */
ska::InputContextManager::InputContextManager(ska::RawInputListener& ril) : m_ril(ril) {	
	for (InputContextPtr& it : m_contexts) {
		it->buildCodeMap();
	}
	m_ranges.resize(INPUT_MAX_RANGE);
}

void ska::InputContextManager::addContext(InputContextPtr& icp) {
	m_contexts.push_back(std::move(icp));
	m_contexts[m_contexts.size() - 1]->buildCodeMap();
}

void ska::InputContextManager::refresh() {
	m_ril.update();
	m_actions.reset();
	InputRange empty;
	std::fill(m_ranges.begin(), m_ranges.end(), empty);
	m_toggles.reset();
	for (InputContextPtr& it : m_contexts) {
		it->queryActions(m_ril, m_actions);
		it->queryRanges(m_ril, m_ranges);
		it->queryToggles(m_ril, m_toggles);
		m_textInput = it->queryText(m_ril);
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
