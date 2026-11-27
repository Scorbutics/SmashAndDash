#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillFactory.h"
#include <algorithm>
#include <fstream>
#include <vector>
#include "Base/IO/Readers/IniReader.h"
#include "SkillDescriptor.h"
#include "SkillsHolderComponent.h"
#include "Game/Gameplay/Fight/BattleRules.h"

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
	ska::IniReader skillData;
	if (!skillData.load(skillPath)) {
		SLOG(ska::LogLevel::Warn) << "Skill file not found : " << skillPath;
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

	sd.power = skillData.exists("Particle damage") ? skillData.get<int>("Particle damage") : -sd.buffEnemy.hp;
	sd.contact = sd.style1 == "Contact" || sd.style2 == "Contact";
	sd.kind = sd.contact ? SkillKind::Contact : (sd.style2 == "Area" ? SkillKind::Area : (sd.style2 == "Self" ? SkillKind::Self : SkillKind::Projectile));
	// An area or self skill has no particle : it can be used all the same
	if (sd.kind == SkillKind::Area || sd.kind == SkillKind::Self) {
		sd.particleNumber = std::max(sd.particleNumber, 1);
	}
	const auto effect = [&skillData](const std::string& key, int fallback) {
		return skillData.exists("Effects " + key) ? skillData.get<int>("Effects " + key) : fallback;
	};
	sd.targetStages = { effect("target_attack", 0), effect("target_defense", 0), effect("target_speed", 0) };
	sd.selfStages = { effect("self_attack", 0), effect("self_defense", 0), effect("self_speed", 0) };
	sd.status = skillData.exists("Effects status") ? skillData.get<std::string>("Effects status") : std::string{};
	sd.statusChance = effect("status_chance", 100);
	sd.effectDuration = static_cast<unsigned int>(effect("duration", 6000));
	sd.fixedDamage = effect("fixed_damage", 0);
	sd.showSprite = !skillData.exists("Particle show_sprite") || skillData.get<int>("Particle show_sprite") != 0;
	sd.startup = skillData.exists("Stats startup") ? skillData.get<unsigned int>("Stats startup") : (sd.contact ? 200 : 250);
	sd.recovery = skillData.exists("Stats recovery") ? skillData.get<unsigned int>("Stats recovery") : 250;
	sd.special = skillData.exists("Description class") && skillData.get<std::string>("Description class") == "Speciale";
}

void SkillFactory::loadSkills(const ska::IniReader& reader, const ska::EntityId, SkillsHolderComponent& shc) const {
	// The skills it knows (Data/Battle/skills.miniska) : from the levels its skills are learned at. A skill
	// without its file (not written yet) is left out : the Pokemon knows the latest ones that exist
	auto names = std::vector<std::string>{};
	auto levels = std::vector<int>{};
	for (auto i = 0; reader.exists("Skills " + ska::StringUtils::intToStr(i)); i++) {
		const auto levelKey = "Skills " + ska::StringUtils::intToStr(i) + "_level";
		const auto name = reader.get<std::string>("Skills " + ska::StringUtils::intToStr(i));
		if (!std::ifstream{ name }.good()) {
			SLOG(ska::LogLevel::Warn) << "Skill file not found, left out : " << name;
			continue;
		}
		names.push_back(name);
		levels.push_back(reader.exists(levelKey) ? reader.get<int>(levelKey) : 1);
	}
	const auto known = m_worldState.getBattleRules().knownSkills(levels, static_cast<int>(m_level), static_cast<int>(shc.skills.size()));
	for (auto k = 0u; k < known.size() && k < shc.skills.size(); k++) {
		createSkill(shc.skills[k], names[static_cast<std::size_t>(known[k])]);
	}

	// Every Pokemon has Charge (the basic attack) : in the first slot that cannot be used in a battle
	// (no skill, or a skill without projectile : the buffs do not exist in this version of the game)
	for (auto& skill : shc.skills) {
		if (skill.particleNumber <= 0) {
			skill = SkillDescriptor{};
			createSkill(skill, BasicAttack);
			break;
		}
	}
}