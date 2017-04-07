#include <string>
#include "CommandMessage.h"
#include "../../Graphic/GUI/MessageDialogBox.h"

#include "../../Utils/IDs.h"
#include "Utils/StringUtils.h"
#include "Rectangle.h"


CommandMessage::CommandMessage(PokemonGameEventDispatcher& pged, ska::EntityManager& entityManager) :
    AbstractFunctionCommand(entityManager),
    m_ged(pged) {
}

int CommandMessage::argumentsNumber() {
	return 2;
}

std::string CommandMessage::execute(ska::ScriptComponent&, std::vector<std::string>& args) {
	if (MessageDialogBox::instanceExists()) {
		return "";
	}

	const auto& talkerId = args[0];
	//const std::string& text = args[1];

	std::string imageId;
	if (talkerId != "f") {
		imageId = atoi(talkerId.c_str()) >= 0 ? talkerId : ("pnj" + ska::StringUtils::intToStr(abs(atoi(talkerId.c_str()))));
	}

	/*IDialogMenuPtr menu = IDialogMenuPtr(new MessageDialogBox(text, (talkerId != "f" ? "." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Facesets" FILE_SEPARATOR "" + imageId + ".png" : ""), wScreen.getHeight(), wScreen.getWidth()));
	menu->hide(false);
	wScreen.getGUI().addDialog(menu);*/

	DialogEvent de("test", "scriptDialog", ska::Rectangle{0, 0, 8*TAILLEBLOCFENETRE, 4*TAILLEBLOCFENETRE}, true);
    m_ged.ska::Observable<DialogEvent>::notifyObservers(de);

	return "";
}

CommandMessage::~CommandMessage() {
}
