#include "CustomEntityManager.h"
#include "./Fight/FightComponent.h"
#include "./Mobs/MobSpawnAreaComponent.h"

CustomEntityManager::CustomEntityManager()
{
}

ska::EntityId CustomEntityManager::createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize) {
	ska::EntityId trainer = ska::PrefabEntityManager::createTrainer(startPos, worldBlockSize);
	addComponent<MobSpawnAreaComponent>(trainer, MobSpawnAreaComponent());
	addComponent<FightComponent>(trainer, FightComponent());
	return trainer;
}

ska::EntityId CustomEntityManager::createCharacter(const ska::Point<int> startPos, const int id, const unsigned int worldBlockSize) {
	return ska::PrefabEntityManager::createCharacter(startPos, id, worldBlockSize);
}

CustomEntityManager::~CustomEntityManager()
{
}
