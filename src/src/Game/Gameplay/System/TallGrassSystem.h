#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Graphic/HasShadowComponent.h"

#include "../Mobs/WildPokemonComponent.h"
class WorldState;

// The characters in tall grass (the blocks of the middle layer with the property TilePropertyGrass, tools/worldgen) : the
// bottom of their drawing is not drawn (GraphicComponent::cropBottom), the grass behind them shows instead of their
// legs. As much as their footprint (a block under their feet, where they are drawn) is on tall grass : during a step
// from a block to another the legs sink in or come out progressively, in every direction. Less when they are in the air.
// The wild Pokemon in the water (a Tentacool, a Magicarpe : they swim) : the same, the water over their lower body.
class TallGrassSystem : public ska::System<ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::GraphicComponent, ska::HasShadowComponent>, ska::PossibleComponent<WildPokemonComponent>> {
public:
	TallGrassSystem(ska::EntityManager& entityManager, WorldState& worldState);
	TallGrassSystem& operator=(const TallGrassSystem&) = delete;
	~TallGrassSystem() override = default;

	// The property of the tiles of tall grass (tools/worldgen GRASS) ; the pixels of a character hidden in it
	static constexpr unsigned int TilePropertyGrass = 4;
	// The water of the maps (the ground layer : ska TileProperties, bit 1)
	static constexpr unsigned int TilePropertyWater = 1;
	static constexpr int Depth = 22;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool grass(int x, int y) const;
	bool water(int x, int y) const;

	WorldState& m_worldState;
};
