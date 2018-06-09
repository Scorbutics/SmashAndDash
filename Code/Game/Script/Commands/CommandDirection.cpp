#include "CommandDirection.h"
#include "Utils/StringUtils.h"
#include "Script/ScriptComponent.h"
#include "ECS/EntityManager.h"
#include "Script/System/ScriptAutoSystem.h"

CommandDirection::CommandDirection(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandDirection::~CommandDirection()
{
}

int CommandDirection::argumentsNumber() {
	return 2;
}

std::string CommandDirection::execute(ska::ScriptComponent& script, ska::MemoryScript& memoryScript, const std::vector<std::string>& args) {
	const std::string& idStr = args[0];
	const int dir = ska::StringUtils::strToInt(args[1]);

	ska::EntityId internalId = script.parent->getEntityFromName(idStr);
	//ska::DirectionalAnimationComponent& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(internalId);
	//dac.direction = dir;
	return "";
}
