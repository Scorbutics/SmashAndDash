#include "CommandShop.h"
#include "../../Gameplay\WGameCore.h"


CommandShop::CommandShop()
{
}


CommandShop::~CommandShop()
{
}

int CommandShop::argumentsNumber() {
	return 0;
}

std::string CommandShop::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	wScreen.getGUI().getWindowShop()->reset();
	wScreen.getGUI().getWindowShop()->hide(false);
	wScreen.waitQuit(wScreen.getGUI().getWindowShop()->getShopBar());
	return "";
}
