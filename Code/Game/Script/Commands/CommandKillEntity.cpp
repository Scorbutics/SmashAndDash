#include "CommandKillEntity.h"
#include "Utils/StringUtils.h"
#include "ECS/EntityManager.h"
#include "Script/ScriptComponent.h"
#include "Script/System/ScriptAutoSystem.h"
#include <string>

CommandKillEntity::CommandKillEntity(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandKillEntity::~CommandKillEntity()
{
}

int CommandKillEntity::argumentsNumber() {
	return 1;
}

std::string CommandKillEntity::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args)
{
	const std::string& id = args[0];

	ska::EntityId internalEntity =  script.parent->getEntityFromName(id);
	m_entityManager.removeEntity(internalEntity);
	return "";
}
