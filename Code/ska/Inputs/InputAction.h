#pragma once

#include <bitset>

namespace ska {
	enum InputAction {
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Jump,
		DoAction,
		ShotSkill1,
		ShotSkill2,
		ShotSkill3,
		ShotSkill4,
		Run,
		Quit,
		UseObject1,
		UseObject2,
		UseObject3,
		UseObject4,
		UseObject5,
		UseObject6,
		MoveTo,
		PathfindTo,
		LClic,
		RClic
	};

#define INPUT_MAX_ACTION 22

	typedef std::bitset<INPUT_MAX_ACTION> InputActionContainer;
}