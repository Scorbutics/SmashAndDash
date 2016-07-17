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
CommandExpulse::CommandExpulse(ska::World& w, ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager), 
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

	ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	const ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);
	const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(pc, hc);

	ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, hc.width, hc.height };
	const ska::Point<int> offsetHitboxBlock = m_world.alignOnBlock(hitbox);
	const ska::Rectangle targetBlock = m_world.placeOnNearestPracticableBlock(hitbox, 1);
	pc.x = targetBlock.x /*+ offsetHitboxBlock.x*/ - hc.xOffset; 
	pc.y = targetBlock.y /*+ offsetHitboxBlock.y*/ - hc.yOffset;

	return "";
}

CommandExpulse::~CommandExpulse() {
}
