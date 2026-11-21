#pragma once
#include "../Data/RawStatistics.h"

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
	// A contact skill shows its sprite where it hits ([Particle] show_sprite, 1 by default)
	bool showSprite = true;
	// Milliseconds before the skill goes (the Pokemon prepares it) and after it (the Pokemon recovers) : it cannot move,
	// it can be hit ([Stats] startup, recovery)
	unsigned int startup = 0;
	unsigned int recovery = 0;
};
