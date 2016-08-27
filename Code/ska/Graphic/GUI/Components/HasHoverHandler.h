#pragma once

namespace ska {
	class ClickEvent;

	class HasHoverHandler{
	public:
		HasHoverHandler() = default;
		virtual ~HasHoverHandler() = default;

		virtual void mouseHover() = 0;
		virtual void mouseOut() = 0;
	};
}