#pragma once
#include "../Fight/FightComponent.h"

enum class MapEventType {
	Switch_Battle,
	Switch_Map
};

struct MapEvent {

	explicit MapEvent(MapEventType met) : 
		eventType(met) {
	}

	const MapEventType eventType;
	
	FightComponent* fightComponent = nullptr;
	ska::Point<int> position;

	std::string mapName;
	std::string chipsetName;
	// The player comes through a door or a map edge : it appears one block behind the position, and walks to it
	bool walkIn = false;
	// Back from a battle : the map is still there (its characters, its physics), it is not loaded again, no fade
	bool resume = false;

	~MapEvent() = default;

};
