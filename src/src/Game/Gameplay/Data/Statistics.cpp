#include "Statistics.h"
#include "../../Utils/IDs.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Base/IO/Readers/IniReader.h"


Statistics::Statistics(int hp, int attack, int defense, int spe_attack, int spe_defense, int speed)
{
	m_stats.hp = hp;
	m_stats.defense = defense;
	m_stats.speDefense = spe_defense;
	m_stats.speAttack = spe_attack;
	m_stats.speed = speed / 2;
	m_stats.attack = attack;
	m_slopes.hp = 0;
	m_slopes.speDefense = 0;
	m_slopes.speed = 1.0;
	m_slopes.speAttack = 0;
	m_slopes.attack = 0;
	m_slopes.defense = 0;

	m_type = STATS_TYPE_LINEAR;
	m_level = 1;
}

const RawStatistics<int>& Statistics::getRawStats() const {
	return m_stats;
}

Statistics::Statistics(ska::IniReader* data, std::string block)
{

	m_stats.hp = data->get<int>(block + " hp");
	m_stats.defense = data->get<int>(block + " defense");
	m_stats.speDefense = data->get<int>(block + " spe_defense");
	m_stats.speAttack = data->get<int>(block + " spe_attack");
	m_stats.speed = data->get<int>(block + " speed") / 2;
	m_stats.attack = data->get<int>(block + " attack");
	m_stats.droppedExp = data->get<int>("Experience level_1_exp_dropped");
	m_slopes.hp = ((data->get<int>(block + " hp_max") - m_stats.hp) / 100.F);
	m_slopes.defense = ((data->get<int>(block + " spe_defense_max") - m_stats.speDefense) / 100.F);
	m_slopes.speed = ((data->get<int>(block + " speed_max") - m_stats.speed) / 100.F);
	m_slopes.speAttack = ((data->get<int>(block + " spe_attack_max") - m_stats.speAttack) / 100.F);
	m_slopes.attack = ((data->get<int>(block + " attack_max") - m_stats.attack) / 100.F);
	m_slopes.defense = ((data->get<int>(block + " defense_max") - m_stats.defense) / 100.F);
	m_stats.exp = data->get<int>("Experience level_1_exp_needed");
	m_slopes.exp = static_cast<float>(m_stats.exp);

	m_slopes.droppedExp = (ska::NumberUtils::absolute(static_cast<int>(m_slopes.exp - m_stats.droppedExp)) / 3.F);


	m_type = STATS_TYPE_LINEAR;
	m_level = 1;
}

void Statistics::nextLevel() {
	m_level++;
	m_stats.exp += static_cast<int>(m_slopes.exp);
	switch(m_type) {
		case STATS_TYPE_LINEAR:
			m_stats.hp += static_cast<int>(m_slopes.hp);
			m_stats.attack += static_cast<int>(m_slopes.attack);
			m_stats.defense += static_cast<int>(m_slopes.defense);
			m_stats.speed += static_cast<int>(m_slopes.speed);
			m_stats.speAttack += static_cast<int>(m_slopes.speAttack);
			m_stats.speDefense += static_cast<int>(m_slopes.speDefense);
			m_stats.droppedExp += static_cast<int>(m_slopes.droppedExp);
			break;

		case STATS_TYPE_SQUARE:
			//???
			break;

		default:
			break;
	}
}

void Statistics::setLevel(unsigned int level) {
	if(m_level != static_cast<int>(level)) {
		m_stats.hp -= static_cast<int>(m_slopes.hp);
		m_stats.attack -= static_cast<int>(m_slopes.attack);
		m_stats.defense -= static_cast<int>(m_slopes.defense);
		m_stats.speed -= static_cast<int>(m_slopes.speed);
		m_stats.speAttack -= static_cast<int>(m_slopes.speAttack);
		m_stats.speDefense -= static_cast<int>(m_slopes.speDefense);
		m_stats.droppedExp -= static_cast<int>(m_slopes.droppedExp);
		m_level = level;
		m_stats.exp += static_cast<int>(m_slopes.exp)*level;
		m_stats.hp += static_cast<int>(m_slopes.hp)*level;
		m_stats.attack += static_cast<int>(m_slopes.attack)*level;
		m_stats.defense += static_cast<int>(m_slopes.defense)*level;
		m_stats.speed += static_cast<int>(m_slopes.speed)*level;
		m_stats.speAttack += static_cast<int>(m_slopes.speAttack)*level;
		m_stats.speDefense += static_cast<int>(m_slopes.speDefense)*level;
		m_stats.droppedExp += static_cast<int>(m_slopes.droppedExp)*level;

	}
}

int Statistics::getMaxSpeed() const {
	return static_cast<int>(m_stats.speed + 21 * m_slopes.speed);
}

int Statistics::getLevel() const
{
	return m_level;
}

int Statistics::getHpMax() const
{
	return m_stats.hp;
}

int Statistics::getAttack() const
{
	return m_stats.attack;
}

int Statistics::getDefense() const
{
	return m_stats.defense;
}

int Statistics::getSpeAttack() const
{
	return m_stats.speAttack;
}

int Statistics::getSpeDefense() const
{
	return m_stats.speDefense;
}

int Statistics::getSpeed() const
{
	return m_stats.speed;
}

int Statistics::getExperience() const
{
	return m_stats.exp;
}

int Statistics::getDroppedExperience() const
{
	return m_stats.droppedExp;
}

void Statistics::setExperience(int exp) {
	setLevel(static_cast<int>(exp / m_slopes.exp));
}

bool Statistics::addExperience(int exp) {
	unsigned int lastexp = m_stats.exp + exp;
	auto lastlevel = m_level;
	m_stats.exp += exp;
	setLevel(static_cast<int>(m_stats.exp / m_slopes.exp));
	m_stats.exp = lastexp;

	return (m_level != lastlevel);
}

Statistics::~Statistics() {
}


