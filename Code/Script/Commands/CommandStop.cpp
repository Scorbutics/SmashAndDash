#include "CommandStop.h"

#include "../../Gameplay\WGameCore.h"
#include "../../Utils\IDs.h"

using namespace std;

CommandStop::CommandStop()
{
}


CommandStop::~CommandStop()
{
}

int CommandStop::argumentsNumber() {
	return 0;
}

std::string CommandStop::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	scriptList.open(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::app);
	scriptList << script->getExtendedName() << endl;
	scriptList.close();
	return false;
}
