#include "CommandEnd.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Utils\StringUtils.h"
#include <iosfwd>
#include "../../Utils\IDs.h"
#include "../../Gameplay\WGameCore.h"

using namespace std;

CommandEnd::CommandEnd()
{
}


CommandEnd::~CommandEnd()
{
}

int CommandEnd::argumentsNumber() {
	return 1;
}

std::string CommandEnd::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) {
	WGameCore& wScreen = WGameCore::getInstance();
	string& tmpScritFileName = ("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"tmpscripts.data");
	scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << script->getExtendedName() << endl;
		scriptList.close();
	}

	script->stop();
	return args[0];
}
