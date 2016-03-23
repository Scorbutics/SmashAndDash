#include "CommandPokemonOut.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Utils/IDs.h"

CommandPokemonOut::CommandPokemonOut()
{
}


CommandPokemonOut::~CommandPokemonOut()
{
}

int CommandPokemonOut::argumentsNumber() {
	return 1;
}

std::string CommandPokemonOut::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	int index;
	unsigned int duree = 2000, t0 = 0;
	WGameCore& wScreen = WGameCore::getInstance();

	index = ska::StringUtils::strToInt(args[0]);
	//ska::Point<int> launchPos = wScreen.getEntityFactory().getTrainer()->getCenterPos();
	//launchPos.y += TAILLEBLOC;

	//wScreen.getPokeball().launch(wScreen.getEntityFactory().getTrainer(), launchPos, PokeballLaunchReason::Throw);

	t0 = SDL_GetTicks();

	while (SDL_GetTicks() - t0 < duree)
	{

		wScreen.graphicUpdate();
		wScreen.eventUpdate(true);

		wScreen.flip();
		SDL_Delay(20);

	}
	//wScreen.getEntityFactory().addNPC(wScreen.getPokemonManager().getPokemon(index)->getID(), launchPos, "1");
	return "";
}
