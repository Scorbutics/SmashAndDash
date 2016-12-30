#pragma once
#include "../../Texture.h"
#include "../../Draw/DrawableFixedPriority.h"

namespace ska {
	class MouseCursor : public DrawableFixedPriority {
	public:
		MouseCursor(const std::string& styleName);
		void move(const Point<int>& mousePos);

		bool isVisible() const;

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
