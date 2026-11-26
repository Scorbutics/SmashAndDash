#pragma once
#include "Canvas.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/GraphicComponent.h"
#include "../../Gameplay/Control/GridMovementComponent.h"
#include "../../Gameplay/Mobs/WildPokemonComponent.h"
#include "../../Gameplay/Mobs/System/ArrivalSystem.h"

namespace ska {
	class CameraSystem;
	class DrawableContainer;
}

// Over the map, zoomed with it : a mark over a wild Pokemon that notices the player (GridMovementComponent::seeing) :
// a red "!" when it charges, a blue "?!" when it runs away. It pops, then fades. A hideout rustles before a Pokemon comes
// out of it.
class WildPokemonMarks :
	public ska::System<ska::RequiredComponent<WildPokemonComponent, GridMovementComponent, ska::PositionComponent, ska::HitboxComponent, ska::GraphicComponent>, ska::PossibleComponent<>> {
public:
	// arrivals : the hideouts that rustle (bits of leaves and dust jump out of them)
	WildPokemonMarks(ska::EntityManager& entityManager, ska::CameraSystem& camera, const ArrivalSystem* arrivals = nullptr);
	~WildPokemonMarks() override = default;

	static constexpr unsigned int Duration = 1100;

	void graphicUpdate(ska::DrawableContainer& drawables);

protected:
	void refresh(unsigned int) override {}

private:
	void render(Ui::Canvas& canvas);

	ska::CameraSystem& m_camera;
	const ArrivalSystem* m_arrivals;
	Ui::Layer m_layer;
};
