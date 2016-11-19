#include <string>
#include "CommandJump.h"
#include "../../ska/Exceptions/ScriptException.h"
#include "../../Utils/IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
#include "../../ska/Physic/ForceComponent.h"
#include "../../ska/AI/IADefinedMovementComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Physic/HitboxComponent.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Script/ScriptTriggerType.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/PhysicUtils.h"
#include "../../ska/Utils/NumberUtils.h"


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
