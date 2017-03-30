#pragma once
#include "Draw/DrawableFixedPriority.h"
#include "Point.h"
#include "Graphic/Texture.h"


namespace ska {
	class MouseCursor : public DrawableFixedPriority {
	public:
		explicit MouseCursor(const std::string& styleName);
		void move(const Point<int>& mousePos);

		bool isVisible() const override;

		void grab();
		void basic();

		virtual ~MouseCursor();
		void display() const override;

	private:
		Texture m_basicCursor;
		Texture m_grabCursor;
		Texture * m_currentCursor;
		Point<int> m_mousePos;
		bool m_visible;

	};
}
