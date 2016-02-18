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

std::string CommandHeal::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int value;
	unsigned int index;
	
	index = StringUtils::strToInt(args[0]);
	value = StringUtils::strToInt(args[1]);
	
	if (index >= 0 && index < wScreen.getPokemonManager().getPokemonTeamSize()) {
		wScreen.getPokemonManager().getPokemon(index)->setHP(wScreen.getPokemonManager().getPokemon(index)->getHp() + value);
	}
	return "";
}
