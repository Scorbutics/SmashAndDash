#pragma once
#include <array>
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

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
	ska::EntityId opponent = 0;
	// Played by the player (keys), else by the battle AI
	bool player = false;
	bool controlledByAI = false;

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

	// Hit points shown by its bar : they follow the real ones with a delay (the lost part stays visible a moment)
	float shownHp = 0.F;
	unsigned int hpDelay = 0;
	// The bar shakes after a hit
	unsigned int barShake = 0;
	// Time spent outside of the arena (leaving it for 1 s : the player flees)
	unsigned int outside = 0;

	// Busy with an attack (startup, lunge, recovery) or stunned : it cannot move nor attack
	bool busy() const { return stun > 0 || windUp > 0 || lunge > 0 || recovery > 0; }
};

SKA_DECLARE_COMPONENT(FighterComponent);
