#pragma once
#include "EntityManager.h"
#include "../Point.h"

namespace ska {
	class PrefabEntityManager : public EntityManager {
	public:
		PrefabEntityManager();
		virtual ~PrefabEntityManager();
		virtual EntityId createCharacter(const Point<int> startPos, const int id, const unsigned int worldBlockSize);
		virtual EntityId createTrainer(const Point<int> startPos, const unsigned int worldBlockSize);

	};
}
