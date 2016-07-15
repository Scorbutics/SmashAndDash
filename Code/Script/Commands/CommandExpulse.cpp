#include <string>
#include "CommandExpulse.h"
#include "../../ska/Exceptions/ScriptException.h"
#include "../../Utils\IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
#include "../../ska/Physic/MovementComponent.h"
#include "../../ska/Physic/WorldCollisionComponent.h"
#include "../../ska/AI/IADefinedMovementComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Physic/HitboxComponent.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Script/ScriptTriggerType.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Utils/PhysicUtils.h"
#include "../../ska/Utils/NumberUtils.h"
#include "../../ska/Utils/RectangleUtils.h"

CommandExpulse::CommandExpulse(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
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

	if (!m_entityManager.hasComponent<ska::WorldCollisionComponent>(internalEntity)) {
		return "";
	}

	m_entityManager.removeComponent<ska::WorldCollisionComponent>(internalEntity);
	const ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	const ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);
	const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(pc, hc);


	const ska::WorldCollisionComponent& wcc = m_entityManager.getComponent<ska::WorldCollisionComponent>(internalEntity);
	ska::Point<int> diffToMove;
	ska::Point<int> offsetPCWithCenterPos = ska::Point<int>(pc.x, pc.y) - centerPos;
	offsetPCWithCenterPos.x = ska::NumberUtils::absolute(offsetPCWithCenterPos.x);
	offsetPCWithCenterPos.y = ska::NumberUtils::absolute(offsetPCWithCenterPos.y);
	//TODO blockSize from World
	for (auto& b : wcc.blockColPosX) {
		const ska::Rectangle intersection = ska::RectangleUtils::intersect(ska::Rectangle{ b.x, b.y, 48, 48 }, ska::Rectangle{ pc.x + hc.xOffset, pc.y + hc.yOffset, hc.width, hc.height });
		
		diffToMove += ska::Point<int>(intersection.w /*+ (intersection.w < 0 ? -15 : 15)*/, intersection.h /*+ (intersection.h < 0 ? -15 : 15)*/);
	}
	/*for (auto& b : wcc.blockColPosY) {
		const ska::Rectangle intersection = ska::RectangleUtils::intersect(ska::Rectangle{ b.x, b.y, 48, 48 }, ska::Rectangle{ pc.x + hc.xOffset, pc.y + hc.yOffset, hc.width, hc.height });
		diffToMove += ska::Point<int>(intersection.w, intersection.h);
	}*/
	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;
	iamc.ghost = true;
	iamc.delay = -1;
	iamc.directions.push_back(diffToMove /*+ offsetPCWithCenterPos*/);
	m_entityManager.addComponent<ska::IADefinedMovementComponent>(internalEntity, iamc);

	return "";
}

CommandExpulse::~CommandExpulse() {
}
