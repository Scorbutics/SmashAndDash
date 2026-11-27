#pragma once
#include <array>
#include <optional>
#include <string>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"
#include "../FighterComponent.h"

class WorldState;
class BattleRules;

// The trainer of the player during a battle, as in the anime : it follows the fight from behind its Pokemon (out of the
// way of the attacks, in the arena : on the screen), it faces its Pokemon, and it comments the battle (a speech balloon
// over it : BattleStage::say, drawn by GUIBattle). What it says comes from Data/Battle/lines.miniska, asked when
// something happens (the start, a skill, a hit, a dodge, an item, a K.O. : BattleStage::cue), never every frame.
class TrainerCoachSystem : public ska::System<ska::RequiredComponent<FighterComponent, BattleComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>> {
public:
	TrainerCoachSystem(ska::EntityManager& em, WorldState& worldState, BattleStage& stage);
	TrainerCoachSystem(const TrainerCoachSystem&) = delete;
	TrainerCoachSystem& operator=(const TrainerCoachSystem&) = delete;
	~TrainerCoachSystem() override = default;

	// The trainer stays this far behind its Pokemon (blocks), at least this far from the opponent
	static constexpr float Behind = 2.5F;
	static constexpr float KeepAway = 2.F;
	// ... and at least this far from its own Pokemon
	static constexpr float Clear = 1.5F;
	// Its walk (pixels per second), how often it chooses where to stand (ms)
	static constexpr float Speed = 120.F;
	static constexpr unsigned int PlanPeriod = 500;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	ska::Point<float> center(ska::EntityId entity) const;
	bool standing(ska::EntityId entity) const;
	// The trainer can stand there (world pixels : its center) : no wall, no water
	bool walkable(ska::Point<float> point);
	// Where it stands now : behind its Pokemon, away from the opponent, in the arena (none : it stays)
	std::optional<ska::Point<float>> plan(ska::EntityId trainer);
	void walk(ska::EntityId trainer, unsigned int ellapsedTime);
	// What the battle did since the last frame : the lines of the trainer
	void watch();
	void comment(const std::string& event, const std::string& detail = "");
	// The bond between the trainer and its Pokemon grows (BattleStage::bond)
	void feel(const std::string& event);
	const std::string& monsterName();

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	BattleStage& m_stage;
	BattleRules& m_rules;
	std::optional<ska::Point<float>> m_target;
	unsigned int m_planTime = PlanPeriod;
	// Its exact position (the positions of the entities are integers)
	std::optional<ska::Point<float>> m_exact;
	// Real time since its last line
	unsigned int m_quiet = 0;
	bool m_spoke = false;
	// What was seen of the battle last frame
	BattlePhase m_phase = BattlePhase::Intro;
	std::array<unsigned long, 4> m_lastUses{};
	bool m_dashing = false;
	bool m_low = false;
	std::string m_name;
};
