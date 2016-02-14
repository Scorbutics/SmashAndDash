#include <cstdlib>
#include <SDL2/SDL.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>
#include <sstream>
#include <SDL2/SDL_image.h>
#include "Gameplay/WGameCore.h"
#include "./Utils/ChargementDonneesMonde.h"



using namespace std;
typedef unique_ptr<Character> Character_ptr;

WGameCore* wScreen;

int main (int argc, char *argv[])
{
    argc = argc;
    argv = argv;
	
	//freopen("stderr.txt", "w", stderr);

    int startPosx = 0, startPosy = 0, widthBlocks = 0, heightBlocks = 0;
    string startMapName, startMapChipsetName;

    ofstream flux("stdlog.txt", ios::trunc); //Reset du fichier "stdlog.txt"
    flux.close();
    srand((unsigned int)time(NULL));

    // Chargement de la vidéo, de l'audio et du texte
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        cerr << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }


	if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) )
		cerr <<  "Impossible d'initialiser SDL_image : " << IMG_GetError() << endl;

	

    if(TTF_Init() == -1)
    {
        cerr << "Erreur d'initialisation de TTF_Init : " << TTF_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    LoadGameCoreData(widthBlocks, heightBlocks);


	//(widthBlocks*TAILLEBLOC > TAILLEECRANMINX ? widthBlocks*TAILLEBLOC: TAILLEECRANMINX), (heightBlocks*TAILLEBLOC > TAILLEECRANMINY ? heightBlocks*TAILLEBLOC: TAILLEECRANMINY)
    WGameCore& wScreen = WGameCore::getInstance(); //Crée une fenetre de type "WGameCore", Génère ce monde sur la fenetre (unique)

    while(wScreen.refresh()); //boucle principale
    wScreen.quitter(false);

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

    return 0;

}









