#ifndef DEF_POKEMON_MANAGER
#define DEF_POKEMON_MANAGER

#include <list>
#include "Player.h"

class WGameCore;


//PokemonManager permet de gérer de façon simplifiée les ajouts/modifications à la team Pokémon et aux Pokémon sauvages, ainsi que de les stocker.
//L'instance est unique et appartient au coeur du moteur de jeu (WGameCore)
class PokemonManager
{
    public:
    PokemonManager();
    void generateOpponent(WGameCore* wm); //Génère un Pokémon sauvage en fonction du lieu où se trouve le héros

    Character* getOpponent(); //Renvoie le Pokémon sauvage généré aléatoirement
    Player* getPokemon(unsigned int index); //Renvoie le Pokémon numéro "index" de la team
    size_t getPokemonTeamSize();
    Player* getLastPokemon();
	Player* getFirstUsablePokemon();
	void useFirstUsablePokemon();

	void heal();
    void swapPokemonOrders(unsigned int index1, unsigned int index2); //Bouge deux Pokémon dans la team
	void add(Character* pkmn);
	void add(int id);
	void remove(unsigned int index); //Supprime le Pokémon numéro "index" de la team
    ~PokemonManager();
	
    private:
	std::unique_ptr<Player>& getPokemonPtr(unsigned int index);
	

	std::list<std::unique_ptr<Player>> m_team;
    Character_ptr m_opponent;
};

#endif
