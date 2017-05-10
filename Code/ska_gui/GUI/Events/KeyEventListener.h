#pragma once
#include <functional>

#include "EventListener.h"

namespace ska {
	class KeyEvent;
	class Widget;

	/* Handler triggered when a key is pressed */
	using KeyEventHandler = std::function<void(Widget*, KeyEvent&)>;

	class KeyEventListener : public EventListener<KeyEvent> {
	public:
		KeyEventListener();
		virtual bool notify(Widget& tthis, KeyEvent& e) override;
		virtual ~KeyEventListener() = default;

	};
}
