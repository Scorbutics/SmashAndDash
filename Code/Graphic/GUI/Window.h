#ifndef DEF_WINDOW
#define DEF_WINDOW


#include <windows.h>
//#include <fmodex/fmod.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

class Window
{
    public:
    Window();
    void pause();
    //FMOD_SYSTEM* getMusicSystem();
	SDL_Renderer* getRenderer();
    ~Window();
	unsigned int getWidth();
	unsigned int getHeight();
	SDL_Window* getHandle();
	void flip();
	void setNextFrameFlip(bool b);

protected:
    std::string m_wName, m_bFilename;
    int m_loFenetre, m_laFenetre;
	bool m_flip;

    protected:
    //FMOD_SYSTEM *m_musicSystem;
	SDL_Renderer *m_renderer;
	SDL_Texture* m_texture;
    SDL_Surface *m_bmp;
	SDL_Window *m_screen;
    SDL_Rect m_positionFond;

};

#endif
