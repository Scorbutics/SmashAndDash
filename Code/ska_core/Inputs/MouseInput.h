#pragma once
#include <SDL.h>
#include "../Point.h"

namespace ska {
	class MouseInput {
	public:
		MouseInput();

		void setMouseLastPos(const Point<int>& mouselastpos);
		void setMousePos(SDL_Event event);

		void setMouseState(int touche, int x);
		bool mouseClick(int touche);

		const Point<int>& getMouseClickPos() const;
		const Point<int>& getMouseLastPos() const;
		const Point<int>& getMousePos() const;
		Point<int> getMouseTranslation() const;
		int trigger(int touche) const;
		int toggle(int touche) const;

		void resetTriggers();
		void resetAll();

		~MouseInput();

	private:
		Point<int> m_mouse;
		Point<int> m_mouseRel;

		int m_mouseState[8];
		int m_mouseToggle[8];
		Point<int> m_clickPos;
		Point<int> m_mouseLastPos;
	};
}
