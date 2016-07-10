#include "CommandBlockAuthorize.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"
#include "../../Physic/CollidableComponent.h"
#include "../System/ScriptAutoSystem.h"
#include "../../Exceptions/ScriptException.h"

ska::CommandBlockAuthorize::CommandBlockAuthorize(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandBlockAuthorize::~CommandBlockAuthorize()
{
}

int ska::CommandBlockAuthorize::argumentsNumber() {
	return 3;
}

std::string ska::CommandBlockAuthorize::execute(ScriptComponent& script, std::vector<std::string>& args) {
	int blockId = ska::StringUtils::strToInt(args[0]);
	int entity = ska::StringUtils::strToInt(args[1]);
	bool unauth = args[2] != "true";

	if (!m_entityManager.hasComponent<CollidableComponent>(entity)) {
		//TODO Mess
		throw ska::ScriptException("");
	}

	CollidableComponent& cc = m_entityManager.getComponent<CollidableComponent>(entity);

	auto& it = cc.authorizedBlockIds.find(entity);
	if (unauth && it != cc.authorizedBlockIds.end()) {
		cc.authorizedBlockIds.erase(it);
	} else {
		cc.authorizedBlockIds.emplace(blockId);
	}

	return "";
}
