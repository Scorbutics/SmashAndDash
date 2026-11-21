#pragma once
#include <string>
#include <vector>

// Content of the bag of the player, sent to the interface when it changes
struct InventoryEvent {
	struct Entry {
		int item = 0;
		std::string name;
		int amount = 0;
	};
	std::vector<Entry> items;
	int money = 0;
};
