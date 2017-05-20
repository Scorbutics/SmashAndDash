#include <SDL.h>
#include "KeyboardInputMapContext.h"


ska::KeyboardInputMapContext::KeyboardInputMapContext(ska::RawInputListener& ril) : KeyboardInputContext(ril) {
}

void ska::KeyboardInputMapContext::buildCodeMap(std::unordered_map<int, InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = Quit;
	toggles[SDL_SCANCODE_LSHIFT] = Run;
	codeMap[SDL_SCANCODE_RETURN] = DoAction;
	codeMap[SDL_SCANCODE_SPACE] = Jump;

	toggles[SDL_SCANCODE_A] = MoveLeft;
	toggles[SDL_SCANCODE_W] = MoveUp;
	toggles[SDL_SCANCODE_S] = MoveDown;
	toggles[SDL_SCANCODE_D] = MoveRight;
	
	codeMap[SDL_SCANCODE_Z] = UseObject1;
	codeMap[SDL_SCANCODE_X] = UseObject2;
	codeMap[SDL_SCANCODE_C] = UseObject3;
	codeMap[SDL_SCANCODE_V] = UseObject4;
	codeMap[SDL_SCANCODE_B] = UseObject5;
	codeMap[SDL_SCANCODE_N] = UseObject6;

	codeMap[SDL_SCANCODE_1] = ShotSkill1;
	codeMap[SDL_SCANCODE_2] = ShotSkill2;
	codeMap[SDL_SCANCODE_3] = ShotSkill3;
	codeMap[SDL_SCANCODE_4] = ShotSkill4;


	/* Mouse click */
	/* Left */
	toggles[SDL_NUM_SCANCODES + 0] = MoveTo;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = PathfindTo;
}

ska::KeyboardInputMapContext::~KeyboardInputMapContext() {
}
