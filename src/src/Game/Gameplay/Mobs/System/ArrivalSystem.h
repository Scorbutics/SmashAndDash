#pragma once
#include <functional>
#include <vector>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Graphic/GraphicComponent.h"
#include "../ArrivalComponent.h"

// Moves the wild Pokemon arriving on the map or leaving it (ArrivalComponent) : a glide down from the sky, a hop out of a
// hideout, a flight away ; facing where it goes, its wings beating (its walk frames). Arrived, "arrived" makes it a wild
// Pokemon (WorldState::wildArrived) ; the drawing is removed.
class ArrivalSystem : public ska::System<ska::RequiredComponent<ArrivalComponent, ska::PositionComponent, ska::GraphicComponent>, ska::PossibleComponent<>> {
public:
	using Arrived = std::function<void(const ArrivalComponent&)>;
	ArrivalSystem(ska::EntityManager& entityManager, Arrived arrived);
	~ArrivalSystem() override = default;

	// The hideouts that rustle (a Pokemon waits in it before it comes out) : the block, the time it has rustled (ms)
	struct Rustle {
		ska::Point<int> block;
		unsigned int age;
	};
	std::vector<Rustle> rustles() const;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	Arrived m_arrived;
};
