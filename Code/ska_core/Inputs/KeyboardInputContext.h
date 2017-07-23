#pragma once
#include "InputContext.h"

namespace ska {
	class KeyboardInputContext : public InputContext {
	public:
		explicit KeyboardInputContext(RawInputListener& ril);

		virtual void queryActions(InputActionContainer& actions) override;
		virtual void queryRanges(InputRangeContainer& ranges) override;
		virtual void queryToggles(InputToggleContainer& toggles) override;
		virtual const ska::WindowInput& queryWindowData() override;
		virtual const std::wstring& queryText() override;

		virtual ~KeyboardInputContext();
	private:
		RawInputListener& m_ril;
	};
}

