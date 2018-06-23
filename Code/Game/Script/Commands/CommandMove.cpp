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
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Script/ScriptUtils.h"

CommandMove::CommandMove(ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int CommandMove::argumentsNumber() {
	return -1;
}

std::string CommandMove::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) {

	if (args.size() < 4) {
		throw ska::ScriptException("This command needs at least 4 parameters");
	}

    const auto& id = args[0];
	const auto dir = ska::StringUtils::strToInt(args[1]);
	const auto distance = ska::StringUtils::strToInt(args[2]);
	const auto ghost = ska::StringUtils::strToInt(args[3]) != 0;
	const auto internalEntity = ska::StringUtils::fromString<ska::EntityId>(id);

	if (!m_entityManager.hasComponent<ska::MovementComponent>(internalEntity)) {
		throw ska::ScriptException(("The targetted entity cannot move : " + id).c_str());
	}

	const auto& centerPos = ska::PositionComponent::getCenterPosition(m_entityManager.getComponent<ska::PositionComponent>(internalEntity),
																			m_entityManager.getComponent<ska::HitboxComponent>(internalEntity));

	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;

	const auto moveForce = ska::PhysicUtils::getMovement(dir, static_cast<float>(distance));
	iamc.delay = static_cast<unsigned int>(-1);
	m_entityManager.getComponent<ska::CollidableComponent>(internalEntity).ghost = ghost;

	/* if there is a script callback provided */
	const auto ssc = ska::ScriptUtils::instantiateScript(args, 4, script);
	if (ssc.has_value()) {
		iamc.callback = ssc.value();
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
