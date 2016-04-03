#pragma once
#include "../ska/ECS/Component.h"
#include "GUI\DialogMenu.h"

class DialogComponent : public ska::Component {
public:
	DialogComponent() : dialog("", { 0 }, 3000) {}
	DialogComponent(const DialogMenu& dm) : dialog(dm) {}
	virtual ~DialogComponent() {}

	DialogMenu dialog;
};