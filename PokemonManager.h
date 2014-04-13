#ifndef DEF_POKEMON_MANAGER
#define DEF_POKEMON_MANAGER

#include <list>
#include "Player.h"

class WGameCore;


//PokemonManager permet de g�rer de fa�on simplifi�e les ajouts/modifications � la team Pok�mon et aux Pok�mon sauvages, ainsi que de les stocker.
//L'instance est unique et appartient au coeur du moteur de jeu (WGameCore)
class PokemonManager
{
    public:
    PokemonManager();
    void generateOpponent(WGameCore* wm); //G�n�re un Pok�mon sauvage en fonction du lieu o� se trouve le h�ros

    Character* getOpponent(); //Renvoie le Pok�mon sauvage g�n�r� al�atoirement
    Player* getPokemon(unsigned int index); //Renvoie le Pok�mon num�ro "index" de la team
    size_t getPokemonTeamSize();
    Player* getLastPokemon();
	Player* getFirstUsablePokemon();
	void useFirstUsablePokemon();

	void heal();
    void swapPokemonOrders(unsigned int index1, unsigned int index2); //Bouge deux Pok�mon dans la team
	void add(Character* pkmn);
	void add(int id);
	void remove(unsigned int index); //Supprime le Pok�mon num�ro "index" de la team
    ~PokemonManager();
	
    private:
	std::unique_ptr<Player>& getPokemonPtr(unsigned int index);
	

	std::list<std::unique_ptr<Player>> m_team;
    Character_ptr m_opponent;
};

#endif
