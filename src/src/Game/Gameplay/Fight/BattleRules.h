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
	// Damage of a skill hitting a Pokemon : "effectiveness" of its type (percents), "stab" : a type of the attacker
	int damage(int power, int attack, int defense, int level, int effectiveness, bool stab);
	// Effectiveness of a skill of a type on a Pokemon of two types (percents : 0, 25, 50, 100, 200, 400)
	int effectiveness(const std::string& skillType, const std::string& type1, const std::string& type2);
	// A Pokemon played by the AI dodges the attack coming ("startupLeft" : milliseconds before it goes)
	bool dodge(int monster, int startupLeft, int hp, int maxHp);
	// Skill used by a wild Pokemon (its index), -1 for none. "ready" : the skills it can use now
	int decide(int monster, int distanceBlocks, int hp, int maxHp, const std::vector<bool>& ready);
	// Movement of a Pokemon played by the AI : 0 holds, 1 approaches, 2 retreats, 3 and 4 sidestep (left, right).
	// "distance" to its opponent and "reach" of its longest ready skill (0 : none ready), in pixels
	int move(int monster, int distance, int reach, int hp, int maxHp);

	static constexpr auto DefaultRulesFile = "./Data/Battle/rules.miniska";

private:
	long call(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, long fallback);
	bool callBool(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, bool fallback);

	ska::scripting::ScriptVM& m_vm;
	std::string m_rulesFile;
	std::string m_aiFile;
	std::unordered_set<std::string> m_reported;
};
