#include "CommandKillEntity.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/ECS/EntityManager.h"
#include "../../ska/Script/ScriptComponent.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
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

std::string CommandKillEntity::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	const std::string& id = args[0];

	ska::EntityId internalEntity =  script.parent->getEntityFromName(id);
	m_entityManager.removeEntity(internalEntity);
	return "";
}
