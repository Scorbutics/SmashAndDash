#pragma once

template <typename T>
struct RawStatistics {
	RawStatistics() {
		hp = 0;
		attack = 0;
		defense = 0;
		speAttack = 0;
		speDefense = 0;
		speed = 0;
		exp = 0;
		droppedExp = 0;
	}
	T hp,
	attack,
	defense,
	speAttack,
	speDefense,
	speed,
	exp,
	droppedExp;
};
