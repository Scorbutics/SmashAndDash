#pragma once
#include "../ska/ECS/Component.h"
#include "../ska/Graphic/Point.h"

class PokeballComponent : public ska::Component {
public:
	PokeballComponent() {
		started = false;
		isOpenning = false;
		isInactive = false;
		count = 0;
		speed = 1;
	}
	virtual ~PokeballComponent() = default;

	/* Parabolic coeffs */
	float a;
	float b;
	float c;

	ska::Point<int> finalPos;

	bool isOpenning;
	bool isInactive;
	bool started;

	unsigned int count;
	unsigned int speed;

	int sens;

};