#pragma once
#include "KeyboardInputContext.h"

namespace ska {
	class KeyboardInputGUIContext : public KeyboardInputContext {
	public:
		KeyboardInputGUIContext();
		virtual ~KeyboardInputGUIContext();

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap) override;
	};
}

