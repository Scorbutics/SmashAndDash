#pragma once

#include <SDL2/SDL.h>
#include <bitset>

namespace ska {
	class KeyInput
	{

	public:
		KeyInput() = default;

		bool trigger(int key);
		bool toggle(int key);

		void setKeyState(int key, bool state);
		void setLastKeyState(int key, bool state);

		void resetTriggers();

		~KeyInput() = default;

	private:
		std::bitset<SDL_NUM_SCANCODES> m_keys;
		std::bitset<SDL_NUM_SCANCODES> m_toggle;

	};
}

