#include "IDs.h"
#include "GetIDmob.h"
#include "WGameCore.h"

using namespace std;

SDL_Rect GetIDmob()
{
    vector<SDL_Rect> ids;
	WGameCore& wScreen = WGameCore::getInstance();
    SDL_Rect hitbox = wScreen.getHero()->getHitbox();
    int direction = wScreen.getHero()->getDirection();

    ids = wScreen.detectEntity(hitbox, direction);

    if(!ids.empty())
    {
		//On vérifie que ce n'est pas le héros qui est détecté en premier
		if (ids[0].x != 0)
		{
			SDL_Rect mobPos = wScreen.getEntityFactory().getNPC(ids[0].x, ids[0].y)->getPos();
			SDL_Rect heroPos = wScreen.getHero()->getPos();
			return ids[0];
		}
		else if (ids.size() >= 2) //Si c'est le héros en premier, on prend le deuxième élément, qui n'est donc pas le héros
		{
			SDL_Rect mobPos = wScreen.getEntityFactory().getNPC(ids[1].x, ids[1].y)->getPos();
			SDL_Rect heroPos = wScreen.getHero()->getPos();
			return ids[1];
		}
    }
	SDL_Rect idmob;
	idmob.x = -ENTITEMAX;
	idmob.y = -ENTITEMAX;
	idmob.w = 0;
	idmob.h = 0;
    return idmob;


    /*SDL_Rect chd, chg, cbd, cbg, buf;

    chd.x = wScreen.getHero()->getPos().x + wScreen.getHero()->getWidth()-1 - wScreen.getHero()->getOffset(1);
    chd.y = wScreen.getHero()->getPos().y + wScreen.getHero()->getOffset(2);

    chg.x = wScreen.getHero()->getPos().x + wScreen.getHero()->getOffset(3);
    chg.y = wScreen.getHero()->getPos().y + wScreen.getHero()->getOffset(2);

    cbd.x = wScreen.getHero()->getPos().x + wScreen.getHero()->getWidth()-1 - wScreen.getHero()->getOffset(1);
    cbd.y = wScreen.getHero()->getPos().y + wScreen.getHero()->getHeight()-1 - wScreen.getHero()->getOffset(0);

    cbg.x = wScreen.getHero()->getPos().x + wScreen.getHero()->getOffset(3);
    cbg.y = wScreen.getHero()->getPos().y + wScreen.getHero()->getHeight()-1 - wScreen.getHero()->getOffset(0);*/


/*  if(direction == 1 || wScreen.getHero()->getDirection() == 5)
    {
        buf = DetectNPC(wScreen, chd.x+speed, chd.y, &(*wScreen.getHero()) );

        if(buf.x == -ENTITEMAX)
            buf = DetectNPC(wScreen, cbd.x+speed, cbd.y, &(*wScreen.getHero()));

        if(buf.x != -ENTITEMAX)
            wScreen.getEntityFactory().getNPC(buf.x, buf.y)->setDirection(3);
    }
    else if (wScreen.getHero()->getDirection() == 3 || wScreen.getHero()->getDirection() == 7)
    {
        buf = DetectNPC(wScreen, chg.x-speed, chg.y, &(*wScreen.getHero()));

        if(buf.x == -ENTITEMAX)
            buf = DetectNPC(wScreen, cbg.x-speed, cbg.y, &(*wScreen.getHero()));

        if(buf.x != -ENTITEMAX)
            wScreen.getEntityFactory().getNPC(buf.x, buf.y)->setDirection(1);
    }
    else if (wScreen.getHero()->getDirection() == 0 || wScreen.getHero()->getDirection() == 4)
    {
        buf = DetectNPC(wScreen, cbg.x, cbg.y+speed, &(*wScreen.getHero()));

        if(buf.x == -ENTITEMAX)
            buf = DetectNPC(wScreen, cbd.x, cbd.y+speed, &(*wScreen.getHero()));

        if(buf.x != -ENTITEMAX)
            wScreen.getEntityFactory().getNPC(buf.x, buf.y)->setDirection(2);
    }
    else if (wScreen.getHero()->getDirection() == 2 || wScreen.getHero()->getDirection() == 6)
    {
        buf = DetectNPC(wScreen, chg.x, chg.y-speed, &(*wScreen.getHero()));

        if(buf.x == -ENTITEMAX)
            buf = DetectNPC(wScreen, chd.x, chd.y-speed, &(*wScreen.getHero()));

        if(buf.x != -ENTITEMAX)
            wScreen.getEntityFactory().getNPC(buf.x, buf.y)->setDirection(0);
    }*/


}
