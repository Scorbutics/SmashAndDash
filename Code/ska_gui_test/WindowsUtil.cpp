#include <doctest.h>
#include "WindowsUtil.h"
#include "Core/Window.h"
#include <Data/Events/GameEventDispatcher.h>
#include "Inputs/InputContextManager.h"
#include "InputContextTest.h"

ska::GameEventDispatcher& GetGED() {
	static ska::GameEventDispatcher ged;
	return ged;
}

ska::RawInputListener& GetRawInputListener() {
	static ska::RawInputListener ril;
	return ril;
}

InputContextTest* SetICT(InputContextTest* ict) {
	static InputContextTest* rIct = ict;
	return rIct;
}

InputContextTest* GetICT() {
    return SetICT(nullptr);
}

ska::InputContextManager& GetICM() {
	static ska::InputContextManager playerICM(GetRawInputListener(), GetGED());
	static auto initialized = false;
	if(!initialized) {
		initialized = true;
		auto ict = std::make_unique<InputContextTest>();
		SetICT(ict.get());
		playerICM.addContext(ska::CONTEXT_GUI, std::move(ict));
	}
	return playerICM;
}
