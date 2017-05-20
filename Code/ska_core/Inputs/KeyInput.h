#pragma once

#include <SDL.h>
#include <bitset>

namespace ska {
	class KeyInput {

	public:
		KeyInput() = default;

		bool trigger(int key) const;
		bool toggle(int key) const;

		void setKeyState(int key, bool state);

		void resetTriggers();

		~KeyInput() = default;

	private:
		std::bitset<SDL_NUM_SCANCODES> m_keys;
		std::bitset<SDL_NUM_SCANCODES> m_toggle;

	};
}

