#include <string>
#include "CommandJump.h"
#include "Exceptions/ScriptException.h"
#include "../../Utils/IDs.h"
#include "Script/System/ScriptAutoSystem.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "Utils/StringUtils.h"
#include "Utils/PhysicUtils.h"
#include "Utils/NumberUtils.h"


CommandJump::CommandJump(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
}

int CommandJump::argumentsNumber() {
	return 2;
}

std::string CommandJump::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {

    const std::string& id = args[0];
	const int power = ska::StringUtils::strToInt(args[1]);
	ska::EntityId internalEntity = script.parent->getEntityFromName(id);

	if (!m_entityManager.hasComponent<ska::ForceComponent>(internalEntity)) {
		throw ska::ScriptException("The targetted entity cannot jump : " + id);
	}

	ska::ForceComponent& fc = m_entityManager.getComponent<ska::ForceComponent>(internalEntity);
	fc.z += power;

	return "";
}

CommandJump::~CommandJump() {
}