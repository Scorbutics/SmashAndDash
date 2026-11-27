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
	explicit BattleRules(ska::scripting::ScriptVM& vm, std::string rulesFile = "./Data/Battle/rules.miniska", std::string aiFile = "./Data/Battle/ai.miniska",
		std::string statusFile = "./Data/Battle/status.miniska", std::string stagesFile = "./Data/Battle/stages.miniska",
		std::string encountersFile = "./Data/Battle/encounters.miniska", std::string skillsFile = "./Data/Battle/skills.miniska",
		std::string rewardsFile = "./Data/Battle/rewards.miniska");

	// Statistic of a Pokemon at a level, from its base statistic ([BaseStats] of Data/Monsters/<id>.ini)
	int stat(int base, int level);
	int maxHp(int baseHp, int level);
	// Level of a Pokemon from its experience ("experienceNeeded" : [Experience] level_1_exp_needed)
	int level(int experience, int experienceNeeded);
	// The experience a Pokemon has when it reaches a level (the start of the level : its progress bar)
	int levelExperience(int level, int experienceNeeded);
	// Experience won by defeating a Pokemon ("dropped" : [Experience] level_1_exp_dropped of the defeated Pokemon, "level" : its level)
	int experience(int dropped, int level);
	// Damage of a skill hitting a Pokemon : "effectiveness" of its type (percents), "stab" : a type of the attacker
	int damage(int power, int attack, int defense, int level, int effectiveness, bool stab);
	// Damage of a hit given by a wild Pokemon (the player has one Pokemon against them : softer), from its damage
	int wildDamage(int damage);
	// Chance (percents, 1 .. 100) to capture a wild Pokemon with a pokeball : its capture rate, hit points, status
	// ("none", "poison"...). The battle rolls the dice
	int captureChance(int rate, int hp, int maxHp, const std::string& status);
	// Effectiveness of a skill of a type on a Pokemon of two types (percents : 0, 25, 50, 100, 200, 400)
	int effectiveness(const std::string& skillType, const std::string& type1, const std::string& type2);
	// A Pokemon played by the AI dodges the attack coming ("startupLeft" : milliseconds before it goes)
	bool dodge(int monster, int startupLeft, int hp, int maxHp);
	// Skill used by a wild Pokemon (its index), -1 for none. "ready" : the skills it can use now
	int decide(int monster, int distanceBlocks, int hp, int maxHp, const std::vector<bool>& ready);
	// Movement of a Pokemon played by the AI : 0 holds, 1 approaches, 2 retreats, 3 and 4 sidestep (left, right).
	// "distance" to its opponent and "reach" of its longest ready skill (0 : none ready), in pixels
	int move(int monster, int distance, int reach, int hp, int maxHp);

	// Statuses (Data/Battle/status.miniska ; the names : "none", "poison", "burn", "paralysis", "sleep", "leech",
	// "flinch"). The dice are rolled by the battle : "roll" 0 .. 99
	bool statusLands(const std::string& status, int chance, int roll, const std::string& current);
	unsigned int statusDuration(const std::string& status, unsigned int effect);
	unsigned int statusStun(const std::string& status);
	unsigned int statusTickPeriod(const std::string& status);
	int statusTickDamage(const std::string& status, int maxHp);
	bool statusLeech(const std::string& status);
	int statusSpeedPercent(const std::string& status);
	int statusFailOdds(const std::string& status);
	int statusAttackPercent(const std::string& status, bool special);
	bool statusWakes(const std::string& status, int damage);

	// The changes of the statistics (Data/Battle/stages.miniska) : the stage after a change, how long it lasts, its factor
	int stage(int current, int change);
	unsigned int stageDuration(unsigned int effect);
	float stageMultiplier(int stage);

	// The encounters (Data/Battle/encounters.miniska) : the attitude of a wild Pokemon on the map ("aggressive" : it
	// charges the player in sight, "fearful" : it runs away, "passive" : it wanders ; "teamLevel" : the level of the first
	// Pokemon of the team able to fight, "roll" 0 .. 99), the size of the dice of a group, the companions
	std::string attitude(int monster, int level, int teamLevel, int roll);
	// How it arrives on the map : "fly", "hideout" (Data/Battle/arrivals.miniska places it) or "walk"
	std::string arrival(int monster);
	int groupOdds();
	int groupSize();
	int levelSpread();
	int companionLevel(int level, int offset);
	// The skills a Pokemon knows (Data/Battle/skills.miniska) : the indexes of its skills in its data ("levels" : the level
	// each is learned at), "slots" at most
	std::vector<int> knownSkills(const std::vector<int>& levels, int level, int slots);

	// The rewards of a battle (Data/Battle/rewards.miniska) : the experience won by the Pokemon of the player, for the wild
	// Pokemon K.O. ("dropped" : the experience of their data, their "levels")
	int rewardExperience(const std::vector<int>& dropped, const std::vector<int>& levels, bool won, bool fled);

	static constexpr auto DefaultRulesFile = "./Data/Battle/rules.miniska";

private:
	long call(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, long fallback);
	std::string callString(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, std::string fallback);
	bool callBool(const std::string& file, const std::string& function, std::vector<ska::NodeValue> arguments, bool fallback);

	ska::scripting::ScriptVM& m_vm;
	std::string m_rulesFile;
	std::string m_aiFile;
	std::string m_statusFile;
	std::string m_stagesFile;
	std::string m_encountersFile;
	std::string m_skillsFile;
	std::string m_rewardsFile;
	std::unordered_set<std::string> m_reported;
};
