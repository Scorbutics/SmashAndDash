#include "CommandTranslationCamera.h"
#include <SDL2\SDL_rect.h>
#include <SDL2\SDL_timer.h>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandTranslationCamera::CommandTranslationCamera()
{
}


CommandTranslationCamera::~CommandTranslationCamera()
{
}

int CommandTranslationCamera::argumentsNumber() {
	return 3;
}

std::string CommandTranslationCamera::execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList)
{
	WGameCore& wScreen = WGameCore::getInstance();
	unsigned int duree = 0, t0 = 0;
	int speedx, speedy;
	SDL_Rect tmpRelativeOrigin;

	duree = StringUtils::strToInt(args[0]);
	speedx = StringUtils::strToInt(args[1]);
	speedy = StringUtils::strToInt(args[2]);

	tmpRelativeOrigin = wScreen.getORel();
	wScreen.activeScrolling(false);

	t0 = SDL_GetTicks();

	while (SDL_GetTicks() - t0 < duree)
	{
		tmpRelativeOrigin.x -= speedx;
		tmpRelativeOrigin.y -= speedy;

		wScreen.graphicUpdate();
		wScreen.eventUpdate(true);
		wScreen.setORel(tmpRelativeOrigin);

		wScreen.flip();
		SDL_Delay(20);

	}

	wScreen.activeScrolling(true);
	return "";
}
