#include "CommandFollow.h"
#include "../../ska/Utils/StringUtils.h"

CommandFollow::CommandFollow(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandFollow::~CommandFollow()
{
}

int CommandFollow::argumentsNumber() {
	return 5;
}

std::string CommandFollow::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	int id, number, id_target, number_target, flag;

	id = ska::StringUtils::strToInt(args[0]);
	number = ska::StringUtils::strToInt(args[1]);
	id_target = ska::StringUtils::strToInt(args[2]);
	number_target = ska::StringUtils::strToInt(args[3]);
	flag = ska::StringUtils::strToInt(args[4]);

	/* TODO en m�me temps que IAPathSystem */

	//wScreen.getEntityFactory().getNPC(id, number)->followEntity(flag ? wScreen.getEntityFactory().getNPC(id_target, number_target) : NULL);
	return "";
}
