#ifndef DEF_KEYINPUT
#define DEF_KEYINPUT

#include <SDL2/SDL.h>
#include <bitset>

namespace ska {
	class KeyInput
	{

	public:
		KeyInput();

		bool trigger(int key);
		bool toggle(int key);

		void setKeyState(int key, bool state);
		void setLastKeyState(int key, bool state);

		void resetTriggers();

		~KeyInput();

	private:
		std::bitset<SDL_NUM_SCANCODES> m_keys;
		std::bitset<SDL_NUM_SCANCODES> m_toggle;

	};
}
#endif
