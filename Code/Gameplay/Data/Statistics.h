#pragma once
#include <string>
#include "RawStatistics.h"
#include "../../ska/Inputs/Readers/IniReader.h"


class Statistics {

public:
	Statistics(ska::IniReader* data, std::string block);
	Statistics(int hp, int attack, int defense, int spe_attack, int spe_defense, int speed);
	void nextLevel();
	void setLevel(unsigned int level);

	bool addExperience(int exp);
	void setExperience(int exp);
	
	const RawStatistics<int>& getRawStats() const;
	int getAttack() const;
	int getDefense() const;
	int getSpeAttack() const;
	int getSpeDefense() const;
	int getSpeed() const;
	int getHpMax() const;
	int getLevel() const;
	int getExperience() const;
	int getMaxSpeed() const;
	int getDroppedExperience() const;

	~Statistics();
	
private:
	RawStatistics<int> m_stats;
	
	int m_type;
	int m_level;
	RawStatistics<float> m_slopes;
};
