#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/EntityCollisionResponse.h"

template <typename T>
struct RawStatistics;

using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

class SkillEntityCollisionResponse : public ska::EntityCollisionResponse, public StatisticsChangeObservable {
public:
	SkillEntityCollisionResponse(ska::CollisionSystem& w, ska::EntityManager& entityManager);
	SkillEntityCollisionResponse(std::function<void(const ska::CollisionEvent&, ska::CollisionComponent&, const ska::CollidableComponent&)> onEntityCollision, ska::CollisionSystem& w, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse();
	void onEntityCollision(const ska::CollisionEvent& e, ska::CollisionComponent& col, const ska::CollidableComponent& cc);

};

