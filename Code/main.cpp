#include <cstdlib>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Gameplay/WGameCore.h"
#include "./ska/Exceptions/GenericException.h"
#include "./ska/Exceptions/TerminateProcessException.h"
#include "./ska/Utils/MessagePopup.h"
#include "./Utils/IDs.h"

#define SKA_DEBUG

int main (int argc, char *argv[])
{
    argc = argc;
    argv = argv;

    int startPosx = 0, startPosy = 0, widthBlocks = 0, heightBlocks = 0;
    std::string startMapName, startMapChipsetName;

	std::ofstream logFile("stdlog.txt", std::ios::trunc);
	std::clog.rdbuf(logFile.rdbuf());

	std::wofstream wlogFile("wstdlog.txt", std::ios::trunc);
	std::wclog.rdbuf(wlogFile.rdbuf());

	std::ofstream errFile("stderr.txt", std::ios::trunc);
	std::cerr.rdbuf(errFile.rdbuf());

	/* TODO : ne plus utiliser srand...  */
    srand((unsigned int)time(NULL));


	/* TODO : réécrire dans la classe Core (pour une fois, un singleton, car une seule initialisation par application cliente) */

    // Chargement de la vidéo, de l'audio et du texte
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cerr << "Impossible d'initialiser SDL_image : " << IMG_GetError() << std::endl;
	}

	if (!(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))) {
		std::cerr << "Impossible d'initialiser SDL_mixer : " << SDL_GetError() << std::endl;
	}

    if(TTF_Init() == -1) {
		std::cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

	ska::IniReader reader("gamesettings.ini");
	widthBlocks = reader.get<int>("Window width_blocks");
	heightBlocks = reader.get<int>("Window height_blocks");
	const std::string& title = reader.get<std::string>("Window title");

	try {
		//(widthBlocks*TAILLEBLOC > TAILLEECRANMINX ? widthBlocks*TAILLEBLOC: TAILLEECRANMINX), (heightBlocks*TAILLEBLOC > TAILLEECRANMINY ? heightBlocks*TAILLEBLOC: TAILLEECRANMINY)

		//Crée une fenetre de type "WGameCore", Génère ce monde sur la fenetre (unique)
		WGameCore wScreen(title, widthBlocks * TAILLEBLOC, heightBlocks * TAILLEBLOC);
		//boucle principale
		while (wScreen.refresh());
	} catch (ska::TerminateProcessException& tpe) {
		std::clog << tpe.what() << std::endl;
	} catch (ska::GenericException& e) {
		/* Handles Generics Game exceptions */
		std::cerr << e.what() << std::endl;
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), NULL);
	}

	IMG_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();

	SDL_Quit();
	

    return 0;

}









