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
	int id;
	int cooldown;
	int range;

	RawStatistics<int> buffAlly;
	RawStatistics<int> buffEnemy;
	int alterAlly;
	int alterEnemy;

	int particleNumber;
};
