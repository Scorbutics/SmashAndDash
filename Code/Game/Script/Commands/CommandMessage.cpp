#include <string>
#include <limits>
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
	const std::string& text = args[1];

	std::string imageId;
	if (talkerId != "f") {
		imageId = atoi(talkerId.c_str()) >= 0 ? talkerId : ("pnj" + ska::StringUtils::intToStr(abs(atoi(talkerId.c_str()))));
	}

	DialogEvent de(text, "scriptDialog", ska::Rectangle{0, std::numeric_limits<int>::max(), -1, 4*TAILLEBLOCFENETRE}, true);
    m_ged.ska::Observable<DialogEvent>::notifyObservers(de);

	return "";
}

CommandMessage::~CommandMessage() {
}
