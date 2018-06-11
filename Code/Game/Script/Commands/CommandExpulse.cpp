#include <string>
#include "CommandExpulse.h"
#include "Exceptions/ScriptException.h"
#include "Script/System/ScriptAutoSystem.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "AI/IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "Utils/RectangleUtils.h"
#include "World/TileWorld.h"
#include "ECS/Basics/Physic/CollidableComponent.h"

CommandExpulse::CommandExpulse(const ska::TileWorld& w, ska::EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager),
	m_world(w) {
}

int CommandExpulse::argumentsNumber() {
	return -1;
}

std::string CommandExpulse::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		throw ska::ScriptException("This command needs at least 2 parameters");
	}

    const auto& id = args[0];
	const auto& speed = ska::StringUtils::strToFloat(args[1]);
	const auto internalEntity = ska::StringUtils::fromString<ska::EntityId>(id);

	if (!m_entityManager.hasComponent<ska::MovementComponent>(internalEntity)) {
		throw ska::ScriptException(("The targetted entity cannot move : " + id).c_str());
	}

	auto& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	auto& mc = m_entityManager.getComponent<ska::MovementComponent>(internalEntity);
	const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);
	const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(pc, hc);

	const ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };
	const auto targetBlock = m_world.getCollisionProfile().placeOnNearestPracticableBlock(pc.layer, hitbox, 1);
	ska::Point<int> vector;
	vector.x = targetBlock.x - (pc.x + hc.xOffset);
	vector.y = targetBlock.y - (pc.y + hc.yOffset);

	mc.vx = 0;
	mc.vy = 0;
	mc.ax = 0;
	mc.ay = 0;

	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;
	iamc.speed = static_cast<float>(speed);
	iamc.delay = static_cast<unsigned int>(-1);
	m_entityManager.getComponent<ska::CollidableComponent>(internalEntity).ghost = true;

	iamc.directions.push_back(vector);
	m_entityManager.addComponent<ska::IADefinedMovementComponent>(internalEntity, std::move(iamc));


	return "";
}

CommandExpulse::~CommandExpulse() {
}
