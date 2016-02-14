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

bool CommandEnd::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string& tmpScritFileName = ("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"tmpscripts.data");
	scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << extendedName << endl;
		scriptList.close();
	}

	std::string returnValue;
	returnValue = args[0];

	*result = StringUtils::intToStr(ScriptUtils::getValueFromVarOrSwitchNumber(extendedName, returnValue, varMap));

	return false;
}
