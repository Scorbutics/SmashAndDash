#include "CommandPokemonOut.h"
#include <SDL2\SDL_rect.h>
#include <SDL2\SDL_timer.h>
#include "Gameplay\WGameCore.h"
#include "Utils\StringUtils.h"

CommandPokemonOut::CommandPokemonOut()
{
}


CommandPokemonOut::~CommandPokemonOut()
{
}

int CommandPokemonOut::argumentsNumber() {
	return 1;
}

bool CommandPokemonOut::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	int index;
	unsigned int duree = 2000, t0 = 0;
	WGameCore& wScreen = WGameCore::getInstance();

	index = StringUtils::strToInt(args[0]);
	SDL_Rect launchPos = wScreen.getEntityFactory().getTrainer()->getCenterPos();
	launchPos.y += TAILLEBLOC;

	wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), launchPos, PokeballLaunchReason::Throw);

	t0 = SDL_GetTicks();

	while (SDL_GetTicks() - t0 < duree)
	{

		wScreen.graphicUpdate();
		wScreen.eventUpdate(true);

		wScreen.flip();
		SDL_Delay(20);

	}
	wScreen.getEntityFactory().addNPC(wScreen.getPokemonManager().getPokemon(index)->getID(), launchPos, "1");
	return true;
}
