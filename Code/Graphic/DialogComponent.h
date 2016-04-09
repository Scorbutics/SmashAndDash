#pragma once
#include "../ska/ECS/Component.h"
#include "GUI\DialogMenu.h"


class DialogComponent : public ska::Component {
public:
	DialogComponent() {}
	
	void operator=(const DialogComponent& dm) {
		dialog = dm.dialog;
	}

	DialogComponent(const DialogComponent& dm) {
		operator=(dm);
	}

	DialogComponent(const DialogMenu& dm) {
		dialog = dm;
	}
	virtual ~DialogComponent() {}

	DialogMenu dialog;
};