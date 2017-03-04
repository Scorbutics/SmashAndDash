#include <string>
#include "CommandChoice.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/ScriptUtils.h"
#include "../../Graphic/GUI/MessageDialogBox.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Utils/IDs.h"
#include "../../ska/Script/System/ScriptAutoSystem.h"

CommandChoice::CommandChoice(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandChoice::~CommandChoice()
{
}

int CommandChoice::argumentsNumber() {
	return 3;
}

std::string CommandChoice::execute(ska::ScriptComponent&, std::vector<std::string>& args) {
	std::string fname, texte, buf, var;
	var = args[0];
	fname = args[1];
	texte = args[2];
	//ska::Rectangle choicePos;

	if (fname != "f")
	{
		if (atoi(fname.c_str()) >= 0)
			buf = fname;
		else
			buf = "pnj" + ska::StringUtils::intToStr(abs(atoi(fname.c_str())));
	}

	/*MessageDialogBox box(texte, (fname != "f" ? "."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Facesets"FILE_SEPARATOR"" + buf + ".png" : ""), wScreen.getHeight(), wScreen.getWidth());
	ska::Rectangle menuPos = box.getRect();

	choicePos = menuPos;
	choicePos.x += menuPos.w;
	choicePos.w = TAILLEBLOCFENETRE / 2;
	choicePos.h = TAILLEBLOCFENETRE * 3;
	choicePos.y -= choicePos.h;

	if ((unsigned int)choicePos.x >= wScreen.getWidth()) {
		choicePos.x = wScreen.getWidth() - choicePos.w;
	}

	DialogMenu choice("Entrée:Oui¤Echap:Non", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"menu.png", choicePos, 22);
	*/
	//ska::ScriptUtils::setValueFromVarOrSwitchNumber(script.parent->getSavegame(), script.extendedName, var, doAction ? "1" : "0", script.varMap);
	
	return "";
}
