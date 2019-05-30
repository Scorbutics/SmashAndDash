#include "Game/__internalConfig/LoggerConfig.h"
#include "DialogMenu.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/Button.h"
#include "GUI/Components/Concrete/ScrollingMultiLineLabel.h"
#include "GUI/Components/Concrete/TileSurface.h"
#include "../../Utils/IDs.h"
#include "GUI/GUI.h"

DialogMenu::DialogMenu(ska::Widget& parent, const std::string& text, const std::string& name, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
TimeScrollableWindowIG<ska::KeyEventListener>(parent, rect),
m_fontSize(fontSize),
m_timeout(timeout),
m_name(name),
m_scroll(scroll) {
	auto& tileSurface = setBackground<ska::TileSurface>(ska::Rectangle{ 0, 0, rect.w, rect.h }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu.png");
	tileSurface.setOpacity(170);
	addWidget<ska::ScrollingMultiLineLabel>(text, m_fontSize, 5.F, ska::Point<int>(16, 16));
}

void DialogMenu::render(ska::Renderer& renderer) const{
	TimeScrollableWindowIG<ska::KeyEventListener>::render(renderer);
}

bool DialogMenu::isVisible() const {
    return TimeScrollableWindowIG<ska::KeyEventListener>::isVisible();
}

int DialogMenu::getPriority() const {
	return ska::DrawableFixedPriority::getPriority();
}

const std::string& DialogMenu::getName() const {
    return m_name;
}
