#include "CommandPlayerPresence.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ScriptUtils.h"

using namespace std;

bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

int CommandPlayerPresence::argumentsNumber() {
	return 4;
}

bool CommandPlayerPresence::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string posFromX, posFromY, posToX, posToY;
	SDL_Rect collisionRect;
	SDL_Rect idPlayer;
	posFromX = args[0];
	posFromY = args[1];
	posToX = args[2];
	posToY = args[3];

	idPlayer.x = wScreen.getEntityFactory().getTrainer()->getID();
	idPlayer.y = wScreen.getEntityFactory().getTrainer()->getEntityNumber();
	idPlayer.w = 0;
	idPlayer.h = 0;

	SDL_Rect pos1, pos2;
	
	pos1.x = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, posFromX, varMap)*TAILLEBLOC;
	pos1.y = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, posFromY, varMap)*TAILLEBLOC;
	pos2.x = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, posToX, varMap)*TAILLEBLOC;
	pos2.y = ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, posToY, varMap)*TAILLEBLOC;
	collisionRect.x = pos1.x > pos2.x ? pos2.x : pos1.x;
	collisionRect.y = pos1.y > pos2.y ? pos2.y : pos1.y;
	collisionRect.w = abs(pos1.x - pos2.x) + TAILLEBLOC - 1;
	collisionRect.h = abs(pos1.y - pos2.y) + TAILLEBLOC - 1;
	
	vector<SDL_Rect>& ids = wScreen.detectEntity(collisionRect);
	if (!ids.empty() && find(ids.begin(), ids.end(), idPlayer) != ids.end())
	{
		*result = "1";
	} else {
		*result = "0";
	}
	return true;
}