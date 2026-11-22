#pragma once
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Inputs/InputToggle.h"
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
#include "../FighterComponent.h"
#include "../../PokemonGameEventDispatcher.h"

class WorldState;

// The fighters of a battle, each logic update, as in a fighting game : they walk at a constant speed and stop at once
// (no sliding, no running) : the player with the direction keys, the AI as its script decided (BattleStage intent :
// approach, retreat, sidestep). They cannot move during an attack (startup, lunge, recovery) nor when stunned ; a hit
// pushes them back. They face each other. Everything stops during a hit stop. The AI stays in the arena.
// Their drawing shows what happens : white flash and blinking when hit, preparation and lunge of an attack, K.O.
class FighterSystem :
	public ska::System<
		ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, ska::ForceComponent, ska::AnimationComponent, ska::GraphicComponent, BattleComponent, FighterComponent>,
		ska::PossibleComponent<ska::InputComponent, ska::MovementComponent>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	FighterSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, BattleStage& stage);
	FighterSystem(const FighterSystem&) = delete;
	FighterSystem& operator=(const FighterSystem&) = delete;
	~FighterSystem() override = default;

	static constexpr unsigned int FlashDuration = 150;
	static constexpr unsigned int ReleaseDuration = 140;
	// The dodge : a dash of 0.2 s, it cannot be hit during 0.16 s, once every 0.9 s
	static constexpr unsigned int DashDuration = 200;
	static constexpr unsigned int DashInvulnerability = 160;
	static constexpr unsigned int DashCooldown = 900;
	static constexpr float DashSpeed = 620.F;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	ska::Point<float> center(ska::EntityId entity);
	ska::Point<float> keysDirection() const;
	void control(ska::EntityId entity, FighterComponent& fighter, unsigned int ellapsedTime);
	void setVelocity(ska::EntityId entity, ska::Point<float> velocity);
	void face(ska::EntityId entity, ska::Point<float> direction);
	void draw(ska::EntityId entity, FighterComponent& fighter);
	// The changes of its statistics and its status over time
	void updateEffects(ska::EntityId entity, FighterComponent& fighter, unsigned int delta);

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	BattleStage& m_stage;
	// The direction keys held (several input contexts send their keys : any of them since the last update)
	ska::InputToggleContainer m_toggles;
	ska::InputToggleContainer m_receivedToggles;
	bool m_received = false;
	bool m_dodgeKey = false;
	unsigned int m_time = 0;
	// The walk power of the Pokemon of the player (its InputComponent) : the keys move it through this system in a
	// battle, its power is given back at the end
	float m_playerPower = -1.F;
};
