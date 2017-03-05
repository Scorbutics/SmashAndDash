#include "CommandShop.h"
#include "../../Graphic/GUI/WindowShop.h"

CommandShop::CommandShop(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandShop::~CommandShop()
{
}

int CommandShop::argumentsNumber() {
	return 0;
}

std::string CommandShop::execute(ska::ScriptComponent&, std::vector<std::string>&)
{
	//WGameCore& wScreen = WGameCore::getInstance();
	/*wScreen.getGUI().getWindowShop()->reset();
	wScreen.getGUI().getWindowShop()->hide(false);
	wScreen.waitQuit(wScreen.getGUI().getWindowShop()->getShopBar());*/
	return "";
}
