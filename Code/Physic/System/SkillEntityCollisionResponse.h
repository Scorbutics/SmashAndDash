#pragma once
#include "../../ska/Physic/System/EntityCollisionResponse.h"

template <typename T>
struct RawStatistics;

namespace ska {
	class CollisionSystem;
}

using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

class SkillEntityCollisionResponse : public ska::EntityCollisionResponse, public StatisticsChangeObservable {
public:
	SkillEntityCollisionResponse(ska::CollisionSystem& colSys, ska::GameEventDispatcher& w, ska::EntityManager& entityManager);
	SkillEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onEntityCollision, ska::GameEventDispatcher& w, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse();
	bool onEntityCollision(ska::CollisionEvent& e);
private:
	ska::CollisionSystem& m_collisionSystem;

};

