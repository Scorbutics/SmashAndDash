#include <string>
#include "CommandMove.h"
#include "Utils\IDs.h"
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"
using namespace std;

CommandMove::CommandMove()
{
}

int CommandMove::argumentsNumber() {
	return 4;
}

bool CommandMove::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int idtype, id, speed;
	string dir;
	WGameCore& wScreen = WGameCore::getInstance();

	idtype = StringUtils::strToInt(args[0]);
	id = StringUtils::strToInt(args[1]);
	dir = args[2];
	speed = StringUtils::strToInt(args[3]);

	for (unsigned int j = 0; j < dir.size(); j++)
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
	}
	return true;
}

CommandMove::~CommandMove()
{
}
