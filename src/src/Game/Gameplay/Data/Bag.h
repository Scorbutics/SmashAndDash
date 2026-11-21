#pragma once
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "../PokemonGameEventDispatcher.h"

// Data of an item : Data/Items/<id>.ini
struct ItemData {
	std::string name;
	std::string description;
	// [Shop] price
	int price = 0;
	// Hit points given to a Pokemon ([Buffs] hp of a "Buff" item), 0 : no healing
	int heal = 0;
};

// The bag of the player (items, money), saved in the trainer file of the save as it changes :
// [Items] <n>_id / <n>_amount, [Trainer] money
class Bag {
public:
	static constexpr int DefaultMoney = 3000;

	explicit Bag(PokemonGameEventDispatcher& ged);

	void load(const std::string& saveName);

	int count(int item) const;
	// The items of the bag (amount > 0), in the order they were added
	std::vector<int> items() const;
	void add(int item, int amount);
	// false (nothing removed) when there is not enough of the item
	bool remove(int item, int amount);

	int money() const { return m_money; }
	void earn(int amount);
	// false (nothing paid) when there is not enough money
	bool pay(int amount);

	// Sends the content of the bag to the interface (InventoryEvent)
	void notify();

	static std::optional<ItemData> data(int item);

private:
	void save();

	PokemonGameEventDispatcher& m_ged;
	std::string m_saveName;
	// Never shrinks : an item with no amount left keeps its entry (its line in the save)
	std::vector<std::pair<int, int>> m_items;
	int m_money = DefaultMoney;
};
