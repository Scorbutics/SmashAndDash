#pragma once
#include <array>
#include <string>
#include "../Data/RawStatistics.h"

// How a skill reaches : particles thrown towards the opponent, a lunge hitting in front, an area around the Pokemon, the
// Pokemon itself, dashes of which the last one strikes ([Description] style_2 : Projectile, Contact, Area, Self, Dash)
enum class SkillKind { Projectile, Contact, Area, Self, Dash };

// A dash skill ([Dash]) : how many dashes (the last one strikes, the others place the Pokemon), their speed (pixels per
// second), the time of a dash and of the strike, the time to ask for the next one (else it goes by itself), the recovery
// after a strike that touched nothing (milliseconds)
struct DashDescriptor {
	int count = 2;
	float speed = 640.F;
	unsigned int duration = 160;
	unsigned int strikeDuration = 220;
	unsigned int window = 250;
	unsigned int missRecovery = 420;
};

class SkillDescriptor {
public:

	std::string style1;
	std::string style2;
	std::string description;
	std::string name;
	std::string type;
	std::string context;
	int id = 0;

	RawStatistics<int> buffAlly;
	RawStatistics<int> buffEnemy;
	int alterAlly = 0;
	int alterEnemy = 0;

	float speed = 0;
	int particleNumber = 0;
	unsigned int cooldown = 0;
	int range = 0;
	int knockback = 0;
	int noise = 0;
	float amplitude = 0;
	// Power of the skill ([Particle] damage), for the damage rule
	int power = 0;
	// Special skill ([Description] class=Speciale) : the special statistics are used
	bool special = false;
	// A contact skill (style Contact, ex : Charge) : the Pokemon lunges and hits what is in its range, in front of it.
	// Else the skill throws its particles (style Projectile)
	bool contact = false;
	SkillKind kind = SkillKind::Projectile;
	DashDescriptor dash;
	// Effects ([Effects]) : changes of the statistics of the target and of the Pokemon (attack, defense, speed : -3 to 3),
	// a status given to the target ("poison", "burn", "paralysis", "sleep", "leech", "flinch") or to the Pokemon ("guard")
	// with a chance (percents), their duration (milliseconds), a fixed damage instead of the damage rule
	std::array<int, 3> targetStages{};
	std::array<int, 3> selfStages{};
	std::string status;
	int statusChance = 100;
	unsigned int effectDuration = 6000;
	int fixedDamage = 0;
	// A contact skill shows its sprite where it hits ([Particle] show_sprite, 1 by default)
	bool showSprite = true;
	// Milliseconds before the skill goes (the Pokemon prepares it) and after it (the Pokemon recovers) : it cannot move,
	// it can be hit ([Stats] startup, recovery)
	unsigned int startup = 0;
	unsigned int recovery = 0;
};
