#include "CommandHeal.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"

CommandHeal::CommandHeal(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandHeal::~CommandHeal()
{
}

int CommandHeal::argumentsNumber() {
	return 2;
}

std::string CommandHeal::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	//WGameCore& wScreen = WGameCore::getInstance();
	int value;
	unsigned int index;
	
	index = ska::StringUtils::strToInt(args[0]);
	value = ska::StringUtils::strToInt(args[1]);
	
	//if (index >= 0 && index < wScreen.getPokemonManager().getPokemonTeamSize()) {
		//wScreen.getPokemonManager().getPokemon(index)->setHP(wScreen.getPokemonManager().getPokemon(index)->getHp() + value);
	//}
	return "";
}
