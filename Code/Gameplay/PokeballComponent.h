#pragma once
#include "../ska/ECS/Component.h"
#include "../ska/Graphic/Point.h"

class PokeballComponent : public ska::Component {
	friend class PokeballSystem;

public:
	PokeballComponent() {
		started = false;
		isOpenning = false;
		isInactive = false;
		count = 0;
		speed = 1;
	}
	virtual ~PokeballComponent() = default;

	ska::Point<int> finalPos;
	unsigned int speed;

private:
	/* Parabolic coeffs */
	float a;
	float b;
	float c;

	

	bool isOpenning;
	bool isInactive;
	bool started;

	unsigned int count;

	int sens;

};