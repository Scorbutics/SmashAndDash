#pragma once
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "Utils/SubObserver.h"

namespace ska {
	class CollisionSystem;
}

class SkillEntityCollisionResponse : 
	public ska::MovableNonCopyable,
	public ska::SubObserver<ska::CollisionEvent> {
public:
	SkillEntityCollisionResponse(PokemonGameEventDispatcher& ged, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse() = default;

private:
	bool onCollisionEvent(ska::CollisionEvent& e);

	PokemonGameEventDispatcher& m_ged;
	ska::EntityManager& m_entityManager;

};

