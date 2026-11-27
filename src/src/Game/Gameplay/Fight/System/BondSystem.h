#pragma once
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
class BattleSystem;

// The bond between the trainer and its Pokemon in a battle : its gauge (BattleStage::bond) fills when the battle goes on
// (TrainerCoachSystem, Data/Battle/bond.miniska). Full, F (the "Lien" of the pad) unleashes the strongest skill of the
// Pokemon : a cinematic (BattleStage::cinematic, drawn by GUIBattle, framed by BattleCameraSystem). The battle stops, the
// trainer calls the skill, a ring closes on the Pokemon : F again when it closes is a perfect timing. Then the skill
// goes, its hits stronger (Data/Battle/bond.miniska : power). Played by the AI (SAD_BATTLE_AUTOPLAY) : unleashed as soon
// as the gauge is full.
class BondSystem :
	public ska::System<ska::RequiredComponent<FighterComponent, BattleComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	BondSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, BattleSystem& battle);
	BondSystem(const BondSystem&) = delete;
	BondSystem& operator=(const BondSystem&) = delete;
	~BondSystem() override = default;

	// The hits of the skill unleashed are stronger this long (battle time, ms)
	static constexpr unsigned int BondTime = 2200;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool standing(ska::EntityId entity) const;
	// The strongest skill of the Pokemon of the player that hits (-1 : none)
	int strongestSkill() const;
	// It can be unleashed now
	bool ready() const;
	void start();
	void press(bool perfect);
	void release();

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	BattleStage& m_stage;
	BattleSystem& m_battle;
	bool m_requested = false;
	int m_skill = -1;
};
