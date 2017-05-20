#include <SDL.h>
#include "KeyboardInputGUIContext.h"


ska::KeyboardInputGUIContext::KeyboardInputGUIContext(ska::RawInputListener& ril) : ska::KeyboardInputContext(ril) {
}

void ska::KeyboardInputGUIContext::buildCodeMap(std::unordered_map<int, InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = Quit;
	codeMap[SDL_SCANCODE_RETURN] = DoAction;

	codeMap[SDL_SCANCODE_BACKSPACE] = DeleteChar;

	/* Mouse click */
	/* Left */
	codeMap[SDL_NUM_SCANCODES + 0] = LClic;
	toggles[SDL_NUM_SCANCODES + 0] = MoveWindow;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = RClic;
}

ska::KeyboardInputGUIContext::~KeyboardInputGUIContext() {
}
