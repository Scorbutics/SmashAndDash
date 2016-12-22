#pragma once
#include "../../ska/Physic/System/EntityCollisionResponse.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

template <typename T>
struct RawStatistics;

namespace ska {
	class CollisionSystem;
}

class SkillEntityCollisionResponse : public ska::EntityCollisionResponse {
public:
	SkillEntityCollisionResponse(ska::CollisionSystem& colSys, PokemonGameEventDispatcher& ged, ska::EntityManager& entityManager);
	SkillEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onEntityCollision, PokemonGameEventDispatcher& ged, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse();
	bool onEntityCollision(ska::CollisionEvent& e);
private:
	ska::CollisionSystem& m_collisionSystem;
	PokemonGameEventDispatcher& m_ged;

};

