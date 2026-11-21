#pragma once
#include <functional>
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
	// Called once the dialog is closed by the player (ex : resumes the script that shows the message),
	// with the answer of the player to a question (validated : true)
	std::function<void(bool)> onClose;
	// Yes / no question : the player validates (Return) or refuses (Backspace)
	bool question = false;
	const bool scroll;
};
