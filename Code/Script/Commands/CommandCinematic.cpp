#include "CommandCinematic.h"
#include <SDL2\SDL_rect.h>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\StringUtils.h"

CommandCinematic::CommandCinematic()
{
}


CommandCinematic::~CommandCinematic()
{
}

int CommandCinematic::argumentsNumber() {
	return 7;
}

bool CommandCinematic::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	unsigned int frameNumber, delay, id_anim, duration;
	bool continuer = true;
	SDL_Rect pos, menuPos;
	string message;
	SpriteAnimation* cinematic;
	KeyInput* in = wScreen.getInputListener().getKeyInput();

	menuPos.x = 0;
	menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE * 4;
	menuPos.w = wScreen.getWidth();
	menuPos.h = TAILLEBLOCFENETRE * 4;

	id_anim = StringUtils::strToInt(args[0]);
	delay = StringUtils::strToInt(args[1]);
	duration = StringUtils::strToInt(args[2]);
	frameNumber = StringUtils::strToInt(args[3]);
	pos.x = StringUtils::strToInt(args[4]);
	pos.y = StringUtils::strToInt(args[5]);
	message = args[6];	

	if (pos.x != -1)
		pos.x -= wScreen.getORel().x;

	if (pos.y != -1)
		pos.y -= wScreen.getORel().y;

	DialogMenu messageBox(message, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 21);

	wScreen.getSpriteAnimationManager().reset();

	cinematic = wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, id_anim, pos, -1, -1, frameNumber, delay);

	while (continuer)
	{
		SDL_RenderClear(wScreen.getRenderer());
		wScreen.getSpriteAnimationManager().refresh();
		wScreen.getInputListener().refresh(true);
		if (in->getKeyState(SDL_SCANCODE_SPACE) || in->getKeyState(SDL_SCANCODE_RETURN) || in->getKeyState(SDL_SCANCODE_ESCAPE)) {
			continuer = false;
		}

		messageBox.refresh();
		messageBox.display();
		wScreen.flip();
		SDL_Delay(20);
	}
	in->resetAll();
	wScreen.getSpriteAnimationManager().reset();
	return true;
}
