#pragma once
#include <vector>
#include "Audio/Music.h"
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
class WorldState;

// The clashes of the real time battle (BattleStage::turn, drawn by ClashArena), a cross-over of the first games and the
// fighting games. A clash : the Pokemon of the player and its opponent attack each other at once, close ; an opening :
// the Pokemon of the player dodges an attack at the last moment (Data/Battle/clash.miniska decides). The battle stops :
// the impact, the camera dives on them, the map around breaks, a screen "VS", a battleground rebuilt with its tiles, its
// own music. Then rounds (3) as in the first games : the menu of the skills of the Pokemon of the player (W S and Return,
// or 1 .. 4, 10 s ; the AI chooses with SAD_BATTLE_AUTOPLAY), each skill with its message and its hit (the bars drain),
// the faster first ; an opening : the player alone in the first round, its hits cannot miss. A K.O. ends it. Then the
// tiles go back, the music of the map, the real time goes on.
class ClashSystem :
	public ska::System<ska::RequiredComponent<FighterComponent, BattleComponent, ska::PositionComponent, ska::HitboxComponent>, ska::PossibleComponent<>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	ClashSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, unsigned int blockSize);
	ClashSystem(const ClashSystem&) = delete;
	ClashSystem& operator=(const ClashSystem&) = delete;
	~ClashSystem() override = default;

	// The battle is left during a clash : it ends (the interface, the music of the map again). Not in the destructor : when
	// the game quits, what it uses (the map, the state of the battle) is destroyed before the systems
	void leave();

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	struct Action {
		ska::EntityId attacker = 0;
		ska::EntityId target = 0;
		int skill = -1;
		// An item of the bag instead of a skill (-1 : none)
		int item = -1;
		// Its status hurts the attacker at the end of the round (poison, burn, leech : "target" its source), or a message
		// only (a status that wears off : "notice")
		bool tick = false;
		std::string notice;
	};

	bool onKeyInputEvent(ska::InputKeyEvent& event);
	bool standing(ska::EntityId entity) const;
	ska::Point<float> center(ska::EntityId entity) const;
	// The skill it is attacking with (its startup, its lunge), -1 : none
	int attacking(ska::EntityId entity) const;
	// A clash or an opening starts now
	void watch();
	void start(BattleTurn::Kind kind, ska::EntityId opponent, int opponentSkill);
	void step(BattleTurn::Step step);
	// The steps before the rounds : their sounds on time
	void intro(unsigned int before, unsigned int now);
	void choose(std::size_t index);
	// The bag of the menu : open (its items), closed (the skills again), an item chosen
	void openBag();
	void closeBag();
	void chooseItem(std::size_t index);
	// The round : the action of the player, then the one of the opponent
	void play(const Action& mine, bool first);
	// An item of the bag played : a healing item, a pokeball (its course, the capture)
	void actItem(const Action& action, unsigned int before);
	// The strongest skill that hits (the AI of the player), the skill of the opponent after the first round
	int strongest(ska::EntityId entity) const;
	int opponentPick() const;
	std::string who(ska::EntityId entity) const;
	void act(unsigned int before);
	void hit(const Action& action);
	// Its status hurts it (an action of the end of the round)
	void tick(const Action& action);
	// What keeps the attacker from acting (asleep, flinched, paralysed : the message), "" : it acts
	std::string blocked(ska::EntityId attacker);
	// The name of a fighter in the messages ("Rattata sauvage")
	std::string named(ska::EntityId entity) const;
	// The end of the actions of a round : the statuses hurt, then the time of the round passes (statuses and changes of
	// statistics wear off : their messages), then the next round
	void endRound();
	void passRound();
	void end();
	// The music of the clash, the music of the map again
	void music(bool clash);
	// The buttons of the map (they would stay over the battleground)
	void interface(bool hidden);

	ska::EntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	WorldState& m_worldState;
	BattleRules& m_rules;
	BattleStage& m_stage;
	float m_blockSize;
	ska::Music m_music;
	bool m_musicPlayed = false;
	// Battle time, the end of the last turn
	unsigned long m_time = 0;
	unsigned long m_lastTurn = 0;
	bool m_hadTurn = false;
	unsigned int m_turns = 0;
	// What was seen last frame : the dash of the Pokemon of the player, two attacks at once
	bool m_dashing = false;
	bool m_bothAttacking = false;
	// The turn : the opponent, the skill it was attacking with, the actions of the round, the one played
	ska::EntityId m_opponent = 0;
	int m_opponentSkill = -1;
	std::vector<Action> m_actions;
	std::size_t m_action = 0;
	bool m_announced = false;
	// The action played cannot act (its message only)
	bool m_blocked = false;
	// The messages after the hit of the action (its effectiveness, its effects : "L'Attaque de Rattata baisse !") and the
	// time they add to it
	std::vector<std::string> m_followUps;
	unsigned int m_extra = 0;
	static constexpr unsigned int FollowUpDuration = 1300;
	static constexpr unsigned int NoticeDuration = 1300;
	// The fighter flinched in this round (it loses its action)
	ska::EntityId m_flinched = 0;
	// The end of the round : 0 the actions, 1 the statuses hurt, 2 the time passed
	int m_roundEnd = 0;
	Ui::ChoiceList m_choice;
};
