#include <string>
#include "CommandBlockCollision.h"
#include "../../ska/Exceptions/ScriptException.h"
#include "../../Utils\IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../../ska/Physic/ForceComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Utils/PhysicUtils.h"
#include "../../ska/Utils/NumberUtils.h"

using namespace std;

CommandBlockCollision::CommandBlockCollision(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
}

int CommandBlockCollision::argumentsNumber() {
	return 2;
}

std::string CommandBlockCollision::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {
    const int blockId = ska::StringUtils::strToInt(args[0]);
	const bool col = args[1] == "true";

	/* TODO prendre le World et modifier la propriété de collision du block d'id blockId */

	return "";
}

CommandBlockCollision::~CommandBlockCollision() {
}
