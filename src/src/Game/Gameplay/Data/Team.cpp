#include <vector>
#include <iterator>
#include "Game/__internalConfig/LoggerConfig.h"
#include "Game/Gameplay/Data/SavePaths.h"
#include "Team.h"
#include <fstream>
#include "Base/IO/Readers/IniReader.h"
#include "Base/IO/Files/FileUtils.h"
#include "../Fight/BattleRules.h"
#include <algorithm>

Team::Team(std::string saveName) :
	m_directory(SaveDirectory() + std::move(saveName) + "/Team/") {
}

std::string Team::file(int index) const {
	return m_directory + std::to_string(index) + ".ini";
}

int Team::size() const {
	auto index = 0;
	while (index < MaxSize && std::ifstream{ file(index) }.good()) {
		index++;
	}
	return index;
}

std::optional<TeamMember> Team::member(int index) const {
	if (index < 0 || index >= size()) {
		return {};
	}
	const auto reader = ska::IniReader{ file(index) };
	if (!reader.exists("Data id")) {
		return {};
	}
	auto member = TeamMember{};
	member.id = reader.get<int>("Data id");
	member.experience = reader.exists("Data experience") ? reader.get<int>("Data experience") : 0;
	member.hp = reader.exists("Stats hp") ? reader.get<int>("Stats hp") : 0;
	for (auto slot = 0; reader.exists("Skills " + std::to_string(slot)); slot++) {
		member.skills.push_back(reader.get<int>("Skills " + std::to_string(slot)));
	}
	return member;
}

void Team::setMember(int index, const TeamMember& member) {
	// A new member : its file does not exist yet (the reader of a missing file throws)
	auto reader = std::ifstream{ file(index) }.good() ? ska::IniReader{ file(index) } : ska::IniReader{};
	reader.set("Data id", member.id);
	reader.set("Data experience", member.experience);
	reader.set("Stats hp", member.hp < 0 ? 0 : member.hp);
	for (auto slot = std::size_t{ 0 }; slot < member.skills.size(); slot++) {
		reader.set("Skills " + std::to_string(slot), member.skills[slot]);
	}
	reader.save(file(index));
}

std::optional<int> Team::add(const TeamMember& member) {
	const auto index = size();
	if (index >= MaxSize) {
		return {};
	}
	ska::FileUtils::createDirectory(m_directory);
	setMember(index, member);
	return index;
}

bool Team::move(int from, int to) {
	const auto count = size();
	if (from < 0 || to < 0 || from >= count || to >= count) {
		return false;
	}
	if (from == to) {
		return true;
	}
	// The files as they are (every field of a Pokemon goes with it)
	auto files = std::vector<std::string>{};
	for (auto index = 0; index < count; index++) {
		auto in = std::ifstream{ file(index), std::ios::binary };
		files.emplace_back(std::istreambuf_iterator<char>{ in }, std::istreambuf_iterator<char>{});
	}
	auto moved = std::move(files[static_cast<std::size_t>(from)]);
	files.erase(files.begin() + from);
	files.insert(files.begin() + to, std::move(moved));
	for (auto index = 0; index < count; index++) {
		auto out = std::ofstream{ file(index), std::ios::binary | std::ios::trunc };
		out << files[static_cast<std::size_t>(index)];
	}
	return true;
}

std::optional<int> Team::firstAble() const {
	for (auto index = 0; index < size(); index++) {
		const auto pokemon = member(index);
		if (pokemon.has_value() && pokemon->hp > 0) {
			return index;
		}
	}
	return {};
}

int HealTeamMember(const std::string& saveName, int index, int amount, BattleRules& rules) {
	auto team = Team{ saveName };
	auto pokemon = team.member(index);
	if (!pokemon.has_value() || amount <= 0) {
		return 0;
	}
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(pokemon->id) + ".ini" };
	const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
	const auto baseHp = data.exists("BaseStats hp") ? data.get<int>("BaseStats hp") : 10;
	const auto maxHp = rules.maxHp(baseHp, rules.level(pokemon->experience, needed));
	const auto before = std::max(0, pokemon->hp);
	// Not hurt (hit points saved above the maximum by an old version are kept)
	if (before >= maxHp) {
		return 0;
	}
	pokemon->hp = std::min(maxHp, before + amount);
	team.setMember(index, *pokemon);
	return std::max(0, pokemon->hp - before);
}
