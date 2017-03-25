#include "CommandRemoveComponent.h"
#include "../ScriptUtils.h"
#include "Utils/StringUtils.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandRemoveComponent::CommandRemoveComponent(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandRemoveComponent::~CommandRemoveComponent()
{
}

int ska::CommandRemoveComponent::argumentsNumber() {
	return 2;
}

std::string ska::CommandRemoveComponent::execute(ScriptComponent& script, std::vector<std::string>& args) {
	const std::string& componentName = args[0];
	const std::string& entity = args[1];

	script.parent->removeComponent(componentName, entity);

	return "";
}
