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

// The multiplier of a statistic changed in a battle : 1.5, 2, 2.5 for +1, +2, +3 ; 0.67, 0.5, 0.4 for -1, -2, -3
inline float StageMultiplier(int stage) {
	return stage >= 0 ? (2.F + static_cast<float>(stage)) / 2.F : 2.F / (2.F - static_cast<float>(stage));
}

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

	FighterIntent intent = FighterIntent::Hold;
	// Battle time of its next AI decision
	unsigned long nextDecision = 0;
	// Battle time of the last use of each skill (0 : never used) : the cooldowns
	std::array<unsigned long, 4> lastUses {};

	// Milliseconds left : stunned after a hit (no control), white flash
	unsigned int stun = 0;
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
	std::array<unsigned int, 3> stageTime{};
	// Its status, the time it lasts, the time until it hurts again, the fighter who gave it (leech)
	FighterStatus status = FighterStatus::None;
	unsigned int statusTime = 0;
	unsigned int statusTick = 0;
	ska::EntityId statusSource = 0;
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
	bool busy() const { return knockedOut || stun > 0 || windUp > 0 || lunge > 0 || recovery > 0 || dash > 0 || status == FighterStatus::Sleep; }
};

SKA_DECLARE_COMPONENT(FighterComponent);
