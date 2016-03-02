#include "KeyInput.h"
#include "MouseInput.h"
#include "../Utils\Observable.h"
#include "../Graphic/Rectangle.h"

#ifndef DEF_KEYBOARD_LISTENER
#define DEF_KEYBOARD_LISTENER

class WGameCore;
class Layer;
class LayerE;

//bool Inertie(WGameCore *wScreen, World* w, int type = 1);
//ska::Rectangle MovementManagerSouris(MouseInput *in, std::unique_ptr<Character>& hero);
//ska::Rectangle GetCurrentSpritePosOfHero(std::unique_ptr<Character>& hero);
//int GetDirectionFromChar(char directionChar);
//char GetCharFromDirection(int dir);

namespace ska {
	class InputListener : public Observable
	{
	public:

		InputListener();
		KeyInput* getKeyInput();
		MouseInput* getMouseInput();
		bool refresh(bool stuck);
		void updateEvents();
		bool update();
		//void updateNonDirectionalKeys();
		virtual ~InputListener();

	private:
		KeyEvent m_keyEvent;
		MouseEvent m_mouseEvent;
		KeyInput m_keyIn;
		MouseInput m_mouseIn;

	};
}
#endif
