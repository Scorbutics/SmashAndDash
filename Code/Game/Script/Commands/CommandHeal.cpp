#include "CommandHeal.h"
#include "Utils/StringUtils.h"

CommandHeal::CommandHeal(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandHeal::~CommandHeal()
{
}

int CommandHeal::argumentsNumber() {
	return 2;
}

std::string CommandHeal::execute(ska::ScriptComponent&, ska::MemoryScript& memory, std::vector<std::string>& )
{
	//WGameCore& wScreen = WGameCore::getInstance();
	/*int value;
	unsigned int index;

	index = ska::StringUtils::strToInt(args[0]);
	value = ska::StringUtils::strToInt(args[1]);*/

	//if (index >= 0 && index < wScreen.getPokemonManager().getPokemonTeamSize()) {
		//wScreen.getPokemonManager().getPokemon(index)->setHP(wScreen.getPokemonManager().getPokemon(index)->getHp() + value);
	//}
	return "";
}
