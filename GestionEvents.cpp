#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "IDs.h"
#include "LayerE.h"
#include "WGameCore.h"
#include "GestionEvents.h"
#include "ScriptRead.h"
#include "GUI.h"
#include "IniReader.h"

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

    for(i = i; i < w.getLayerEvent()->getNbrLignes(); i++)
    {
        if(w.getLayerEvent()->getBlocX(i) == ((posCentre.x - (posCentre.x%TAILLEBLOC))/TAILLEBLOC) && w.getLayerEvent()->getBlocY(i) == ((posCentre.y - (posCentre.y%TAILLEBLOC))/TAILLEBLOC) && (w.getLayerEvent()->getTrigger(i) == 1))
        {

            if(w.getLayerEvent()->getAction(i) == "teleport")
            {
                TeleportHeroToMap(w.getLayerEvent()->getParam(i));
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

                 ScriptRead(w.getLayerEvent()->getParam(i));
            }
        }


    }
    return 0;
}
















string ExtractTo(int depart, std::string chaine, char jusquaCeCaractere, int *entier)
{
    size_t pos = chaine.substr(depart,chaine.size()).find_first_of(jusquaCeCaractere);
    string result = "ERROR";
    if(pos != string::npos)
    {
        result = chaine.substr(depart, pos);
        if(entier != NULL)
            *entier = atoi(result.c_str());
    }

    return result;
}














void TeleportHeroToMap(string param)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	Character* hero = wScreen.getHero();
	
    string fichier, fichier2, fichierD;
    int x = 1, y;


    fichier = ExtractTo(0, param, '/');
    fichierD = ExtractTo((int)strlen(fichier.c_str())+1, param, '/');
    fichier2 = ExtractTo((int)strlen(fichier.c_str())+(int)strlen(fichierD.c_str())+2, param, '/');
    ExtractTo((int)strlen(fichier.c_str())+(int)strlen(fichierD.c_str())+(int)strlen(fichier2.c_str())+3, param, ':', &x);

    // créer un flux de sortie
    ostringstream oss;
    oss << x; //on ecrit la valeur de l'entier x dedans
    string buf; //on le transforme en string
    buf = oss.str();
    oss.clear();
    y = atoi(param.substr(param.find_last_of(':')+1, param.find_last_of('\0')).c_str());

    //wScreen.transition(0);

    buf = ".\\Levels"FILE_SEPARATOR""
    + fichier.substr(0, fichier.find_last_of("."))
    + ""FILE_SEPARATOR""
    + fichier.substr(0, fichier.find_last_of("."))
    + ".ini";

	IniReader mapReader(buf);

    string chipsetName = mapReader.getString("Chipset file");

    if(chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA" )
        cerr << "Erreur : impossible de trouver le nom du chipset de la map de depart" << endl;

    w.changeLevel(fichier, chipsetName);
	
    hero->teleport(x*TAILLEBLOC, y*TAILLEBLOC);

}


