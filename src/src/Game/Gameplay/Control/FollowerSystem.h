#pragma once
#include <deque>
#include <optional>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "GridMovementComponent.h"

class CustomEntityManager;
class WorldState;

// The Pokemon of the team that follows the player on the map : the first one able to fight (the one the battles send).
// It walks the trail of the player (its positions, its hops), Distance pixels behind it : it never cuts a corner, nor
// gets stuck. A drawing only : no physics body (it blocks nothing, it touches no wild Pokemon), no scripts.
// - it comes out (a flash, a pop) once the player walked Distance away from where it appeared : after a map change, a
//   door, a battle (it starts on the player)
// - it faces the player when they stand still
// - away while the player surfs (its sprite bobs on the water : AnimationComponent::animatedWhenStill), during a battle
//   (StateFight : the same Pokemon comes out of its pokeball there), without a Pokemon able to fight
// Its entity is named Name ; the map loads remove it, it is made again.
class FollowerSystem :
	public ska::System<ska::RequiredComponent<GridMovementComponent, ska::PositionComponent, ska::AnimationComponent>, ska::PossibleComponent<>> {
public:
	FollowerSystem(CustomEntityManager& entityManager, WorldState& worldState);
	~FollowerSystem() override = default;

	static constexpr const char* Name = "follower";
	static constexpr float Distance = 56.F;
	// Its Pokemon is read again from the team this often (ms) : a capture, a heal, a K.O.
	static constexpr unsigned int TeamCheck = 2000;
	static constexpr unsigned int AppearTime = 220;
	// Still this long (ms) : it turns to the player
	static constexpr unsigned int FaceAfter = 500;

	// Away at once (ex : a battle starts)
	static void dismiss(CustomEntityManager& entityManager, WorldState& worldState);

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	struct TrailPoint {
		float x = 0.F;
		float y = 0.F;
		long z = 0;
	};

	// The monster id of the Pokemon that follows (none : no follower)
	std::optional<int> lead();
	// The point of the trail "Distance" behind the player (along it), none when the trail is shorter
	std::optional<TrailPoint> behind();
	void restart(const ska::PositionComponent& player);
	ska::EntityId make(int species, const TrailPoint& at);
	void animate(ska::EntityId follower, const TrailPoint& at, const ska::PositionComponent& player, unsigned int ellapsedTime);

	CustomEntityManager& m_cem;
	WorldState& m_worldState;
	std::deque<TrailPoint> m_trail;
	std::optional<int> m_species;
	unsigned int m_teamCheck = 0;
	bool m_checked = false;
	unsigned int m_appear = 0;
	unsigned int m_still = 0;
	TrailPoint m_last;
};
