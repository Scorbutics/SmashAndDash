#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleItems.h"
#include <algorithm>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/EntityManager.h"
#include "BattleComponent.h"
#include "BattleEffects.h"
#include "BattleRules.h"
#include "BattleStage.h"
#include "FighterComponent.h"
#include "../Data/Bag.h"
#include "../Data/EntityLoadEvent.h"
#include "../Data/Team.h"
#include "../World/WorldState.h"

namespace {
	class BattleItemsLog {};
}

std::vector<int> BattleItems(const Bag& bag) {
	auto usable = std::vector<int>{};
	for (const auto item : bag.items()) {
		const auto data = Bag::data(item);
		if (data.has_value() && data->battle() && bag.count(item) > 0) {
			usable.push_back(item);
		}
	}
	return usable;
}

int HealFighter(ska::EntityManager& entities, BattleStage& stage, Bag& bag, int item, int amount, ska::EntityId fighter) {
	if (amount <= 0 || !entities.hasComponent<BattleComponent>(fighter)) {
		return 0;
	}
	auto& bc = entities.getComponent<BattleComponent>(fighter);
	const auto healed = std::min(amount, bc.maxHp - bc.hp);
	if (healed <= 0 || bc.hp <= 0) {
		MarkText(entities, stage, fighter, "AUCUN EFFET", 0);
		return 0;
	}
	bc.hp += healed;
	bag.remove(item, 1);
	MarkText(entities, stage, fighter, "+" + std::to_string(healed) + " PV", 1);
	stage.sound(BattleSound::Status);
	return healed;
}

CaptureRoll RollCapture(ska::EntityManager& entities, WorldState& worldState, ska::EntityId target) {
	const auto& bc = entities.getComponent<BattleComponent>(target);
	const auto& fighter = entities.getComponent<FighterComponent>(target);
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(bc.monster) + ".ini" };
	const auto rate = data.exists("Description capture_rate") ? data.get<int>("Description capture_rate") : 45;
	auto result = CaptureRoll{};
	result.chance = worldState.getBattleRules().captureChance(rate, bc.hp, bc.maxHp, StatusKey(fighter.status));
	result.roll = ska::NumberUtils::random(0, 99);
	result.caught = result.roll < result.chance;
	result.shakes = result.caught ? 3 : (result.roll - result.chance < 15 ? 2 : (result.roll - result.chance < 40 ? 1 : 0));
	SLOG_STATIC(ska::LogLevel::Info, BattleItemsLog) << "Battle : a pokeball at the wild pokemon " << bc.monster << " (chance " << result.chance << "%, roll " << result.roll << ")";
	return result;
}

void CaptureFighter(ska::EntityManager& entities, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, ska::EntityId target) {
	auto& fighter = entities.getComponent<FighterComponent>(target);
	const auto& bc = entities.getComponent<BattleComponent>(target);
	fighter.captured = true;
	fighter.knockedOut = true;
	fighter.knockedOutTime = 0;

	// It joins the team : its level (its experience at the start of the level) and its hit points left
	auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(bc.monster) + ".ini" };
	const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
	auto member = TeamMember{};
	member.id = bc.monster;
	member.experience = worldState.getBattleRules().levelExperience(bc.level, needed);
	member.hp = std::max(1, bc.hp);
	auto team = Team{ worldState.getSaveGame().getPathName() };
	if (team.add(member).has_value()) {
		auto loadEvent = EntityLoadEvent{ data, static_cast<unsigned int>(bc.monster), static_cast<unsigned int>(member.hp) };
		ged.ska::Observable<EntityLoadEvent>::notifyObservers(loadEvent);
	}
	stage.captured = true;
	stage.capturedName = data.exists("Description name") ? data.get<std::string>("Description name") : std::string{ "?" };
	stage.sound(BattleSound::Status);
	SLOG_STATIC(ska::LogLevel::Info, BattleItemsLog) << "Battle : the wild pokemon " << bc.monster << " is captured, level " << bc.level << ", " << member.hp << " hit points";
}
