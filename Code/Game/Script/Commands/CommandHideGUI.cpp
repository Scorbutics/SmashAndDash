#include "CommandHideGUI.h"
#include "Utils/StringUtils.h"
#include "../../Graphic/GUI/ToolBar.h"

CommandHideGUI::CommandHideGUI(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandHideGUI::argumentsNumber() {
	return 1;
}

std::string CommandHideGUI::execute(ska::ScriptComponent&, ska::MemoryScript& memory, const std::vector<std::string>& )
{
	/*int b;
	//WGameCore& wScreen = WGameCore::getInstance();

	b = ska::StringUtils::strToInt(args[0]);*/

	//wScreen.getGUI().getToolbar()->hide(b != 0);
	//wScreen.getGUI().hide(b != 0);
	return "";
}
