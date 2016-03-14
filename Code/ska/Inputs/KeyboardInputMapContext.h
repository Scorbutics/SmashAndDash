#pragma once
#include "KeyboardInputContext.h"

namespace ska {
	class KeyboardInputMapContext : public KeyboardInputContext {
	public:
		KeyboardInputMapContext();
		virtual ~KeyboardInputMapContext();

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) override;
	};
}

