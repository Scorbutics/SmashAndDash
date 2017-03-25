#pragma once
#include "ECS/Component.h"
#include "Point.h"

class PokeballComponent : public ska::Component {
	friend class PokeballSystem;

public:
	PokeballComponent(): a(0), b(0), c(0), sens(0){
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
