#include <SDL2/SDL.h>
#include "KeyboardInputContext.h"


ska::KeyboardInputContext::KeyboardInputContext() {
}

void ska::KeyboardInputContext::queryRanges(RawInputListener& ril, InputRangeContainer& ranges)  {
	MouseInput& mouseKeys = ril.getMouseInput();
	ska::Rectangle lastPos = mouseKeys.getMouseLastPos();
	ska::Rectangle pos = mouseKeys.getMousePos();

	ranges[InputRangeType::MousePos] = InputRange( pos.x, pos.y );
	ranges[InputRangeType::LastMousePos] = InputRange( lastPos.x, lastPos.y );
}

void ska::KeyboardInputContext::queryActions(RawInputListener& ril, InputActionContainer& actions) {
	KeyInput& keys = ril.getKeyInput();
	MouseInput& mouseKeys = ril.getMouseInput();
	for (auto& it = m_codesMapper.begin(); it != m_codesMapper.end(); it++) {
		if (it->first >= SDL_NUM_SCANCODES &&
			(it->first == SDL_NUM_SCANCODES && mouseKeys.mouseClick(SDL_BUTTON_LEFT)
			|| it->first == SDL_NUM_SCANCODES + 1 && mouseKeys.mouseClick(SDL_BUTTON_RIGHT))) {
			actions[it->second] = true;
		}
		else if (keys.getKeyState(it->first) == 1) {
			actions[it->second] = true;
		}
	}
}

ska::KeyboardInputContext::~KeyboardInputContext() {
}
