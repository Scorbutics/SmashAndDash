#pragma once

namespace ska {
	class ClickEvent;

	class HasClickHandler{
	public:
		HasClickHandler() = default;
		virtual ~HasClickHandler() = default;

		virtual bool click(ska::ClickEvent& e) = 0;
	};
}