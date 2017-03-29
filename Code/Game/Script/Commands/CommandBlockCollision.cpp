#include "CommandBlockCollision.h"
#include "Script/System/ScriptAutoSystem.h"
#include "Utils/NumberUtils.h"

CommandBlockCollision::CommandBlockCollision(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
}

int CommandBlockCollision::argumentsNumber() {
	return 2;
}

std::string CommandBlockCollision::execute(ska::ScriptComponent&, std::vector<std::string>&) {
    //const int blockId = ska::StringUtils::strToInt(args[0]);
	//const bool col = args[1] == "true";

	/* TODO prendre le World et modifier la propriété de collision du block d'id blockId */

	return "";
}

CommandBlockCollision::~CommandBlockCollision() {
}
