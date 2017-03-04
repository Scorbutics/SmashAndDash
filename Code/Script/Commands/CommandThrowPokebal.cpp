#include "CommandThrowPokebal.h"
#include "../../Gameplay/WGameCore.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Graphic/Rectangle.h"

CommandThrowPokebal::CommandThrowPokebal(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandThrowPokebal::~CommandThrowPokebal()
{
}

int CommandThrowPokebal::argumentsNumber() {
	return 5;
}

std::string CommandThrowPokebal::execute(ska::ScriptComponent&, std::vector<std::string>& args)
{
	ska::Point<int> srcId, dst;
	int launchReason;
	//0 : Capture
	//1 : Recall
	//2 : Throw

	srcId.x = ska::StringUtils::strToInt(args[0]);
	srcId.y = ska::StringUtils::strToInt(args[1]);
	dst.x = ska::StringUtils::strToInt(args[2]);
	dst.y = ska::StringUtils::strToInt(args[3]);
	launchReason = ska::StringUtils::strToInt(args[4]);

	//wScreen.getPokeball().launch(wScreen.getEntityFactory().getNPC(srcId.x, srcId.y), dst, static_cast<PokeballLaunchReason::Enum> (launchReason));
	return "";
}
