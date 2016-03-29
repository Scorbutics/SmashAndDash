#pragma once
#include "../ska/ECS/PrefabEntityManager.h"

class CustomEntityManager : public ska::PrefabEntityManager {
public:
	CustomEntityManager();
	virtual ska::EntityId createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize) override;
	virtual ~CustomEntityManager();
};

