#include <string>
#include "CommandMessage.h"
#include "../../Graphic/GUI/DialogMenu.h"
#include "../../Utils\IDs.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils\StringUtils.h"
#include "../../ska/Graphic/Rectangle.h"


using namespace std;

CommandMessage::CommandMessage(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}

int CommandMessage::argumentsNumber() {
	return 2;
}

std::string CommandMessage::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string fname, texte, buf;
	fname = args[0];
	texte = args[1];

	ska::Rectangle menuPos;

	menuPos.x = 0;
	menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE * 4;
	menuPos.w = wScreen.getWidth() / 2;
	menuPos.h = TAILLEBLOCFENETRE * 4;

	DialogMenu menu(texte, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);

	menuPos.x += TAILLEBLOCFENETRE;
	menuPos.y -= TAILLEBLOCFENETRE * 5;
	menuPos.w = 4 * TAILLEBLOCFENETRE;
	menuPos.h = 5 * TAILLEBLOCFENETRE;

	if (fname != "f")
	{
		if (atoi(fname.c_str()) >= 0)
			buf = fname;
		else
			buf = "pnj" + ska::StringUtils::intToStr(abs(atoi(fname.c_str())));
	}

	DialogMenu imgDial("", (fname != "f" ? "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" + buf + ".png" : ""), "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);
	const ska::InputActionContainer& in = wScreen.getActions();

	while (menu.getX(menu.getLines() - 1) < menu.getWLine(menu.getLines() - 1))
	{
		wScreen.eventUpdate(true);
		wScreen.graphicUpdate();
		menu.display();
		if (fname != "f")
			imgDial.display();

		wScreen.flip();
		SDL_Delay(20);
	}

	while (!(in[ska::InputAction::DoAction] || in[ska::InputAction::Quit]))
	{
		wScreen.eventUpdate(true);
		wScreen.graphicUpdate();
		menu.display();
		if (fname != "f")
			imgDial.display();

		wScreen.flip();
		SDL_Delay(20);
	}

	//in->resetAll();
	//menu.pause();
	return "";
}

CommandMessage::~CommandMessage()
{
}
