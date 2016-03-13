#include <cstdlib>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>
#include "Gameplay/WGameCore.h"
#include "./ska/Exceptions/GenericException.h"
#include "./ska/Exceptions/TerminateProcessException.h"
#include "./ska/Utils/MessagePopup.h"
using namespace std;
typedef unique_ptr<Character> Character_ptr;

WGameCore* wScreen;

int main (int argc, char *argv[])
{
    argc = argc;
    argv = argv;

	//ECSTest();
    int startPosx = 0, startPosy = 0, widthBlocks = 0, heightBlocks = 0;
    string startMapName, startMapChipsetName;

	ofstream logFile("stdlog.txt", ios::trunc);
	clog.rdbuf(logFile.rdbuf());

	ofstream errFile("stderr.txt", ios::trunc);
	cerr.rdbuf(errFile.rdbuf());

    srand((unsigned int)time(NULL));

    // Chargement de la vidéo, de l'audio et du texte
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        cerr << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		cerr << "Impossible d'initialiser SDL_image : " << IMG_GetError() << endl;
	}

    if(TTF_Init() == -1) {
        cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << endl;
        exit(EXIT_FAILURE);
    }

	ska::IniReader reader("gamesettings.ini");
	widthBlocks = reader.getInt("Settings window_width_blocks");
	heightBlocks = reader.getInt("Settings window_height_blocks");
	    
	try {
		//(widthBlocks*TAILLEBLOC > TAILLEECRANMINX ? widthBlocks*TAILLEBLOC: TAILLEECRANMINX), (heightBlocks*TAILLEBLOC > TAILLEECRANMINY ? heightBlocks*TAILLEBLOC: TAILLEECRANMINY)
		WGameCore& wScreen = WGameCore::getInstance(); //Crée une fenetre de type "WGameCore", Génère ce monde sur la fenetre (unique)
		while (wScreen.refresh()); //boucle principale
		wScreen.quitter(false);
	} catch (ska::TerminateProcessException& tpe) {
		clog << tpe.what() << endl;
	} catch (ska::GenericException& e) {
		/* Handles Generics Game exceptions */
		cerr << e.what() << endl;
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), NULL);
	}
    
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

    return 0;

}









