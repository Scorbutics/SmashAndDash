#ifndef DEF_STATISTICS
#define DEF_STATISTICS
#include <string>
#include "RawStatistics.h"
#include "../../ska/Inputs/Readers/IniReader.h"


class Statistics
{

	public:
	Statistics(ska::IniReader* data, std::string block);
	Statistics(int hp, int attack, int defense, int spe_attack, int spe_defense, int speed);
	void nextLevel();
	void setLevel(unsigned int level);

	bool addExperience(int exp);
	void setExperience(int exp);
	
	const RawStatistics<int>& getRawStats() const;
	int getAttack();
	int getDefense();
	int getSpeAttack();
	int getSpeDefense();
	int getSpeed();
	int getHpMax();
	int getLevel();
	int getExperience();
	int getMaxSpeed();
	int getDroppedExperience();

	~Statistics();
	
private:
	RawStatistics<int> m_stats;
	
	int m_type;
	int m_level;
	RawStatistics<float> m_slopes;
};

#endif