#include "CommandThrowPokebal.h"
#include <SDL2\SDL_rect.h>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandThrowPokebal::CommandThrowPokebal()
{
}


CommandThrowPokebal::~CommandThrowPokebal()
{
}

int CommandThrowPokebal::argumentsNumber() {
	return 5;
}

std::string CommandThrowPokebal::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect srcId, dst;
	int launchReason;
	//0 : Capture
	//1 : Recall
	//2 : Throw

	srcId.x = StringUtils::strToInt(args[0]);
	srcId.y = StringUtils::strToInt(args[1]);
	dst.x = StringUtils::strToInt(args[2]);
	dst.y = StringUtils::strToInt(args[3]);
	launchReason = StringUtils::strToInt(args[4]);

	wScreen.getPokeball().launch(wScreen.getEntityFactory().getNPC(srcId.x, srcId.y), dst, static_cast<PokeballLaunchReason::Enum> (launchReason));
	return "";
}
