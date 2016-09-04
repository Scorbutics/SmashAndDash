#pragma once

namespace ska {
	class HoverEvent;

	class HasHoverHandler{
	public:
		HasHoverHandler() = default;
		virtual ~HasHoverHandler() = default;

		virtual bool mouseHover(HoverEvent& e) = 0;
		
	};
}