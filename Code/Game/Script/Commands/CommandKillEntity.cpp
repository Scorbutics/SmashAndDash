#include "CommandKillEntity.h"
#include "Utils/StringUtils.h"
#include "ECS/EntityManager.h"
#include "Script/ScriptComponent.h"
#include "Script/System/ScriptAutoSystem.h"
#include <string>

CommandKillEntity::CommandKillEntity(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandKillEntity::argumentsNumber() {
	return 1;
}

std::string CommandKillEntity::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args)
{
	const std::string& id = args[0];

	ska::EntityId internalEntity = ska::StringUtils::fromString<ska::EntityId>(id);
	m_entityManager.removeEntity(internalEntity);
	return "";
}
