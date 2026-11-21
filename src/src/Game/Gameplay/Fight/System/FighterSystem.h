#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Graphic/GraphicComponent.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"

class WorldState;

// The fighters of a battle, each logic update : the AI walks as its script decided (BattleStage intent : approach,
// retreat, sidestep) and stays in the arena, a fighter hit is stunned and pushed back, everything stops during a hit stop.
// The fighters face each other. Their drawing shows what happens : white flash when hit, squash before a skill, K.O.
class FighterSystem : public ska::System<
	ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::ForceComponent, ska::AnimationComponent, ska::GraphicComponent, BattleComponent>,
	ska::PossibleComponent<ska::InputComponent, ska::MovementComponent>> {
public:
	FighterSystem(ska::EntityManager& em, WorldState& ws, BattleStage& stage);
	FighterSystem(const FighterSystem&) = delete;
	FighterSystem& operator=(const FighterSystem&) = delete;
	~FighterSystem() override = default;

	static constexpr unsigned int FlashDuration = 150;
	static constexpr unsigned int WindUpDuration = 110;
	static constexpr unsigned int ReleaseDuration = 140;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	ska::Point<float> center(ska::EntityId entity);
	void control(FighterState& fighter, unsigned int ellapsedTime);
	void stop(FighterState& fighter);
	void draw(FighterState& fighter);

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	BattleStage& m_stage;
	// The walk power of the Pokemon of the player (its InputComponent), kept while its control is disabled
	float m_playerPower = -1.F;
};
