#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "../Utils\IDs.h"
#include "WGameCore.h"
#include "../Script/Commands/CommandTeleport.h"
#include "World/LayerE.h"
#include "../Script/ScriptDispatcher.h"

using namespace std;

//posHero : position coin haut gauche du hero
//monde : le monde courant des evenements
//autotrigger : 0: presser une touche 1: sous le heros 2: au chargement de la map
int GestionEvents()
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

    if(wScreen.getFight().isFighting())
        return 0;

    SDL_Rect posCentre;
    int i = 0;

    posCentre.x = wScreen.getHero()->getHitboxCenterPos().x;
    posCentre.y = wScreen.getHero()->getHitboxCenterPos().y;
	posCentre.x /= TAILLEBLOC;
	posCentre.y /= TAILLEBLOC;

    for(i = i; i < w.getLayerEvent()->getNbrLignes(); i++)
    {
		if (w.getLayerEvent()->getBlocX(i) == posCentre.x && w.getLayerEvent()->getBlocY(i) == posCentre.y && (w.getLayerEvent()->getTrigger(i) == 1))
        {

            if(w.getLayerEvent()->getAction(i) == "teleport")
            {
                CommandTeleport::teleportHeroToMap(w.getLayerEvent()->getParam(i));
                return 1;

            }
            else if(w.getLayerEvent()->getAction(i) == "script")
            {
                ifstream fscript(w.getLayerEvent()->getParam(i).c_str());
                char buf;
                fscript.seekg(-1, ios::end);
                fscript >> buf;
                fscript.seekg(0, ios::beg);
                fscript.close();

                if(buf == '£')
                {
                    ofstream fwriteScript(w.getLayerEvent()->getParam(i).c_str());
                    fwriteScript.seekp(-1, ios::end);
                    fwriteScript << "";
                    fwriteScript.close();
                }
				Uint32 scriptPeriod = 1000;
				ScriptDispatcher::getInstance().addRunningScript(NULL, w.getLayerEvent()->getParam(i), "", 2, &scriptPeriod);
            }
        }


    }
    return 0;
}

