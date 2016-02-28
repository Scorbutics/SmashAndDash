#include "CommandHideGUI.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandHideGUI::CommandHideGUI()
{
}


CommandHideGUI::~CommandHideGUI()
{
}

int CommandHideGUI::argumentsNumber() {
	return 1;
}

std::string CommandHideGUI::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	int b;
	WGameCore& wScreen = WGameCore::getInstance();

	b = ska::StringUtils::strToInt(args[0]);

	wScreen.getGUI().getToolbar()->hide(b != 0);
	wScreen.getGUI().hide(b != 0);
	return "";
}
