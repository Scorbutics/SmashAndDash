#include <string>
#include "CommandMove.h"
#include "../../ska/Exceptions/ScriptException.h"
#include "../../Utils\IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../../ska/Physic/ForceComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Utils/PhysicUtils.h"
#include "../../ska/Utils/NumberUtils.h"

using namespace std;

CommandMove::CommandMove(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}

int CommandMove::argumentsNumber() {
	return 3;
}

std::string CommandMove::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	
    const std::string& id = args[0];
	const int dir = ska::StringUtils::strToInt(args[1]);
	const int speed = ska::StringUtils::strToInt(args[2]);

	ska::EntityId internalEntity = script.parent->getEntityFromName(id);

	if (!m_entityManager.hasComponent<ska::ForceComponent>(internalEntity)) {
		throw ska::ScriptException("The targetted entity cannot move : " + id);
	}

	ska::ForceComponent& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(internalEntity);

	const ska::Force moveForce = ska::PhysicUtils::getMovement(dir, speed*10.0);

	forceComponent.x += moveForce.getPower() * ska::NumberUtils::cosinus(moveForce.getAngle());
	forceComponent.y += moveForce.getPower() * ska::NumberUtils::sinus(moveForce.getAngle());

	/*for (unsigned int j = 0; j < dir.size(); j++)
	{
		for (int i = 0; i < (TAILLEBLOC + 0.5*speed) / speed; i += ((speed + 1) / 2))
		{
			wScreen.getEntityFactory().getNPC(idtype, id)->reset();
			wScreen.getEntityFactory().getNPC(idtype, id)->applyForce(dir[j] - '0', (float)(100 * speed));
			wScreen.getEntityFactory().getNPC(idtype, id)->setDirection(dir[j] - '0');
			wScreen.eventUpdate(true);
			wScreen.graphicUpdate();

			wScreen.flip();
			SDL_Delay(30);
		}
	}*/
	return "";
}

CommandMove::~CommandMove()
{
}
