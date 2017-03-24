#pragma once
#include "Utils/Observer.h"
#include "../Events/ClickEvent.h"

namespace ska {

	using ClickObserver = Observer<ClickEvent>;
	using HoverObserver = Observer<HoverEvent>;

	class MouseObserver :
		public ClickObserver,
		public HoverObserver {
	public:
		MouseObserver(std::function<bool(HoverEvent&)> const& hover, std::function<bool(ClickEvent&)> const& click) :
			ClickObserver(click),
			HoverObserver(hover) {
		}
		virtual ~MouseObserver() = default;
	};
}
