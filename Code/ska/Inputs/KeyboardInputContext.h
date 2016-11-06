#pragma once
#include "InputContext.h"

namespace ska {
	class KeyboardInputContext : public InputContext {
	public:
		KeyboardInputContext();

		virtual void queryActions(RawInputListener& ril, InputActionContainer& actions) override;
		virtual void queryRanges(RawInputListener& ril, InputRangeContainer& ranges) override;
		virtual void queryToggles(RawInputListener& ril, InputToggleContainer& toggles) override;
		virtual const std::wstring& queryText(RawInputListener& ril) override;

		virtual ~KeyboardInputContext();

	};
}

