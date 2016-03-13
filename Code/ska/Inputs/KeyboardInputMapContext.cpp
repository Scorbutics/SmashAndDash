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
	codeMap[SDL_NUM_SCANCODES + 0] = InputAction::MoveTo;

	/* Right */
	codeMap[SDL_NUM_SCANCODES + 1] = InputAction::PathfindTo;
}

void ska::KeyboardInputMapContext::queryRanges(RawInputListener& ril, InputRangeContainer& ranges)  {
	MouseInput& mouseKeys = ril.getMouseInput();
	ska::Rectangle lastPos = mouseKeys.getMouseLastPos();
	ska::Rectangle pos = mouseKeys.getMousePos();

	ranges[InputRangeType::MousePos] = InputRange( pos.x, pos.y );
	ranges[InputRangeType::LastMousePos] = InputRange( lastPos.x, lastPos.y );
}

void ska::KeyboardInputMapContext::queryActions(RawInputListener& ril, InputActionContainer& actions) {
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

ska::KeyboardInputMapContext::~KeyboardInputMapContext() {
}
