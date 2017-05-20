#pragma once
#include "KeyboardInputContext.h"

namespace ska {
	class KeyboardInputGUIContext : public KeyboardInputContext {
	public:
		explicit KeyboardInputGUIContext(ska::RawInputListener& );
		virtual ~KeyboardInputGUIContext();

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap, std::unordered_map<int, InputToggle>& toggles) override;
	};
}

