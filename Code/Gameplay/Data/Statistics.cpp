#include "Statistics.h"
#include "../../Utils\IDs.h"
#include "../../Inputs\Readers\IniReader.h"

using namespace std;

Statistics::Statistics(int hp, int attack, int defense, int spe_attack, int spe_defense, int speed)
{
	m_hp = hp;
	m_defense = defense;
	m_spe_defense = spe_defense;
	m_spe_attack = spe_attack;
	m_speed = speed/2;
	m_attack = attack;
	m_slopeHp = 0;
	m_slopeSpeDefense = 0;
	m_slopeSpeed = 1.0;
	m_slopeSpeAttack = 0;
	m_slopeAttack = 0;
	m_slopeDefense = 0;

	m_type = STATS_TYPE_LINEAR;
	m_level = 1;
}

Statistics::Statistics(IniReader* data, string block)
{

	m_hp = data->getInt(block + " hp");
	m_defense = data->getInt(block + " defense");
	m_spe_defense = data->getInt(block + " spe_defense");
	m_spe_attack = data->getInt(block + " spe_attack");
	m_speed = data->getInt(block + " speed")/2;
	m_attack = data->getInt(block + " attack");
	m_droppedExp = data->getInt("Experience level_1_exp_dropped");
	m_slopeHp = (float)((data->getInt(block + " hp_max") - m_hp) / 100.);
	m_slopeSpeDefense = (float)((data->getInt(block + " spe_defense_max") - m_spe_defense) / 100.);
	m_slopeSpeed = (float)((data->getInt(block + " speed_max") - m_speed) / 100.);
	m_slopeSpeAttack = (float)((data->getInt(block + " spe_attack_max") - m_spe_attack) / 100.);
	m_slopeAttack = (float)((data->getInt(block + " attack_max") - m_attack) / 100.);
	m_slopeDefense = (float)((data->getInt(block + " defense_max") - m_defense) / 100.);
	m_slopeExp = (float)data->getInt("Experience level_1_exp_needed");
	m_exp = data->getInt("Experience level_1_exp_needed");
	m_slopeDroppedExp = (float) (abs((int)(m_exp - m_droppedExp))/3.);
	

	m_type = STATS_TYPE_LINEAR;
	m_level = 1;
}

void Statistics::nextLevel()
{
	m_level++;
	m_exp += (int)m_slopeExp;
	switch(m_type)
	{
		case STATS_TYPE_LINEAR:
			m_hp += (int)m_slopeHp;
			m_attack += (int)m_slopeAttack;
			m_defense += (int)m_slopeDefense;
			m_speed += (int)m_slopeSpeed;
			m_spe_attack += (int)m_slopeSpeAttack;
			m_spe_defense += (int)m_slopeSpeDefense;
			m_droppedExp += (int)m_slopeDroppedExp;
			break;
		
		case STATS_TYPE_SQUARE:
			//???
			break;

		default:
			break;
	}
}

void Statistics::setLevel(unsigned int level)
{
	if(m_level != level)
	{
		m_exp -= (int)m_slopeExp*m_level;
		m_hp -= (int)m_slopeHp*m_level;
		m_attack -= (int)m_slopeAttack*m_level;
		m_defense -= (int)m_slopeDefense*m_level;
		m_speed -= (int)m_slopeSpeed*m_level;
		m_spe_attack -= (int)m_slopeSpeAttack*m_level;
		m_spe_defense -= (int)m_slopeSpeDefense*m_level;
		m_droppedExp -= (int)m_slopeDroppedExp*m_level;
		m_level = level;
		m_exp += (int)m_slopeExp*level;
		m_hp += (int)m_slopeHp*level;
		m_attack += (int)m_slopeAttack*level;
		m_defense += (int)m_slopeDefense*level;
		m_speed += (int)m_slopeSpeed*level;
		m_spe_attack += (int)m_slopeSpeAttack*level;
		m_spe_defense += (int)m_slopeSpeDefense*level;
		m_droppedExp += (int)m_slopeDroppedExp*level;

	}
}

int Statistics::getMaxSpeed()
{
	return (int)(m_speed + 21 * m_slopeSpeed);
}

int Statistics::getLevel()
{
	return m_level;
}

int Statistics::getHpMax()
{
	return m_hp;
}

int Statistics::getAttack()
{
	return m_attack;
}

int Statistics::getDefense()
{
	return m_defense;
}

int Statistics::getSpeAttack()
{
	return m_spe_attack;
}

int Statistics::getSpeDefense()
{
	return m_spe_defense;
}

int Statistics::getSpeed()
{
	return m_speed;
}

int Statistics::getExperience()
{
	return m_exp;
}

int Statistics::getDroppedExperience()
{
	return m_droppedExp;
}

void Statistics::setExperience(int exp)
{
	setLevel((int)(exp / m_slopeExp));
}

bool Statistics::addExperience(int exp)
{
	unsigned int lastexp = m_exp + exp, lastlevel = m_level;
	m_exp += exp;
	setLevel((int)(m_exp / m_slopeExp));
	m_exp = lastexp;
	
	return (m_level != lastlevel);
}

Statistics::~Statistics()
{
}


