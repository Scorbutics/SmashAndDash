#include <SDL.h>
#include "KeyboardInputContext.h"


ska::KeyboardInputContext::KeyboardInputContext(RawInputListener& ril) : m_ril(ril) {
}

void ska::KeyboardInputContext::queryRanges(InputRangeContainer& ranges)  {
	auto& mouseKeys = m_ril.getMouseInput();
	auto& lastPos = mouseKeys.getMouseLastPos();
	auto& pos = mouseKeys.getMousePos();

	ranges[MousePos] = InputRange(static_cast<float>(pos.x), static_cast<float>(pos.y));
	ranges[LastMousePos] = InputRange(static_cast<float>(lastPos.x), static_cast<float>(lastPos.y));
}

void ska::KeyboardInputContext::queryActions(InputActionContainer& actions) {
	auto& keys = m_ril.getKeyInput();
	auto& mouseKeys = m_ril.getMouseInput();
	for (auto it = m_actionsMapper.begin(); it != m_actionsMapper.end(); ++it) {
		if (((it->first >= SDL_NUM_SCANCODES &&
			(it->first == SDL_NUM_SCANCODES && mouseKeys.trigger(SDL_BUTTON_LEFT)))
			|| (it->first == SDL_NUM_SCANCODES + 1 && mouseKeys.trigger(SDL_BUTTON_RIGHT)))) {
			actions[it->second] = true;
		} else if (it->first < SDL_NUM_SCANCODES && keys.trigger(it->first)) {
			actions[it->second] = true;
		}
	}
}

void ska::KeyboardInputContext::queryToggles(InputToggleContainer& toggles) {
	auto& keys = m_ril.getKeyInput();
	auto& mouseKeys = m_ril.getMouseInput();
	for (auto it = m_togglesMapper.begin(); it != m_togglesMapper.end(); ++it) {
		if (it->first >= SDL_NUM_SCANCODES &&
			((it->first == SDL_NUM_SCANCODES && mouseKeys.toggle(SDL_BUTTON_LEFT))
			|| (it->first == SDL_NUM_SCANCODES + 1 && mouseKeys.toggle(SDL_BUTTON_RIGHT)))) {
			toggles[it->second] = true;
		} else if (it->first < SDL_NUM_SCANCODES && keys.toggle(it->first)) {
			toggles[it->second] = true;
		}
	}
}

const ska::WindowInput& ska::KeyboardInputContext::queryWindowData() {
	return m_ril.getWindowInput();
}

const std::wstring& ska::KeyboardInputContext::queryText() {
	return m_ril.getTextInput();
}

ska::KeyboardInputContext::~KeyboardInputContext() {
}
