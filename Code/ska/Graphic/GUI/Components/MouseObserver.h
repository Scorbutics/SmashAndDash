#pragma once
#include "../../../Utils/Observer.h"
#include "HasHoverHandler.h"
#include "HasClickHandler.h"
#include "ClickEvent.h"

namespace ska {

	using ClickObserver = Observer<ClickEvent>;
	using HoverObserver = Observer<HoverEvent>;

	class MouseObserver : 
		public HasClickHandler,
		public HasHoverHandler,
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