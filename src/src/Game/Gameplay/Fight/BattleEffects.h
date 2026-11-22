#pragma once
#include <string>
#include "Core/ECS/ECSDefines.h"
#include "FighterComponent.h"

namespace ska {
	class EntityManager;
}
class BattleStage;
class SkillDescriptor;

// The effects of the skills beyond their damage : changes of the statistics (attack, defense, speed), statuses (poison,
// burn, paralysis, sleep, leech, flinch), protection (guard). Shown over the fighters (BattleStage marks).

// The effects of a skill : on its target (0 : none, ex : a self skill), and on the Pokemon that used it
void ApplySkillEffects(ska::EntityManager& entities, BattleStage& stage, const SkillDescriptor& skill, ska::EntityId user, ska::EntityId target);
// A word over a fighter ("tone" : 0 neutral, 1 good for it, 2 bad for it)
void MarkText(ska::EntityManager& entities, BattleStage& stage, ska::EntityId fighter, std::string text, int tone);
// The name of a status (shown when it is given) and its short name (the bar of the fighter)
std::string StatusName(FighterStatus status);
std::string StatusShortName(FighterStatus status);
