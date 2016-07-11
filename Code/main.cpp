#include <cstdlib>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>
#include <SDL2\SDL_ttf.h>
#include "Gameplay/WGameCore.h"
#include "./ska/Exceptions/GenericException.h"
#include "./ska/Exceptions/TerminateProcessException.h"
#include "./ska/Utils/MessagePopup.h"

#define SKA_DEBUG

int main (int argc, char *argv[])
{
    argc = argc;
    argv = argv;

	//ECSTest();
    int startPosx = 0, startPosy = 0, widthBlocks = 0, heightBlocks = 0;
    std::string startMapName, startMapChipsetName;

	std::ofstream logFile("stdlog.txt", ios::trunc);
	std::clog.rdbuf(logFile.rdbuf());

	std::ofstream errFile("stderr.txt", ios::trunc);
	std::cerr.rdbuf(errFile.rdbuf());

	/* TODO : ne plus utiliser srand  */
    srand((unsigned int)time(NULL));


	/* TODO : réécrire */

    // Chargement de la vidéo, de l'audio et du texte
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cerr << "Impossible d'initialiser SDL_image : " << IMG_GetError() << std::endl;
	}

    if(TTF_Init() == -1) {
		std::cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

	ska::IniReader reader("gamesettings.ini");
	widthBlocks = reader.getInt("Settings window_width_blocks");
	heightBlocks = reader.getInt("Settings window_height_blocks");
	    
	try {
		//(widthBlocks*TAILLEBLOC > TAILLEECRANMINX ? widthBlocks*TAILLEBLOC: TAILLEECRANMINX), (heightBlocks*TAILLEBLOC > TAILLEECRANMINY ? heightBlocks*TAILLEBLOC: TAILLEECRANMINY)
		WGameCore& wScreen = WGameCore::getInstance(); //Crée une fenetre de type "WGameCore", Génère ce monde sur la fenetre (unique)
		while (wScreen.refresh()); //boucle principale
		//wScreen.quitter(false);
	} catch (ska::TerminateProcessException& tpe) {
		std::clog << tpe.what() << std::endl;
	} catch (ska::GenericException& e) {
		/* Handles Generics Game exceptions */
		std::cerr << e.what() << std::endl;
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), NULL);
	}
    
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

    return 0;

}









