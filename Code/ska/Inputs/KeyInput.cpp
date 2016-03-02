#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "KeyInput.h"

using namespace std;


ska::KeyInput::KeyInput()
{
	m_quit = 0;
	this->resetAll();
}

ska::KeyInput::~KeyInput()
{
}

void ska::KeyInput::setKeyState(int touche, int x)
{
	if (touche < SDL_NUM_SCANCODES)
	{
		m_key[touche] = x;
	}
	else
		cerr << "Erreur (classe Input) : Impossible d'accéder à l'appuie de la touche " << touche << endl;
}

char ska::KeyInput::getKeyState(int touche)
{
	if(touche < SDL_NUM_SCANCODES)
		return m_key[touche];
	else
		cerr << "Erreur (classe Input) : Impossible d'accéder à l'appuie de la touche " << touche << endl;
	
	return 0;
}


void ska::KeyInput::setQuit(int x)
{
    m_quit = x;
}



void ska::KeyInput::resetDirectionStates()
{
    m_key[SDL_SCANCODE_UP] = 0;
    m_key[SDL_SCANCODE_DOWN] = 0;
    m_key[SDL_SCANCODE_RIGHT] = 0;
    m_key[SDL_SCANCODE_LEFT] = 0;
}




void ska::KeyInput::resetAll()
{
    m_key[SDL_SCANCODE_UP] = 0;
    m_key[SDL_SCANCODE_DOWN] = 0;
    m_key[SDL_SCANCODE_RIGHT] = 0;
    m_key[SDL_SCANCODE_LEFT] = 0;
    m_key[SDL_SCANCODE_ESCAPE] = 0;
    m_key[SDL_SCANCODE_LSHIFT] = 0;
    m_key[SDL_SCANCODE_RETURN] = 0;
    m_key[SDL_SCANCODE_SPACE] = 0;
    m_key[SDL_SCANCODE_DELETE] = 0;
    m_key[SDL_SCANCODE_A] = 0;
    m_key[SDL_SCANCODE_Q] = 0;
	m_key[SDL_SCANCODE_W] = 0;
	m_key[SDL_SCANCODE_Z] = 0;
    m_key[SDL_SCANCODE_X] = 0;
    m_key[SDL_SCANCODE_C] = 0;
    m_key[SDL_SCANCODE_V] = 0;
    m_key[SDL_SCANCODE_B] = 0;
    m_key[SDL_SCANCODE_N] = 0;
    m_key[SDL_SCANCODE_S] = 0;
    m_key[SDL_SCANCODE_D] = 0;
    m_key[SDL_SCANCODE_1] = 0;
    m_key[SDL_SCANCODE_2] = 0;
    m_key[SDL_SCANCODE_3] = 0;
    m_key[SDL_SCANCODE_4] = 0;
}

int ska::KeyInput::getQuit()
{
    return m_quit;
}
