#pragma once

namespace ska {
	class HoverEvent;

	class HasHoverHandler{
	public:
		HasHoverHandler() = default;
		virtual ~HasHoverHandler() = default;

		virtual void mouseHover(const HoverEvent& e) = 0;
		
	};
}