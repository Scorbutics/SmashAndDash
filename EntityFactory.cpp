#include <stdint.h>
#include "WGameCore.h"
#include "EntityFactory.h"

using namespace std;

EntityFactory::EntityFactory()
{
    this->createTrainer();
	m_pokemon.resize(ENTITEMAX);
	m_characters.resize(ENTITEMAX);
}

void EntityFactory::createTrainer()
{
	WGameCore& wScreen = WGameCore::getInstance();

    m_trainer = unique_ptr<Player>(new Player(0));
    m_trainer->setEntityNumber(0);
    m_trainer->setOffset(0, 0);
    m_trainer->setOffset(1, 20);
    m_trainer->setOffset(2, 27);
    m_trainer->setOffset(3, 20);
	wScreen.getInputListener().addObserver(&(*m_trainer));

	
}


Character* EntityFactory::createOpponent(IniReader* dataMob)
{

    m_opponent = Character_ptr(new Character(dataMob->getInt("Data id")));

    if(m_opponent == NULL)
        cerr << "Erreur (classe PokemonManager) : Impossible de générer un adversaire de combat" << endl;

    stringstream ss;
    m_opponent->setEntityNumber(INT16_MAX);
    m_opponent->teleport(m_trainer->getPos().x+ TAILLEBLOC, m_trainer->getPos().y);
	m_opponent->setSpeedLimit(6);

    ss << P_RANDOM;
    m_opponent->getPath()->setPathString(ss.str());
    m_opponent->setOffset(0, 0);
    m_opponent->setOffset(1, 20);
    m_opponent->setOffset(2, 37);
    m_opponent->setOffset(3, 20);

	return &(*m_opponent);

}

Character* EntityFactory::getOpponent()
{
    return &(*m_opponent);
}

Player* EntityFactory::getTrainer()
{
    return &(*m_trainer);
}

void EntityFactory::addNPC(Character* npc)
{

	if(npc == NULL)
		return;

	int id = npc->getID();
	unsigned int i;
	vector<vector<Character_ptr>>* target;

	if(id == 0)
	{
		cerr << "Erreur (classe EntityFactory) : Id numéro " << id << " réservée" << endl;
		return;
	}

	if(abs(id) >= ENTITEMAX)
	{
		cerr << "Erreur (EntityFactory) : ajout d'un pokémon d'id inexistante" << endl;
		return;
	}


	if(id > 0)
		target = &m_pokemon;
	else
		target = &m_characters;
	
	id = abs(id);

	for(i = 0; i < (*target)[id].size() && (*target)[id][i] != NULL; i++);
	if(i == (*target)[id].size())
		(*target)[id].push_back(Character_ptr(new Character(target == &m_pokemon ? id : -id)));
	else
		(*target)[id][i] = Character_ptr(new Character(target == &m_pokemon ? id : -id));
	
	(*target)[id][i]->setSpeedLimit(npc->getSpeedLimit());
	(*target)[id][i]->setDirection(npc->getDirection());
	(*target)[id][i]->setID(target == &m_pokemon ? id : -id);
	(*target)[id][i]->setEntityNumber(i);
	(*target)[id][i]->teleport(npc->getPos().x, npc->getPos().y);

	if(npc->getPath()->isMotionless())
		(*target)[id][i]->getPath()->setPathString(intToStr(P_FIXED));
	else if((npc->getPath()->isRandom()))
		(*target)[id][i]->getPath()->setPathString(intToStr(P_RANDOM));
	else
		(*target)[id][i]->getPath()->setPathString(npc->getPath()->getPathString());

	for(unsigned int j = 0; j < 4; j++)
		(*target)[id][i]->setOffset(j, npc->getOffset(j));

	m_currentEntities.push_front(&(*(*target)[id][i]));
}

void EntityFactory::addNPC(int id, SDL_Rect posEntity, string pathStringEntity)
{

	unique_ptr<Character> npc = unique_ptr<Character>(new Character(id));

	npc->teleport(posEntity.x, posEntity.y);
	npc->getPath()->setPathString(pathStringEntity);
	npc->setDirection(0);
	npc->setSpeedLimit(6);

    if(id > 0)
    {
		npc->setOffset(0, 3);
		npc->setOffset(1, 20);
		npc->setOffset(2, 37);
		npc->setOffset(3, 20);

    }
    else
    {
        npc->setOffset(0, 0);
        npc->setOffset(1, 20);
        npc->setOffset(2, 27);
        npc->setOffset(3, 20);
    }

	addNPC(&(*npc));

}

