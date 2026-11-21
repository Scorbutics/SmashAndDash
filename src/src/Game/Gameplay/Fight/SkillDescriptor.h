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
};
