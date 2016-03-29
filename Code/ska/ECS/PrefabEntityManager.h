#pragma once
#include "EntityManager.h"
#include "../Graphic/Point.h"

namespace ska {
	class PrefabEntityManager : public EntityManager {
	public:
		PrefabEntityManager();
		virtual ~PrefabEntityManager();
		virtual ska::EntityId createCharacter(const ska::Point<int> startPos, const int id, const unsigned int worldBlockSize);
		virtual ska::EntityId createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize);

	};
}