list<Character*>& EntityFactory::getNPCList()
{
	return m_currentEntities;
}

Character* EntityFactory::getNPC(int id, unsigned int number)
{
    //unsigned int i;
	WGameCore& wScreen = WGameCore::getInstance();

    if(id == 0)
        return &(*m_trainer);
    else if(number == INT16_MAX)
        return wScreen.getFight().getOpponent();
    else if(number == INT16_MAX-1)
		return wScreen.getFight().getPokemon();
     


    if(id > 0)
    {
        if((unsigned int) id < m_pokemon.size() && number < m_pokemon[id].size())
			if(m_pokemon[id][number] != NULL)
				return &(*m_pokemon[id][number]);
			else
				return NULL;
        else
        {
            cerr << "Erreur (classe EntityFactory) : Dépassement mémoire lors de la tentative d'accès au pokémon numéro " << number << " d'ID n°" << -id << endl;
            return &(*m_pokemon[0][0]);
        }
    }
    else
    {
        if((unsigned int)(-id) < m_characters.size() && number < m_characters[-id].size())
			if(m_characters[-id][number] != NULL)
				return &(*m_characters[-id][number]);
			else
				return NULL;
        else
        {
            cerr << "Erreur (classe EntityFactory) : Dépassement mémoire lors de la tentative d'accès au personnage numéro " << number << " d'ID en valeur absolue n°" << -id << endl;
            return &(*m_characters[0][0]);
        }
    }
}

size_t EntityFactory::getNumberOfEntity(int id)
{

    if(id > 0)
    {
        if((unsigned int)id < m_pokemon.size())
            return m_pokemon[id].size();
        else
            return 0;
    }
    else
    {
        if((unsigned int)(-id) < m_characters.size())
            return m_characters[-id].size();
        else
            return 0;
    }
}

void EntityFactory::deleteAll()
{
    for(unsigned int i = 0; i < m_pokemon.size(); i++)
        m_pokemon[i].clear();

    for(unsigned int i = 0; i < m_characters.size(); i++)
        m_characters[i].clear();

	m_currentEntities.clear();
}

EntityFactory::~EntityFactory()
{
    this->deleteAll();
}

void EntityFactory::remove(int id, unsigned number)
{
	if(id == 0)
	{
		cerr << "Erreur (classe EntityFactory) : ID 0 réservée, impossible de supprimer le personnage correspondant." << endl;
		return;
	}

	if(abs(id) >= ENTITEMAX)
	{
		cerr << "Erreur (classe EntityFactory) : ID " << id << " inexistante, cette id dépasse les " << ENTITEMAX << " ids autorisées." << endl;
		return;
	}

	if(number == INT16_MAX || number == INT16_MAX-1)
		return;

	if(id > 0)
	{
		if ((size_t)id < m_pokemon.size() && number < m_pokemon[id].size())
		{
			if (m_pokemon[id][number] != NULL)
			{ 
				std::list<Character*>::iterator iter = std::find(m_currentEntities.begin(), m_currentEntities.end(), &(*m_pokemon[id][number]));

				if (iter != m_currentEntities.end())
					m_currentEntities.remove(&(*m_pokemon[id][number]));
				m_pokemon[id][number] = NULL;
			}
		}
		else
			cerr << "Erreur (classe EntityFactory) : Dépassement mémoire lors de la tentative d'accès au pokémon numéro " << number << " d'ID n°" << -id << endl;
		
	}
	else
	{
		if ((size_t)(-id) < m_characters.size() && number < m_characters[-id].size())
		{
			if (m_characters[-id][number] != NULL)
			{
				std::list<Character*>::iterator iter = std::find(m_currentEntities.begin(), m_currentEntities.end(), &(*m_characters[-id][number]));

				if (iter != m_currentEntities.end())
					m_currentEntities.remove(&(*m_characters[-id][number]));
				m_characters[-id][number] = NULL;
			}
		}
		else
			cerr << "Erreur (classe EntityFactory) : Dépassement mémoire lors de la tentative d'accès au personnage numéro " << number << " d'ID en valeur absolue n°" << -id << endl;
		
	}
}
