#include "Game/__internalConfig/LoggerConfig.h"
#include "Moveset.h"
#include <algorithm>
#include <fstream>
#include "Base/IO/Readers/IniReader.h"
#include "BattleRules.h"
#include "../Data/Team.h"
#include "../World/WorldState.h"

std::vector<LearnableSkill> LearnableSkills(const ska::IniReader& data) {
	auto skills = std::vector<LearnableSkill>{};
	for (auto i = 0; data.exists("Skills " + std::to_string(i)); i++) {
		auto skill = LearnableSkill{};
		skill.index = i;
		skill.file = data.get<std::string>("Skills " + std::to_string(i));
		const auto levelKey = "Skills " + std::to_string(i) + "_level";
		skill.level = data.exists(levelKey) ? data.get<int>(levelKey) : 1;
		auto reader = ska::IniReader{};
		if (!reader.load(skill.file)) {
			continue;
		}
		skill.name = reader.exists("Description name") ? reader.get<std::string>("Description name") : skill.file;
		skills.push_back(std::move(skill));
	}
	return skills;
}

std::vector<int> DefaultMoveset(BattleRules& rules, const ska::IniReader& data, int level) {
	const auto skills = LearnableSkills(data);
	auto levels = std::vector<int>{};
	for (const auto& skill : skills) {
		levels.push_back(skill.level);
	}
	auto moveset = std::vector<int>{};
	for (const auto k : rules.knownSkills(levels, level, MovesetSize)) {
		if (k >= 0 && k < static_cast<int>(skills.size())) {
			moveset.push_back(skills[static_cast<std::size_t>(k)].index);
		}
	}
	return moveset;
}

std::vector<int> TeamMoveset(WorldState& worldState, int teamIndex) {
	auto team = Team{ worldState.getSaveGame().getPathName() };
	auto member = team.member(teamIndex);
	if (!member.has_value()) {
		return {};
	}
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(member->id) + ".ini" };
	// The skills saved whose file exists
	auto known = std::vector<int>{};
	const auto learnable = LearnableSkills(data);
	for (const auto index : member->skills) {
		if (std::any_of(learnable.begin(), learnable.end(), [index](const LearnableSkill& s) { return s.index == index; })) {
			known.push_back(index);
		}
	}
	if (!known.empty()) {
		return known;
	}
	auto& rules = worldState.getBattleRules();
	const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
	member->skills = DefaultMoveset(rules, data, rules.level(member->experience, needed));
	team.setMember(teamIndex, *member);
	return member->skills;
}

std::vector<int> LearnedBetween(const ska::IniReader& data, int from, int to, const std::vector<int>& known) {
	auto learned = std::vector<int>{};
	for (const auto& skill : LearnableSkills(data)) {
		if (skill.level > from && skill.level <= to && std::find(known.begin(), known.end(), skill.index) == known.end()) {
			learned.push_back(skill.index);
		}
	}
	return learned;
}
