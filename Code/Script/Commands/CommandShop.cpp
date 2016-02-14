#include "CommandShop.h"
#include "Gameplay\WGameCore.h"


CommandShop::CommandShop()
{
}


CommandShop::~CommandShop()
{
}

int CommandShop::argumentsNumber() {
	return 0;
}

bool CommandShop::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	wScreen.getGUI().getWindowShop()->reset();
	wScreen.getGUI().getWindowShop()->hide(false);
	wScreen.waitQuit(wScreen.getGUI().getWindowShop()->getShopBar());
	return true;
}
