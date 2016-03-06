#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <fstream>

#include "../Utils\IDs.h"
#include "Chargemententitees.h"
#include "../Gameplay/WGameCore.h"
#include "../ska/World/LayerE.h"
#include "../ska/Graphic/Rectangle.h"

using namespace std;


void LoadEntities()
{
	ska::Rectangle posEntityId;
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();


    //Suppression des anciennes entités
    wScreen.getEntityFactory().deleteAll();

    //Chargement des NPC sur la map (personnages & pokémon)
    for(int i = 1; i < w.getLayerEvent()->getNbrLignes(); i++)
    {
        posEntityId.y = w.getLayerEvent()->getBlocY(i) * TAILLEBLOC;
        posEntityId.x = w.getLayerEvent()->getBlocX(i) * TAILLEBLOC;
        int id = w.getLayerEvent()->getID(i);
        if(abs(id) <= ENTITEMAX)
                wScreen.getEntityFactory().addNPC(id, posEntityId, w.getLayerEvent()->getPath(i));
        else
            cerr << "Erreur (fonction LoadEntities) : Impossible de lire l'ID de l'entité ligne " << i << endl;

    }


    //Chargement des sprites de l'équipe pokémon
    const size_t teamSize = wScreen.getPokemonManager().getPokemonTeamSize();
    for(unsigned int i = 0; i < teamSize; i++)
    {
        wScreen.getPokemonManager().getPokemon(i)->setID(wScreen.getPokemonManager().getPokemon(i)->getID());
        wScreen.getPokemonManager().getPokemon(i)->setDirection(0);
    }
}
