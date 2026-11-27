#pragma once
#include <optional>
#include <string>

// A Pokemon of the team of the player, saved in Data/Saves/<save>/Team/<index>.ini
struct TeamMember {
	int id = 0;
	int experience = 0;
	// Hit points left
	int hp = 0;
};

class BattleRules;

// Gives hit points to a Pokemon of the team, up to its hit points at its level (battle rules) : the hit points given
int HealTeamMember(const std::string& saveName, int index, int amount, BattleRules& rules);

// The team of the player : the files of the save are the reference (up to 6 Pokemon)
class Team {
public:
	static constexpr int MaxSize = 6;

	explicit Team(std::string saveName);

	int size() const;
	std::optional<TeamMember> member(int index) const;
	void setMember(int index, const TeamMember& member);
	// Adds a Pokemon at the end of the team : its index, or nothing when the team is full
	std::optional<int> add(const TeamMember& member);
	// Moves the Pokemon at "from" to "to" (the others follow : 0 1 2 3, move(3, 1) : 0 3 1 2). False : out of the team
	bool move(int from, int to);
	// First Pokemon able to fight (hit points left)
	std::optional<int> firstAble() const;

private:
	std::string file(int index) const;
	std::string m_directory;
};
