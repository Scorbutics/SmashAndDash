#pragma once
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

struct DialogEvent {
	DialogEvent(const std::string& m, const std::string& n, const ska::Rectangle& b, bool s) :
        message(m), name(n), box(b), scroll(s) {
	}

	DialogEvent(const DialogEvent&) = delete;
	DialogEvent& operator=(const DialogEvent&) = delete;
	~DialogEvent() = default;

	const std::string message;
	const std::string name;
	const ska::Rectangle box;
	ska::ScriptSleepComponent callback;
	bool callbackActive = false;
	const bool scroll;
};
