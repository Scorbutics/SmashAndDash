#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"
#include "../FighterComponent.h"
#include "../../World/WorldState.h"
#include "../../PokemonGameEventDispatcher.h"

// Course of a battle (BattleStage) : the fight starts when both fighters are there, in an arena around them, framed by
// the camera. It ends when a fighter is K.O. (slow motion) or when the player leaves the arena (it flees a wild
// Pokemon). Then the result is saved and the player goes back to the map.
// Its steps are a script (Data/Battle/steps.miniska : the banners, the waits) : at each step (start, knockout, fled)
// the battle starts it and waits for its end before going on.
class BattleDirectorSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, BattleComponent, FighterComponent>, ska::PossibleComponent<>> {
public:
	// "teamIndex" : index of the Pokemon of the player in the team (-1 : not from the team)
	BattleDirectorSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, BattleStage& stage, int teamIndex);
	BattleDirectorSystem(const BattleDirectorSystem&) = delete;
	BattleDirectorSystem& operator=(const BattleDirectorSystem&) = delete;
	~BattleDirectorSystem() override = default;

	// Size of the arena, in blocks
	static constexpr int ArenaWidth = 11;
	static constexpr int ArenaHeight = 7;
	static constexpr unsigned int KnockOutDuration = 2200;
	static constexpr unsigned int FleeDuration = 900;
	// The script of the steps, and how long the battle waits for a step at most (a script that never ends)
	static constexpr auto StepsFile = "./Data/Battle/steps.miniska";
	static constexpr unsigned int StepTimeout = 10000;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool present(ska::EntityId fighter) const;
	ska::Point<float> center(ska::EntityId fighter);
	void start();
	void updateBars(unsigned int ellapsedTime);
	void end();
	// Starts the script of a step (arguments : the step, then what it is about) ; stepDone : it ended (or timed out)
	void runStep(std::vector<std::string> arguments);
	bool stepDone() const;
	std::string name(ska::EntityId fighter) const;

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	PokemonGameEventDispatcher& m_ged;
	BattleStage& m_stage;
	int m_teamIndex;
	std::shared_ptr<bool> m_stepDone;
};
