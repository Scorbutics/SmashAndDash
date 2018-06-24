#include <string>
#include <limits>
#include "CommandMessage.h"
#include "../../Graphic/GUI/MessageDialogBox.h"
#include "Script/ScriptUtils.h"
#include "../../Utils/IDs.h"
#include "Utils/StringUtils.h"
#include "Rectangle.h"
#include "Exceptions/ScriptException.h"

CommandMessage::CommandMessage(PokemonGameEventDispatcher& pged, ska::EntityManager& entityManager) :
    AbstractFunctionCommand(entityManager),
    m_ged(pged) {
}

int CommandMessage::argumentsNumber() {
	return -1;
}

std::string CommandMessage::execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) {
	if (MessageDialogBox::instanceExists()) {
		return "";
	}

	if (args.size() < 2) {
		throw ska::ScriptException("This command needs at least 2 parameters");
	}

	const auto& talkerId = args[0];
	const std::string& text = args[1];

	std::string imageId;
	if (talkerId != ".") {
		imageId = atoi(talkerId.c_str()) >= 0 ? talkerId : ("pnj" + ska::StringUtils::intToStr(abs(atoi(talkerId.c_str()))));
	}

	DialogEvent de(text, "scriptDialog", ska::Rectangle{ 0, std::numeric_limits<int>::max(), -1, 4 * 48 }, true);

	/* if there is a script callback provided */
	const auto ssc = ska::ScriptUtils::instantiateScript(args, 2, script);
	if (ssc.has_value()) {
		de.callback = ssc.value();
		de.callbackActive = true;
	}
	
    m_ged.ska::Observable<DialogEvent>::notifyObservers(de);

	return "";
}
