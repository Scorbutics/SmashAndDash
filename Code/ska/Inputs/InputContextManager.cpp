#include "InputContextManager.h"
#include "KeyboardInputMapContext.h"
#include "KeyboardInputGUIContext.h"

ska::InputContextManager::InputContextManager(ska::RawInputListener& ril) : m_ril(ril) {
	m_contexts.push_back(InputContextPtr(new ska::KeyboardInputMapContext()));
	m_contexts.push_back(InputContextPtr(new ska::KeyboardInputGUIContext()));
	
	for (InputContextPtr& it : m_contexts) {
		it->buildCodeMap();
	}
	m_ranges.resize(INPUT_MAX_RANGE);
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
	}
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

ska::InputContextManager::~InputContextManager()
{
}
