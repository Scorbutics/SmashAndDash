#include "Game/__internalConfig/LoggerConfig.h"
#include "Team.h"
#include <fstream>
#include "Base/IO/Readers/IniReader.h"
#include "Base/IO/Files/FileUtils.h"

Team::Team(std::string saveName) :
	m_directory("./Data/Saves/" + std::move(saveName) + "/Team/") {
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
	return member;
}

void Team::setMember(int index, const TeamMember& member) {
	auto reader = ska::IniReader{ file(index) };
	reader.set("Data id", member.id);
	reader.set("Data experience", member.experience);
	reader.set("Stats hp", member.hp < 0 ? 0 : member.hp);
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

std::optional<int> Team::firstAble() const {
	for (auto index = 0; index < size(); index++) {
		const auto pokemon = member(index);
		if (pokemon.has_value() && pokemon->hp > 0) {
			return index;
		}
	}
	return {};
}
