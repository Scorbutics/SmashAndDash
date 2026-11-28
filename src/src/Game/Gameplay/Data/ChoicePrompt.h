#pragma once
#include <optional>
#include <string>
#include <vector>

// A choice asked to the player by a script (GameDialog.select) : drawn by the menu of the trainer (TrainerMenu), on the
// map and in the battles. The script waits for the answer : the index of the choice, -1 when the player cancels
struct ChoicePrompt {
	bool active = false;
	std::string question;
	std::vector<std::string> choices;
	std::optional<int> answer;
};
