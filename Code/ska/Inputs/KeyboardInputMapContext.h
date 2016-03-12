#pragma once
#include "InputContext.h"

namespace ska {
	class KeyboardInputMapContext : public InputContext {
	public:
		KeyboardInputMapContext();
		virtual ~KeyboardInputMapContext();

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap) = 0;
	};
}

