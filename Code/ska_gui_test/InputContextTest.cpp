#include "InputContextTest.h"


void InputContextTest::queryActions(ska::InputActionContainer& actions) {
	for (auto it = m_actionsMapper.begin(); it != m_actionsMapper.end(); ++it) {
		if (it->first < SDL_NUM_SCANCODES) {
			actions[it->second] = keys.trigger(it->first);
		}
	}
	actions[ska::LClic] = mouseKeys.trigger(0);
	actions[ska::RClic] = mouseKeys.trigger(1);
}

void InputContextTest::queryRanges(ska::InputRangeContainer& ranges) {
	auto lastPos = mouseKeys.getMouseLastPos();
	auto pos = mouseKeys.getMousePos();

	ranges[ska::MousePos] = ska::InputRange(static_cast<float>(pos.x), static_cast<float>(pos.y));
	ranges[ska::LastMousePos] = ska::InputRange(static_cast<float>(lastPos.x), static_cast<float>(lastPos.y));
}

void InputContextTest::queryToggles(ska::InputToggleContainer& toggles) {
	for (auto it = m_togglesMapper.begin(); it != m_togglesMapper.end(); ++it) {
		if (it->first < SDL_NUM_SCANCODES) {
			toggles[it->second] = keys.toggle(it->first);
		}
	}
	toggles[ska::MoveWindow] = mouseKeys.toggle(0);
}

const ska::WindowInput & InputContextTest::queryWindowData() {
	return wi;
}

void InputContextTest::buildCodeMap(std::unordered_map< int, ska::InputAction>& codeMap, std::unordered_map<int, ska::InputToggle>& toggles) {

	codeMap[SDL_SCANCODE_ESCAPE] = ska::Quit;
	codeMap[SDL_SCANCODE_RETURN] = ska::DoAction;

	codeMap[SDL_SCANCODE_BACKSPACE] = ska::DeleteChar;

	/* Mouse click */
	/* Left */
	codeMap[SDL_NUM_SCANCODES + 0] = ska::LClic;
	toggles[SDL_NUM_SCANCODES + 0] = ska::MoveWindow;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = ska::RClic;

}
