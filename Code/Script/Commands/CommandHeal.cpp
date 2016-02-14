#include "CommandHeal.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandHeal::CommandHeal()
{
}


CommandHeal::~CommandHeal()
{
}

int CommandHeal::argumentsNumber() {
	return 2;
}

bool CommandHeal::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int value;
	unsigned int index;
	
	index = StringUtils::strToInt(args[0]);
	value = StringUtils::strToInt(args[1]);
	
	if (index >= 0 && index < wScreen.getPokemonManager().getPokemonTeamSize()) {
		wScreen.getPokemonManager().getPokemon(index)->setHP(wScreen.getPokemonManager().getPokemon(index)->getHp() + value);
	}
	return true;
}
