#include "CommandPlayerPresence.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Utils/IDs.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/ECS/EntityManager.h"
#include "../../ska/Script/ScriptComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Physic/HitboxComponent.h"

bool operator==(const ska::Rectangle& lhs, const ska::Rectangle& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

int CommandPlayerPresence::argumentsNumber() {
	return 4;
}

std::string CommandPlayerPresence::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	std::string posFromX, posFromY, posToX, posToY;
	ska::Rectangle collisionRect;

	posFromX = args[0];
	posFromY = args[1];
	posToX = args[2];
	posToY = args[3];

	ska::EntityId internalEntity = script.getOrigin();
	if (!m_entityManager.hasComponent<ska::PositionComponent>(internalEntity) ||
		!m_entityManager.hasComponent<ska::HitboxComponent>(internalEntity)) {
		return "0";
	}

	ska::PositionComponent& positionComponent = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	ska::HitboxComponent& hitboxComponent = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);

	ska::Rectangle pos1, pos2;
	ska::Rectangle entityPos = { positionComponent.x + hitboxComponent.xOffset, positionComponent.y + hitboxComponent.yOffset, hitboxComponent.width, hitboxComponent.height};

	pos1.x = ska::StringUtils::strToInt(posFromX);
	pos1.y = ska::StringUtils::strToInt(posFromY);
	pos2.x = ska::StringUtils::strToInt(posToX);
	pos2.y = ska::StringUtils::strToInt(posToY);

	collisionRect.x = pos1.x > pos2.x ? pos2.x : pos1.x;
	collisionRect.y = pos1.y > pos2.y ? pos2.y : pos1.y;
	collisionRect.w = abs(pos1.x - pos2.x) + 48 - 1;
	collisionRect.h = abs(pos1.y - pos2.y) + 48 - 1;

	if (ska::RectangleUtils::collisionBoxABoxB(collisionRect, entityPos)) {
		return "1";
	}
	return "0";
}
