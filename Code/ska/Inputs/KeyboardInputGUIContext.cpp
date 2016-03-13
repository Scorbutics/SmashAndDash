#include <SDL2/SDL.h>
#include "KeyboardInputGUIContext.h"


ska::KeyboardInputGUIContext::KeyboardInputGUIContext() {
}

void ska::KeyboardInputGUIContext::buildCodeMap(std::unordered_map<int, ska::InputAction>& codeMap) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = InputAction::Quit;
	codeMap[SDL_SCANCODE_RETURN] = InputAction::DoAction;

	codeMap[SDL_SCANCODE_A] = InputAction::MoveLeft;
	codeMap[SDL_SCANCODE_W] = InputAction::MoveUp;
	codeMap[SDL_SCANCODE_S] = InputAction::MoveDown;
	codeMap[SDL_SCANCODE_D] = InputAction::MoveRight;

	/* Mouse click */
	/* Left */
	codeMap[SDL_NUM_SCANCODES + 0] = InputAction::LClic;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = InputAction::RClic;
}

ska::KeyboardInputGUIContext::~KeyboardInputGUIContext() {
}
