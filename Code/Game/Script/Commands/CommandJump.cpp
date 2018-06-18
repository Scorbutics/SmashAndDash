#include "CommandJump.h"
#include "Exceptions/ScriptException.h"
#include "Script/System/ScriptAutoSystem.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "Utils/StringUtils.h"
#include "Utils/NumberUtils.h"
#include <ECS/Basics/Physic/MovementComponent.h>


CommandJump::CommandJump(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandJump::argumentsNumber() {
	return 2;
}

std::string CommandJump::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) {

    const auto& id = args[0];
	const auto power = ska::StringUtils::strToInt(args[1]);
	auto internalEntity = ska::StringUtils::fromString<ska::EntityId>(id);

	if (!m_entityManager.hasComponent<ska::ForceComponent>(internalEntity)) {
		throw ska::ScriptException(("The targetted entity cannot jump : " + id).c_str());
	}

	auto& mc = m_entityManager.getComponent<ska::MovementComponent>(internalEntity);
	mc.vz = power;

	return "";
}
