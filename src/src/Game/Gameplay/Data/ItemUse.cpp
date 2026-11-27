#include "Game/__internalConfig/LoggerConfig.h"
#include "ItemUse.h"
#include <limits>
#include "Bag.h"
#include "Team.h"
#include "DialogEvent.h"
#include "../World/WorldState.h"
#include "../Fight/BattleRules.h"
#include "Core/Inputs/InputContextManager.h"

bool UseItem(WorldState& worldState, Bag& bag, int item, int teamIndex) {
	const auto data = Bag::data(item);
	if (bag.count(item) <= 0 || !data.has_value() || data->heal <= 0) {
		return false;
	}
	if (HealTeamMember(worldState.getSaveGame().getPathName(), teamIndex, data->heal, worldState.getBattleRules()) <= 0) {
		return false;
	}
	bag.remove(item, 1);
	return true;
}

ItemShortcuts::ItemShortcuts(PokemonGameEventDispatcher& ged, WorldState& worldState) :
	SubObserver<ska::InputKeyEvent>(std::bind(&ItemShortcuts::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_ged(ged),
	m_worldState(worldState) {
}

void ItemShortcuts::message(const std::string& text) {
	static auto messageNumber = 0u;
	auto dialog = DialogEvent{ text, "itemMessage" + std::to_string(messageNumber++), ska::Rectangle{ 0, std::numeric_limits<int>::max(), -1, 2 * 48 }, true };
	m_ged.ska::Observable<DialogEvent>::notifyObservers(dialog);
}

bool ItemShortcuts::onKeyInputEvent(ska::InputKeyEvent& event) {
	// During a battle the items are used by the trainer in the battle (TrainerItemSystem : on the Pokemon fighting)
	if (m_worldState.battleStage() != nullptr) {
		return true;
	}
	const auto& actions = event.icm.getActions();
	const ska::InputAction shortcuts[] = { ska::UseObject1, ska::UseObject2, ska::UseObject3, ska::UseObject4, ska::UseObject5, ska::UseObject6 };
	auto& bag = m_worldState.getBag();
	for (auto place = std::size_t{ 0 }; place < 6; place++) {
		if (!actions[shortcuts[place]]) {
			continue;
		}
		const auto items = bag.items();
		if (place >= items.size()) {
			return true;
		}
		const auto item = items[place];
		const auto data = Bag::data(item);
		const auto name = data.has_value() ? data->name : "?";
		const auto team = Team{ m_worldState.getSaveGame().getPathName() };
		for (auto index = 0; index < team.size(); index++) {
			if (UseItem(m_worldState, bag, item, index)) {
				message(name + " : " + std::to_string(team.member(index)->hp) + " PV");
				return true;
			}
		}
		message(name + " n'a aucun effet.");
		return true;
	}
	return true;
}
