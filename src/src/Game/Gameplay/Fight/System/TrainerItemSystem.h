#pragma once
#include <optional>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../../PokemonGameEventDispatcher.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"
#include "../FighterComponent.h"

class WorldState;

// The item slot of the trainer in a battle : the items of the bag usable in a battle (a healing item, a pokeball), one
// chosen at a time (BattleStage::itemSlot, drawn by GUIBattle). The action key (Return, the A of the pad) uses it, the
// key of "delete" (Backspace, the B of the pad) chooses the next one. A healing item heals the Pokemon of the player
// fighting ; a pokeball is thrown at its opponent : the wild Pokemon goes in, the ball shakes, it is captured (it joins
// the team, the battle rules give the chance : BattleRules::captureChance) or it breaks free. A pause after each use.
class TrainerItemSystem :
	public ska::System<ska::RequiredComponent<FighterComponent, BattleComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	TrainerItemSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage);
	TrainerItemSystem(const TrainerItemSystem&) = delete;
	TrainerItemSystem& operator=(const TrainerItemSystem&) = delete;
	~TrainerItemSystem() override = default;

	// The pause after an item (battle time, ms)
	static constexpr unsigned int Cooldown = 2500;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	// The slot shows the item chosen (the next usable one when it ran out)
	void updateSlot();
	void next();
	void use();
	bool heal(int item, int amount);
	bool throwBall(int item);
	void updateBall();
	// The wild Pokemon in the ball breaks free / is captured
	void release(ska::EntityId target);
	void capture(ska::EntityId target);
	ska::Point<float> center(ska::EntityId entity) const;
	bool standing(ska::EntityId entity) const;

	ska::EntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	WorldState& m_worldState;
	BattleStage& m_stage;
	int m_item = -1;
	unsigned int m_cooldown = 0;
	// The pokeball thrown and the wild Pokemon it was thrown at
	std::optional<ska::EntityId> m_ball;
	ska::EntityId m_target = 0;
	bool m_caught = false;
};
