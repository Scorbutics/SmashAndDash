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

bool CommandStop::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	scriptList.open(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + wScreen.getSavegameManager().getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::app);
	scriptList << extendedName << endl;
	scriptList.close();
	return false;
}
