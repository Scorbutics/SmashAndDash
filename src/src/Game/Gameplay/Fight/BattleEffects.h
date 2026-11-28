#pragma once
class BattleRules;
#include <string>
#include <vector>
#include "Core/ECS/ECSDefines.h"
#include "FighterComponent.h"

namespace ska {
	class EntityManager;
}
class BattleStage;
class SkillDescriptor;

// The effects of the skills beyond their damage : changes of the statistics (attack, defense, speed), statuses (poison,
// burn, paralysis, sleep, leech, flinch), protection (guard). Shown over the fighters (BattleStage marks).

// What the effects of a skill did (the messages of a clash, as in the first games : "L'Attaque de Rattata baisse !")
struct SkillEffect {
	enum class Kind { Stat, Status, Flinch, Guard };
	Kind kind = Kind::Stat;
	ska::EntityId fighter = 0;
	// Stat : the statistic (FighterStat), the change asked (+1, -2...), the stage reached (unchanged : at its limit)
	int stat = 0;
	int change = 0;
	bool changed = true;
	// Status : the status given
	FighterStatus status = FighterStatus::None;
};

// The effects of a skill : on its target (0 : none, ex : a self skill), and on the Pokemon that used it : the changes of
// statistics, the status (rules : Data/Battle/status.miniska), the guard. "report" : what they did (nullptr : not asked)
void ApplySkillEffects(ska::EntityManager& entities, BattleStage& stage, BattleRules& rules, const SkillDescriptor& skill, ska::EntityId user, ska::EntityId target,
	std::vector<SkillEffect>* report = nullptr);
// The message of an effect, as in the first games ("name" : the Pokemon it changed, "Rattata sauvage")
std::string SkillEffectMessage(const SkillEffect& effect, const std::string& name);
// The name of a status for the rules ("none", "poison"...)
const char* StatusKey(FighterStatus status);
// A word over a fighter ("tone" : 0 neutral, 1 good for it, 2 bad for it)
void MarkText(ska::EntityManager& entities, BattleStage& stage, ska::EntityId fighter, std::string text, int tone);
// The name of a status (shown when it is given) and its short name (the bar of the fighter)
std::string StatusName(FighterStatus status);
std::string StatusShortName(FighterStatus status);
