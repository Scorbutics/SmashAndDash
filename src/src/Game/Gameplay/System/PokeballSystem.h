#pragma once
#include "Core/ECS/System.h"
#include "../PokeballComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Graphic/AnimatedTexture.h"

// The pokeballs of the battles (PokeballComponent) : the flight on its arc (the ball spins), the bounce where it lands,
// the opening (the open ball and a burst of light that grows and fades), or its return in the hands of the trainer.
// Sprites : Sprites/Fight/pokeball*.png (tools/tilegen/pokeball.py).
class PokeballSystem : public ska::System< ska::RequiredComponent<PokeballComponent, ska::PositionComponent, ska::GraphicComponent>, ska::PossibleComponent<>>  {
public:
	explicit PokeballSystem(ska::EntityManager& entityManager);
	PokeballSystem& operator=(const PokeballSystem&) = delete;
	~PokeballSystem() override = default;

	// A pokeball thrown from a point on the ground to another (world pixels) : its entity. "returning" : the recall (it
	// disappears at the end)
	static ska::EntityId Throw(ska::EntityManager& entityManager, ska::Point<float> from, ska::Point<float> to, bool returning);

	static constexpr unsigned int BounceDuration = 220;
	static constexpr unsigned int OpenDuration = 650;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	// Its sprite with its bottom centre on a point of the ground, at a height
	void place(ska::EntityId entity, ska::Point<float> ground, float height, int width, int spriteHeight);
	void open(ska::EntityId entity, PokeballComponent& ball);

	ska::EntityManager& m_entities;
	ska::AnimatedTexture m_spin;
	ska::AnimatedTexture m_open;
	ska::AnimatedTexture m_burst;
};
