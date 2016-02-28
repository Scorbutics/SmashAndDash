#include "CommandPlayerPresence.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"
#include "../../Graphic/Rectangle.h"

using namespace std;

bool operator==(const ska::Rectangle& lhs, const ska::Rectangle& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

int CommandPlayerPresence::argumentsNumber() {
	return 4;
}

std::string CommandPlayerPresence::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string posFromX, posFromY, posToX, posToY;
	ska::Rectangle collisionRect;
	ska::Rectangle idPlayer;
	posFromX = args[0];
	posFromY = args[1];
	posToX = args[2];
	posToY = args[3];

	idPlayer.x = wScreen.getEntityFactory().getTrainer()->getID();
	idPlayer.y = wScreen.getEntityFactory().getTrainer()->getEntityNumber();
	idPlayer.w = 0;
	idPlayer.h = 0;

	ska::Rectangle pos1, pos2;
	
	pos1.x = StringUtils::strToInt(posFromX)*TAILLEBLOC;
	pos1.y = StringUtils::strToInt(posFromY)*TAILLEBLOC;
	pos2.x = StringUtils::strToInt(posToX)*TAILLEBLOC;
	pos2.y = StringUtils::strToInt(posToY)*TAILLEBLOC;
	collisionRect.x = pos1.x > pos2.x ? pos2.x : pos1.x;
	collisionRect.y = pos1.y > pos2.y ? pos2.y : pos1.y;
	collisionRect.w = abs(pos1.x - pos2.x) + TAILLEBLOC - 1;
	collisionRect.h = abs(pos1.y - pos2.y) + TAILLEBLOC - 1;
	
	vector<ska::Rectangle>& ids = wScreen.detectEntity(collisionRect);
	if (!ids.empty() && find(ids.begin(), ids.end(), idPlayer) != ids.end())
	{
		return "1";
	} else {
		return "0";
	}
}