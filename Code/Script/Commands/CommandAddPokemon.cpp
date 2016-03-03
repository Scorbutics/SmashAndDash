#include "CommandAddPokemon.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"

CommandAddPokemon::CommandAddPokemon()
{
}


CommandAddPokemon::~CommandAddPokemon()
{
}

int CommandAddPokemon::argumentsNumber() {
	return 1;
}

std::string CommandAddPokemon::execute(ska::IScript* script, std::vector<std::string>& args) {
	int id;
	WGameCore& wScreen = WGameCore::getInstance();

	id = ska::StringUtils::strToInt(args[0]);

	wScreen.getPokemonManager().add(id);
	return "";
}
