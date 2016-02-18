#include "CommandAddPokemon.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandAddPokemon::CommandAddPokemon()
{
}


CommandAddPokemon::~CommandAddPokemon()
{
}

int CommandAddPokemon::argumentsNumber() {
	return 1;
}

std::string CommandAddPokemon::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	int id;
	WGameCore& wScreen = WGameCore::getInstance();

	id = StringUtils::strToInt(args[0]);

	wScreen.getPokemonManager().add(id);
	return "";
}
