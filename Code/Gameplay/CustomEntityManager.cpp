#include "CustomEntityManager.h"
#include "./Mobs/MobSpawnAreaComponent.h"

CustomEntityManager::CustomEntityManager()
{
}

ska::EntityId CustomEntityManager::createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize) {
	ska::EntityId trainer = ska::PrefabEntityManager::createTrainer(startPos, worldBlockSize);
	addComponent<MobSpawnAreaComponent>(trainer, MobSpawnAreaComponent());
	return trainer;
}

CustomEntityManager::~CustomEntityManager()
{
}
