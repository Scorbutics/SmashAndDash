#ifndef DEF_ENTITY_MANAGER
#define DEF_ENTITY_MANAGER

#include "../ska/Inputs/Readers/IniReader.h"

class WGameCore;

class EntityFactory
{
    public:
		EntityFactory();
		~EntityFactory();

		Player* getTrainer();
		Character* getOpponent();
		Character* getNPC(int id, unsigned int number);
		std::list<Character*>& getCharacterList();
		size_t getNumberOfEntity(int id);												//nombre d'entités d'une même id
		size_t getNumberOfCharID();														//nombre total de personnages humains d'id différente
		size_t getNumberOfPkmnID();														//nombre total de pokémon d'id différente
	
	
		void createTrainer();
		Character* createOpponent(ska::IniReader* dataMob);
		void addNPC(int id, ska::Rectangle posEntity, string pathStringEntity);
		void addNPC(Character* npc);
		void remove(int id, unsigned entityNumber);
		void deleteAll();																//Détruit toutes les entités (à faire à chaque changement de monde (= téléportation))
																						// sinon on va se retrouver avec énormément d'entités inutiles en mémoire

		Character* setOpponent(Character* opponent);
		void setPokemon(Character* pkmn);

    private:
		void removeOpponent();
		void removePokemon();

		Character_ptr m_opponent, m_pkmn;
		std::unique_ptr<Player> m_trainer;
		std::vector<std::vector<Character_ptr>> m_pokemon;								//tableau contenant les entity d'ids positives (les pokémon)
		std::vector<std::vector<Character_ptr>> m_characters;							//tableau contenant les entity d'ids négatives (les humains)
		std::list<Character*> m_currentEntities;										//liste contenant les entités actuellement en mémoire
};

#endif
