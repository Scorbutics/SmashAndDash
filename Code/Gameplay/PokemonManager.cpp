#include <stdint.h>
#include <sstream>
#include "PokemonManager.h"
#include "Data/Statistics.h"
#include "../Utils/IDs.h"
#include "../ska/AI/Path.h"



PokemonManager::PokemonManager()
{
    //m_opponent = NULL;
}


/*
unique_ptr<Player>& PokemonManager::getPokemonPtr(unsigned int index)
{
	list<unique_ptr<Player>>::iterator iter = m_team.begin();
    for(unsigned int i = 0; i < index && iter != m_team.end(); ++iter, i++);

    if(*iter == NULL)
    {
        cerr << "Erreur (classe PokemonManager) : Impossible de trouver le Pokémon numéro " << index << " de l'équipe" << endl;
        return *m_team.begin();
    }

	return (unique_ptr<Player>&) (*iter);
}

void PokemonManager::heal()
{
	for (list<unique_ptr<Player>>::iterator iter = m_team.begin(); iter != m_team.end(); ++iter)
		(*iter)->setHP((*iter)->getStatistics()->getHpMax());
	
}

Player* PokemonManager::getFirstUsablePokemon()
{
	list<unique_ptr<Player>>::iterator iter;
	for(iter = m_team.begin(); iter != m_team.end() && (*iter)->getHp() <= 0; ++iter);

	if(iter == m_team.end())
		return NULL;

	return &(**iter);

}

Player* PokemonManager::getPokemon(unsigned int index)
{
	unique_ptr<Player>& pkmn = getPokemonPtr(index);
	return &(*pkmn);
}

Player* PokemonManager::getLastPokemon()
{
	list<unique_ptr<Player>>::iterator iterEnd = m_team.end();
    iterEnd--;

    if(*iterEnd == NULL)
    {
        cerr << "Erreur (classe PokemonManager) : Impossible de trouver le dernier Pokémon de l'équipe" << endl;
        return NULL;
    }

    return &(*((unique_ptr<Player>&) (*iterEnd)));
}

size_t PokemonManager::getPokemonTeamSize()
{
    return m_team.size();
}

void PokemonManager::swapPokemonOrders(unsigned int index1, unsigned int index2)
{
    list<unique_ptr<Player>>::iterator iter1 = m_team.begin(), iter2 = m_team.begin();
    for(unsigned int i = 0; i < index1 && iter1 != m_team.end(); iter1++, i++);
    for(unsigned int i = 0; i < index2 && iter2 != m_team.end(); iter2++, i++);
    if(iter1 != iter2)
        iter_swap(iter1, iter2);
}

void PokemonManager::add(int id)
{
    if(id != 0) //0 est réservée
    {
        list<unique_ptr<Player>>::iterator iterEnd;
        m_team.push_back(unique_ptr<Player>(new Player(id)));
        iterEnd = m_team.end();
        iterEnd--;

        if(*iterEnd == NULL)
            cerr << "Erreur (classe PokemonManager) : Impossible d'ajouter un Pokémon à l'équipe" << endl;

        (*iterEnd)->setEntityNumber(ID_CURRENT_POKEMON);
        (*iterEnd)->getPath()->setPathString("");

    }
    else
        cerr << "Erreur (classe PokemonManager) : Id 0 réservée" << endl;
}

void PokemonManager::add(Character* pkmn)
{
	if(pkmn == NULL)
		cerr << "Erreur (classe PokemonManager) : Tentative d'ajout d'un Pokémon NULL à l'équipe" << endl;
	else
	{
		m_team.push_back(unique_ptr<Player>(new Player(pkmn->getID())));
		Player* lastPkmn = getLastPokemon();
		lastPkmn->getPath()->setPathString("");
		lastPkmn->setHP(pkmn->getHp());
		lastPkmn->setSpeedLimit(pkmn->getSpeedLimit());
		lastPkmn->setDirection(pkmn->getDirection());
		lastPkmn->setEntityNumber(ID_CURRENT_POKEMON);
		lastPkmn->getStatistics()->setExperience(pkmn->getStatistics()->getExperience());
	}

}

void PokemonManager::remove(unsigned int index)
{
	list<unique_ptr<Player>>::iterator iter = m_team.begin();
    for(unsigned int i = 0; i < index && iter != m_team.end(); iter++, i++);
    if(iter != m_team.end())
        m_team.erase(iter);
    else
        cerr << "Erreur (classe PokemonManager) : Impossible de supprimer le Pokémon numéro " << index << " de l'équipe" << endl;
}
*/
PokemonManager::~PokemonManager()
{
}
/*
void PokemonManager::useFirstUsablePokemon()
{
	unsigned int index;
	list<unique_ptr<Player>>::iterator iter;
	for(iter = m_team.begin(), index = 0; iter != m_team.end() && (*iter)->getHp() <= 0; ++iter, index++);

	if(iter != m_team.end())
		swapPokemonOrders(0, index);
	
}*/

