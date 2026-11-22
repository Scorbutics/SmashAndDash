#pragma once

enum BattleEventType {
	BATTLE_START,
	BATTLE_END
};

// A battle starts or ends (the interface of the map hides its windows during a battle)
struct BattleEvent {
	BattleEventType type;
};
