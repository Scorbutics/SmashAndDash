#include "CommandDirection.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"


CommandDirection::CommandDirection(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandDirection::~CommandDirection()
{
}

int CommandDirection::argumentsNumber() {
	return 2;
}

std::string CommandDirection::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string idStr, dirStr;
	int dir, id;

	idStr = args[0];
	dirStr = args[1];

	id = ska::StringUtils::strToInt(idStr);
	dir = ska::StringUtils::strToInt(dirStr);

	//wScreen.getEntityFactory().getNPC(idType, id)->setDirection(dir);
	return "";
}
