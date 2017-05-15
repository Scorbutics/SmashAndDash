#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "GameApp.h"
#include "Exceptions/IllegalStateException.h"
#include <iostream>

ska::GameApp::GameApp() {
	/* TODO : ne plus utiliser srand...  */
	srand(static_cast<unsigned int>(time(nullptr)));

	// Chargement des images, de l'audio et du texte
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de la SDL : " + std::string(SDL_GetError()));
	}

	/* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
	if (!SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
		std::clog << "Attention : Windows nomme actuellement les threads cr��s par l'application alors que le programme tente de d�sactiver cette fonctionnalit�." << std::endl;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		std::clog << "Attention : Linear texture filtering impossible � activer." << std::endl;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de SDL_image : " + std::string(IMG_GetError()));
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		std::cerr << "Impossible d'initialiser SDL_mixer : " << Mix_GetError() << std::endl;
	}

	if (TTF_Init() == -1) {
		std::cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << std::endl;
	}
}

ska::GameApp::~GameApp() {
	TTF_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}