#pragma once
#include <vector>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../../PokemonGameEventDispatcher.h"
#include "../../../Graphic/GUI/Canvas.h"
#include "../BattleComponent.h"
#include "../BattleStage.h"
#include "../FighterComponent.h"

class BattleRules;

// The turns of the classic battles in the real time battle (BattleStage::turn, drawn by GUIBattle). A clash : the Pokemon
// of the player and its opponent attack each other at once, close ; an opening : the Pokemon of the player dodges an
// attack at the last moment (Data/Battle/clash.miniska decides). The battle stops, the menu of the skills of the Pokemon
// of the player (W S and Return, or 1 .. 4 ; the AI chooses with SAD_BATTLE_AUTOPLAY), then one round as in the first
// games : each skill with its message, its hit (the bars drain), the faster first ; an opening : the player alone, its
// hit cannot miss. Then the real time goes on.
class ClashSystem :
	public ska::System<ska::RequiredComponent<FighterComponent, BattleComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	ClashSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, unsigned int blockSize);
	ClashSystem(const ClashSystem&) = delete;
	ClashSystem& operator=(const ClashSystem&) = delete;
	~ClashSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	struct Action {
		ska::EntityId attacker = 0;
		ska::EntityId target = 0;
		int skill = -1;
	};

	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool standing(ska::EntityId entity) const;
	ska::Point<float> center(ska::EntityId entity) const;
	// The skill it is attacking with (its startup, its lunge), -1 : none
	int attacking(ska::EntityId entity) const;
	// A clash or an opening starts now
	void watch();
	void start(BattleTurn::Kind kind, ska::EntityId opponent, int opponentSkill);
	void choose(std::size_t index);
	// The strongest skill that hits (the AI)
	int strongest(ska::EntityId entity) const;
	std::string who(ska::EntityId entity) const;
	void hit(const Action& action);
	void end();

	ska::EntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	BattleRules& m_rules;
	BattleStage& m_stage;
	float m_blockSize;
	// Battle time, the end of the last turn
	unsigned long m_time = 0;
	unsigned long m_lastTurn = 0;
	bool m_hadTurn = false;
	// What was seen last frame : the dash of the Pokemon of the player, two attacks at once
	bool m_dashing = false;
	bool m_bothAttacking = false;
	// The turn : the opponent, the skill it was attacking with, the actions of the round, the one played
	ska::EntityId m_opponent = 0;
	int m_opponentSkill = -1;
	std::vector<Action> m_actions;
	std::size_t m_action = 0;
	bool m_hit = false;
	bool m_announced = false;
	Ui::ChoiceList m_choice;
};
