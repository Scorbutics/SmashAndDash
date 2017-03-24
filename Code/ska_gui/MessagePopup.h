#pragma once
#include <string>
#include "MessageType.h"


namespace ska {
	class Window;

	class MessagePopup
	{
	public:
		MessagePopup(MessageType::Enum mt, const std::string& title, const std::string& message, Window* w);
		~MessagePopup();
	};

}
