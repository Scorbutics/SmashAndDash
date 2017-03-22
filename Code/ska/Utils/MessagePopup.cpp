#include <SDL.h>
#include "MessagePopup.h"
#include "../Core/Window.h"

ska::MessagePopup::MessagePopup(MessageType::Enum mt, const std::string& title, const std::string& message, Window* w) {
	Uint32 flags = 0;

	switch (mt) {
	case MessageType::Warn:
		flags = SDL_MESSAGEBOX_WARNING;
		break;
	case MessageType::Error:
		flags = SDL_MESSAGEBOX_ERROR;
		break;
	
	default:
	case MessageType::Info:
		flags = SDL_MESSAGEBOX_INFORMATION;
		break;
	}

	SDL_ShowSimpleMessageBox(flags,
		title.c_str(),
		message.c_str(),
		w == nullptr ? NULL : 
		w->getHandle());
}


ska::MessagePopup::~MessagePopup() {
}
