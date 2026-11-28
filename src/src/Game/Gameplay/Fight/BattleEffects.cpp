#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleEffects.h"
#include <algorithm>
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "BattleStage.h"
#include "SkillDescriptor.h"
#include "BattleRules.h"
#include "BattleComponent.h"

namespace {
	FighterStatus StatusFromName(const std::string& name) {
		if (name == "poison") return FighterStatus::Poison;
		if (name == "burn") return FighterStatus::Burn;
		if (name == "paralysis") return FighterStatus::Paralysis;
		if (name == "sleep") return FighterStatus::Sleep;
		if (name == "leech") return FighterStatus::Leech;
		return FighterStatus::None;
	}

	const char* StatShortName(int stat) {
		switch (stat) {
		case StatAttack: return "ATQ";
		case StatDefense: return "DEF";
		default: return "VIT";
		}
	}

	void ChangeStages(ska::EntityManager& entities, BattleStage& stage, BattleRules& rules, ska::EntityId entity, const std::array<int, 3>& changes, unsigned int duration,
		std::vector<SkillEffect>* report) {
		auto& fighter = entities.getComponent<FighterComponent>(entity);
		auto rose = false;
		auto fell = false;
		for (auto stat = 0; stat < 3; stat++) {
			const auto change = changes[static_cast<std::size_t>(stat)];
			if (change == 0) {
				continue;
			}
			auto& value = fighter.stages[static_cast<std::size_t>(stat)];
			const auto before = value;
			value = rules.stage(value, change);
			fighter.stageMultipliers[static_cast<std::size_t>(stat)] = rules.stageMultiplier(value);
			fighter.stageTime[static_cast<std::size_t>(stat)] = rules.stageDuration(duration);
			const auto up = change > 0;
			MarkText(entities, stage, entity, std::string{ StatShortName(stat) } + (up ? " +" : " -"), up ? 1 : 2);
			if (value != before) {
				rose = rose || up;
				fell = fell || !up;
			}
			if (report != nullptr) {
				auto effect = SkillEffect{};
				effect.kind = SkillEffect::Kind::Stat;
				effect.fighter = entity;
				effect.stat = stat;
				effect.change = change;
				effect.changed = value != before;
				report->push_back(effect);
			}
		}
		// The arrows over it, its tint, the sound of the first games (a fall shown over a rise : it is the news)
		if (rose || fell) {
			fighter.statChange = FighterComponent::StatChangeDuration;
			fighter.statChangeUp = !fell;
			stage.sound(fell ? BattleSound::StatDown : BattleSound::StatUp);
		}
	}

	const char* StatFullName(int stat) {
		switch (stat) {
		case StatAttack: return "L'Attaque";
		case StatDefense: return "La Défense";
		default: return "La Vitesse";
		}
	}
}

std::string SkillEffectMessage(const SkillEffect& effect, const std::string& name) {
	switch (effect.kind) {
	case SkillEffect::Kind::Stat: {
		const auto up = effect.change > 0;
		const auto subject = std::string{ StatFullName(effect.stat) } + " de " + name;
		if (!effect.changed) {
			return subject + (up ? " ne peut plus augmenter !" : " ne peut plus baisser !");
		}
		const auto strong = effect.change >= 2 || effect.change <= -2;
		return subject + (up ? (strong ? " augmente beaucoup !" : " augmente !") : (strong ? " baisse beaucoup !" : " baisse !"));
	}
	case SkillEffect::Kind::Status:
		switch (effect.status) {
		case FighterStatus::Poison: return name + " est empoisonné !";
		case FighterStatus::Burn: return name + " est brûlé !";
		case FighterStatus::Paralysis: return name + " est paralysé ! Il aura du mal à attaquer !";
		case FighterStatus::Sleep: return name + " s'endort !";
		case FighterStatus::Leech: return name + " est infecté par Vampigraine !";
		default: return "";
		}
	case SkillEffect::Kind::Flinch:
		return name + " est apeuré !";
	case SkillEffect::Kind::Guard:
		return name + " se protège !";
	}
	return "";
}

std::string StatusName(FighterStatus status) {
	switch (status) {
	case FighterStatus::Poison: return "POISON";
	case FighterStatus::Burn: return "BRÛLURE";
	case FighterStatus::Paralysis: return "PARALYSIE";
	case FighterStatus::Sleep: return "SOMMEIL";
	case FighterStatus::Leech: return "VAMPIGRAINE";
	default: return "";
	}
}

std::string StatusShortName(FighterStatus status) {
	switch (status) {
	case FighterStatus::Poison: return "PSN";
	case FighterStatus::Burn: return "BRL";
	case FighterStatus::Paralysis: return "PAR";
	case FighterStatus::Sleep: return "SOM";
	case FighterStatus::Leech: return "VMP";
	default: return "";
	}
}

