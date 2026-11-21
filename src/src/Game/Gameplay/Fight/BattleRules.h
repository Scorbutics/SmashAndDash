#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include "Runtime/Value/NodeValue.h"

namespace ska {
	namespace scripting {
		class ScriptVM;
	}
}

// The battle rules and the AI of the wild Pokemon are scripts (Data/Battle) : the battle (C++) asks them for values.
// When a script fails (or is missing), the error is logged once and a default value is used : the battle goes on.
class BattleRules {
public:
	explicit BattleRules(ska::scripting::ScriptVM& vm, std::string rulesFile = "./Data/Battle/rules.miniska", std::string aiFile = "./Data/Battle/ai.miniska");

	// Statistic of a Pokemon at a level, from its base statistic ([BaseStats] of Data/Monsters/<id>.ini)
	int stat(int base, int level);
	int maxHp(int baseHp, int level);
	// Level of a Pokemon from its experience ("experienceNeeded" : [Experience] level_1_exp_needed)
	int level(int experience, int experienceNeeded);
	// Experience won by defeating a Pokemon ("dropped" : [Experience] level_1_exp_dropped of the defeated Pokemon, "level" : its level)
	int experience(int dropped, int level);
	// Damage of a skill hitting a Pokemon
	int damage(int power, int attack, int defense, int level);
	// Skill used by a wild Pokemon (its index), -1 for none. "ready" : the skills it can use now
	int decide(int monster, int distanceBlocks, int hp, int maxHp, const std::vector<bool>& ready);

	static constexpr auto DefaultRulesFile = "./Data/Battle/rules.miniska";

private:
	long call(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, long fallback);

	ska::scripting::ScriptVM& m_vm;
	std::string m_rulesFile;
	std::string m_aiFile;
	std::unordered_set<std::string> m_reported;
};
