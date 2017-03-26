#include "CommandAddPokemon.h"
#include "Utils/StringUtils.h"

CommandAddPokemon::CommandAddPokemon(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandAddPokemon::~CommandAddPokemon()
{
}

int CommandAddPokemon::argumentsNumber() {
	return 1;
}

std::string CommandAddPokemon::execute(ska::ScriptComponent&, std::vector<std::string>& ) {
	/*int id;
	//WGameCore& wScreen = WGameCore::getInstance();

	id = ska::StringUtils::strToInt(args[0]);*/

	//wScreen.getPokemonManager().add(id);
	return "";
}
