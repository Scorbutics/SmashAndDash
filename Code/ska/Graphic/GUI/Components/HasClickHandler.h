#pragma once

namespace ska {
	class ClickEvent;

	class HasClickHandler{
	public:
		HasClickHandler() = default;
		virtual ~HasClickHandler() = default;

		virtual void click(const ska::ClickEvent& e) = 0;
		virtual void release(const ska::ClickEvent& e) = 0;
	};
}