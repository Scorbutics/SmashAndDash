#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Graphic/HasShadowComponent.h"

class WorldState;

// The characters hidden behind the objects of the map (a tree, a roof, a building : the tiles standing on a base line,
// drawn over the characters behind it) : their silhouette is drawn over the world (GraphicComponent::silhouette), a
// fighter behind a tree stays seen. The more of its body is covered, the more its silhouette shows (a post barely).
class SilhouetteSystem : public ska::System<ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::GraphicComponent, ska::HasShadowComponent>, ska::PossibleComponent<>> {
public:
	SilhouetteSystem(ska::EntityManager& entityManager, WorldState& worldState);
	SilhouetteSystem& operator=(const SilhouetteSystem&) = delete;
	~SilhouetteSystem() override = default;

	// The opacity of a silhouette when the body is hidden enough ; the part of the body hidden from which it shows, and
	// from which it is full
	static constexpr int Opacity = 110;
	static constexpr float ShowFrom = 0.3F;
	static constexpr float FullFrom = 0.6F;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	// The part (0 .. 1) of a body (world pixels) covered by the objects drawn over a character sorted at "ground"
	float covered(const ska::Rectangle& body, long ground) const;

	WorldState& m_worldState;
};
