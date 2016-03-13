#pragma once
#include "InputContext.h"

namespace ska {
	class KeyboardInputMapContext : public InputContext {
	public:
		KeyboardInputMapContext();

		virtual void queryActions(RawInputListener& ril, InputActionContainer& actions) override;
		virtual void queryRanges(RawInputListener& ril, InputRangeContainer& ranges) override;

		virtual ~KeyboardInputMapContext();

	protected:
		virtual void buildCodeMap(std::unordered_map<int, InputAction>& codeMap) override;
	};
}

