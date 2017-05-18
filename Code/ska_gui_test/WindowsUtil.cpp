#include <doctest.h>
#include "WindowsUtil.h"
#include "Core/Window.h"
#include <Data/Events/GameEventDispatcher.h>
#include "Core/BaseWindow.h"
#include "Inputs/InputContextManager.h"

ska::GameEventDispatcher& GetGED() {
	static ska::GameEventDispatcher ged;
	return ged;
}

ska::BaseWindow& GetWindow() {
	static ska::Window w("title", 100, 100);
	return w;
}

ska::InputContextManager& GetICM() {
	static ska::RawInputListener ril;
	static ska::InputContextManager playerICM(ril);
	return playerICM;
}