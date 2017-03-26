#include "CommandRestoreComponent.h"
#include "../ScriptUtils.h"
#include "Utils/StringUtils.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandRestoreComponent::CommandRestoreComponent(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandRestoreComponent::~CommandRestoreComponent()
{
}

int ska::CommandRestoreComponent::argumentsNumber() {
	return 2;
}

std::string ska::CommandRestoreComponent::execute(ScriptComponent& script, std::vector<std::string>& args) {
	const std::string& componentName = args[0];
	const std::string& entity = args[1];

	script.parent->restoreComponent(componentName, entity);

	return "";
}
