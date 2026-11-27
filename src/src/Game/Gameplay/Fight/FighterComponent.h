#pragma once
#include <array>
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

// A status : it hurts over time (poison, burn : its attack is halved too, leech : it heals the one who gave it), it slows
// down and sometimes fails its skills (paralysis), it cannot act until it is hit or wakes up (sleep)
enum class FighterStatus { None, Poison, Burn, Paralysis, Sleep, Leech };

// The statistics changed in a battle (FighterComponent::stages)
enum FighterStat { StatAttack = 0, StatDefense = 1, StatSpeed = 2 };

// The multiplier of a statistic changed in a battle : the rules, Data/Battle/stages.miniska (BattleRules::stageMultiplier)

// What a fighter does now, decided by the battle AI (script) or by the player (keys)
enum class FighterIntent {
	Hold = 0,
	Approach = 1,
	Retreat = 2,
	SidestepLeft = 3,
	SidestepRight = 4
};

// A Pokemon fighting in a battle (with its BattleComponent, its statistics) : who it fights, who plays it, what it does
// now (an attack, a stun...). Added when the battle starts, removed when it ends (StateFight).
class FighterComponent {
public:
	// Its side : 0 the player, 1 the wild Pokemon. It fights the nearest fighter of the other side still standing
	int team = 0;
	ska::EntityId opponent = 0;
	// Played by the player (keys), else by the battle AI
	bool player = false;
	bool controlledByAI = false;
	// K.O. : it does not fight anymore (it sinks and disappears), since "knockedOutTime" milliseconds
	bool knockedOut = false;
	unsigned int knockedOutTime = 0;
	// It comes out of its pokeball : milliseconds since (it grows out of the light ; AppearDuration : done)
	unsigned int appear = AppearDuration;
	static constexpr unsigned int AppearDuration = 420;
	// Recalled in its pokeball at the end of the battle : milliseconds since (it turns red and shrinks, then it is not
	// drawn anymore), -1 : not recalled
	int recall = -1;
	static constexpr int RecallDuration = 450;
	// A wild Pokemon in a pokeball thrown by the trainer (it shrinks in it : "recall") : it does not fight, it cannot be
	// hit, until it breaks free (it comes out : "appear") or it is captured ("captured", with "knockedOut" : out of the
	// battle, it joined the team of the player)
	bool inBall = false;
	bool captured = false;

	FighterIntent intent = FighterIntent::Hold;
	// Battle time of its next AI decision
	unsigned long nextDecision = 0;
	// Battle time of the last use of each skill (0 : never used) : the cooldowns
	std::array<unsigned long, 4> lastUses {};

	// Milliseconds left : stunned after a hit (no control), white flash
	unsigned int stun = 0;
	// Milliseconds left before it can flinch again (a flinch is not chained : BattleRules::statusImmunity)
	unsigned int flinchGuard = 0;
	// A skill unleashed by the bond with its trainer : its hits deal "bondPower" percents of their damage for "bondTime"
	// milliseconds more (BondSystem)
	unsigned int bondTime = 0;
	int bondPower = 100;
	unsigned int flash = 0;
	// An attack : the startup (the Pokemon prepares it), the lunge of a contact skill (it hits), the recovery. The
	// Pokemon cannot move during all of it, it can be hit (a hit during the startup cancels the attack)
	unsigned int windUp = 0;
	unsigned int windUpDuration = 0;
	unsigned int lunge = 0;
	unsigned int recovery = 0;
	unsigned int recoveryDuration = 0;
	// Stretch when a projectile goes
	unsigned int release = 0;
	// The skill used at the end of the startup (-1 : none), towards its opponent when it started
	int pendingSkill = -1;
	bool contactAttack = false;
	ska::Point<float> attackDirection;
	// Pushed back after a hit (pixels per second), applied when the hit stop ends, then slowed down
	ska::Point<float> knockback;

	// Its statistics changed by skills (stages, see FighterStat), and the time left of each change
	std::array<int, 3> stages{};
	// The factors of the stages (Data/Battle/stages.miniska), asked when a stage changes : read by the hits and the
	// speed (every frame)
	std::array<float, 3> stageMultipliers{ 1.F, 1.F, 1.F };
	std::array<unsigned int, 3> stageTime{};
	// Its status, the time it lasts, the time until it hurts again, the fighter who gave it (leech)
	FighterStatus status = FighterStatus::None;
	unsigned int statusTime = 0;
	unsigned int statusTick = 0;
	ska::EntityId statusSource = 0;
	// The rules of its status (Data/Battle/status.miniska), asked when it lands : every statusTickPeriod milliseconds it
	// loses statusDamage hit points (statusLeech : they heal its source), its speed in percents
	unsigned int statusTickPeriod = 0;
	int statusDamage = 0;
	bool statusLeech = false;
	int statusSpeed = 100;
	// The status is over : no status, no effect
	void clearStatus() {
		status = FighterStatus::None;
		statusTime = 0;
		statusTickPeriod = 0;
		statusDamage = 0;
		statusLeech = false;
		statusSpeed = 100;
	}
	// Protected (Abri) : the hits do nothing
	unsigned int guard = 0;
	// The dodge : a short dash (it cannot be hit at its start), then a delay before the next one
	unsigned int dash = 0;
	unsigned int invulnerable = 0;
	unsigned int dashCooldown = 0;
	ska::Point<float> dashDirection;
	bool dodgeRequested = false;
	// The attack of an opponent the AI already thought about dodging (its startup)
	ska::EntityId dodgeChecked = 0;

	// Hit points shown by its bar : they follow the real ones with a delay (the lost part stays visible a moment)
	float shownHp = 0.F;
	unsigned int hpDelay = 0;
	// The bar shakes after a hit
	unsigned int barShake = 0;
	// Time spent outside of the arena (leaving it for 1 s : the player flees)
	unsigned int outside = 0;
	// Its last position out of the walls (a fast push can cross the outline of a wall : it goes back there)
	ska::Point<float> lastFree;
	bool hasLastFree = false;

	// Busy with an attack (startup, lunge, recovery) or stunned : it cannot move nor attack
	bool busy() const { return knockedOut || inBall || stun > 0 || windUp > 0 || lunge > 0 || recovery > 0 || dash > 0 || status == FighterStatus::Sleep; }
};

SKA_DECLARE_COMPONENT(FighterComponent);
