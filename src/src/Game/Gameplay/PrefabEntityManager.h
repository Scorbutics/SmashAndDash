#pragma once
#include "Core/ECS/EntityManager.h"
#include "Core/Point.h"
#include "Core/Data/Events/ExtensibleGameEventDispatcher.h"

namespace ska {
	class PrefabEntityManager : 
		public EntityManager {
	public:
		explicit PrefabEntityManager(ska::GameEventDispatcher& ged);
		PrefabEntityManager(PrefabEntityManager&&) = default;
		PrefabEntityManager& operator=(PrefabEntityManager&&) = default;
		virtual ~PrefabEntityManager() = default;
		virtual EntityId createCharacter(Point<int> startPos, int spriteId, unsigned int worldBlockSize, const std::string& name = "");
		virtual EntityId createTrainer(Point<int> startPos, unsigned int worldBlockSize);

	};
}
