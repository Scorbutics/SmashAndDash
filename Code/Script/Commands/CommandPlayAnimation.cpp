#include "CommandPlayAnimation.h"
#include "Utils\IDs.h"
#include <SDL2\SDL_rect.h>
#include "Gameplay\WGameCore.h"
#include "Utils\ScriptUtils.h"
#include "Utils\StringUtils.h"

CommandPlayAnimation::CommandPlayAnimation()
{
}


CommandPlayAnimation::~CommandPlayAnimation()
{
}

int CommandPlayAnimation::argumentsNumber() {
	return 3;
}

bool CommandPlayAnimation::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string idType, id;
	int id_anim;
	int idTypeInt, idInt;
	SDL_Rect posAnim;

	idType = args[0];
	id = args[1];
	id_anim = StringUtils::strToInt(args[2]);

	idTypeInt = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, idType, varMap);
	idInt = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, id, varMap);
	Character* c = wScreen.getEntityFactory().getNPC(idTypeInt, idInt);
	if (c != NULL)
	{
		posAnim = c->getHitboxCenterPos();
		posAnim.y -= c->getHeight();
		posAnim.x -= TAILLEBLOC / 2;
		wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, posAnim, 2, 200);
	}
	return true;
}
