#include <string>
#include "CommandMove.h"
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


CommandMove::CommandMove(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager) {
}

int CommandMove::argumentsNumber() {
	return -1;
}

std::string CommandMove::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {
	
	if (args.size() < 3) {
		throw ska::ScriptException("This command needs at least 3 parameters");
	}

    const std::string& id = args[0];
	const int dir = ska::StringUtils::strToInt(args[1]);
	const int speed = ska::StringUtils::strToInt(args[2]);

	ska::EntityId internalEntity = script.parent->getEntityFromName(id);

	if (!m_entityManager.hasComponent<ska::MovementComponent>(internalEntity)) {
		throw ska::ScriptException("The targetted entity cannot move : " + id);
	}

	const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(m_entityManager.getComponent<ska::PositionComponent>(internalEntity), 
																			m_entityManager.getComponent<ska::HitboxComponent>(internalEntity));

	ska::IADefinedMovementComponent iamc;
	iamc.origin = centerPos;
	
	const ska::Force moveForce = ska::PhysicUtils::getMovement(dir, speed);
	iamc.delay = -1;
	
	/* if there is a script callback provided */
	if (args.size() > 3) {
		std::vector<std::string> extraArgs;

		const std::string& scriptName = args[3];
		/* Rebuild an argument string to be read by the new running script */
		for (unsigned int i = 4; i < args.size(); i++) {
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

	const ska::Point<int> vector = ska::NumberUtils::cartesian(moveForce.getPower(), moveForce.getAngle());
	iamc.directions.push_back(vector);
	m_entityManager.addComponent<ska::IADefinedMovementComponent>(internalEntity, iamc);

	return "";
}

CommandMove::~CommandMove() {
}
