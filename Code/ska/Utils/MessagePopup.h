#pragma once
#include <string>
#include "MessageType.h"

class Window;
namespace ska {
	class MessagePopup
	{
	public:
		MessagePopup(MessageType::Enum mt, const std::string& title, const std::string& message, Window* w);
		~MessagePopup();
	};

}
