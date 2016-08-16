#include "CommandAddPokemon.h"
#include "../../ska/Utils\StringUtils.h"

CommandAddPokemon::CommandAddPokemon(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandAddPokemon::~CommandAddPokemon()
{
}

int CommandAddPokemon::argumentsNumber() {
	return 1;
}

std::string CommandAddPokemon::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {
	int id;
	//WGameCore& wScreen = WGameCore::getInstance();

	id = ska::StringUtils::strToInt(args[0]);

	//wScreen.getPokemonManager().add(id);
	return "";
}
