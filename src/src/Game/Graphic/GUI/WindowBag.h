#pragma once
#include <vector>
#include "GUI/Windows/MoveableWindow.h"
#include "../../Gameplay/Data/InventoryEvent.h"

namespace ska {
	class Label;
}

// The bag of the player : its items (with their shortcut key) and its money
class WindowBag : public ska::MoveableWindow<ska::KeyEventListener> {
public:
	static constexpr std::size_t Lines = 6;

	WindowBag(ska::WidgetPanel& parent, const ska::Point<int>& absolutePos);
	~WindowBag() override = default;

	void refresh(const InventoryEvent& content);

private:
	std::vector<ska::Label*> m_lines;
	ska::Label* m_money = nullptr;
};
