#include "CommandAddCharacter.h"
#include "Exceptions/ScriptException.h"
#include "Utils/StringUtils.h"
#include "ECS/ECSDefines.h"
#include "ECS/EntityManager.h"
#include "Utils/NumberUtils.h"
#include "../../Gameplay/CustomEntityManager.h"
#include "Script/ScriptComponent.h"
#include "Script/ScriptUtils.h"

CommandAddCharacter::CommandAddCharacter(CustomEntityManager& entityManager, ska::cp::Space& space, const unsigned int blockSize) :
	AbstractFunctionCommand(entityManager),
	m_customEntityManager(entityManager),
	m_space(space),
	m_blockSize(blockSize) {
}

std::string CommandAddCharacter::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) {
	if (args.size() < 4) {
		throw ska::ScriptException("This command needs at least 4 parameters (id, pos x, pos y, name)");
	}

	const auto& idGamePlay = ska::StringUtils::fromString<int>(args[0]);
	const auto& posX = ska::StringUtils::fromString<int>(args[1]);
	const auto& posY = ska::StringUtils::fromString<int>(args[2]);
	const auto& name = args[3];
	
	auto character = CustomEntityManager::createCharacterNG(m_customEntityManager, m_space, ska::Point<int>{ posX, posY }, idGamePlay, m_blockSize, name );

	auto sscOpt = ska::ScriptUtils::instantiateScript(args, 4, script);
	if (sscOpt.has_value()) {
		auto& ssc = *sscOpt;
		//ssc.context = m_world.getName();
		ssc.triggeringType = static_cast<ska::ScriptTriggerType>(ska::ScriptTriggerType::ACTION);
		ssc.period = 1000;
		m_entityManager.addComponent<ska::ScriptSleepComponent>(character, std::move(ssc));
	}

	return "";
}

int CommandAddCharacter::argumentsNumber() {
	return -1;
}
