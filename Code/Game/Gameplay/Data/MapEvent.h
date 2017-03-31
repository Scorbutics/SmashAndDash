#pragma once
#include "../Fight/FightComponent.h"

struct MapEvent {
	enum MapEventType {
		BATTLE,
		MAP
	};

	explicit MapEvent(MapEventType met) : eventType(met), fightComponent(nullptr), fightPos(nullptr){
	}

	const MapEventType eventType;
	
	FightComponent* fightComponent;
	ska::Point<int>* fightPos;

	std::string mapName;
	std::string chipsetName;

	~MapEvent() = default;

};
