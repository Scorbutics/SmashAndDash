#include <SDL2/SDL.h>
#include "KeyboardInputContext.h"


ska::KeyboardInputContext::KeyboardInputContext() {
}

void ska::KeyboardInputContext::queryRanges(RawInputListener& ril, InputRangeContainer& ranges)  {
	MouseInput& mouseKeys = ril.getMouseInput();
	ska::Point<int> lastPos = mouseKeys.getMouseLastPos();
	ska::Point<int> pos = mouseKeys.getMousePos();

	ranges[InputRangeType::MousePos] = InputRange( pos.x, pos.y );
	ranges[InputRangeType::LastMousePos] = InputRange( lastPos.x, lastPos.y );
}

void ska::KeyboardInputContext::queryActions(RawInputListener& ril, InputActionContainer& actions) {
	KeyInput& keys = ril.getKeyInput();
	MouseInput& mouseKeys = ril.getMouseInput();
	for (auto& it = m_actionsMapper.begin(); it != m_actionsMapper.end(); it++) {
		if (it->first >= SDL_NUM_SCANCODES &&
			(it->first == SDL_NUM_SCANCODES && mouseKeys.trigger(SDL_BUTTON_LEFT)
			|| it->first == SDL_NUM_SCANCODES + 1 && mouseKeys.trigger(SDL_BUTTON_RIGHT))) {
			actions[it->second] = true;
		} else if (it->first < SDL_NUM_SCANCODES && keys.trigger(it->first)) {
			actions[it->second] = true;
		}
	}
}

void ska::KeyboardInputContext::queryToggles(RawInputListener& ril, InputToggleContainer& toggles) {
	KeyInput& keys = ril.getKeyInput();
	MouseInput& mouseKeys = ril.getMouseInput();
	for (auto& it = m_togglesMapper.begin(); it != m_togglesMapper.end(); it++) {
		if (it->first >= SDL_NUM_SCANCODES &&
			(it->first == SDL_NUM_SCANCODES && mouseKeys.toggle(SDL_BUTTON_LEFT)
			|| it->first == SDL_NUM_SCANCODES + 1 && mouseKeys.toggle(SDL_BUTTON_RIGHT))) {
			toggles[it->second] = true;
		} else if (it->first < SDL_NUM_SCANCODES && keys.toggle(it->first)) {
			toggles[it->second] = true;
		}
	}
}

const std::wstring& ska::KeyboardInputContext::queryText(RawInputListener& ril) {
	return ril.getTextInput();
}

ska::KeyboardInputContext::~KeyboardInputContext() {
}
