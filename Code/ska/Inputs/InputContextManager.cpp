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
	for (InputContextPtr& it : m_contexts) {
		it->queryActions(m_ril, m_actions);
		it->queryRanges(m_ril, m_ranges);
	}
}

const ska::InputRangeContainer& ska::InputContextManager::getRanges() const {
	return m_ranges;
}

const ska::InputActionContainer& ska::InputContextManager::getActions() const {
	return m_actions;
}

ska::InputContextManager::~InputContextManager()
{
}
