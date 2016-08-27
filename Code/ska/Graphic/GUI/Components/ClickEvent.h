#pragma once

#include "../../Point.h"

namespace ska {
	class ClickEvent {
	public:
		ClickEvent(ska::Point<int>& pos);
		const ska::Point<int>& getClickPosition() const;
		~ClickEvent() = default;
	private:
		const ska::Point<int> m_clickPosition;
	};
}