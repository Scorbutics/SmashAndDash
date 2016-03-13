#include <string>
#include "CommandChoice.h"
#include "../../Gameplay/WGameCore.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/ScriptUtils.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Utils/IDs.h"
#include "../../ska/Script/IScript.h"
#include "../../ska/Script/ScriptDispatcher.h"

using namespace std;

CommandChoice::CommandChoice()
{
}


CommandChoice::~CommandChoice()
{
}

int CommandChoice::argumentsNumber() {
	return 3;
}

std::string CommandChoice::execute(ska::IScript* script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	string fname, texte, buf, var;
	var = args[0];
	fname = args[1];
	texte = args[2];
	ska::Rectangle menuPos, choicePos;

	menuPos.x = 0;
	menuPos.y = wScreen.getHeight() - TAILLEBLOCFENETRE * 4;
	menuPos.w = wScreen.getWidth() / 2;
	menuPos.h = TAILLEBLOCFENETRE * 4;

	choicePos = menuPos;
	choicePos.x += menuPos.w;
	choicePos.w = TAILLEBLOCFENETRE/2;
	choicePos.h = TAILLEBLOCFENETRE * 3;
	choicePos.y -= choicePos.h;

	if ((unsigned int)choicePos.x >= wScreen.getWidth())
		choicePos.x = wScreen.getWidth() - choicePos.w;

	DialogMenu menu(texte, "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", menuPos, 22);
	DialogMenu choice("Entrée:Oui¤Echap:Non", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", choicePos, 22);

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

	while (menu.getX(menu.getLines() - 1) < menu.getWLine(menu.getLines() - 1))
	{
		wScreen.eventUpdate(true);
		wScreen.graphicUpdate();
		menu.display();
		if (fname != "f")
			imgDial.display();

		wScreen.flip();
		SDL_Delay(30);
	}

	const ska::InputActionContainer& in = wScreen.getActions();
	bool doAction = false;

	do
	{
		wScreen.eventUpdate(true);
		wScreen.graphicUpdate();
		menu.display();
		choice.display();
		if (fname != "f")
			imgDial.display();

		wScreen.flip();
		SDL_Delay(30);

	} while (!(doAction = in[ska::InputAction::DoAction] || in[ska::InputAction::Quit]));

	ska::ScriptUtils::setValueFromVarOrSwitchNumber(script->getParent().getSavegame(), script->getExtendedName(), var, doAction ? "1" : "0", script->getVarMap());
	
	return "";
}
