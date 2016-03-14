#pragma once

#include <bitset>

namespace ska {
	enum InputAction {
		Jump,
		DoAction,
		ShotSkill1,
		ShotSkill2,
		ShotSkill3,
		ShotSkill4,
		Quit,
		UseObject1,
		UseObject2,
		UseObject3,
		UseObject4,
		UseObject5,
		UseObject6,
		PathfindTo,
		LClic,
		RClic
	};

#define INPUT_MAX_ACTION 16

	typedef std::bitset<INPUT_MAX_ACTION> InputActionContainer;
}