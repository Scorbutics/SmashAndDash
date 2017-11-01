#pragma once
#include "ECS/EntityManager.h"
#include "Point.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"

namespace ska {
	class PrefabEntityManager : public EntityManager {
	public:
		explicit PrefabEntityManager(ska::GameEventDispatcher& ged);
		virtual ~PrefabEntityManager();
		virtual EntityId createCharacter(const Point<int> startPos, const int id, const unsigned int worldBlockSize);
		virtual EntityId createTrainer(const Point<int> startPos, const unsigned int worldBlockSize);

	};
}
