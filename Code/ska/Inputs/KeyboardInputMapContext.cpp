#include <SDL2/SDL.h>
#include "KeyboardInputMapContext.h"


ska::KeyboardInputMapContext::KeyboardInputMapContext() {
}

void ska::KeyboardInputMapContext::buildCodeMap(std::unordered_map<int, ska::InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = InputAction::Quit;
	toggles[SDL_SCANCODE_LSHIFT] = InputToggle::Run;
	codeMap[SDL_SCANCODE_RETURN] = InputAction::DoAction;
	codeMap[SDL_SCANCODE_SPACE] = InputAction::Jump;

	toggles[SDL_SCANCODE_A] = InputToggle::MoveLeft;
	toggles[SDL_SCANCODE_W] = InputToggle::MoveUp;
	toggles[SDL_SCANCODE_S] = InputToggle::MoveDown;
	toggles[SDL_SCANCODE_D] = InputToggle::MoveRight;
	
	codeMap[SDL_SCANCODE_Z] = InputAction::UseObject1;
	codeMap[SDL_SCANCODE_X] = InputAction::UseObject2;
	codeMap[SDL_SCANCODE_C] = InputAction::UseObject3;
	codeMap[SDL_SCANCODE_V] = InputAction::UseObject4;
	codeMap[SDL_SCANCODE_B] = InputAction::UseObject5;
	codeMap[SDL_SCANCODE_N] = InputAction::UseObject6;

	codeMap[SDL_SCANCODE_1] = InputAction::ShotSkill1;
	codeMap[SDL_SCANCODE_2] = InputAction::ShotSkill2;
	codeMap[SDL_SCANCODE_3] = InputAction::ShotSkill3;
	codeMap[SDL_SCANCODE_4] = InputAction::ShotSkill4;


	/* Mouse click */
	/* Left */
	toggles[SDL_NUM_SCANCODES + 0] = InputToggle::MoveTo;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = InputAction::PathfindTo;
}

ska::KeyboardInputMapContext::~KeyboardInputMapContext() {
}
