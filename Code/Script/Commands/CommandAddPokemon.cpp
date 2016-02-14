#include "CommandAddPokemon.h"
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"

CommandAddPokemon::CommandAddPokemon()
{
}


CommandAddPokemon::~CommandAddPokemon()
{
}

int CommandAddPokemon::argumentsNumber() {
	return 1;
}

bool CommandAddPokemon::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int id;
	WGameCore& wScreen = WGameCore::getInstance();

	id = StringUtils::strToInt(args[0]);

	wScreen.getPokemonManager().add(id);
	return true;
}
