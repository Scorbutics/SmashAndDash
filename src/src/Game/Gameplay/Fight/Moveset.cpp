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

namespace {
	class MovesetLog {};

	int LevelOf(BattleRules& rules, const ska::IniReader& data, int experience) {
		const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
		return rules.level(experience, needed);
	}

	int MaxHpOf(BattleRules& rules, const ska::IniReader& data, int level) {
		return rules.maxHp(data.exists("BaseStats hp") ? data.get<int>("BaseStats hp") : 10, level);
	}
}

int TeamEvolution(WorldState& worldState, int teamIndex, int reached) {
	const auto member = Team{ worldState.getSaveGame().getPathName() }.member(teamIndex);
	if (!member.has_value()) {
		return -1;
	}
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(member->id) + ".ini" };
	const auto level = data.exists("Evolution level") ? data.get<int>("Evolution level") : -1;
	const auto into = data.exists("Evolution next_pokemon_id") ? data.get<int>("Evolution next_pokemon_id") : -1;
	const auto item = data.exists("Evolution item_id") ? data.get<int>("Evolution item_id") : -1;
	if (level <= 0 || into <= 0 || item >= 0 || reached < level) {
		return -1;
	}
	if (!std::ifstream{ "./Data/Monsters/" + std::to_string(into) + ".ini" }.good() || !std::ifstream{ "./Sprites/Charsets/" + std::to_string(into) + ".png" }.good()) {
		SLOG_STATIC(ska::LogLevel::Warn, MovesetLog) << "Team : the Pokemon " << teamIndex << " should evolve into " << into << ", which has no data or no sheet";
		return -1;
	}
	return into;
}

bool EvolveTeamMember(WorldState& worldState, int teamIndex, int into) {
	const auto known = TeamMoveset(worldState, teamIndex);
	auto team = Team{ worldState.getSaveGame().getPathName() };
	auto member = team.member(teamIndex);
	const auto file = "./Data/Monsters/" + std::to_string(into) + ".ini";
	if (!member.has_value() || !std::ifstream{ file }.good()) {
		return false;
	}
	auto& rules = worldState.getBattleRules();
	const auto before = ska::IniReader{ "./Data/Monsters/" + std::to_string(member->id) + ".ini" };
	const auto after = ska::IniReader{ file };
	// The skills it knows, found by their file in the data of the new species
	const auto oldSkills = LearnableSkills(before);
	const auto newSkills = LearnableSkills(after);
	auto moveset = std::vector<int>{};
	for (const auto index : known) {
		const auto old = std::find_if(oldSkills.begin(), oldSkills.end(), [index](const LearnableSkill& s) { return s.index == index; });
		if (old == oldSkills.end()) {
			continue;
		}
		const auto found = std::find_if(newSkills.begin(), newSkills.end(), [&old](const LearnableSkill& s) { return s.file == old->file; });
		if (found != newSkills.end()) {
			moveset.push_back(found->index);
		} else {
			SLOG_STATIC(ska::LogLevel::Warn, MovesetLog) << "Team : the skill " << old->file << " is not in the data of " << into << " : forgotten";
		}
	}
	// The hit points it lost are kept (its maximum grows)
	const auto lost = std::max(0, MaxHpOf(rules, before, LevelOf(rules, before, member->experience)) - member->hp);
	const auto maxHp = MaxHpOf(rules, after, LevelOf(rules, after, member->experience));
	member->hp = member->hp <= 0 ? 0 : std::max(1, maxHp - lost);
	member->id = into;
	member->skills = moveset;
	team.setMember(teamIndex, *member);
	SLOG_STATIC(ska::LogLevel::Info, MovesetLog) << "Team : the Pokemon " << teamIndex << " evolves into " << into << " (" << member->hp << " / " << maxHp << " hit points)";
	return true;
}
