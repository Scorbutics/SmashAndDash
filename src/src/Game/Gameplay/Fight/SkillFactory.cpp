#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillFactory.h"
#include "Base/IO/Readers/IniReader.h"
#include "SkillDescriptor.h"
#include "SkillsHolderComponent.h"

void LoadRawStatistics(RawStatistics<int>& stats, ska::IniReader& data, const std::string& block) {
	stats.hp = data.get<int>(block + " hp");
	stats.defense = data.get<int>(block + " defense");
	stats.speDefense = data.get<int>(block + " spe_defense");
	stats.speAttack = data.get<int>(block + " spe_attack");
	stats.speed = data.get<int>(block + " speed") / 2;
	stats.attack = data.get<int>(block + " attack");
}

SkillFactory::SkillFactory(WorldState& ws, unsigned int level): 
	m_worldState(ws),
	m_level(level) {
	
}

void SkillFactory::createSkill(SkillDescriptor& sd, const std::string& skillPath) const {
	ska::IniReader skillData(skillPath);

	if (!skillData.isLoaded()) {
		return;
	}

	sd.style1 = skillData.get<std::string>("Description style_1");
	sd.style2 = skillData.get<std::string>("Description style_2");
	sd.description = skillData.get<std::string>("Description description");
	sd.name = skillData.get<std::string>("Description name");
	sd.type = skillData.get<std::string>("Description type");
	sd.context = skillData.get<std::string>("Description context");

	const std::string& particleNumber = "Particle number";
	const std::string& particleSpeed = "Particle speed";
	const std::string& particleKnockback = "Particle knockback";
	const std::string& particleNoise = "Particle noise";
	const std::string& particleAmplitude = "Particle amplitude";

	sd.particleNumber = skillData.exists(particleNumber) ? skillData.get<int>(particleNumber) : 0;
	sd.speed = skillData.exists(particleSpeed) ? skillData.get<float>(particleSpeed) : 0;
	sd.knockback = static_cast<int>(skillData.exists(particleKnockback) ? skillData.get<float>(particleKnockback) : 0);
	sd.noise = static_cast<int>(skillData.exists(particleNoise) ? skillData.get<float>(particleNoise) : 0);
	sd.amplitude = skillData.exists(particleAmplitude) ? skillData.get<float>(particleAmplitude) : 0;;


	sd.id = skillData.get<int>("Description id");

	sd.cooldown = skillData.get<unsigned int>("Stats cooldown");
	sd.range = skillData.get<int>("Stats blocks_range") * static_cast<int>(m_worldState.getWorld().getBlockSize());

	if (sd.style1 == "Buff" || sd.style2 == "Buff") {
		LoadRawStatistics(sd.buffAlly, skillData, "BuffsAlly");
		LoadRawStatistics(sd.buffEnemy, skillData, "BuffsEnemy");
		sd.alterAlly = skillData.get<int>("StatusAlter ally");
		sd.alterEnemy = skillData.get<int>("StatusAlter enemy");
	}
}

void SkillFactory::loadSkills(const ska::IniReader& reader, const ska::EntityId, SkillsHolderComponent& shc) const {
	for (unsigned int i = 0; reader.exists("Skills " + ska::StringUtils::intToStr(static_cast<int>(i))) && i < shc.skills.size(); i++) {
		if (reader.get<unsigned int>("Skills " + ska::StringUtils::intToStr(static_cast<int>(i)) + "_level") <= m_level) {
			createSkill(shc.skills[i], reader.get<std::string>("Skills " + ska::StringUtils::intToStr(static_cast<int>(i))));
		}
	}
}