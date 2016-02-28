#ifndef DEF_KEYINPUT
#define DEF_KEYINPUT

#include <SDL2/SDL.h>

namespace ska {
	class KeyInput
	{

	public:
		KeyInput();

		char getKeyState(int touche);
		void setKeyState(int touche, int x);

		int getQuit();
		void setQuit(int x);

		void resetAll();
		void resetDirectionStates();


		~KeyInput();

	private:
		char m_key[SDL_NUM_SCANCODES];
		char m_quit;


	};
}
#endif
