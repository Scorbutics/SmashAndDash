#include "Game/__internalConfig/LoggerConfig.h"
#include "Bag.h"
#include <algorithm>
#include <fstream>
#include "Base/IO/Readers/IniReader.h"
#include "InventoryEvent.h"

Bag::Bag(PokemonGameEventDispatcher& ged) :
	m_ged(ged) {
}

void Bag::load(const std::string& saveName) {
	m_saveName = saveName;
	m_items.clear();
	const auto reader = ska::IniReader{ "./Data/Saves/" + saveName + "/trainer.ini" };
	for (auto index = 0; reader.exists("Items " + std::to_string(index) + "_id"); index++) {
		const auto key = "Items " + std::to_string(index);
		const auto amount = reader.exists(key + "_amount") ? reader.get<int>(key + "_amount") : 0;
		m_items.emplace_back(reader.get<int>(key + "_id"), std::max(0, amount));
	}
	m_money = reader.exists("Trainer money") ? reader.get<int>("Trainer money") : DefaultMoney;
	notify();
}

void Bag::save() {
	if (m_saveName.empty()) {
		return;
	}
	const auto path = "./Data/Saves/" + m_saveName + "/trainer.ini";
	// The other keys (the trainer position...) are kept
	auto reader = ska::IniReader{ path };
	for (auto index = std::size_t{ 0 }; index < m_items.size(); index++) {
		const auto key = "Items " + std::to_string(index);
		reader.set(key + "_id", m_items[index].first);
		reader.set(key + "_amount", m_items[index].second);
	}
	reader.set("Trainer money", m_money);
	reader.save(path);
}

int Bag::count(int item) const {
	auto total = 0;
	for (const auto& [id, amount] : m_items) {
		if (id == item) {
			total += amount;
		}
	}
	return total;
}

std::vector<int> Bag::items() const {
	auto result = std::vector<int>{};
	for (const auto& [id, amount] : m_items) {
		if (amount > 0 && std::find(result.begin(), result.end(), id) == result.end()) {
			result.push_back(id);
		}
	}
	return result;
}

void Bag::add(int item, int amount) {
	if (amount <= 0) {
		return;
	}
	auto entry = std::find_if(m_items.begin(), m_items.end(), [item](const auto& e) { return e.first == item; });
	if (entry == m_items.end()) {
		m_items.emplace_back(item, amount);
	} else {
		entry->second += amount;
	}
	save();
	notify();
}

bool Bag::remove(int item, int amount) {
	if (amount <= 0 || count(item) < amount) {
		return false;
	}
	auto left = amount;
	for (auto& [id, owned] : m_items) {
		if (id == item && left > 0) {
			const auto removed = std::min(owned, left);
			owned -= removed;
			left -= removed;
		}
	}
	save();
	notify();
	return true;
}

void Bag::earn(int amount) {
	if (amount <= 0) {
		return;
	}
	m_money += amount;
	save();
	notify();
}

bool Bag::pay(int amount) {
	if (amount < 0 || amount > m_money) {
		return false;
	}
	m_money -= amount;
	save();
	notify();
	return true;
}

void Bag::notify() {
	auto event = InventoryEvent{};
	event.money = m_money;
	for (const auto item : items()) {
		const auto itemData = data(item);
		event.items.push_back(InventoryEvent::Entry{ item, itemData.has_value() ? itemData->name : "?", count(item) });
	}
	m_ged.ska::Observable<InventoryEvent>::notifyObservers(event);
}

std::optional<ItemData> Bag::data(int item) {
	const auto file = "./Data/Items/" + std::to_string(item) + ".ini";
	if (!std::ifstream{ file }.good()) {
		return {};
	}
	const auto reader = ska::IniReader{ file };
	auto result = ItemData{};
	result.name = reader.exists("Description name") ? reader.get<std::string>("Description name") : "?";
	result.description = reader.exists("Description description") ? reader.get<std::string>("Description description") : "";
	result.price = reader.exists("Shop price") ? reader.get<int>("Shop price") : 0;
	const auto isBuff = reader.exists("Effect style") && reader.get<std::string>("Effect style") == "Buff";
	result.heal = isBuff && reader.exists("Buffs hp") ? reader.get<int>("Buffs hp") : 0;
	return result;
}