void MarkText(ska::EntityManager& entities, BattleStage& stage, ska::EntityId fighter, std::string text, int tone) {
	if (!entities.hasComponent<ska::PositionComponent>(fighter) || !entities.hasComponent<ska::HitboxComponent>(fighter)) {
		return;
	}
	const auto& hitbox = entities.getComponent<ska::HitboxComponent>(fighter);
	const auto center = ska::PositionComponent::getCenterPosition(entities.getComponent<ska::PositionComponent>(fighter), hitbox);
	// The words of a fighter are stacked a little (several at once)
	auto above = 0.F;
	for (const auto& mark : stage.marks()) {
		if (mark.kind == BattleMark::Kind::Text && mark.age < 300 && std::abs(mark.position.x - center.x) < 1.F) {
			above += 16.F;
		}
	}
	auto mark = BattleMark{};
	mark.kind = BattleMark::Kind::Text;
	mark.position = ska::Point<float>{ center.x, center.y - static_cast<float>(hitbox.height) - 18.F - above };
	mark.value = tone;
	mark.text = std::move(text);
	stage.mark(std::move(mark));
}

const char* StatusKey(FighterStatus status) {
	switch (status) {
	case FighterStatus::Poison: return "poison";
	case FighterStatus::Burn: return "burn";
	case FighterStatus::Paralysis: return "paralysis";
	case FighterStatus::Sleep: return "sleep";
	case FighterStatus::Leech: return "leech";
	default: return "none";
	}
}

void ApplySkillEffects(ska::EntityManager& entities, BattleStage& stage, BattleRules& rules, const SkillDescriptor& skill, ska::EntityId user, ska::EntityId target,
		std::vector<SkillEffect>* report) {
	const auto reported = [report](SkillEffect::Kind kind, ska::EntityId fighter, FighterStatus status) {
		if (report != nullptr) {
			auto effect = SkillEffect{};
			effect.kind = kind;
			effect.fighter = fighter;
			effect.status = status;
			report->push_back(effect);
		}
	};
	const auto targetFights = target != 0 && entities.hasComponent<FighterComponent>(target) && !entities.getComponent<FighterComponent>(target).knockedOut &&
		!entities.getComponent<FighterComponent>(target).inBall;
	if (targetFights) {
		ChangeStages(entities, stage, rules, target, skill.targetStages, skill.effectDuration, report);
		auto& fighter = entities.getComponent<FighterComponent>(target);
		// The dice are rolled here, the rules decide (Data/Battle/status.miniska)
		if (!skill.status.empty() && skill.status != "guard"
				&& rules.statusLands(skill.status, skill.statusChance, ska::NumberUtils::random(0, 99), StatusKey(fighter.status))) {
			if (skill.status == "flinch") {
				// Flinched : it stops a moment (its attack in preparation is lost) ; not again before a while (no chain of
				// flinches : Data/Battle/status.miniska "immunity")
				if (fighter.flinchGuard == 0) {
					fighter.stun = std::max(fighter.stun, rules.statusStun("flinch"));
					fighter.flinchGuard = rules.statusImmunity("flinch");
					fighter.windUp = 0;
					fighter.pendingSkill = -1;
					MarkText(entities, stage, target, "APEURÉ", 2);
					reported(SkillEffect::Kind::Flinch, target, FighterStatus::None);
				}
			} else {
				const auto status = StatusFromName(skill.status);
				// One status at a time
				if (status != FighterStatus::None && fighter.status == FighterStatus::None) {
					const auto maxHp = entities.hasComponent<BattleComponent>(target) ? entities.getComponent<BattleComponent>(target).maxHp : 1;
					fighter.status = status;
					fighter.statusTime = rules.statusDuration(skill.status, skill.effectDuration);
					fighter.statusTickPeriod = rules.statusTickPeriod(skill.status);
					fighter.statusTick = fighter.statusTickPeriod;
					fighter.statusDamage = rules.statusTickDamage(skill.status, maxHp);
					fighter.statusLeech = rules.statusLeech(skill.status);
					fighter.statusSpeed = rules.statusSpeedPercent(skill.status);
					fighter.statusSource = user;
					if (status == FighterStatus::Sleep) {
						fighter.windUp = 0;
						fighter.pendingSkill = -1;
					}
					fighter.statusBurst = FighterComponent::StatusBurstDuration;
					MarkText(entities, stage, target, StatusName(status), 2);
					stage.sound(BattleSound::Status);
					reported(SkillEffect::Kind::Status, target, status);
				}
			}
		}
	}

	if (user != 0 && entities.hasComponent<FighterComponent>(user)) {
		ChangeStages(entities, stage, rules, user, skill.selfStages, skill.effectDuration, report);
		if (skill.status == "guard") {
			auto& fighter = entities.getComponent<FighterComponent>(user);
			fighter.guard = std::min(skill.effectDuration, 1500u);
			MarkText(entities, stage, user, "ABRI", 1);
			reported(SkillEffect::Kind::Guard, user, FighterStatus::None);
		}
	}
}
