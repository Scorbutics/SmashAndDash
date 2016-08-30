#pragma once

#include "../../Point.h"

#include "HoverEvent.h"

namespace ska {
	class ClickEvent : public HoverEvent {
	public:
		ClickEvent(const ska::MouseEventType& state, ska::Point<int>& pos);
		const ska::Point<int>& getClickPosition() const;
		~ClickEvent() = default;
	private:
		const ska::Point<int> m_clickPosition;
		
	};
}