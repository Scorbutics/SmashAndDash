#include "Game/__internalConfig/LoggerConfig.h"
#include "WindowBag.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/TileSurface.h"
#include "GUI/Components/Concrete/Label.h"

WindowBag::WindowBag(ska::WidgetPanel& parent, const ska::Point<int>& absolutePos) :
	MoveableWindow<ska::KeyEventListener>(parent, ska::Rectangle{ absolutePos.x, absolutePos.y, 8 * 32, 8 * 32 }) {
	setTitle("Sac");
	auto& tileSurface = setBackground<ska::TileSurface>(ska::Rectangle{ 0, 0, 1, 1 }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu.png");
	tileSurface.setOpacity(210);
	for (auto line = std::size_t{ 0 }; line < Lines; line++) {
		m_lines.push_back(&addWidget<ska::Label>(" ", 11, ska::Point<int>(16, 32 + static_cast<int>(line) * 20)));
	}
	m_money = &addWidget<ska::Label>(" ", 11, ska::Point<int>(16, 40 + static_cast<int>(Lines) * 20));
}

void WindowBag::refresh(const InventoryEvent& content) {
	// The keys of the item shortcuts : Z, X, C, V, B, N
	static const char* keys[] = { "Z", "X", "C", "V", "B", "N" };
	for (auto line = std::size_t{ 0 }; line < Lines; line++) {
		if (line < content.items.size()) {
			const auto& item = content.items[line];
			m_lines[line]->modifyText(std::string{ "[" } + keys[line] + "] " + item.name + " x" + std::to_string(item.amount));
		} else {
			m_lines[line]->modifyText(" ");
		}
	}
	m_money->modifyText("Argent : " + std::to_string(content.money));
}
