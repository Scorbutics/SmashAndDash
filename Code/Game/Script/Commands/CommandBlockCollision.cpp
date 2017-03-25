#include <string>
#include "CommandBlockCollision.h"
#include "Exceptions/ScriptException.h"
#include "../../Utils/IDs.h"
#include "Script/System/ScriptAutoSystem.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "Utils/StringUtils.h"
#include "Utils/PhysicUtils.h"
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
