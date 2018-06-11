#pragma once
#include "ECS/EntityManager.h"
#include "Point.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"

namespace ska {
	class PrefabEntityManager : 
		public EntityManager {
	public:
		explicit PrefabEntityManager(ska::GameEventDispatcher& ged);
		virtual ~PrefabEntityManager() = default;
		virtual EntityId createCharacter(Point<int> startPos, int spriteId, unsigned int worldBlockSize, const std::string& name = "");
		virtual EntityId createTrainer(Point<int> startPos, unsigned int worldBlockSize);

	};
}
