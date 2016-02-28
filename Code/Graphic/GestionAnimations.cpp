#include "../Utils\IDs.h"
#include "GestionAnimations.h"
#include "../Gameplay\WGameCore.h"
#include "../Gameplay\World\Layer.h"

#define COMPTEUR_ANIM 26

using namespace std;


ska::Rectangle TileAnimationManagement(Layer *monde, ska::Rectangle posMap, ska::Rectangle &OfChip)
{
   //Uint32 /*brightColor = SDL_MapRGB(screen->format, 170, 170, 170),*/ darkColor = SDL_MapRGB(screen->format, 70, 70, 70);
	WGameCore& wScreen = WGameCore::getInstance();
    if(monde->getBlock(posMap.x/TAILLEBLOC, posMap.y/TAILLEBLOC)->getProperties()) //si on trouve un tile anim� (caract�ris� par une de deux couleurs dans le fichier col du monde)
    {
        wScreen.getChipsetAnimation().setOffsetAndFrameSize(OfChip);//envoie l'"image" courante � animer (plus exactement la localisation de cette image dans le chipset.png)
        OfChip = wScreen.getChipsetAnimation().getRectOfCurrentFrame(); //r�cupere l'animation courante correspondante � l'image
    }

    return OfChip;
}
