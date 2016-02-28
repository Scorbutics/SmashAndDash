#include "CommandPlayAnimation.h"
#include "../../Utils\IDs.h"
#include <SDL2\SDL.h>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"
#include "../../Graphic/Rectangle.h"

CommandPlayAnimation::CommandPlayAnimation()
{
}


CommandPlayAnimation::~CommandPlayAnimation()
{
}

int CommandPlayAnimation::argumentsNumber() {
	return 3;
}

std::string CommandPlayAnimation::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string idType, id;
	int id_anim;
	int idTypeInt, idInt;
	ska::Rectangle posAnim;

	idType = args[0];
	id = args[1];
	id_anim = ska::StringUtils::strToInt(args[2]);

	idTypeInt = ska::StringUtils::strToInt(idType);
	idInt = ska::StringUtils::strToInt(id);
	Character* c = wScreen.getEntityFactory().getNPC(idTypeInt, idInt);
	if (c != NULL)
	{
		posAnim = c->getHitboxCenterPos();
		posAnim.y -= c->getHeight();
		posAnim.x -= TAILLEBLOC / 2;
		wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, posAnim, 2, 200);
	}
	return "";
}
