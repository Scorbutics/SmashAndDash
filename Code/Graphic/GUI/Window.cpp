#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include "Utils/IDs.h"
#include "Window.h"

using namespace std;


Window::Window()
{
	m_flip = true;
    m_laFenetre = TAILLEECRANMINX*2;
	m_loFenetre = TAILLEECRANMINY*3/2;
    m_wName = "Fenetre";
    m_bFilename = "menu.bmp";
    m_positionFond.x = 0;
    m_positionFond.y = 0;
    m_bmp = NULL;

    FMOD_System_Create(&m_musicSystem);
    FMOD_System_Init(m_musicSystem, 2, FMOD_INIT_NORMAL, NULL);

	m_screen = SDL_CreateWindow("Pokemon Smash&Dash",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_laFenetre, m_loFenetre,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	
    if(m_screen == NULL)
        cerr << "Erreur lors de la création de la fenêtre SDL :" << SDL_GetError() << endl;
	
	m_renderer = SDL_CreateRenderer(m_screen, -1, SDL_RENDERER_ACCELERATED);
	
	if(m_renderer == NULL)
		cerr << "Erreur lors de la création de la fenêtre SDL (renderer) :" << SDL_GetError() << endl;

	SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

	if( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"))
		cerr << "Attention : Linear texture filtering inactivé !" << endl;
	

	//SDL_RenderSetLogicalSize(m_renderer, m_laFenetre, m_loFenetre);
	
	/*
	m_texture = SDL_CreateTexture(m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		m_laFenetre, m_loFenetre);

	if(m_texture == NULL)
		cerr << "Erreur lors de la création de la fenêtre SDL (surface de texture) :" << SDL_GetError() << endl;*/

}

unsigned int Window::getWidth()
{
	return m_laFenetre;
}

unsigned int Window::getHeight()
{
	return m_loFenetre;
}

SDL_Renderer* Window::getRenderer()
{
	return m_renderer;
}

FMOD_SYSTEM* Window::getMusicSystem()
{
    return m_musicSystem;
}

void Window::flip()
{
	if(m_flip)
		SDL_RenderPresent(m_renderer);
	else
		m_flip = true;
}

void Window::setNextFrameFlip( bool b )
{
	m_flip = b;
}

SDL_Window* Window::getHandle()
{
	return m_screen;
}


Window::~Window()
{
    //FMOD_System_Close(m_musicSystem);
    //FMOD_System_Release(m_musicSystem);
    SDL_FreeSurface(m_bmp);
	//SDL_DestroyTexture( m_texture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_screen);
}
