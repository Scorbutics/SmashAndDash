#include <string>
#include "CommandMove.h"
#include "Exceptions/ScriptException.h"
#include "../../Utils/IDs.h"
#include "Script/System/ScriptAutoSystem.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "Utils/StringUtils.h"
#include "Utils/PhysicUtils.h"
#include "Utils/NumberUtils.h"


CommandMove::CommandMove(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
}

int CommandMove::argumentsNumber() {
	return -1;
}

std::string CommandMove::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {

	if (args.size() < 4) {
		throw ska::ScriptException("This command needs at least 4 parameters");
	}

    const auto& id = args[0];
	const auto dir = ska::StringUtils::strToInt(args[1]);
	const auto speed = ska::StringUtils::strToInt(args[2]);
	const auto ghost = ska::StringUtils::strToInt(args[3]) != 0;
	auto internalEntity = script.parent->getEntityFromName(id);

	if (!m_entityManager.hasComponent<ska::MovementComponent>(internalEntity)) {
		throw ska::ScriptException("The targetted entity cannot move : " + id);
	}

	const auto& centerPos = ska::PositionComponent::getCenterPosition(m_entityManager.getComponent<ska::PositionComponent>(internalEntity),
																			m_entityManager.getComponent<ska::HitboxComponent>(internalEntity));

	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;

	const auto moveForce = ska::PhysicUtils::getMovement(dir, static_cast<float>(speed));
	iamc.delay = static_cast<unsigned int>(-1);
	iamc.ghost = ghost;

	/* if there is a script callback provided */
	if (args.size() > 4) {
		std::vector<std::string> extraArgs;

		const auto& scriptName = args[4];
		/* Rebuild an argument string to be read by the new running script */
		for (unsigned int i = 5; i < args.size(); i++) {
			extraArgs.push_back(args[i]);
		}
		ska::ScriptSleepComponent ssc;
		ssc.context = script.context;
		ssc.args = extraArgs;
		ssc.period = 2000;
		ssc.deleteEntityWhenFinished = true;
		ssc.triggeringType = ska::EnumScriptTriggerType::AUTO;
		ssc.name = scriptName;
		iamc.callback = ssc;
		iamc.callbackActive = true;
	}
	auto& mc = m_entityManager.getComponent<ska::MovementComponent>(internalEntity);
	mc.vx = 0;
	mc.vy = 0;
	mc.ax = 0;
	mc.ay = 0;
	const auto vector = ska::Point<int>::cartesian(moveForce.power, moveForce.angle);
	iamc.directions.push_back(vector);
	m_entityManager.addComponent<ska::IADefinedMovementComponent>(internalEntity, std::move(iamc));

	return "";
}

CommandMove::~CommandMove() {
}
