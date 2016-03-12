#include <SDL2/SDL.h>
#include "KeyboardInputMapContext.h"


ska::KeyboardInputMapContext::KeyboardInputMapContext() {
}

void ska::KeyboardInputMapContext::buildCodeMap(std::unordered_map<int, ska::InputAction>& codeMap) {
	
	codeMap[SDL_SCANCODE_ESCAPE] = InputAction::Quit;
	codeMap[SDL_SCANCODE_LSHIFT] = InputAction::Run;
	codeMap[SDL_SCANCODE_RETURN] = InputAction::DoAction;
	codeMap[SDL_SCANCODE_SPACE] = InputAction::Jump;

	codeMap[SDL_SCANCODE_A] = InputAction::MoveLeft;
	codeMap[SDL_SCANCODE_W] = InputAction::MoveUp;
	codeMap[SDL_SCANCODE_S] = InputAction::MoveDown;
	codeMap[SDL_SCANCODE_D] = InputAction::MoveRight;
	
	codeMap[SDL_SCANCODE_Z] = InputAction::UseObject;
	codeMap[SDL_SCANCODE_X] = InputAction::UseObject;
	codeMap[SDL_SCANCODE_C] = InputAction::UseObject;
	codeMap[SDL_SCANCODE_V] = InputAction::UseObject;
	codeMap[SDL_SCANCODE_B] = InputAction::UseObject;
	codeMap[SDL_SCANCODE_N] = InputAction::UseObject;

	codeMap[SDL_SCANCODE_1] = InputAction::ShotSkill;
	codeMap[SDL_SCANCODE_2] = InputAction::ShotSkill;
	codeMap[SDL_SCANCODE_3] = InputAction::ShotSkill;
	codeMap[SDL_SCANCODE_4] = InputAction::ShotSkill;
}

ska::KeyboardInputMapContext::~KeyboardInputMapContext() {
}
