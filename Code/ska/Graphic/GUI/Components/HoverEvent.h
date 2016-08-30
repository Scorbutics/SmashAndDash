#pragma once
#include "ButtonState.h"

namespace ska {
	enum MouseEventType {
		MOUSE_HOVER,
		MOUSE_OUT,
		MOUSE_CLICK,
		MOUSE_RELEASE
	};

	class HoverEvent {
	public:
		HoverEvent(const MouseEventType& state);
		const MouseEventType& getState() const;
		virtual ~HoverEvent() = default;

	private:
		MouseEventType m_state;

	};
}