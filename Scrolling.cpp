#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <memory>

#include "IDs.h"
#include "Scrolling.h"
#include "WGameCore.h"

using namespace std;

void Scrolling()
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
    SDL_Rect origineRelative;

    int x, y;
    x = wScreen.getEntityFactory().getTrainer()->getPos().x;
    y = wScreen.getEntityFactory().getTrainer()->getPos().y;

    if(wScreen.isScrollingActive())
    {
        origineRelative.x = -x + wScreen.getWidth()/2;
        origineRelative.y = -y + wScreen.getHeight()/2;

        if(origineRelative.x > 0)
            origineRelative.x = 0;
        else if(-origineRelative.x + (int)wScreen.getWidth() > w.getNbrBlocX()*TAILLEBLOC)
            origineRelative.x = wScreen.getWidth() - w.getNbrBlocX()* TAILLEBLOC;

        if(origineRelative.y > 0)
            origineRelative.y = 0;
        else if(-origineRelative.y + (int)wScreen.getHeight() > w.getNbrBlocY()*TAILLEBLOC)
            origineRelative.y = wScreen.getHeight() - w.getNbrBlocY()* TAILLEBLOC;

        wScreen.setORel(origineRelative);
    }
}
