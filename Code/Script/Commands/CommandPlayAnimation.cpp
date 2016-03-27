#include "CommandPlayAnimation.h"
#include "../../Utils\IDs.h"
#include "../../ska/Graphic/SpritePath.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\ScriptUtils.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Graphic/Rectangle.h"

CommandPlayAnimation::CommandPlayAnimation(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandPlayAnimation::~CommandPlayAnimation()
{
}

int CommandPlayAnimation::argumentsNumber() {
	return 2;
}

std::string CommandPlayAnimation::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	

	const std::string& id = args[0];
	const ska::EntityId internalEntity = script.parent->getEntityFromName(id);

	if (m_entityManager.hasComponent<ska::PositionComponent>(internalEntity) &&
		m_entityManager.hasComponent<ska::HitboxComponent>(internalEntity) &&
		m_entityManager.hasComponent<ska::GraphicComponent>(internalEntity)) {

		WGameCore& wScreen = WGameCore::getInstance();
		ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
		ska::HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(internalEntity);
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(internalEntity);
		
		const int id_anim = ska::StringUtils::strToInt(args[1]);
		ska::EntityId animation = m_entityManager.createEntity();
		ska::PositionComponent pcANim;
		pcANim.x = pc.x;
		pcANim.y = (int)(pc.y + hc.yOffset - gc.sprite.getHeight());
		m_entityManager.addComponent<ska::PositionComponent>(animation, pcANim);
		ska::GraphicComponent gcAnim;
		gcAnim.sprite.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, id_anim), 4, 1, 4);
		gcAnim.sprite.setDelay(300);
		m_entityManager.addComponent<ska::GraphicComponent>(animation, gcAnim);
		ska::DeleterComponent dc;
		dc.delay = 2100;
		m_entityManager.addComponent<ska::DeleterComponent>(animation, dc);
		//wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, posAnim, 2, 200);
	}
	return "";
}
