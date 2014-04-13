#ifndef DEF_STATISTICS
#define DEF_STATISTICS
#include <string>

class IniReader;

class Statistics
{

	public:
	Statistics(IniReader* data, std::string block);
	Statistics(int hp, int attack, int defense, int spe_attack, int spe_defense, int speed);
	void nextLevel();
	
	bool addExperience(int exp);
	void setExperience(int exp);

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
	void setLevel(unsigned int level);
	int m_hp,
	m_attack,
	m_defense,
	m_spe_attack,
	m_spe_defense,
	m_speed,
	m_level,
	m_exp,
	m_droppedExp;
	
	int m_type;

	float m_slopeHp,
	m_slopeAttack,
	m_slopeDefense,
	m_slopeSpeAttack,
	m_slopeSpeDefense,
	m_slopeSpeed,
	m_slopeExp,
	m_slopeDroppedExp;
};

#endif