#include "IDs.h"
#include "GestionAnimations.h"
#include "WGameCore.h"
#include "Layer.h"

#define COMPTEUR_ANIM 26

using namespace std;


SDL_Rect TileAnimationManagement(Layer *monde, SDL_Rect posMap, SDL_Rect &OfChip)
{
   //Uint32 /*brightColor = SDL_MapRGB(screen->format, 170, 170, 170),*/ darkColor = SDL_MapRGB(screen->format, 70, 70, 70);
	WGameCore& wScreen = WGameCore::getInstance();
    if(monde->getBlock(posMap.x/TAILLEBLOC, posMap.y/TAILLEBLOC)->getProperties()) //si on trouve un tile animé (caractérisé par une de deux couleurs dans le fichier col du monde)
    {
        wScreen.getChipsetAnimation().setOffsetAndFrameSize(OfChip);//envoie l'"image" courante à animer (plus exactement la localisation de cette image dans le chipset.png)
        OfChip = wScreen.getChipsetAnimation().getRectOfCurrentFrame(); //récupere l'animation courante correspondante à l'image
    }

    return OfChip;
}
