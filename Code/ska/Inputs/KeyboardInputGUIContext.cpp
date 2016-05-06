#include <SDL2/SDL.h>
#include "KeyboardInputGUIContext.h"


ska::KeyboardInputGUIContext::KeyboardInputGUIContext() {
}

void ska::KeyboardInputGUIContext::buildCodeMap(std::unordered_map<int, ska::InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = InputAction::Quit;
	codeMap[SDL_SCANCODE_RETURN] = InputAction::DoAction;

	toggles[SDL_SCANCODE_A] = InputToggle::MoveLeft;
	toggles[SDL_SCANCODE_W] = InputToggle::MoveUp;
	toggles[SDL_SCANCODE_S] = InputToggle::MoveDown;
	toggles[SDL_SCANCODE_D] = InputToggle::MoveRight;
	
	/* Mouse click */
	/* Left */
	codeMap[SDL_NUM_SCANCODES + 0] = InputAction::LClic;
	toggles[SDL_NUM_SCANCODES + 0] = InputToggle::MoveWindow;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = InputAction::RClic;
}

ska::KeyboardInputGUIContext::~KeyboardInputGUIContext() {
}
