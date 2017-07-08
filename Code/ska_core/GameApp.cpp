#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "GameApp.h"
#include "Exceptions/IllegalStateException.h"
#include "Logging/Logger.h"
#include <random>


ska::GameApp::GameApp() {

	// Chargement des images, de l'audio et du texte
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de la SDL : " + std::string(SDL_GetError()));
	}

	/* Fix GDB Bug with named thread on windows (Mixer raises an exception when init) */
	if (!SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1")) {
		SKA_LOG_MESSAGE("Attention : Windows nomme actuellement les threads créés par l'application alors que le programme tente de désactiver cette fonctionnalité.");
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		SKA_LOG_MESSAGE("Attention : Linear texture filtering impossible à activer." );
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		throw ska::IllegalStateException("Erreur lors de l'initialisation de SDL_image : " + std::string(IMG_GetError()));
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
		SKA_LOG_ERROR("Impossible d'initialiser SDL_mixer : ", Mix_GetError());
	}

	if (TTF_Init() == -1) {
		SKA_LOG_ERROR("Erreur d'initialisation de TTF_Init : ", TTF_GetError());
	}
}

ska::GameApp::~GameApp() {
	TTF_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}
