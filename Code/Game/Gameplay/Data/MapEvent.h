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

	~MapEvent() = default;

};
