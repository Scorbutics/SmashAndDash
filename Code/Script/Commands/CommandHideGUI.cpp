#include "CommandHideGUI.h"
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"

CommandHideGUI::CommandHideGUI()
{
}


CommandHideGUI::~CommandHideGUI()
{
}

int CommandHideGUI::argumentsNumber() {
	return 1;
}

bool CommandHideGUI::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int b;
	WGameCore& wScreen = WGameCore::getInstance();

	b = StringUtils::strToInt(args[0]);

	wScreen.getGUI().getToolbar()->hide(b != 0);
	wScreen.getGUI().hide(b != 0);
	return true;
}
