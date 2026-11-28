#pragma once
#include <string>
#include <vector>

namespace ska {
	class IniReader;
}
class BattleRules;
class WorldState;

// The skills a Pokemon can know : those of its data ([Skills] of Data/Monsters/<id>.ini : the file of each skill, the
// level it is learned at), without the skills whose file is not written yet
struct LearnableSkill {
	// Its place in the data ("Skills <index>")
	int index = 0;
	std::string file;
	int level = 1;
	std::string name;
};
std::vector<LearnableSkill> LearnableSkills(const ska::IniReader& data);

// A Pokemon knows 4 skills at most
constexpr int MovesetSize = 4;

// The skills of a Pokemon of a level as the battle rules give them (the last ones learned : Data/Battle/skills.miniska),
// their indexes in its data
std::vector<int> DefaultMoveset(BattleRules& rules, const ska::IniReader& data, int level);

// The moveset of a Pokemon of the team : the one saved, else the one of its level (then saved : it is chosen from now
// on, the skills it learns are asked at its next levels)
std::vector<int> TeamMoveset(WorldState& worldState, int teamIndex);

// The skills learned at the levels after "from" up to "to" that it does not know yet (their indexes in its data)
std::vector<int> LearnedBetween(const ska::IniReader& data, int from, int to, const std::vector<int>& known);
