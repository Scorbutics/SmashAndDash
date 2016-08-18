#include <string>
#include "CommandExpulse.h"
#include "../../ska/Exceptions/ScriptException.h"
#include "../../Utils\IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
#include "../../ska/Physic/MovementComponent.h"
#include "../../ska/AI/IADefinedMovementComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Physic/HitboxComponent.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Script/ScriptTriggerType.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Utils/PhysicUtils.h"
#include "../../ska/Utils/NumberUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/World/World.h"
CommandExpulse::CommandExpulse(const ska::World& w, ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager), 
m_world(w) {
}

int CommandExpulse::argumentsNumber() {
	return -1;
}

std::string CommandExpulse::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {
	
	if (args.size() < 1) {
		throw ska::ScriptException("This command needs at least 3 parameters");
	}

    const std::string& id = args[0];

	ska::EntityId internalEntity = script.parent->getEntityFromName(id);

	if (!m_entityManager.hasComponent<ska::MovementComponent>(internalEntity)) {
		throw ska::ScriptException("The targetted entity cannot move : " + id);
	}

	auto& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	auto& mc = m_entityManager.getComponent<ska::MovementComponent>(internalEntity);
	const ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);
	const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(pc, hc);

	ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, hc.width, hc.height };
	const ska::Point<int> offsetHitboxBlock = m_world.alignOnBlock(hitbox);
	const ska::Rectangle targetBlock = m_world.placeOnNearestPracticableBlock(hitbox, 1);
	ska::Point<int> vector;
	vector.x = targetBlock.x /*- offsetHitboxBlock.x*/ - (pc.x + hc.xOffset);
	vector.y = targetBlock.y /*- offsetHitboxBlock.y*/ - (pc.y + hc.yOffset);

	mc.vx = 0;
	mc.vy = 0;
	mc.ax = 0;
	mc.ay = 0;

	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;
	iamc.delay = -1;
	iamc.ghost = true;

	iamc.directions.push_back(vector);
	m_entityManager.addComponent<ska::IADefinedMovementComponent>(internalEntity, iamc);


	return "";
}

CommandExpulse::~CommandExpulse() {
}
