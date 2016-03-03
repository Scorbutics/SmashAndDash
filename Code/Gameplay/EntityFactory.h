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
		size_t getNumberOfEntity(int id);												//nombre d'entit�s d'une m�me id
		size_t getNumberOfCharID();														//nombre total de personnages humains d'id diff�rente
		size_t getNumberOfPkmnID();														//nombre total de pok�mon d'id diff�rente
	
	
		void createTrainer();
		Character* createOpponent(ska::IniReader* dataMob);
		void addNPC(int id, ska::Rectangle posEntity, string pathStringEntity);
		void addNPC(Character* npc);
		void remove(int id, unsigned entityNumber);
		void deleteAll();																//D�truit toutes les entit�s (� faire � chaque changement de monde (= t�l�portation))
																						// sinon on va se retrouver avec �norm�ment d'entit�s inutiles en m�moire

		Character* setOpponent(Character* opponent);
		void setPokemon(Character* pkmn);

    private:
		void removeOpponent();
		void removePokemon();

		Character_ptr m_opponent, m_pkmn;
		std::unique_ptr<Player> m_trainer;
		std::vector<std::vector<Character_ptr>> m_pokemon;								//tableau contenant les entity d'ids positives (les pok�mon)
		std::vector<std::vector<Character_ptr>> m_characters;							//tableau contenant les entity d'ids n�gatives (les humains)
		std::list<Character*> m_currentEntities;										//liste contenant les entit�s actuellement en m�moire
};

#endif
